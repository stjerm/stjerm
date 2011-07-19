/*
 * mainwindow.c
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

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <gdk/gdkkeysyms.h>
#include <vte/vte.h>
#include <stdlib.h>
#include "stjerm.h"

extern GtkWidget *popupmenu;
extern GtkWidget *close_tab;
extern gboolean popupmenu_shown;

GtkWidget *mainwindow;
int activetab;
int tabcount;
GArray* tabs;
GtkNotebook* tabbar;

Window mw_xwin;
static Display *dpy = 0;
Atom opacityatom;
gboolean screen_is_composited;
gboolean fullscreen;
gboolean toggled;



void build_mainwindow(void);
void mainwindow_toggle(int sig);
void mainwindow_create_tab(void);
void mainwindow_close_tab(GtkWidget *term);
void mainwindow_toggle_fullscreen(void);
int handle_x_error(Display *dpy, XErrorEvent *evt);

static GRegex **uri_regex;
static guint uri_regex_count;

static void mainwindow_reset_position(void);
static void mainwindow_focus_terminal(void);
static void mainwindow_show(GtkWidget*, gpointer);
static void mainwindow_focus_out_event(GtkWindow*, GdkEventFocus*, gpointer);
static gboolean mainwindow_expose_event(GtkWidget*, GdkEventExpose*, gpointer);
static void mainwindow_destroy(GtkWidget*, gpointer);
static void mainwindow_window_title_changed(VteTerminal *vteterminal,
    gpointer user_data);
static void mainwindow_switch_tab(GtkNotebook *notebook, GtkNotebookPage *page,
    guint page_num, gpointer user_data);
static void mainwindow_goto_tab(gint i);
static void mainwindow_next_tab(GtkWidget *widget, gpointer user_data);
static void mainwindow_prev_tab(GtkWidget *widget, gpointer user_data);
static void mainwindow_new_tab(GtkWidget *widget, gpointer user_data);
static void mainwindow_delete_tab(GtkWidget *widget, gpointer user_data);
static void mainwindow_copy(GtkWidget *widget, gpointer user_data);
static void mainwindow_paste(GtkWidget *widget, gpointer user_data);

static gint mainwindow_tab_at_xy(GtkNotebook *notebook, gint abs_x, gint abs_y);
static void mainwindow_notebook_clicked(GtkWidget *widget, GdkEventButton *event, gpointer func_data);

static void mainwindow_tab_moved(GtkWidget *notebook, GtkWidget *page, guint page_num, gpointer user_data);

void build_mainwindow(void)
{
    guint ic;
  
    uri_regex_count = G_N_ELEMENTS(uri_patterns);
    uri_regex = g_new0(GRegex*, uri_regex_count);

    for(ic = 0; ic < uri_regex_count; ++ic)
    {
        GError *error = NULL;
        
        uri_regex[ic] = g_regex_new(uri_patterns[ic].pattern,
            uri_patterns[ic].flags | G_REGEX_OPTIMIZE, 0, &error);
        
        if(error)
        {
            g_message("%s", error->message);
            g_error_free (error);
        }
    }

    mainwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_widget_set_app_paintable(mainwindow, TRUE);
    gtk_widget_set_size_request(mainwindow, conf_get_width(), conf_get_height());
    gtk_window_set_decorated(GTK_WINDOW(mainwindow), FALSE);
    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(mainwindow), TRUE);
    gtk_window_set_skip_pager_hint(GTK_WINDOW(mainwindow), TRUE);
    gtk_window_set_resizable(GTK_WINDOW(mainwindow), TRUE);
    mainwindow_reset_position();

    fullscreen = FALSE;
    toggled = FALSE;

    GtkAccelGroup* accel_group;
    GClosure *new_tab, *delete_tab, *next_tab, *prev_tab, *delete_all,
        *maximize, *copy, *paste;

    GClosure *goto_tab_closure[10];
    long i;

    accel_group = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(mainwindow), accel_group);

    maximize = g_cclosure_new_swap(G_CALLBACK(mainwindow_toggle_fullscreen),
        NULL, NULL);
    gtk_accel_group_connect(accel_group, GDK_F11, 0,
        GTK_ACCEL_VISIBLE, maximize);

    new_tab = g_cclosure_new_swap(G_CALLBACK(mainwindow_new_tab), 
        NULL, NULL);
    gtk_accel_group_connect(accel_group, 't', conf_get_key_mod(),
        GTK_ACCEL_VISIBLE, new_tab);

    delete_tab = g_cclosure_new_swap(G_CALLBACK(mainwindow_delete_tab), 
        NULL, NULL);
    gtk_accel_group_connect(accel_group, 'w', conf_get_key_mod(),
        GTK_ACCEL_VISIBLE, delete_tab);

    next_tab = g_cclosure_new_swap(G_CALLBACK(mainwindow_next_tab), 
        NULL, NULL);
    gtk_accel_group_connect(accel_group, GDK_Page_Up, conf_get_key_mod(),
        GTK_ACCEL_VISIBLE, next_tab);

    prev_tab = g_cclosure_new_swap(G_CALLBACK(mainwindow_prev_tab), 
        NULL, NULL);
    gtk_accel_group_connect(accel_group, GDK_Page_Down, conf_get_key_mod(),
        GTK_ACCEL_VISIBLE, prev_tab);

    delete_all = g_cclosure_new_swap(G_CALLBACK(mainwindow_destroy), 
        NULL, NULL);
    gtk_accel_group_connect(accel_group, 'q', conf_get_key_mod(),
        GTK_ACCEL_VISIBLE, delete_all);

    /* tab hotkeys, inspired by Tilda -- thanks to castorinop for the patch */
    for(i = 0; i < 10; i++)
    {
        goto_tab_closure[i] = g_cclosure_new_swap(G_CALLBACK(mainwindow_goto_tab),
            (gpointer) i, NULL);
        gtk_accel_group_connect(accel_group, '0' + ((i+1)%10), GDK_MOD1_MASK,
            GTK_ACCEL_VISIBLE, goto_tab_closure[i]);
    }

    copy = g_cclosure_new_swap(G_CALLBACK(mainwindow_copy), 
        NULL, NULL);
    gtk_accel_group_connect(accel_group, 'c', conf_get_key_mod(),
        GTK_ACCEL_VISIBLE, copy);

    paste = g_cclosure_new_swap(G_CALLBACK(mainwindow_paste), 
        NULL, NULL);
    gtk_accel_group_connect(accel_group, 'v', conf_get_key_mod(),
        GTK_ACCEL_VISIBLE, paste);

    activetab = -1;
    tabs = g_array_new(TRUE, FALSE, sizeof(VteTerminal*));
    tabcount = 0;
    GtkVBox* mainbox = GTK_VBOX(gtk_vbox_new(FALSE, 0));
    
    tabbar = GTK_NOTEBOOK(gtk_notebook_new());
    
    g_signal_connect(G_OBJECT(tabbar), "switch-page",
        G_CALLBACK(mainwindow_switch_tab), NULL);

    g_signal_connect(G_OBJECT(tabbar), "page-reordered", 
        G_CALLBACK(mainwindow_tab_moved), NULL);

    if(conf_get_opacity() < 100)
    {
        GdkScreen *screen = gdk_screen_get_default();
        GdkColormap *colormap = gdk_screen_get_rgba_colormap(screen);
        screen_is_composited = (colormap != NULL
            && gdk_screen_is_composited(screen));

        if(screen_is_composited)
        {
            gtk_widget_set_colormap(GTK_WIDGET(mainwindow), colormap);
            gdk_screen_set_default_colormap(screen, colormap);
        }
    }

    gtk_box_pack_start(GTK_BOX(mainbox), GTK_WIDGET(tabbar), TRUE, TRUE, 0);

    mainwindow_create_tab();

    gtk_widget_show_all(GTK_WIDGET(mainbox));
    gtk_container_add(GTK_CONTAINER(mainwindow), GTK_WIDGET(mainbox));

    int border = conf_get_border();
    if(border == BORDER_THIN)
        gtk_container_set_border_width(GTK_CONTAINER(mainwindow), 1);
    else if(border == BORDER_THICK)
        gtk_container_set_border_width(GTK_CONTAINER(mainwindow), 5);
    if(border != BORDER_NONE)
        g_signal_connect(G_OBJECT(mainwindow), "expose-event",
            G_CALLBACK(mainwindow_expose_event), NULL);

    if(conf_get_auto_hide())
        g_signal_connect(G_OBJECT(mainwindow), "focus-out-event",
            G_CALLBACK(mainwindow_focus_out_event), NULL);
    g_signal_connect(G_OBJECT(mainwindow), "show", G_CALLBACK(mainwindow_show), 
        NULL);
    g_signal_connect(G_OBJECT(mainwindow), "destroy",
        G_CALLBACK(mainwindow_destroy), NULL);

    g_signal_connect_after(G_OBJECT(tabbar), "button_press_event", 
        G_CALLBACK(mainwindow_notebook_clicked), NULL);

    gtk_notebook_set_show_border(tabbar, FALSE);
    gtk_notebook_set_scrollable(tabbar, TRUE);
    if (conf_get_show_tab() == TABS_ONE|| conf_get_show_tab() == TABS_NEVER)
        gtk_notebook_set_show_tabs(tabbar, FALSE);
    gtk_notebook_set_tab_pos(tabbar, conf_get_tab_pos());
    gtk_notebook_set_homogeneous_tabs(tabbar, FALSE);

    XSetErrorHandler(handle_x_error);
    init_key();
    grab_key();
    g_thread_create((GThreadFunc)wait_key, NULL, FALSE, NULL);
}

void mainwindow_notebook_clicked(GtkWidget *widget, GdkEventButton *event, gpointer func_data)
{
    gint tabclicked = mainwindow_tab_at_xy(GTK_NOTEBOOK(widget), event->x, event->y);
    
    if(tabclicked > -1)
        gtk_notebook_set_current_page(GTK_NOTEBOOK(widget), tabclicked);
    else if(event->type == GDK_2BUTTON_PRESS)
        mainwindow_create_tab();
    
    if(event->button == 3)
    {
        popupmenu_shown = TRUE;
        gtk_menu_popup(GTK_MENU(popupmenu), NULL, NULL, NULL, NULL, event->button, event->time);
    }
}

/* This code adapted from gnome-terminal */
static gint mainwindow_tab_at_xy(GtkNotebook *notebook, gint x, gint y)
{
    GtkPositionType tab_pos;
    int page_num = 0;
    GtkWidget *page;
    
    tab_pos = gtk_notebook_get_tab_pos(notebook);
    
    if(notebook->first_tab == NULL)
        return -1;

    while((page = gtk_notebook_get_nth_page(notebook, page_num)))
    {
        GtkWidget *screen;
        gint max_x, max_y;

        screen = gtk_notebook_get_tab_label(notebook, page);
        g_return_val_if_fail(screen != NULL, -1);

        if(!GTK_WIDGET_MAPPED(GTK_WIDGET(screen)))
        {
            page_num++;
            continue;
        }

        max_x = screen->allocation.x + screen->allocation.width;
        max_y = screen->allocation.y + screen->allocation.height;

        if(((tab_pos == GTK_POS_TOP) || (tab_pos == GTK_POS_BOTTOM)) && (x <= max_x))
            return page_num;
        else if(((tab_pos == GTK_POS_LEFT) || (tab_pos == GTK_POS_RIGHT)) && (y <= max_y))
            return page_num;
        
        page_num++;
    }

    return -1;
}


void mainwindow_create_tab(void)
{
    GtkWidget* tmp_term = build_term();
    GtkVScrollbar *sbar= NULL;
    GtkHBox *tmp_box = GTK_HBOX(gtk_hbox_new(FALSE, 0));

    if(conf_get_scrollbar() == -1)
        gtk_box_pack_start(GTK_BOX(tmp_box), tmp_term, TRUE, TRUE, 0);
    else if(conf_get_scrollbar() == POS_LEFT)
    {
        sbar = GTK_VSCROLLBAR(gtk_vscrollbar_new(vte_terminal_get_adjustment(
            VTE_TERMINAL(tmp_term))));
        gtk_box_pack_start(GTK_BOX(tmp_box), GTK_WIDGET(sbar), FALSE, FALSE, 0);
        gtk_box_pack_end(GTK_BOX(tmp_box), GTK_WIDGET(tmp_term), TRUE, TRUE, 0);
    } 
    else // (conf_get_scrollbar() == POS_RIGHT)
    {
        sbar = GTK_VSCROLLBAR(gtk_vscrollbar_new(vte_terminal_get_adjustment(
            VTE_TERMINAL(tmp_term))));
        gtk_box_pack_start(GTK_BOX(tmp_box), GTK_WIDGET(tmp_term), TRUE, TRUE, 0);
        gtk_box_pack_end(GTK_BOX(tmp_box), GTK_WIDGET(sbar), FALSE, FALSE, 0);
    }

    char buffer [100];
    sprintf(buffer, "%s %d", conf_get_term_name(), activetab + 1);
    GtkLabel* tmp_label = GTK_LABEL(gtk_label_new(buffer));

    if(conf_get_opacity() < 100)
    {
        if(screen_is_composited)
        {
            vte_terminal_set_background_transparent(VTE_TERMINAL(tmp_term), FALSE);
            vte_terminal_set_opacity(VTE_TERMINAL(tmp_term),
                conf_get_opacity()/100 * 0xffff);
        }
        else
        {
            vte_terminal_set_background_saturation(VTE_TERMINAL(tmp_term),
                1.0 - conf_get_opacity()/100);
            if(conf_get_bg_image() == NULL)
                vte_terminal_set_background_transparent(VTE_TERMINAL(tmp_term), TRUE);
        }
    }

    if(conf_get_opacity() < 100 && screen_is_composited)
    {
        vte_terminal_set_background_transparent(VTE_TERMINAL(tmp_term), FALSE);
        vte_terminal_set_opacity(VTE_TERMINAL(tmp_term),
            conf_get_opacity()/100 * 0xffff);
    }
    
    g_signal_connect(G_OBJECT(tmp_term), "window-title-changed",
        G_CALLBACK(mainwindow_window_title_changed), tmp_label);

    g_array_append_val(tabs, tmp_term);
    tabcount++;

    gtk_widget_show_all(GTK_WIDGET(tmp_box));
    gtk_notebook_append_page(tabbar, GTK_WIDGET(tmp_box), GTK_WIDGET(tmp_label));

    if(conf_get_tab_fill())
        gtk_container_child_set(GTK_CONTAINER(tabbar), GTK_WIDGET(tmp_box),
            "tab-expand", TRUE, "tab-fill", TRUE, NULL);

    if(conf_get_show_tab() == TABS_ONE&& tabcount > 1)
        gtk_notebook_set_show_tabs(tabbar, TRUE);

    activetab = tabcount - 1;
    gtk_notebook_set_current_page(tabbar, activetab);

    if(conf_get_allow_reorder())
        gtk_notebook_set_tab_reorderable(tabbar, GTK_WIDGET(tmp_box), TRUE);
        
    guint i;
    
    for(i = 0; i < uri_regex_count; ++i)
    {
        int tag = vte_terminal_match_add_gregex(VTE_TERMINAL(tmp_term), uri_regex[i], 0);
        vte_terminal_match_set_cursor_type(VTE_TERMINAL(tmp_term), tag, GDK_HAND2);
    }
    
    if(tabcount > 1)
        gtk_widget_set_sensitive(close_tab, TRUE);
        
}

void mainwindow_close_tab(GtkWidget *term)
{
    // Look for the right tab...
    int thetab = activetab;
    
    if(term != NULL)
    {
        int i;
        for(i = 0; i < tabs->len; i++)
        {
            if(g_array_index(tabs, GtkWidget *, i) == term)
            {
                thetab = i;
                break;
            }
        }
    }

    if(tabcount > 1)
    {
        g_array_remove_index(tabs, thetab);
        tabcount--;
        
        gtk_notebook_remove_page(tabbar, thetab);
        activetab = gtk_notebook_get_current_page(tabbar);

        if(tabcount == 1 && conf_get_show_tab() == TABS_ONE)
            gtk_notebook_set_show_tabs(tabbar, FALSE);
    } 
    else
        gtk_widget_destroy(GTK_WIDGET(mainwindow));
    
    if(tabcount == 1)
        gtk_widget_set_sensitive(GTK_WIDGET(close_tab), FALSE);
}

void mainwindow_toggle(int sig)
{
    if((!sig && GTK_WIDGET_VISIBLE(mainwindow)) || (sig && toggled))
    {
        gdk_threads_enter();
        gtk_widget_hide(GTK_WIDGET(mainwindow));
        gdk_flush();
        gdk_threads_leave();
        toggled = FALSE;
        return;
    }
    
    toggled = TRUE;
    gdk_threads_enter();
    
    /* reset the window position before displaying it to avoid flickering if the
       windos is moved */
    mainwindow_reset_position();
    
    if(gtk_window_is_active(GTK_WINDOW(mainwindow)) == FALSE)
        gtk_window_present(GTK_WINDOW(mainwindow));
    else
        gtk_widget_show(mainwindow);
    
    gtk_window_stick(GTK_WINDOW(mainwindow));
    gtk_window_set_keep_above(GTK_WINDOW(mainwindow), TRUE);
    gdk_window_focus(mainwindow->window, gtk_get_current_event_time());
    gdk_flush();
    gdk_threads_leave();
}

void mainwindow_toggle_fullscreen(void)
{
    if(fullscreen)
    {
        gtk_window_unfullscreen(GTK_WINDOW(mainwindow));
        mainwindow_reset_position();
    }
    else
        gtk_window_fullscreen(GTK_WINDOW(mainwindow));
    
    fullscreen = !fullscreen;
    mainwindow_focus_terminal();
}

static void mainwindow_reset_position(void)
{
    int x, y;

    conf_get_position(&x, &y);
    gtk_window_move(GTK_WINDOW(mainwindow), x, y);
    gtk_window_resize(GTK_WINDOW(mainwindow), conf_get_width(), conf_get_height());
}

static void mainwindow_show(GtkWidget *widget, gpointer userdata)
{
    if(dpy != NULL)
        return;

    mw_xwin = GDK_WINDOW_XWINDOW(GTK_WIDGET(mainwindow)->window);
    dpy = GDK_DISPLAY_XDISPLAY(gdk_display_get_default());
}

static void mainwindow_focus_out_event(GtkWindow* window, GdkEventFocus* event, gpointer userdata)
{
    int revert;
    Window w;
    XGetInputFocus(dpy, &w, &revert);

    if (w == mw_xwin)
        return;

    // focus wasn't lost just by pressing the shortcut key

    if(popupmenu_shown == TRUE)
        return;
        
    // focus wasn't lost by popping up popupmenu

    gtk_widget_hide(GTK_WIDGET(mainwindow));
}

static gboolean mainwindow_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer user_data)
{
    gint winw, winh;
    gtk_window_get_size(GTK_WINDOW(widget), &winw, &winh);

    gdk_draw_rectangle(widget->window, widget->style->black_gc, FALSE, 0, 0,
        winw-1, winh-1);

    if(conf_get_border() == BORDER_THIN)
        return FALSE;

    gdk_draw_rectangle(widget->window,
        widget->style->bg_gc[GTK_STATE_SELECTED], TRUE, 1, 1, winw -2, winh -2);

    gdk_draw_rectangle(widget->window, widget->style->bg_gc[GTK_STATE_NORMAL],
        TRUE, 5, 5, winw-10, winh-10);

    return FALSE;
}

static void mainwindow_destroy(GtkWidget *widget, gpointer user_data)
{
    g_array_free(tabs, TRUE);
    gtk_main_quit();
}

static void mainwindow_window_title_changed(VteTerminal *vteterminal, gpointer user_data)
{
    if(vteterminal != NULL && user_data != NULL)
        gtk_label_set_label(GTK_LABEL(user_data), vte_terminal_get_window_title(vteterminal));
}

static void mainwindow_switch_tab(GtkNotebook *notebook, GtkNotebookPage *page,
    guint page_num, gpointer user_data)
{
    activetab = page_num;
}

static void mainwindow_goto_tab(gint i)
{
    gtk_notebook_set_current_page(tabbar, i);
    activetab = gtk_notebook_get_current_page(tabbar);
    mainwindow_focus_terminal();
}

static void mainwindow_next_tab(GtkWidget *widget, gpointer user_data)
{
    if(gtk_notebook_get_current_page(tabbar) == (tabcount - 1))
        gtk_notebook_set_current_page(tabbar, 0);
    else
        gtk_notebook_next_page(tabbar);

    activetab = gtk_notebook_get_current_page(tabbar);
    mainwindow_focus_terminal();
}

static void mainwindow_prev_tab(GtkWidget *widget, gpointer user_data)
{
    if(gtk_notebook_get_current_page(tabbar) == (0))
        gtk_notebook_set_current_page(tabbar, (tabcount - 1));
    else    
        gtk_notebook_prev_page(tabbar);

    activetab = gtk_notebook_get_current_page(tabbar);
    mainwindow_focus_terminal();
}

static void mainwindow_new_tab(GtkWidget *widget, gpointer user_data)
{
    mainwindow_create_tab();
    mainwindow_focus_terminal();
}

static void mainwindow_delete_tab(GtkWidget *widget, gpointer user_data)
{
    mainwindow_close_tab(NULL);

    if(tabcount > 0)
        mainwindow_focus_terminal();
}

int handle_x_error(Display *dpy, XErrorEvent *evt)
{
    if(evt->error_code == BadAccess|| evt->error_code == BadValue || 
        evt->error_code == BadWindow) 
    {
        fprintf(stderr, "error: unable to grab key, is stjerm is already running with the same key?\n");
        exit(1);
    }
    
    return 0;
}

static void mainwindow_tab_moved(GtkWidget *notebook, GtkWidget *page, guint page_num, gpointer user_data)
{
    GList *children = gtk_container_get_children(GTK_CONTAINER(page));
    GtkWidget *term = GTK_WIDGET(children->data);
       
    GArray *newtabs = g_array_new(TRUE, FALSE, sizeof(VteTerminal*));
    
    int i;
    
    for(i = 0; i < tabcount; i++)
    {
        GtkWidget *element = g_array_index(tabs, GtkWidget *, i);
        
        if(i == page_num)
        {
            g_array_append_val(newtabs, term);
            g_array_append_val(newtabs, element);
        }
        else if(element == term)
            continue;
        else
            g_array_append_val(newtabs, element);
    }
    
    g_array_free(tabs, TRUE);
    tabs = newtabs;
    activetab = page_num;
}

static void mainwindow_focus_terminal(void)
{
    if (activetab >= 0)
        gtk_window_set_focus(GTK_WINDOW(mainwindow), 
            GTK_WIDGET(g_array_index(tabs, VteTerminal*, activetab)));
}

static void mainwindow_copy(GtkWidget *widget, gpointer user_data)
{
    vte_terminal_copy_clipboard
        (g_array_index(tabs, VteTerminal*, activetab));
}

static void mainwindow_paste(GtkWidget *widget, gpointer user_data) 
{
    vte_terminal_paste_clipboard
        (g_array_index(tabs, VteTerminal*, activetab));
}

