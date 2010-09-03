/*
 * main.cc
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

#include <gtkmm.h>
#include <gtkmm/window.h>
#include "StjermWindow.h"

int main(int argc, char *argv[])
{
    Gtk::Main kit(argc, argv);

    StjermWindow window;
    
    Gtk::Main::run(window);

    return 0;
}
