/*
 * term.c
 * This file is part of Stjerm
 *
 * Copyright (C) 2007-2010 - Kristopher Wilson, Stjepan Glavina and Markus Groß
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
extern GtkNotebook* tabbar;
extern GtkWidget *popupmenu;
extern gboolean popupmenu_shown;

GtkWidget* build_term(void);
static void term_connect_signals(GtkWidget*);
static gboolean term_button_press(GtkWidget*, GdkEventButton*, gpointer);
static void term_eof_or_child_exited(VteTerminal*, gpointer);
static void term_app_request(VteTerminal*, gpointer);
//static void term_app_request_resize_move(VteTerminal*, guint, guint, gpointer);
static void term_fork_command(VteTerminal*, char*);

GtkWidget* build_term(void)
{
    if(!popupmenu)
        build_popupmenu();

    GtkWidget* term = vte_terminal_new();

    term_fork_command(VTE_TERMINAL(term), conf_get_shell());
    
    if (conf_get_bg_image() != NULL)
        vte_terminal_set_background_image_file(VTE_TERMINAL(term), conf_get_bg_image());
    
    GdkColor fore, back;
    fore = conf_get_fg();
    back = conf_get_bg();
    GdkColor *palette = conf_get_color_palette();
    
    if(palette == NULL)
        vte_terminal_set_colors(VTE_TERMINAL(term), &fore, &back, NULL, 0);
    else
        vte_terminal_set_colors(VTE_TERMINAL(term), &fore, &back, palette, 16);
    
    #if VTE_CHECK_VERSION(0,25,0)
    vte_pty_set_term(vte_terminal_get_pty_object(VTE_TERMINAL(term)), conf_get_emulation());
    #endif
    
    vte_terminal_set_background_tint_color(VTE_TERMINAL(term), &back);

    vte_terminal_set_allow_bold(VTE_TERMINAL(term), conf_get_allow_bold());
    vte_terminal_set_scroll_on_output(VTE_TERMINAL(term), conf_get_scroll_on_output());
    vte_terminal_set_scroll_on_keystroke(VTE_TERMINAL(term), TRUE);
    vte_terminal_set_font_from_string(VTE_TERMINAL(term), conf_get_font());
    vte_terminal_set_scrollback_lines(VTE_TERMINAL(term), conf_get_lines());
    vte_terminal_set_backspace_binding(VTE_TERMINAL(term), 
        VTE_ERASE_ASCII_DELETE);
    vte_terminal_set_word_chars(VTE_TERMINAL(term),
        "-A-Za-z0-9_$.+!*(),;:@&=?/~#%");

    term_connect_signals(term);

    return term;
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
    /*g_signal_connect(G_OBJECT(term), "resize-window",
        G_CALLBACK(term_app_request_resize_move), (gpointer)TERM_RESIZE_WINDOW);
    g_signal_connect(G_OBJECT(term), "move-window",
        G_CALLBACK(term_app_request_resize_move), (gpointer)TERM_MOVE_WINDOW);*/
}

static CursorMatch term_cursor_match_pattern(GdkEventButton* event)
{
    int tag = -1;
    
    gint page = gtk_notebook_get_current_page(GTK_NOTEBOOK(tabbar));
    GList *children = gtk_container_get_children(GTK_CONTAINER(gtk_notebook_get_nth_page(GTK_NOTEBOOK(tabbar), page)));
    GtkWidget *vte = GTK_WIDGET(children->data);

    glong column = ((glong) (event->x) / vte_terminal_get_char_width(VTE_TERMINAL(vte)));
    glong row = ((glong) (event->y) / vte_terminal_get_char_height(VTE_TERMINAL(vte)));
    
    char* current_match = vte_terminal_match_check(VTE_TERMINAL(vte), column, row, &tag);

    CursorMatch match;

    match.tag = tag;
    match.text = current_match;
    
    return match;
}

static gboolean term_button_press(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
    if(event->type == GDK_BUTTON_PRESS && event->button == 3)
    {
        popupmenu_shown = TRUE;
        gtk_menu_popup(GTK_MENU(popupmenu), NULL, NULL, NULL, NULL, event->button, event->time);
    }
    else if (event->button == 1)
    {
        GError *error = NULL; 
        CursorMatch match = term_cursor_match_pattern(event);
        
        if(match.tag >= 0)
        {
            gtk_show_uri(NULL, match.text, event->time, &error);
        }
    }

    return FALSE;
}



static void term_eof_or_child_exited(VteTerminal *term, gpointer user_data)
{
    if(vte_terminal_get_child_exit_status(term) != 0)
    {
        /* restart the terminal if it crashed */
        vte_terminal_reset(VTE_TERMINAL(term), FALSE, TRUE);
        term_fork_command(VTE_TERMINAL(term), conf_get_shell());
    }
    else
    {
        /* else close the tab */
        mainwindow_close_tab(GTK_WIDGET(term));
    }
}

static void term_app_request(VteTerminal *term, gpointer user_data)
{
    int event = GPOINTER_TO_INT(user_data);

    if(event == TERM_ICONIFY_WINDOW)
        gdk_window_iconify(GTK_WIDGET(mainwindow)->window);
    
    if(event == TERM_DEICONIFY_WINDOW)
        gdk_window_deiconify(GTK_WIDGET(mainwindow)->window);
    
    if(event == TERM_RAISE_WINDOW)
        gdk_window_raise(GTK_WIDGET(mainwindow)->window);
    
    if(event == TERM_LOWER_WINDOW)
        gdk_window_lower(GTK_WIDGET(mainwindow)->window);
    
    if(event == TERM_MAXIMIZE_WINDOW)
        gdk_window_maximize(GTK_WIDGET(mainwindow)->window);
    
    if(event == TERM_RESTORE_WINDOW)
        gdk_window_unmaximize(GTK_WIDGET(mainwindow)->window);
    
    if(event == TERM_REFRESH_WINDOW)
    {
        GdkRectangle rect;
     
        rect.x = rect.y = 0;
        rect.width = mainwindow->allocation.width;
        rect.height = mainwindow->allocation.height;
        gdk_window_invalidate_rect(GTK_WIDGET(mainwindow)->window, &rect, TRUE);
    }
}

/*static void term_app_request_resize_move(VteTerminal *term, guint x, guint y,
    gpointer user_data)
{
    // FIXME I can't get this function to call. It may be unused?
    
    int event = GPOINTER_TO_INT(user_data);

    if(event == TERM_RESIZE_WINDOW)
    {
        gint owidth, oheight, xpad, ypad;

        gtk_window_get_size(GTK_WINDOW(mainwindow), &owidth, &oheight);
        owidth -= term->char_width * term->column_count;
        oheight -= term->char_height * term->row_count;
        
        vte_terminal_get_padding(term, &xpad, &ypad);=
        
        owidth -= xpad;
        oheight -= ypad;
        gtk_window_resize(GTK_WINDOW(mainwindow), x+owidth, y+oheight);
    }
    
    if(event == TERM_MOVE_WINDOW)
        gdk_window_move(GTK_WIDGET(mainwindow)->window, x, y);
}*/

static void term_fork_command(VteTerminal *term, char *cmd)
{
    #if VTE_CHECK_VERSION(0,25,0)
    char **argv = NULL;
    
    g_shell_parse_argv(cmd, NULL, &argv, NULL);
    
    vte_terminal_fork_command_full(term, VTE_PTY_DEFAULT, NULL, argv, NULL, 
        G_SPAWN_CHILD_INHERITS_STDIN|G_SPAWN_SEARCH_PATH|G_SPAWN_FILE_AND_ARGV_ZERO,
        NULL, NULL, NULL, NULL);
    #else
    vte_terminal_fork_command(term, cmd, NULL, NULL, "", TRUE, TRUE, TRUE);
    #endif
}

