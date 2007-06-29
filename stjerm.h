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

#define TERM_ICONIFY_WINDOW    0
#define TERM_DEICONIFY_WINDOW  1
#define TERM_RAISE_WINDOW      2
#define TERM_LOWER_WINDOW      3
#define TERM_MAXIMIZE_WINDOW   4
#define TERM_RESTORE_WINDOW    5
#define TERM_REFRESH_WINDOW    6
#define TERM_RESIZE_WINDOW     7
#define TERM_MOVE_WINDOW       8


extern void build_mainwindow(void);
extern GtkWidget* build_term(void);
extern void mainwindow_toggle_visibility(void);

extern void grab_key(void);

extern void term_grab_focus(void);
extern void open_tab(void);
extern void close_tab(void);
extern void next_tab(void);
extern void previous_tab(void);

extern void build_colortabs(void);
extern void colortabs_reload(void);

#endif /* __STJERM_H__ */

