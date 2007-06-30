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


#include <gtk/gtk.h>
#include "stjerm.h"


int sargc;
char **sargv;

void print_help(void);

void print_help(void)
{
	printf("stjerm version %s\n\n"
           "Usage: %s "
	       "[-fn FONT] "
	       "[-bg COLOR] "
	       "[-fg COLOR] "
	       "[-t TYPE] "
	       "[-o OPACITY]\n       "
	       "[-w WIDTH] "
	       "[-h HEIGHT] "
	       "[-p POSITION] "
	       "[-m MOD] "
	       "-k KEY\n\n"
	       "Options:\n"
	       "  -fn FONT         Terminal font\n"
	       "  -bg COLOR        Background color\n"
	       "  -fg COLOR        Foreground color\n"
	       "  -t TYPE          Transparency type: fake, real, best\n"
	       "  -o OPACITY       Opacity\n"
	       "  -w WIDTH         Window width\n"
	       "  -h HEIGHT        Window height\n"
	       "  -p POSITION      Window position: top, bottom, left, right\n"
	       "  -m MOD           Mod key: Shift, Control, Alt, None\n"
	       "  -k KEY           Shortcut key (See /usr/include/X11/keysymdef.h)\n\n"
	       "NOTE: Mod key and shortcut key options are case sensitive!\n"
	       "      Shortcut key is a required option.\n",
	       STJERM_VERSION,
		   sargv[0]);
}


int main(int argc, char *argv[])
{
	sargc = argc;
	sargv = argv;

	gtk_init(&argc, &argv);
	if (!g_thread_supported())
		g_thread_init(NULL);
	gdk_threads_init();
	
	conf_init();
	build_mainwindow();
	
	gdk_threads_enter();
	gtk_main();
	gdk_threads_leave();
	
    return 0;
}

