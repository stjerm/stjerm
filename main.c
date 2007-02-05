/*
 * main.c
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


#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <gtk/gtk.h>
#include "stjerm.h"


char *currdir;


int main(int argc, char *argv[])
{
	currdir = argv[0];
	
	gtk_init(&argc, &argv);
	if (!g_thread_supported())
		g_thread_init(NULL);
	gdk_threads_init();
	
	build_mainwindow();
	
	g_thread_create((GThreadFunc)wait_key, NULL, FALSE, NULL);
	gtk_main();
	
    return 0;
}
