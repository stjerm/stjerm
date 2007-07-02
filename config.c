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


void conf_init(void)
{
	// Xdefaults compatibility (font, bg, fg and scrollbar)
	Display *dpy = GDK_DISPLAY_XDISPLAY(gdk_display_get_default());
	char *op;

	if ((op = XGetDefault(dpy, "stjerm", "font"))) strcpy(_font, op);
	else strcpy(_font, "Bitstream Vera Sans Mono 10");

	if ((op = XGetDefault(dpy, "stjerm", "background")))
	{
		if (!gdk_color_parse(op, &_bg)) gdk_color_parse("black", &_bg);
	}
	else
	{
		gdk_color_parse("black", &_bg);
	}

	if ((op = XGetDefault(dpy, "stjerm", "foreground")))
	{
		if (!gdk_color_parse(op, &_fg)) gdk_color_parse("white", &_fg);
	}
	else
	{
		gdk_color_parse("white", &_fg);
	}

	if ((op = XGetDefault(dpy, "stjerm", "scrollBar")))
	{
		if (!strcmp(op, "true")) _scrollpos = POS_RIGHT;
		else _scrollpos = -1;
	}
	else
	{
		_scrollpos = -1;
	}

	_border = BORDER_NONE;
	_opacity = 100.0f;
	_width = 800;
	_height = 400;
	_pos = POS_TOP;
	_mod = 0;
	_key = 0;

	gboolean keyoption = FALSE;
	int i;
	for (i = 1; i < sargc; i++)
	{
		if (i + 1 >= sargc) continue;

		if (!strcmp(sargv[i], "-fn"))
		{
			strcpy(_font, sargv[i+1]);
		}
		else if (!strcmp(sargv[i], "-o"))
		{
			_opacity = atof(sargv[i+1]);
		}
		else if (!strcmp(sargv[i], "-bg"))
		{
			gdk_color_parse(sargv[i+1], &_bg);
		}
		else if (!strcmp(sargv[i], "-fg"))
		{
			gdk_color_parse(sargv[i+1], &_fg);
		}
		else if (!strcmp(sargv[i], "-b"))
		{
			if (!strcmp(sargv[i+1], "thin")) _border = BORDER_THIN;
			else if (!strcmp(sargv[i+1], "thick")) _border = BORDER_THICK;
			else _border = BORDER_NONE;
		}
		else if (!strcmp(sargv[i], "-m"))
		{
			sargv[i+1][0] = tolower(sargv[i+1][0]);
			if (!strcmp(sargv[i+1], "shift")) _mod = ShiftMask;
			else if (!strcmp(sargv[i+1], "control")) _mod = ControlMask;
			else if (!strcmp(sargv[i+1], "ctrl")) _mod = ControlMask;
			else if (!strcmp(sargv[i+1], "alt")) _mod = Mod1Mask;
			else if (!strcmp(sargv[i+1], "mod1")) _mod = Mod1Mask;
			else if (!strcmp(sargv[i+1], "windows")) _mod = Mod4Mask;
			else if (!strcmp(sargv[i+1], "win")) _mod = Mod4Mask;
			else _mod = 0;
		}
		else if (!strcmp(sargv[i], "-k"))
		{
			keyoption = TRUE;

			sargv[i+1][0] = tolower(sargv[i+1][0]);
			_key = XStringToKeysym(sargv[i+1]);

			if (!_key)
			{
				sargv[i+1][0] = toupper(sargv[i+1][0]);
				_key = XStringToKeysym(sargv[i+1]);
			}
		}
		else if (!strcmp(sargv[i], "-w"))
		{
			_width = atoi(sargv[i+1]);
		}
		else if (!strcmp(sargv[i], "-h"))
		{
			_height = atoi(sargv[i+1]);
		}
		else if (!strcmp(sargv[i], "-p"))
		{
			if      (!strcmp(sargv[i+1], "top"))         _pos = POS_TOP;
			else if (!strcmp(sargv[i+1], "bottom"))      _pos = POS_BOTTOM;
			else if (!strcmp(sargv[i+1], "left"))        _pos = POS_LEFT;
			else if (!strcmp(sargv[i+1], "right"))       _pos = POS_RIGHT;
			else if (!strcmp(sargv[i+1], "topleft"))     _pos = POS_TOPLEFT;
			else if (!strcmp(sargv[i+1], "topright"))    _pos = POS_TOPRIGHT;
			else if (!strcmp(sargv[i+1], "bottomleft"))  _pos = POS_BOTTOMLEFT;
			else if (!strcmp(sargv[i+1], "bottomright")) _pos = POS_BOTTOMRIGHT;
		}
		else if (!strcmp(sargv[i], "-s"))
		{
			if      (!strcmp(sargv[i+1], "left"))  _scrollpos = POS_LEFT;
			else if (!strcmp(sargv[i+1], "right")) _scrollpos = POS_RIGHT;
		}
	}

	if (keyoption == FALSE)
	{
		print_help();
		exit(1);
	}
	else if (keyoption == TRUE && _key == 0)
	{
		fprintf(stderr, "error: wrong shortcut key is defined (see /usr/include/X11/keysymdef.h)\n");
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

