/*
 * termbook.c
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


int termsCount = 0;
GtkWidget *popupMenu;


void openTab(void);
void closeTab(void);


void openTab(void)
{
	GtkHBox *box;
	VteTerminal *term;
	GtkVScrollbar *sbar;
	
	box = GTK_HBOX(gtk_hbox_new(FALSE, 0));
	term = VTE_TERMINAL(vte_terminal_new());
	sbar = GTK_VSCROLLBAR(gtk_vscrollbar_new(vte_terminal_get_adjustment(
	                                                     VTE_TERMINAL(term))));
	
	gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(term), TRUE, TRUE, 0);
	gtk_box_pack_end(GTK_BOX(box), GTK_WIDGET(sbar), FALSE, FALSE, 0);
	
	GtkWidget *label;
	label = gtk_label_new("Terminal 1");
	gtk_notebook_append_page(GTK_NOTEBOOK(termBook), GTK_WIDGET(box), label);
	gtk_notebook_set_tab_label_packing(GTK_NOTEBOOK(termBook), GTK_WIDGET(box),
	                                   TRUE, FALSE, GTK_PACK_START);
	gtk_notebook_set_show_border(GTK_NOTEBOOK(termBook), FALSE);
	vte_terminal_fork_command(term, "/bin/bash", NULL, NULL,
	                          "/home/stjepan/stjerm", FALSE, TRUE, TRUE);
	
	termsCount++;
}


void closeTab(void)
{
	
}
