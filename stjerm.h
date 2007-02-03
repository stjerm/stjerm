/*
 * stjerm.h
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


#ifndef __STJERM_H__
#define __STJERM_H__

#include <gtk/gtk.h>


extern char *currdir;
extern GtkWidget *mainwindow;
extern GtkWidget *termbook;

extern void build_mainwindow(void);
extern void build_termbook(void);
extern void wait_key(void);
extern void mainwindow_toggle_visibility(void);
extern void term_grab_focus(void);


#endif /* __STJERM_H__ */
