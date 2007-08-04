/*
 * mainwindow.c
 * This file is part of Stjerm
 *
 * Copyright (C) 2007 - Stjepan Glavina
 * Copyright (C) 2007 - Markus Groß
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


extern gboolean popupmenu_shown;
GtkWidget *mainwindow;
int activetab;
int tabcount;
GArray* tabs;
GtkHBox* tabbox;
GtkVBox* mainbox;

Window mw_xwin;
static Display *dpy = 0;
Atom opacityatom;
int screen_is_composited;

void build_mainwindow(void);
void mainwindow_toggle(void);
Tab* mainwindow_create_tab(void);
void mainwindow_close_tab(void);
int handle_x_error(Display *dpy, XErrorEvent *evt);

static void mainwindow_reset_position(void);
static void mainwindow_show(GtkWidget*, gpointer);
static void mainwindow_focus_out_event(GtkWindow*, GdkEventFocus*, gpointer);
static gboolean mainwindow_expose_event(GtkWidget*, GdkEventExpose*, gpointer);
static void mainwindow_destroy(GtkWidget*, gpointer);
static void mainwindow_toggle_tab(GtkToggleButton*, gpointer);
static void mainwindow_window_title_changed(VteTerminal *vteterminal, 
		                                    gpointer user_data);


void build_mainwindow(void)
{
	mainwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	
	gtk_widget_set_app_paintable(mainwindow, TRUE);
	gtk_widget_set_size_request(mainwindow, conf_get_width(), conf_get_height());
	gtk_window_set_decorated(GTK_WINDOW(mainwindow), FALSE);
	gtk_window_set_skip_taskbar_hint(GTK_WINDOW(mainwindow), TRUE);
	gtk_window_set_skip_pager_hint(GTK_WINDOW(mainwindow), TRUE);
	gtk_window_set_resizable(GTK_WINDOW(mainwindow), FALSE);
	mainwindow_reset_position();
	
	activetab = -1;
	tabs = g_array_new(TRUE, FALSE, sizeof(Tab*));
	tabcount = 0;
	mainbox = GTK_VBOX(gtk_vbox_new(FALSE, 0));
	tabbox = GTK_HBOX(gtk_hbox_new(FALSE, 0));
	
	if (conf_get_opacity() < 100)
	{
		GdkScreen *screen = gdk_screen_get_default();
		GdkColormap *colormap = gdk_screen_get_rgba_colormap(screen);
		screen_is_composited = (colormap != NULL && gdk_screen_is_composited(screen));
		
		if (screen_is_composited)
		{
			gtk_widget_set_colormap(GTK_WIDGET(mainwindow), colormap);
			gdk_screen_set_default_colormap(screen, colormap);
		}
	}
	
	mainwindow_create_tab();
	gtk_box_pack_end(GTK_BOX(mainbox), GTK_WIDGET(tabbox), FALSE, FALSE, 0);
	if (conf_get_show_tab())
		gtk_widget_show_all(GTK_WIDGET(tabbox));
	gtk_widget_show_all(GTK_WIDGET(mainbox));
	gtk_container_add(GTK_CONTAINER(mainwindow), GTK_WIDGET(mainbox));

	int border = conf_get_border();
	if (border == BORDER_THIN)
		gtk_container_set_border_width(GTK_CONTAINER(mainwindow), 1);
	else if (border == BORDER_THICK)
		gtk_container_set_border_width(GTK_CONTAINER(mainwindow), 5);
	if (border != BORDER_NONE)
		g_signal_connect(G_OBJECT(mainwindow), "expose-event",
		                 G_CALLBACK(mainwindow_expose_event), NULL);

	g_signal_connect(G_OBJECT(mainwindow), "focus-out-event",
	                 G_CALLBACK(mainwindow_focus_out_event), NULL);
	g_signal_connect(G_OBJECT(mainwindow), "show",
	                 G_CALLBACK(mainwindow_show), NULL);
	g_signal_connect(G_OBJECT(mainwindow), "destroy",
	                 G_CALLBACK(mainwindow_destroy), NULL);

	if (!conf_get_show_tab())
		gtk_widget_hide(GTK_WIDGET(tabbox));
	
	XSetErrorHandler(handle_x_error);
	init_key();
	grab_key();
	g_thread_create((GThreadFunc)wait_key, NULL, FALSE, NULL);
}

Tab* mainwindow_create_tab(void)
{
	GtkWidget* tmp_term = build_term();
	GtkVScrollbar *sbar = NULL;
	GtkHBox *tmp_box = GTK_HBOX(gtk_hbox_new(FALSE, 0));

	if (conf_get_scrollbar() == -1)
		gtk_box_pack_start(GTK_BOX(tmp_box), tmp_term, TRUE, TRUE, 0);
	else if (conf_get_scrollbar() == POS_LEFT)
	{
		sbar = GTK_VSCROLLBAR(gtk_vscrollbar_new(vte_terminal_get_adjustment(
				              VTE_TERMINAL(tmp_term))));
		gtk_box_pack_start(GTK_BOX(tmp_box), GTK_WIDGET(sbar), FALSE, FALSE, 0);
		gtk_box_pack_end(GTK_BOX(tmp_box), GTK_WIDGET(tmp_term), TRUE, TRUE, 0);
	} else // (conf_get_scrollbar() == POS_RIGHT)
	{
		sbar = GTK_VSCROLLBAR(gtk_vscrollbar_new(vte_terminal_get_adjustment(
				              VTE_TERMINAL(tmp_term))));
		gtk_box_pack_start(GTK_BOX(tmp_box), GTK_WIDGET(tmp_term), TRUE, TRUE, 0);
		gtk_box_pack_end(GTK_BOX(tmp_box), GTK_WIDGET(sbar), FALSE, FALSE, 0);
	}

	gtk_widget_show_all(GTK_WIDGET(tmp_box));
	gtk_box_pack_start(GTK_BOX(mainbox), GTK_WIDGET(tmp_box), TRUE, TRUE, 0);

	char buffer [100];
	sprintf(buffer, "%s %d", conf_get_term_name(), activetab + 1);
	GtkToggleButton* tmp_tab = GTK_TOGGLE_BUTTON(
			                   gtk_toggle_button_new_with_label(buffer));
	gulong handler = g_signal_connect(G_OBJECT(tmp_tab), "toggled",
			                          G_CALLBACK(mainwindow_toggle_tab), NULL);

	gtk_widget_show_all(GTK_WIDGET(tmp_tab));
	gtk_box_pack_start(GTK_BOX(tabbox), GTK_WIDGET(tmp_tab), FALSE, FALSE, 0);

	if (conf_get_opacity() < 100)
	{
		vte_terminal_set_background_saturation(VTE_TERMINAL(tmp_term),
			                                   1.0 - conf_get_opacity()/100);
		vte_terminal_set_background_transparent(VTE_TERMINAL(tmp_term), TRUE);
	}
	
	g_signal_connect(G_OBJECT(tmp_term), "window-title-changed",
			         G_CALLBACK(mainwindow_window_title_changed), NULL);
	
	Tab *t = (Tab *) malloc(sizeof(Tab));
	t->term = tmp_term;
	t->tab = tmp_tab;
	t->bar = sbar;
	t->box = tmp_box;
	t->handler_id = handler;
	g_array_append_val(tabs, t);
	tabcount++;
	
	if (activetab >= 0)
		gtk_widget_show(GTK_WIDGET(tabbox));
	
	g_signal_emit_by_name(tmp_tab, "toggled");
	return t;
}

void mainwindow_close_tab(void)
{
	if (activetab >= 0)
	{
		Tab *t = g_array_index(tabs, Tab*, activetab);
		g_array_remove_index(tabs, activetab);
		tabcount--;
		if (tabcount == 0)
			gtk_widget_destroy(GTK_WIDGET(mainwindow));
		else
		{
			gtk_widget_destroy(GTK_WIDGET(t->box));
			gtk_widget_destroy(GTK_WIDGET(t->tab));

			if (g_array_index(tabs, Tab*, activetab) == 0)
				activetab--;

			gtk_widget_show(GTK_WIDGET(g_array_index(tabs, Tab*, activetab)->box));
			
			if (tabcount == 1 && conf_get_show_tab() == 0)
				gtk_widget_hide(GTK_WIDGET(tabbox));
			
			g_signal_emit_by_name(g_array_index(tabs, Tab*, activetab)->tab, "toggled");
		}
	}
}


void mainwindow_toggle(void)
{
	if (GTK_WIDGET_VISIBLE(mainwindow))
	{
		gdk_threads_enter();
		gtk_widget_hide(GTK_WIDGET(mainwindow));
		gdk_flush();
		gdk_threads_leave();
		return;
	}

	gdk_threads_enter();
	if (gtk_window_is_active(GTK_WINDOW(mainwindow)) == FALSE)
		gtk_window_present(GTK_WINDOW(mainwindow));
	else
		gtk_widget_show(mainwindow);
	gtk_window_stick(GTK_WINDOW(mainwindow));
	gtk_window_set_keep_above(GTK_WINDOW(mainwindow), TRUE);
	mainwindow_reset_position();
	gdk_window_focus(mainwindow->window, gtk_get_current_event_time());
	gdk_flush();
	gdk_threads_leave();
}


static void mainwindow_reset_position(void)
{
	int x, y;
	conf_get_position(&x, &y);
	gtk_window_move(GTK_WINDOW(mainwindow), x, y);
}


static void mainwindow_show(GtkWidget *widget, gpointer userdata)
{
	if (dpy != NULL) return;

	mw_xwin = GDK_WINDOW_XWINDOW(GTK_WIDGET(mainwindow)->window);
	dpy = GDK_DISPLAY_XDISPLAY(gdk_display_get_default());
}

static void mainwindow_focus_out_event(GtkWindow* window,
                                       GdkEventFocus* event,
									   gpointer userdata)
{
	int revert;
	Window w;
	XGetInputFocus(dpy, &w, &revert);
	if (w == mw_xwin) return;
	// focus wasn't lost just by pressing the shortcut key
	
	if (popupmenu_shown == TRUE) return;
	// focus wasn't lost by popping up popupmenu

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
		               FALSE, 0, 0, winw-1, winh-1);

	if (conf_get_border() == BORDER_THIN) return FALSE;

	gdk_draw_rectangle(widget->window,
	                   widget->style->bg_gc[GTK_STATE_SELECTED],
	                   TRUE, 1, 1, winw-2, winh-2);
	
	gdk_draw_rectangle(widget->window,
	                   widget->style->bg_gc[GTK_STATE_NORMAL],
	                   TRUE, 5, 5, winw-10, winh-10);

	return FALSE;
}


static void mainwindow_destroy(GtkWidget *widget, gpointer user_data)
{
	g_array_free(tabs, TRUE);
	gtk_main_quit();
}

static void mainwindow_toggle_tab(GtkToggleButton *togglebutton,
		gpointer user_data)
{
	if (togglebutton != 0)
	{
		Tab *t = 0;
		int i = -1;
		do
		{
			i++;
			t = g_array_index(tabs, Tab*, i);
		} while (t != 0 && t->tab != togglebutton);

		if (i == activetab)
		{
			gtk_toggle_button_set_active(togglebutton, FALSE);
			gtk_widget_grab_focus(g_array_index(tabs, Tab*, activetab)->term);
			return;
		}

		if (activetab >= 0)
		{
			Tab *tmp = 0;
			int k = -1;
			do
			{
				k++;
				tmp = g_array_index(tabs, Tab*, k);
				if (tmp != 0)
				{
					if (k != i)
					{
						g_signal_handler_disconnect(G_OBJECT(tmp->tab),
								(tmp->handler_id));
						gtk_toggle_button_set_active(tmp->tab, TRUE);
						tmp->handler_id = g_signal_connect(G_OBJECT(tmp->tab),
								"toggled", G_CALLBACK(mainwindow_toggle_tab),
								NULL);
					} else
						gtk_toggle_button_set_active(tmp->tab, FALSE);
				}
			} while (tmp != 0);
		}

		if (t != 0)
		{
			if (activetab >= 0)
				gtk_widget_hide(GTK_WIDGET(g_array_index(tabs, Tab*, 
						        activetab)->box));
			gtk_widget_show(GTK_WIDGET(t->box));
			if (conf_get_opacity() < 100 && screen_is_composited)
			{
				vte_terminal_set_background_transparent(VTE_TERMINAL(t->term), FALSE);
				vte_terminal_set_opacity(VTE_TERMINAL(t->term),
						                 conf_get_opacity()/100 * 0xffff);
			}
			gtk_widget_grab_focus(t->term);
			activetab = i;
		}
	}
}

static void mainwindow_window_title_changed(VteTerminal *vteterminal, 
		                                    gpointer user_data)
{
	if (vteterminal != 0)
	{
		Tab *t = 0;
		int i = -1;
		do
		{
			i++;
			t = g_array_index(tabs, Tab*, i);
		} while (t != 0 && VTE_TERMINAL(t->term) != vteterminal);
		
		gtk_button_set_label(GTK_BUTTON(t->tab),
				vte_terminal_get_window_title(vteterminal));
	}
}

int handle_x_error(Display *dpy, XErrorEvent *evt)
{
	if (evt->error_code == BadAccess || evt->error_code == BadValue ||
	    evt->error_code == BadWindow)
	{
		fprintf(stderr, "error: unable to grab key, is stjerm is already running with the same key?\n");
		exit(1);
	}
	return 0;
}
