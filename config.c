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

/*
 * OPTIONS:
 * -fn font
 * -o opacity
 * -t transparency (fake, real, best)
 * -bg bg color
 * -fg fg color
 * -m mod key (shift, control, alt)
 * -k shortcut key
 * -w width
 * -h height
 * -p position (top, bottom)
 *
 */

#include <gtk/gtk.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include "stjerm.h"


extern int sargc;
extern char **sargv;

char* conf_get_font(void);
float conf_get_opacity(void);
int conf_get_transparency(void);
GdkColor conf_get_bg(void);
GdkColor conf_get_fg(void);
unsigned int conf_get_mod(void);
KeySym conf_get_key(void);
int conf_get_width(void);
int conf_get_height(void);
int conf_get_position(void);


char* conf_get_font(void)
{
	return "Bitstream Vera Sans Mono 10";
}


float conf_get_opacity(void)
{
	return 0.9f;
}


int conf_get_transparency(void)
{
	return TRANS_REAL;
}


GdkColor conf_get_bg(void)
{
	GdkColor c;
	c.red   = 0x0000;
	c.green = 0x0000;
	c.blue  = 0x0000;

	return c;
}


GdkColor conf_get_fg(void)
{
	GdkColor c;
	c.red   = 0xffff;
	c.green = 0xffff;
	c.blue  = 0xffff;

	return c;
}


unsigned int conf_get_mod(void)
{
	return Mod1Mask;
}


KeySym conf_get_key(void)
{
	return XK_f;
}


int conf_get_width(void)
{
	return 800;
}


int conf_get_height(void)
{
	return 400;
}


int conf_get_position(void)
{
	return POS_TOP;
}

