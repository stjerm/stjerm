/*
 * term.c
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


#include <gtk/gtk.h>
#include <vte/vte.h>
#include "stjerm.h"


extern GtkWidget *mainwindow;
extern GtkWidget *popupmenu;
extern gboolean popupmenu_shown;
GtkWidget *term;

void build_term(void);
static void term_connect_signals(GtkWidget*);
static gboolean term_button_press(GtkWidget*, GdkEventButton*, gpointer);
static void term_eof_or_child_exited(VteTerminal*, gpointer);
static void term_app_request(VteTerminal*, gpointer);
static void term_app_request_resize_move(VteTerminal*, guint, guint, gpointer);


void build_term(void)
{
	if (!popupmenu) build_popupmenu();
	
	term = vte_terminal_new();
	
	vte_terminal_fork_command(VTE_TERMINAL(term), "/bin/bash", NULL, NULL,
	                          "", TRUE, TRUE, TRUE);
	
	GdkColor fore, back;
	fore = conf_get_fg();
	back = conf_get_bg();
	vte_terminal_set_colors(VTE_TERMINAL(term), &fore, &back, NULL, 0);

	vte_terminal_set_scroll_on_output(VTE_TERMINAL(term), TRUE);
	vte_terminal_set_scroll_on_keystroke(VTE_TERMINAL(term), TRUE);
	vte_terminal_set_font_from_string(VTE_TERMINAL(term), conf_get_font());
	
	term_connect_signals(term);
}


static void term_connect_signals(GtkWidget *term)
{
	g_signal_connect_swapped(G_OBJECT(term), "button-press-event",
	                         G_CALLBACK(term_button_press), NULL);
	
	g_signal_connect (G_OBJECT(term), "eof",
                      G_CALLBACK(term_eof_or_child_exited), NULL);
    g_signal_connect (G_OBJECT(term), "child-exited",
                      G_CALLBACK(term_eof_or_child_exited), NULL);
	
	g_signal_connect(G_OBJECT(term), "iconify-window",
	                 G_CALLBACK(term_app_request), (gpointer)TERM_ICONIFY_WINDOW);
	g_signal_connect(G_OBJECT(term), "deiconify-window",
	                 G_CALLBACK(term_app_request), (gpointer)TERM_DEICONIFY_WINDOW);
	g_signal_connect(G_OBJECT(term), "raise-window",
	                 G_CALLBACK(term_app_request), (gpointer)TERM_RAISE_WINDOW);
	g_signal_connect(G_OBJECT(term), "lower-window",
	                 G_CALLBACK(term_app_request), (gpointer)TERM_LOWER_WINDOW);
	g_signal_connect(G_OBJECT(term), "maximize-window",
	                 G_CALLBACK(term_app_request), (gpointer)TERM_MAXIMIZE_WINDOW);
	g_signal_connect(G_OBJECT(term), "restore-window",
	                 G_CALLBACK(term_app_request), (gpointer)TERM_RESTORE_WINDOW);
	g_signal_connect(G_OBJECT(term), "refresh-window",
	                 G_CALLBACK(term_app_request), (gpointer)TERM_REFRESH_WINDOW);
	g_signal_connect(G_OBJECT(term), "resize-window",
	                 G_CALLBACK(term_app_request_resize_move), (gpointer)TERM_RESIZE_WINDOW);
	g_signal_connect(G_OBJECT(term), "move-window",
	                 G_CALLBACK(term_app_request_resize_move), (gpointer)TERM_MOVE_WINDOW);
}


static gboolean term_button_press(GtkWidget *widget, GdkEventButton *event,
                                  gpointer user_data)
{
	if (event->type == GDK_BUTTON_PRESS && event->button == 3)
	{
		popupmenu_shown = TRUE;
		gtk_menu_popup(GTK_MENU(popupmenu), NULL, NULL, NULL, NULL,
		               event->button, event->time);
	}
	
	return FALSE;
}


static void term_eof_or_child_exited(VteTerminal *term, gpointer user_data)
{
	vte_terminal_reset(VTE_TERMINAL(term), FALSE, TRUE);
	vte_terminal_fork_command(VTE_TERMINAL(term), "/bin/bash", NULL, NULL,
	                          "", TRUE, TRUE, TRUE);

	gtk_widget_hide(GTK_WIDGET(mainwindow));
}


static void term_app_request(VteTerminal *term, gpointer user_data)
{
	int event = GPOINTER_TO_INT(user_data);
	
	if (event == TERM_ICONIFY_WINDOW)
	{
		gdk_window_iconify(GTK_WIDGET(mainwindow)->window);
	}
	if (event == TERM_DEICONIFY_WINDOW)
	{
		gdk_window_deiconify(GTK_WIDGET(mainwindow)->window);
	}
	if (event == TERM_RAISE_WINDOW)
	{
		gdk_window_raise(GTK_WIDGET(mainwindow)->window);
	}
	if (event == TERM_LOWER_WINDOW)
	{
		gdk_window_lower(GTK_WIDGET(mainwindow)->window);
	}
	if (event == TERM_MAXIMIZE_WINDOW)
	{
		gdk_window_maximize(GTK_WIDGET(mainwindow)->window);
	}
	if (event == TERM_RESTORE_WINDOW)
	{
		gdk_window_unmaximize(GTK_WIDGET(mainwindow)->window);
	}
	if (event == TERM_REFRESH_WINDOW)
	{
		GdkRectangle rect;
		rect.x = rect.y = 0;
		rect.width = mainwindow->allocation.width;
		rect.height = mainwindow->allocation.height;
		gdk_window_invalidate_rect(GTK_WIDGET(mainwindow)->window, &rect, TRUE);
	}
}


static void term_app_request_resize_move(VteTerminal *term, guint x, guint y,
                                         gpointer user_data)
{
	int event = GPOINTER_TO_INT(user_data);
	
	if (event == TERM_RESIZE_WINDOW)
	{
		gint owidth, oheight, xpad, ypad;
		
		gtk_window_get_size(GTK_WINDOW(mainwindow), &owidth, &oheight);
		owidth -= term->char_width * term->column_count;
		oheight -= term->char_height * term->row_count;
		
		vte_terminal_get_padding(term, &xpad, &ypad);
		owidth -= xpad;
		oheight -= ypad;
		gtk_window_resize(GTK_WINDOW(mainwindow), x+owidth, y+oheight);
	}
	if (event == TERM_MOVE_WINDOW)
	{
		gdk_window_move(GTK_WIDGET(mainwindow)->window, x, y);
	}
}

