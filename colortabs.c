/*
 * colortabs.c
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


#include <stdlib.h>
#include <gtk/gtk.h>
#include "stjerm.h"


GtkWidget *colortabs;
static gint tabscount;
static gint activetab;

void build_colortabs(void);
void destroy_colortabs(void);
void colorabs_reload(void);
static void colortabs_redraw(void);
static gboolean colortabs_expose_event(GtkWidget*, GdkEventExpose*, gpointer);


void build_colortabs(void)
{
	activetab = 0;
	colortabs = gtk_drawing_area_new();
	gtk_widget_set_size_request(colortabs, -1, 15);
	
	gtk_signal_connect(GTK_OBJECT(colortabs), "expose-event",
	                   G_CALLBACK(colortabs_expose_event), NULL);
}


void colortabs_reload(void)
{
	tabscount = gtk_notebook_get_n_pages(GTK_NOTEBOOK(termbook));
	activetab = gtk_notebook_get_current_page(GTK_NOTEBOOK(termbook));
	colortabs_redraw();
}


static void colortabs_redraw(void)
{
	gtk_widget_queue_draw_area(GTK_WIDGET(colortabs), 0, 0,
	                           colortabs->allocation.width,
	                           colortabs->allocation.height);
}


static gboolean colortabs_expose_event(GtkWidget *widget, GdkEventExpose *event,
                                       gpointer user_data)
{
	gint w = widget->allocation.width;
	gint h = widget->allocation.height;
	gint tabw = ((gfloat)w - 2.0f) / (gfloat)tabscount;
	
	// background
	gdk_draw_rectangle(widget->window,
	                   widget->style->bg_gc[GTK_STATE_SELECTED],
	                   TRUE,
	                   0, 0, w, h);
	
	GdkGC *gc = gdk_gc_new(widget->window);
	int i;
	for (i = 0; i < tabscount; i++)
	{
		GtkBox *box = GTK_BOX(gtk_notebook_get_nth_page(GTK_NOTEBOOK(termbook), i));
		
		GdkColor color;
		int r, g, b;
		sscanf(gtk_widget_get_name(GTK_WIDGET(box)), "%i %i %i", &r, &g, &b);
		
		color.red = r * 256;
		color.green = g * 256;
		color.blue = b * 256;
		
		gdk_gc_set_rgb_fg_color(gc, &color);
		
		// fill with color
		gdk_draw_rectangle(widget->window,
		                   gc,
		                   TRUE,
		                   i * tabw + 1, 1, tabw-1, 13);
		
		// inner outline
		if (activetab == i)
		{
			gdk_draw_rectangle(widget->window,
			                   widget->style->black_gc,
			                   FALSE,
			                   i * tabw + 1, 0, tabw - 2, 15);
			gdk_draw_rectangle(widget->window,
			                   widget->style->white_gc,
			                   FALSE,
			                   i * tabw + 2, 2, tabw - 4, 15);
			gdk_draw_rectangle(widget->window,
			                   widget->style->black_gc,
			                   FALSE,
			                   i * tabw + 3, 3, tabw - 6, 15);
		}
	}
	
	// separators
	for (i = 0; i < tabscount; i++)
	{
		gdk_draw_line(widget->window,
		              widget->style->black_gc,
		              i * tabw, 1, i * tabw, 13);
	}
	
	// outline
	gdk_draw_rectangle(widget->window,
	                   widget->style->black_gc,
	                   FALSE,
	                   0, 1, tabscount * tabw, h - 2);
	
	return FALSE;
}
