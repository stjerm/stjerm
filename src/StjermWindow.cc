/*
 * StjermWindow.cc
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

#include "StjermWindow.h"

StjermWindow::StjermWindow()
{
    set_app_paintable(TRUE);
    set_size_request(500, 200); // TODO conf_get_width(), conf_get_height()
    set_decorated(FALSE);
    set_skip_taskbar_hint(TRUE);
    set_skip_pager_hint(TRUE);
    set_resizable(TRUE);
    
    move(300, 400);
    
    GtkWidget* term = vte_terminal_new();
    
    vte_terminal_fork_command(VTE_TERMINAL(term), getpwuid(getuid())->pw_shell, NULL, NULL,
        "", TRUE, TRUE, TRUE);
    
    add(_box);

    //_notebook.set_border_width(10);
    _box.pack_start(_notebook);
    
    _notebook.append_page(*Glib::wrap(term), "Second");


    show_all_children();
}


StjermWindow::~StjermWindow()
{
    
}

