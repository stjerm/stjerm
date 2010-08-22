/*
 * stjerm.h
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

#define BORDER_THIN  0
#define BORDER_THICK 1
#define BORDER_NONE  2

#define POS_TOP         0
#define POS_BOTTOM      1
#define POS_LEFT        2
#define POS_RIGHT       3
#define POS_TOPLEFT     4
#define POS_TOPRIGHT    5
#define POS_BOTTOMLEFT  6
#define POS_BOTTOMRIGHT 7

#define TABS_NEVER  0
#define TABS_ONE    1
#define TABS_ALWAYS 2

#define OPTION_COUNT 28


// The following defines borrowed from gnome-terminal:

#define USERCHARS "-[:alnum:]"
#define USERCHARS_CLASS "[" USERCHARS "]"
#define PASSCHARS_CLASS "[-[:alnum:]\\Q,?;.:/!%$^*&~\"#'\\E]"
#define HOSTCHARS_CLASS "[-[:alnum:]]"
#define HOST HOSTCHARS_CLASS "+(\\." HOSTCHARS_CLASS "+)*"
#define PORT "(?:\\:[[:digit:]]{1,5})?"
#define PATHCHARS_CLASS "[-[:alnum:]\\Q_$.+!*,;@&=?/~#%\\E]"
#define PATHTERM_CLASS "[^\\Q]'.}>) \t\r\n,\"\\E]"
#define SCHEME "(?:news:|telnet:|nntp:|file:\\/|https?:|ftps?:|sftp:|webcal:)"
#define USERPASS USERCHARS_CLASS "+(?:" PASSCHARS_CLASS "+)?"
#define URLPATH   "(?:(/"PATHCHARS_CLASS"+(?:[(]"PATHCHARS_CLASS"*[)])*"PATHCHARS_CLASS"*)*"PATHTERM_CLASS")?"


typedef struct {
    char long_name[18];
    char short_name[12];
    char var_type[12];
    char desc[150];
} Option;

typedef enum {
    URL_TYPE_HTTP,
    URL_TYPE_EMAIL,
    URL_TYPE_NEWS
} UrlTypes;

typedef struct {
    const char *pattern;
    UrlTypes url_type;
    GRegexCompileFlags flags;
} RegExPattern;

typedef struct {
    int tag;
    char *text;
} CursorMatch;


// The following patterns borrowed from gnome-terminal:

static const RegExPattern uri_patterns[] = {
    {"news:[[:alnum:]\\Q^_{|}~!\"#$%&'()*+,./;:=?`\\E]+", URL_TYPE_NEWS, G_REGEX_CASELESS },
    {SCHEME "//(?:" USERPASS "\\@)?" HOST PORT URLPATH, URL_TYPE_HTTP, G_REGEX_CASELESS},
    {"(?:www|ftp)" HOSTCHARS_CLASS "*\\." HOST PORT URLPATH , URL_TYPE_HTTP, G_REGEX_CASELESS},
    {"(?:mailto:)?" USERCHARS_CLASS "[" USERCHARS ".]*\\@" HOSTCHARS_CLASS "+\\." HOST, URL_TYPE_EMAIL, G_REGEX_CASELESS},
};


extern void print_help(void);
extern void print_info(void);

extern void build_mainwindow(void);
extern GtkWidget* build_term(void);
extern void build_popupmenu(void);

extern void mainwindow_toggle(int sig);
extern void mainwindow_create_tab(void);
extern void mainwindow_close_tab(GtkWidget *);
extern void mainwindow_toggle_fullscreen(void);
extern void init_key(void);
extern void grab_key(void);
extern void wait_key(void);

extern void conf_init(void);
extern char* conf_get_font(void);
extern float conf_get_opacity(void);
extern GdkColor conf_get_bg(void);
extern GdkColor conf_get_fg(void);
extern int conf_get_border(void);
extern unsigned int conf_get_mod(void);
extern KeySym conf_get_key(void);
extern int conf_get_width(void);
extern int conf_get_height(void);
extern void conf_get_position(int*, int*);
extern int conf_get_scrollbar(void);
extern char* conf_get_shell(void);
extern int conf_get_lines(void);
extern int conf_get_show_tab(void);
extern char* conf_get_term_name(void);
extern GtkPositionType conf_get_tab_pos(void);
extern GdkColor* conf_get_color_palette(void);
extern gboolean conf_get_tab_fill(void);
extern gboolean conf_get_allow_bold(void);
extern GdkModifierType conf_get_key_mod(void);
extern gboolean conf_get_auto_hide(void);
extern char* conf_get_bg_image(void);
extern gboolean conf_get_allow_reorder(void);
extern gboolean conf_get_scroll_on_output();

#endif /* __STJERM_H__ */

