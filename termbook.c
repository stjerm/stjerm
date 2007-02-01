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


void openTab(void)
{
	VteTerminal *term;
	term = vte_terminal_new();
	
	GtkWidget *label;
	label = gtk_label_new("Terminal 1");
	gtk_notebook_append_page(GTK_NOTEBOOK(termBook), GTK_WIDGET(term), label);
	gtk_notebook_set_tab_label_packing(GTK_NOTEBOOK(termBook), GTK_WIDGET(term),
	                                   TRUE, FALSE, GTK_PACK_START);
	vte_terminal_fork_command(VTE_TERMINAL(term), "/bin/bash", NULL, NULL, "/home/stjepan/stjerm", FALSE, TRUE, TRUE);
	
	termsCount++;
}


void closeTab(void)
{
	
}
