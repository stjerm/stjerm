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
#include <X11/Xlib.h>

#define TERM_ICONIFY_WINDOW    0
#define TERM_DEICONIFY_WINDOW  1
#define TERM_RAISE_WINDOW      2
#define TERM_LOWER_WINDOW      3
#define TERM_MAXIMIZE_WINDOW   4
#define TERM_RESTORE_WINDOW    5
#define TERM_REFRESH_WINDOW    6
#define TERM_RESIZE_WINDOW     7
#define TERM_MOVE_WINDOW       8

#define TRANS_FAKE 0
#define TRANS_REAL 1
#define TRANS_BEST 2
#define TRANS_NONE 3

#define POS_TOP         0
#define POS_BOTTOM      1
#define POS_LEFT        2
#define POS_RIGHT       3
#define POS_TOPLEFT     4
#define POS_TOPRIGHT    5
#define POS_BOTTOMLEFT  6
#define POS_BOTTOMRIGHT 7


extern void print_help(void);

extern void build_mainwindow(void);
extern void build_term(void);
extern void build_popupmenu(void);

extern void mainwindow_present(void);
extern void init_key(void);
extern void grab_key(void);
extern void wait_key(void);

extern void conf_init(void);
extern char* conf_get_font(void);
extern float conf_get_opacity(void);
extern int conf_get_transparency(void);
extern GdkColor conf_get_bg(void);
extern GdkColor conf_get_fg(void);
extern unsigned int conf_get_mod(void);
extern KeySym conf_get_key(void);
extern int conf_get_width(void);
extern int conf_get_height(void);
extern void conf_get_position(int*, int*);


#endif /* __STJERM_H__ */

