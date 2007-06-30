/*
 * mainwindow.c
 * This file is part of Stjerm
 *
 * Copyright (C) 2007 - Stjepan Glavina
 *
 * Stjerm is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Stjerm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Stjerm; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */


#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <vte/vte.h>
#include <stdlib.h>
#include "stjerm.h"


GtkWidget *mainwindow;
extern GtkWidget *term;

Window mw_xwin;
Display *dpy = 0;
Atom opacityatom;

void build_mainwindow(void);
void mainwindow_present(void);
static void mainwindow_reset_position(void);
static void mainwindow_set_opacity(void);
static void mainwindow_show(GtkWidget*, gpointer);
static void mainwindow_focus_out_event(GtkWindow*, GdkEventFocus*, gpointer);
static gboolean mainwindow_expose_event(GtkWidget*, GdkEventExpose*, gpointer);
static void mainwindow_destroy(GtkWidget*, gpointer);


void build_mainwindow(void)
{
	mainwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	
	gtk_widget_set_app_paintable(mainwindow, TRUE);
	gtk_container_set_border_width(GTK_CONTAINER(mainwindow), 5);
	gtk_widget_set_size_request(mainwindow, conf_get_width(),
	                                        conf_get_height());
	gtk_window_set_decorated(GTK_WINDOW(mainwindow), FALSE);
	gtk_window_set_keep_above(GTK_WINDOW(mainwindow), TRUE);
	gtk_window_set_skip_taskbar_hint(GTK_WINDOW(mainwindow), TRUE);
	gtk_window_set_skip_pager_hint(GTK_WINDOW(mainwindow), TRUE);
	gtk_window_set_resizable(GTK_WINDOW(mainwindow), FALSE);
	gtk_window_stick(GTK_WINDOW(mainwindow));
	mainwindow_reset_position();
	
	build_term();
	gtk_container_add(GTK_CONTAINER(mainwindow), term);
	gtk_widget_show(GTK_WIDGET(term));
	
	g_signal_connect(G_OBJECT(mainwindow), "focus-out-event",
	                 G_CALLBACK(mainwindow_focus_out_event), NULL);
	g_signal_connect(G_OBJECT(mainwindow), "show",
	                 G_CALLBACK(mainwindow_show), NULL);
	g_signal_connect(G_OBJECT(mainwindow), "expose-event",
	                 G_CALLBACK(mainwindow_expose_event), NULL);
	g_signal_connect(G_OBJECT(mainwindow), "destroy",
	                 G_CALLBACK(mainwindow_destroy), NULL);
	
	if (conf_get_transparency() == TRANS_FAKE)
	{
		vte_terminal_set_background_saturation(VTE_TERMINAL(term),
		                                       1.0 - conf_get_opacity());
		vte_terminal_set_background_transparent(VTE_TERMINAL(term), TRUE);
	}
	else if (conf_get_transparency() == TRANS_BEST)
	{
		GdkScreen *screen;
		GdkColormap *colormap;

		screen = gdk_screen_get_default();
		colormap = gdk_screen_get_rgba_colormap(screen);
		
		if (colormap != NULL && gdk_screen_is_composited(screen))
		{
			gtk_widget_set_colormap(GTK_WIDGET(mainwindow), colormap);
			gdk_screen_set_default_colormap(screen, colormap);
		}
		else
		{
			fprintf(stderr, "error: no composite manager running");
			exit(1);
		}

		vte_terminal_set_background_transparent(VTE_TERMINAL(term), FALSE);
		vte_terminal_set_opacity(VTE_TERMINAL(term),
		                         conf_get_opacity() * 0xffff);
	}
	// "real" transparency is initalized after the first "show" window event

	init_key();
	grab_key();
	g_thread_create((GThreadFunc)wait_key, NULL, FALSE, NULL);
}


void mainwindow_present(void)
{
	if (GTK_WIDGET_VISIBLE(mainwindow))
	{
		// we should hide it, not present
		gtk_widget_hide(GTK_WIDGET(mainwindow));
		return;
	}

	gdk_threads_enter();
	gtk_window_present(GTK_WINDOW(mainwindow));
	gtk_window_stick(GTK_WINDOW(mainwindow));
	gtk_widget_grab_focus(GTK_WIDGET(term));
	gdk_flush();
	gdk_threads_leave();
	mainwindow_reset_position();
}


static void mainwindow_reset_position(void)
{
	int x, y;
	conf_get_position(&x, &y);
	gtk_window_move(GTK_WINDOW(mainwindow), x, y);
}


static void mainwindow_set_opacity(void)
{
	if (conf_get_opacity() >= 1.0f) return;

	unsigned int op = conf_get_opacity() * 0xffffffff;
	XChangeProperty(dpy, mw_xwin, opacityatom, XA_CARDINAL, 32, PropModeReplace, 
	                (unsigned char *) &op, 1L);
}                             


static void mainwindow_show(GtkWidget *widget, gpointer userdata)
{
	if (dpy != NULL) return;

	mw_xwin = GDK_WINDOW_XWINDOW(GTK_WIDGET(mainwindow)->window);
	dpy = GDK_WINDOW_XDISPLAY(GTK_WIDGET(mainwindow)->window);

	if (conf_get_transparency() == TRANS_REAL)
	{
		opacityatom = XInternAtom(dpy, "_NET_WM_WINDOW_OPACITY", False);
		mainwindow_set_opacity();
	}
}


extern Window root;
static void mainwindow_focus_out_event(GtkWindow* window,
                                       GdkEventFocus* event,
									   gpointer userdata)
{
	int revert;
	Window w;
	XGetInputFocus(dpy, &w, &revert);
	if (w == mw_xwin) return;

	// now we're sure focus wasn't lost just by pressing the shortcut key
	gtk_widget_hide(GTK_WIDGET(mainwindow));
}


static gboolean mainwindow_expose_event(GtkWidget *widget,
                                        GdkEventExpose *event,
                                        gpointer user_data)
{
	gint winw, winh;
	gtk_window_get_size(GTK_WINDOW(widget), &winw, &winh);
	
	gdk_draw_rectangle(widget->window,
	                   widget->style->black_gc,
	                   FALSE,
	                   0, 0, winw-1, winh-1);
	
	gdk_draw_rectangle(widget->window,
	                   widget->style->bg_gc[GTK_STATE_SELECTED],
	                   TRUE,
	                   1, 1, winw-2, winh-2);
	return FALSE;
}


static void mainwindow_destroy(GtkWidget *widget, gpointer user_data)
{
	gtk_main_quit();
}

