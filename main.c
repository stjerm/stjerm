/*
 * main.c
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


#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <vte/vte.h>
#include "stjerm.h"


GtkWidget *stWindow;
GtkWidget *termBook;


void centerWindow(GtkWindow* window)
{
	int swidth = gdk_screen_get_width(gdk_screen_get_default());
	gtk_window_move(window, (swidth - 800) / 2, 0);
}


void setOpacity(GtkWindow* window)
{
	Window xwin = GDK_WINDOW_XWINDOW(GTK_WIDGET(window)->window);
	Display *dpy = GDK_WINDOW_XDISPLAY(GTK_WIDGET(window)->window);
	Atom opacityatom = XInternAtom(dpy, "_NET_WM_WINDOW_OPACITY", False);
	unsigned int opacity = 0.9 * 0xffffffff;
	XChangeProperty(dpy, xwin, opacityatom, 
	                XA_CARDINAL, 32, PropModeReplace, 
	                (unsigned char *) &opacity, 1L);
}


gboolean stWindow_expose_event(GtkWidget *widget, GdkEventExpose *event,
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


gboolean stWindow_show(GtkWidget *widget, gpointer user_data)
{
	GList *children;
	GtkWidget *box;
	gint currPage;
	
	currPage = gtk_notebook_get_current_page(GTK_NOTEBOOK(termBook));
	box = gtk_notebook_get_nth_page(GTK_NOTEBOOK(termBook), currPage);
	children = gtk_container_children(GTK_CONTAINER(box));
	gtk_widget_grab_focus(GTK_WIDGET(g_list_nth_data(children, 0)));

	return FALSE;
}


int main(int argc, char *argv[])
{
	gtk_init(&argc, &argv);
	
	stWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	
	gtk_widget_set_app_paintable(stWindow, TRUE);
	gtk_container_set_border_width(GTK_CONTAINER(stWindow), 5);
	gtk_widget_set_size_request(stWindow, 800, 400);
	gtk_window_set_decorated(GTK_WINDOW(stWindow), FALSE);
	gtk_window_set_skip_taskbar_hint(GTK_WINDOW(stWindow), TRUE);
	gtk_window_set_skip_pager_hint(GTK_WINDOW(stWindow), TRUE);
	gtk_window_stick(GTK_WINDOW(stWindow));
	gtk_window_set_keep_above(GTK_WINDOW(stWindow), TRUE);
	centerWindow(GTK_WINDOW(stWindow));
	
	termBook = gtk_notebook_new();
	gtk_container_add(GTK_CONTAINER(stWindow), termBook);
	openTab();
	
	g_signal_connect(G_OBJECT(stWindow), "expose-event",
	                 G_CALLBACK(stWindow_expose_event), NULL);
	g_signal_connect(G_OBJECT(stWindow), "show",
	                 G_CALLBACK(stWindow_show), termBook);
	
	gtk_widget_show_all(stWindow);
	setOpacity(GTK_WINDOW(stWindow));
	gtk_main();
	
    return 0;
}
