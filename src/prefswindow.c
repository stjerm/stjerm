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

GtkTable *generalBox;
GtkTable *appearanceBox;
GtkTable *advancedBox;

void buildprefs_general_tab(void);
void buildprefs_appearance_tab(void);
void buildprefs_advanced_tab(void);

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
    gtk_window_set_resizable(GTK_WINDOW(prefswindow), FALSE);
    
    GtkWidget *preftabs = gtk_notebook_new();
    
    generalBox = GTK_TABLE(gtk_table_new(3, 4, FALSE));
    appearanceBox = GTK_TABLE(gtk_table_new(6, 4, FALSE));
    advancedBox = GTK_TABLE(gtk_table_new(3, 4, FALSE));
    
    GtkWidget *generalLabel = gtk_label_new("General");
    GtkWidget *appearanceLabel = gtk_label_new("Appearance");
    GtkWidget *advancedLabel = gtk_label_new("Advanced");
    
    // build out the three tabs
    buildprefs_general_tab();
    buildprefs_appearance_tab();
    buildprefs_advanced_tab();
    
    
    gtk_widget_show_all(GTK_WIDGET(generalBox));
    gtk_widget_show_all(GTK_WIDGET(appearanceBox));
    gtk_widget_show_all(GTK_WIDGET(advancedBox));
    
    gtk_notebook_append_page(GTK_NOTEBOOK(preftabs), GTK_WIDGET(generalBox), generalLabel);
    gtk_notebook_append_page(GTK_NOTEBOOK(preftabs), GTK_WIDGET(appearanceBox), appearanceLabel);
    gtk_notebook_append_page(GTK_NOTEBOOK(preftabs), GTK_WIDGET(advancedBox), advancedLabel);

    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(preftabs), TRUE);

    gtk_box_pack_start(
        GTK_BOX(GTK_DIALOG(prefswindow)->vbox),
        preftabs, 
        TRUE,
        TRUE,
        0
    );
    gtk_widget_show(GTK_WIDGET(preftabs));
    
    gtk_dialog_run(GTK_DIALOG(prefswindow));
    gtk_widget_destroy(prefswindow);
}

void buildprefs_general_tab(void)
{
    
}

void buildprefs_appearance_tab(void)
{
    gtk_table_set_row_spacings(appearanceBox, 2);
    gtk_table_set_col_spacings(appearanceBox, 4);
    gtk_container_set_border_width(GTK_CONTAINER(appearanceBox), 4);
    
    GtkLabel *borderLabel = GTK_LABEL(gtk_label_new("Border:"));
    gtk_misc_set_alignment(GTK_MISC(borderLabel), 1, 0.5);
    GtkComboBoxText *borderBox = GTK_COMBO_BOX_TEXT(gtk_combo_box_text_new());
    gtk_combo_box_text_append_text(borderBox, "None");
    gtk_combo_box_text_append_text(borderBox, "Thin");
    gtk_combo_box_text_append_text(borderBox, "Thick");
    gtk_table_attach_defaults(appearanceBox, GTK_WIDGET(borderLabel), 0, 1, 0, 1);
    gtk_table_attach_defaults(appearanceBox, GTK_WIDGET(borderBox), 1, 2, 0, 1);
    
    GtkLabel *widthLabel = GTK_LABEL(gtk_label_new("Width:"));
    gtk_misc_set_alignment(GTK_MISC(widthLabel), 1, 0.5);
    GtkWidget *width = gtk_entry_new();
    gtk_table_attach_defaults(appearanceBox, GTK_WIDGET(widthLabel), 0, 1, 1, 2);
    gtk_table_attach_defaults(appearanceBox, width, 1, 2, 1, 2);
    
    
    GtkLabel *heightLabel = GTK_LABEL(gtk_label_new("Height:"));
    gtk_misc_set_alignment(GTK_MISC(heightLabel), 1, 0.5);
    GtkWidget *height = gtk_entry_new();
    gtk_table_attach_defaults(appearanceBox, GTK_WIDGET(heightLabel), 0, 1, 2, 3);
    gtk_table_attach_defaults(appearanceBox, GTK_WIDGET(height), 1, 2, 2, 3);
    
    GtkLabel *backColorLabel = GTK_LABEL(gtk_label_new("Background Color:"));
    gtk_misc_set_alignment(GTK_MISC(backColorLabel), 1, 0.5);
    GtkColorButton *backColor = GTK_COLOR_BUTTON(gtk_color_button_new());
    gtk_table_attach_defaults(appearanceBox, GTK_WIDGET(backColorLabel), 3, 4, 0, 1);
    gtk_table_attach_defaults(appearanceBox, GTK_WIDGET(backColor), 4, 5, 0, 1);
    
    GtkLabel *foreColorLabel = GTK_LABEL(gtk_label_new("Foreground Color:"));
    gtk_misc_set_alignment(GTK_MISC(foreColorLabel), 1, 0.5);
    GtkColorButton *foreColor = GTK_COLOR_BUTTON(gtk_color_button_new());
    gtk_table_attach_defaults(appearanceBox, GTK_WIDGET(foreColorLabel), 3, 4, 1, 2);
    gtk_table_attach_defaults(appearanceBox, GTK_WIDGET(foreColor), 4, 5, 1, 2);
    
    GtkLabel *backImageLabel = GTK_LABEL(gtk_label_new("Background Image:"));
    gtk_misc_set_alignment(GTK_MISC(backImageLabel), 1, 0.5);
    GtkWidget *button = gtk_file_chooser_button_new("Select a file",  GTK_FILE_CHOOSER_ACTION_OPEN);
    //gtk_widget_set_size_request(button, 30, -1);
    gtk_table_attach_defaults(appearanceBox, GTK_WIDGET(backImageLabel), 3, 4, 2, 3);
    gtk_table_attach_defaults(appearanceBox, button, 4, 5, 2, 3);
    
    GtkLabel *opacityLabel = GTK_LABEL(gtk_label_new("Opacity:"));
    gtk_misc_set_alignment(GTK_MISC(opacityLabel), 1, 0.5);
    GtkWidget *opacity = gtk_spin_button_new(
        GTK_ADJUSTMENT(gtk_adjustment_new(100, 0, 100, 1, 1, 0)),
        1,
        0
    );
    gtk_table_attach_defaults(appearanceBox, GTK_WIDGET(opacityLabel), 3, 4, 3, 4);
    gtk_table_attach_defaults(appearanceBox, opacity, 4, 5, 3, 4);
    
    
    GtkLabel *fontLabel = GTK_LABEL(gtk_label_new("Font:"));
    gtk_misc_set_alignment(GTK_MISC(fontLabel), 1, 0.5);
    GtkWidget *font = gtk_font_button_new();
    gtk_table_attach_defaults(appearanceBox, GTK_WIDGET(fontLabel), 3, 4, 4, 5);
    gtk_table_attach_defaults(appearanceBox, GTK_WIDGET(font), 4, 5, 4, 5);
    
    GtkLabel *allowBoldLabel = GTK_LABEL(gtk_label_new("Allow Bold?"));
    gtk_misc_set_alignment(GTK_MISC(allowBoldLabel), 1, 0.5);
    gtk_table_attach_defaults(appearanceBox, GTK_WIDGET(allowBoldLabel), 3, 4, 5, 6);
    
}

void buildprefs_advanced_tab(void)
{
    
}

