/*
 * config.c
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
#include <gdk/gdkx.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include "stjerm.h"


extern int sargc;
extern char **sargv;
extern GtkWidget *mainwindow;
static char _font[100];
static float _opacity;
static GdkColor _bg;
static GdkColor _fg;
static int _border;
static unsigned int _mod;
static KeySym _key;
static int _width;
static int _height;
static int _pos;
static int _posx;
static int _posy;
static int _scrollpos;
static char _shell[100];
static int _lines;
static int _showtab;
static char _termname[100];

static void set_border(char*);
static void set_mod(char*);
static void set_key(char*);
static void set_pos(char *v);

void conf_init(void);
char* conf_get_font(void);
float conf_get_opacity(void);
GdkColor conf_get_bg(void);
GdkColor conf_get_fg(void);
int conf_get_border(void);
unsigned int conf_get_mod(void);
KeySym conf_get_key(void);
int conf_get_width(void);
int conf_get_height(void);
void conf_get_position(int*, int*);
int conf_get_scrollbar(void);
char* conf_get_shell(void);
int conf_get_lines(void);
int conf_get_show_tab(void);
char* conf_get_term_name(void);


void set_border(char *v)
{
	if (!strcmp(v, "thin")) _border = BORDER_THIN;
	else if (!strcmp(v, "thick")) _border = BORDER_THICK;
	else _border = BORDER_NONE;
}


void set_mod(char *v)
{
	v[0] = tolower(v[0]);
	if (!strcmp(v, "shift")) _mod = ShiftMask;
	else if (!strcmp(v, "control")) _mod = ControlMask;
	else if (!strcmp(v, "ctrl")) _mod = ControlMask;
	else if (!strcmp(v, "alt")) _mod = Mod1Mask;
	else if (!strcmp(v, "mod1")) _mod = Mod1Mask;
	else if (!strcmp(v, "windows")) _mod = Mod4Mask;
	else if (!strcmp(v, "win")) _mod = Mod4Mask;
	else _mod = 0;
}


void set_key(char *v)
{
	_key = 0;

	v[0] = tolower(v[0]);
	_key = XStringToKeysym(v);

	if (!_key)
	{
		v[0] = toupper(v[0]);
		_key = XStringToKeysym(v);
	}
}


void set_pos(char *v)
{
	if      (!strcmp(v, "top"))         _pos = POS_TOP;
	else if (!strcmp(v, "bottom"))      _pos = POS_BOTTOM;
	else if (!strcmp(v, "left"))        _pos = POS_LEFT;
	else if (!strcmp(v, "right"))       _pos = POS_RIGHT;
	else if (!strcmp(v, "topleft"))     _pos = POS_TOPLEFT;
	else if (!strcmp(v, "topright"))    _pos = POS_TOPRIGHT;
	else if (!strcmp(v, "bottomleft"))  _pos = POS_BOTTOMLEFT;
	else if (!strcmp(v, "bottomright")) _pos = POS_BOTTOMRIGHT;
}


void conf_init(void)
{
	Display *dpy = GDK_DISPLAY_XDISPLAY(gdk_display_get_default());
	char *op;

	if ((op = XGetDefault(dpy, "stjerm", "font"))) strcpy(_font, op);
	else strcpy(_font, "Bitstream Vera Sans Mono 10");

	if ((op = XGetDefault(dpy, "stjerm", "background")))
	{
		if (!gdk_color_parse(op, &_bg)) gdk_color_parse("black", &_bg);
	}
	else gdk_color_parse("black", &_bg);

	if ((op = XGetDefault(dpy, "stjerm", "foreground")))
	{
		if (!gdk_color_parse(op, &_fg)) gdk_color_parse("white", &_fg);
	}
	else gdk_color_parse("white", &_fg);

	if ((op = XGetDefault(dpy, "stjerm", "scrollBar")) || XGetDefault(dpy, "stjerm", "scrollbar"))
	{
		if (!op) op = XGetDefault(dpy, "stjerm", "scrollbar");
		if      (!strcmp(op, "true"))  _scrollpos = POS_RIGHT;
		else if (!strcmp(op, "left"))  _scrollpos = POS_LEFT;
		else if (!strcmp(op, "right")) _scrollpos = POS_RIGHT;
		else _scrollpos = -1;
	}
	else _scrollpos = -1;

	if ((op = XGetDefault(dpy, "stjerm", "border"))) set_border(op);
	else _border = BORDER_NONE;

	if ((op = XGetDefault(dpy, "stjerm", "opacity"))) _opacity = atof(op);
	else _opacity = 100.0f;
	
	if ((op = XGetDefault(dpy, "stjerm", "width"))) _width = atoi(op);
	else _width = 800;

	if ((op = XGetDefault(dpy, "stjerm", "height"))) _height = atoi(op);
	else _height = 400;

	if ((op = XGetDefault(dpy, "stjerm", "position"))) set_pos(op);
	else _pos = POS_TOP;

	if ((op = XGetDefault(dpy, "stjerm", "mod"))) set_mod(op);
	else _mod = 0;

	if ((op = XGetDefault(dpy, "stjerm", "key"))) set_key(op);
	else _key = 0;

	if ((op = XGetDefault(dpy, "stjerm", "shell"))) strcpy(_shell, op);
	else strcpy(_shell, "/bin/bash");

	if ((op = XGetDefault(dpy, "stjerm", "lines"))) _lines = atoi(op);
	else _lines = 1000;
	
	if ((op = XGetDefault(dpy, "stjerm", "showtab"))) _showtab = 1;
	else _showtab = 0;
	
	if ((op = XGetDefault(dpy, "stjerm", "termname"))) strcpy(_termname, op);
	else strcpy(_termname, "term");

	
	gboolean keyoption = FALSE;
	int i;
	for (i = 1; i < sargc; i++)
	{
		if (sargv[i] != 0)
			if (!strcmp(sargv[i], "--showtab")) _showtab = 1;
		
		if (i + 1 >= sargc) continue;

		if (!strcmp(sargv[i], "-fn")) strcpy(_font, sargv[i+1]);
		else if (!strcmp(sargv[i], "-o")) _opacity = atof(sargv[i+1]);
		else if (!strcmp(sargv[i], "-bg")) gdk_color_parse(sargv[i+1], &_bg);
		else if (!strcmp(sargv[i], "-fg")) gdk_color_parse(sargv[i+1], &_fg);
		else if (!strcmp(sargv[i], "-b")) set_border(sargv[i+1]);
		else if (!strcmp(sargv[i], "-m")) set_mod(sargv[i+1]);
		else if (!strcmp(sargv[i], "-k")) { keyoption = TRUE; set_key(sargv[i+1]); }
		else if (!strcmp(sargv[i], "-w")) _width = atoi(sargv[i+1]);
		else if (!strcmp(sargv[i], "-h")) _height = atoi(sargv[i+1]);
		else if (!strcmp(sargv[i], "-p")) set_pos(sargv[i+1]);
		else if (!strcmp(sargv[i], "-s"))
		{
			if      (!strcmp(sargv[i+1], "left"))  _scrollpos = POS_LEFT;
			else if (!strcmp(sargv[i+1], "right")) _scrollpos = POS_RIGHT;
		}
		else if (!strcmp(sargv[i], "-sh")) strcpy(_shell, sargv[i+1]);
		else if (!strcmp(sargv[i], "-bl")) _lines = atoi(sargv[i+1]);
		else if (!strcmp(sargv[i], "--termname")) strcpy(_termname, sargv[i+1]);
	}

	if (keyoption == FALSE && _key == 0)
	{
		print_help();
		exit(1);
	}

	struct stat st;
	if (stat(_shell, &st) != 0)
	{
		fprintf(stderr, "error: the shell '%s' can't be opened\n", _shell);
		exit(1);
	}

	if (_lines < 0)
	{
		fprintf(stderr, "error: a scrollback line count < 0 is not allowed\n");
		exit(1);
	}

	if (keyoption == TRUE && _key == 0)
	{
		fprintf(stderr, "error: wrong shortcut key is defined\n");
		exit(1);
	}


	int scrw = gdk_screen_get_width(gdk_screen_get_default());
	int scrh = gdk_screen_get_height(gdk_screen_get_default());

	if (_pos == POS_TOP)
	{
		_posx = (scrw - _width) / 2;
		_posy = 0;
	}
	else if (_pos == POS_BOTTOM)
	{
		_posx = (scrw - _width) / 2;
		_posy = scrh - _height;
	}
	else if (_pos == POS_LEFT)
	{
		_posx = 0;
		_posy = (scrh - _height) / 2;
	}
	else if (_pos == POS_RIGHT)
	{
		_posx = scrw - _width;
		_posy = (scrh - _height) / 2;
	}
	else if (_pos == POS_TOPLEFT)
	{
		_posx = 0;
		_posy = 0;
	}
	else if (_pos == POS_TOPRIGHT)
	{
		_posx = scrw - _width;
		_posy = 0;
	}
	else if (_pos == POS_BOTTOMLEFT)
	{
		_posx = 0;
		_posy = scrh - _height;
	}
	else if (_pos == POS_BOTTOMRIGHT)
	{
		_posx = scrw - _width;
		_posy = scrh - _height;
	}
}


char* conf_get_font(void)
{
	return _font;
}


float conf_get_opacity(void)
{
	return _opacity;
}


GdkColor conf_get_bg(void)
{
	return _bg;
}


GdkColor conf_get_fg(void)
{
	return _fg;
}


int conf_get_border(void)
{
	return _border;
}


unsigned int conf_get_mod(void)
{
	return _mod;
}


KeySym conf_get_key(void)
{
	return _key;
}


int conf_get_width(void)
{
	return _width;
}


int conf_get_height(void)
{
	return _height;
}


void conf_get_position(int *x, int *y)
{
	*x = _posx;
	*y = _posy;
}


int conf_get_scrollbar(void)
{
	return _scrollpos;
}


char* conf_get_shell(void)
{
	return _shell;
}


int conf_get_lines(void)
{
	return _lines;
}


int conf_get_show_tab(void)
{
	return _showtab;
}


char* conf_get_term_name(void)
{
	return _termname;
}

