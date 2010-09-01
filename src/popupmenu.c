/*
 * popupmenu.c
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
#include <string.h>
#include "stjerm.h"



extern GtkWidget *mainwindow;
extern int activetab;
extern GArray* tabs;
GtkWidget *popupmenu= NULL;
gboolean popupmenu_shown;

GtkWidget *close_tab;

void build_popupmenu(void);
static void popupmenu_activate(gchar*);
static void popupmenu_deactivate(GtkMenuShell*, gpointer);

void build_popupmenu(void)
{
    popupmenu = gtk_menu_new();
    popupmenu_shown = FALSE;

    GtkWidget *menuitem;
    GtkWidget *img;

    gchar *labels[] = {"New Tab", "Close Tab", "Copy", "Paste", "Toggle Fullscreen", "Quit"};
    gchar *stocks[] = {GTK_STOCK_ADD, GTK_STOCK_CLOSE, GTK_STOCK_COPY,
        GTK_STOCK_PASTE, GTK_STOCK_FULLSCREEN, GTK_STOCK_QUIT};

    int i;
    
    for(i = 0; i < 6; i++)
    {
        if(i == 2 || i == 4 || i == 5)
        {
            menuitem = gtk_separator_menu_item_new();
            gtk_menu_shell_append(GTK_MENU_SHELL(popupmenu), menuitem);
            gtk_widget_show(GTK_WIDGET(menuitem));
        }

        menuitem = gtk_image_menu_item_new_with_label(labels[i]);
        img = gtk_image_new_from_stock(stocks[i], GTK_ICON_SIZE_MENU);
        
        if(i == 1)
        {
            close_tab = menuitem;
            gtk_widget_set_sensitive(menuitem, FALSE);
        }
        
        gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menuitem),
            GTK_WIDGET(img));
        
        g_signal_connect_swapped(G_OBJECT(menuitem), "activate",
            G_CALLBACK(popupmenu_activate), (gpointer)labels[i]);
        
        gtk_menu_shell_append(GTK_MENU_SHELL(popupmenu), menuitem);
        gtk_widget_show(GTK_WIDGET(menuitem));
    }

    g_signal_connect_swapped(G_OBJECT(popupmenu), "deactivate",
        G_CALLBACK(popupmenu_deactivate), NULL);
}

static void popupmenu_activate(gchar *label)
{
    if(!strcmp(label, "New Tab"))
    {
        mainwindow_create_tab();
    }
    else if(!strcmp(label, "Close Tab"))
    {
        mainwindow_close_tab(NULL);
    }
    else if(!strcmp(label, "Copy"))
    {
        vte_terminal_copy_clipboard(g_array_index(tabs, VteTerminal*, activetab));
    }
    else if(!strcmp(label, "Paste"))
    {
        vte_terminal_paste_clipboard(g_array_index(tabs, VteTerminal*, activetab));
    }
    else if(!strcmp(label, "Toggle Fullscreen"))
    {
        mainwindow_toggle_fullscreen();
    }
    else if(!strcmp(label, "Quit"))
    {
        gtk_widget_destroy(GTK_WIDGET(mainwindow));
    }

    popupmenu_shown = FALSE;
}

static void popupmenu_deactivate(GtkMenuShell *menushell, gpointer userdata)
{
    popupmenu_shown = FALSE;
}

