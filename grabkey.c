/*
 * grabkey.c
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
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include "stjerm.h"


void grab_key(void);


void grab_key(void)
{
	Display *dpy;
	Window root;
	int screen;
    
    if (!(dpy = XOpenDisplay(NULL)))
    {
		fprintf(stderr, "error: can not open display %s", XDisplayName(NULL));
		exit(1);
	}
	screen = DefaultScreen(dpy);
	root = RootWindow(dpy, screen);
	
	
	KeySym opt_key = conf_get_key();
	unsigned int modmask = conf_get_mod();
	
	
	int i, j;
	unsigned int numlockmask;
	XModifierKeymap *modmap = XGetModifierMapping(dpy);
	
	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < modmap->max_keypermod; j++)
		{
			if (modmap->modifiermap[i * modmap->max_keypermod + j] ==
			    XKeysymToKeycode(dpy, XK_Num_Lock))
			{
				numlockmask = (1 << i);
			}
		}
	}
	XFreeModifiermap(modmap);
	
	
	XGrabKey(dpy, XKeysymToKeycode(dpy, opt_key), modmask, root, True,
	         GrabModeAsync, GrabModeAsync);
	XGrabKey(dpy, XKeysymToKeycode(dpy, opt_key), LockMask|modmask, root, True,
	         GrabModeAsync, GrabModeAsync);
	
	if (numlockmask)
	{
		XGrabKey(dpy, XKeysymToKeycode(dpy, opt_key), numlockmask|modmask,
		         root, True, GrabModeAsync, GrabModeAsync);
		XGrabKey(dpy, XKeysymToKeycode(dpy, opt_key),
		         numlockmask|LockMask|modmask, root, True,
		         GrabModeAsync, GrabModeAsync);
	}
	
	
	XEvent event;
	while (1)
	{
		XNextEvent(dpy, &event);
		if (event.type == KeyPress)
		{
			if (XKeycodeToKeysym(dpy, event.xkey.keycode, 0) == opt_key)
				mainwindow_show();
		}
	}
}

