/*
 * prefswindow.c
 * This file is part of Stjerm
 *
 * Copyright (C) 2007-2012 - Kristopher Wilson, Stjepan Glavina and Markus Groß
 * 
 * Stjerm is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Stjerm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even thWe implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Stjerm; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */

#include <gtk/gtk.h>
#include "stjerm.h"

extern GtkWidget *mainwindow;

GtkWidget *prefswindow;

void buildprefswindow(void)
{
    prefswindow = gtk_dialog_new_with_buttons(
        "stjerm Preferences",
        GTK_WINDOW(mainwindow),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
        GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
        NULL
    );
    
    gtk_window_set_default_size(GTK_WINDOW(prefswindow), 450, 250);
    //gtk_window_set_resizable(GTK_WINDOW(prefswindow), FALSE);
    
    GtkNotebook *preftabs = GTK_NOTEBOOK(gtk_notebook_new());
    
    GtkHBox *generalBox = GTK_HBOX(gtk_table_new(3, 4, FALSE));
    GtkHBox *appearanceBox = GTK_HBOX(gtk_table_new(3, 4, FALSE));
    GtkHBox *advancedBox = GTK_HBOX(gtk_table_new(3, 4, FALSE));
    
    GtkLabel *generalLabel = GTK_LABEL(gtk_label_new("General"));
    GtkLabel *appearanceLabel = GTK_LABEL(gtk_label_new("Appearance"));
    GtkLabel *advancedLabel = GTK_LABEL(gtk_label_new("Advanced"));
    
    gtk_widget_show_all(GTK_WIDGET(generalBox));
    gtk_widget_show_all(GTK_WIDGET(appearanceBox));
    gtk_widget_show_all(GTK_WIDGET(advancedBox));
    
    gtk_notebook_append_page(preftabs, GTK_WIDGET(generalBox), GTK_WIDGET(generalLabel));
    gtk_notebook_append_page(preftabs, GTK_WIDGET(appearanceBox), GTK_WIDGET(appearanceLabel));
    gtk_notebook_append_page(preftabs, GTK_WIDGET(advancedBox), GTK_WIDGET(advancedLabel));

    gtk_notebook_set_show_tabs(preftabs, TRUE);

    gtk_box_pack_start(
        GTK_BOX(GTK_DIALOG(prefswindow)->vbox),
        GTK_WIDGET(preftabs), 
        TRUE,
        TRUE,
        0
    );
    gtk_widget_show (GTK_WIDGET(preftabs));
    
    gtk_dialog_run(GTK_DIALOG(prefswindow));
    gtk_widget_destroy(prefswindow);
}

