// Name: PopupMenu.cs
// Date: 2006 08 18
// Author: Stjepan Glavina
// Email: stjepang AT gmail DOT com
// 
// Copyright 2006 Stjepan Glavina
// 
// This file is part of Stjerm.
//
// Stjerm is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// Stjerm is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Stjerm; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

using System;
using System.Collections;

namespace Stjerm
{
	public class PopupMenu: Gtk.Menu
	{
		private ArrayList items;
		
		public ArrayList MenuItems
		{
			get
			{
				return items;
			}
			private set
			{
				items = value;
			}
		}
		
		public PopupMenu()
		{
			MenuItems = new ArrayList();
			
			string[] lbls = {"New Tab",
							 "Close Tab",
							 "Copy",
							 "Paste",
							 "Preferences",
							 "Reset Position and Size",
							 "About",
							 "Quit"
			};
			
			string[] imgs = {Gtk.Stock.New.ToString(),
							 Gtk.Stock.Close.ToString(),
							 Gtk.Stock.Copy.ToString(),
							 Gtk.Stock.Paste.ToString(),
							 Gtk.Stock.Preferences.ToString(),
							 Gtk.Stock.Refresh.ToString(),
							 Gtk.Stock.About.ToString(),
							 Gtk.Stock.Quit.ToString()
			};
			
			for (int i = 0; i < lbls.Length; i++)
			{
				string lbl = lbls[i];
				string img = imgs[i];
				
				if (i == 2 || i == 4 || i == 6)
				{
					Gtk.SeparatorMenuItem sepitem = new Gtk.SeparatorMenuItem();
					sepitem.Show();
					Append(sepitem);
				}
				
				MenuItems.Add(new Gtk.ImageMenuItem(lbl));
				Gtk.ImageMenuItem item = (Gtk.ImageMenuItem)MenuItems[i];
				item.Name = lbl;
				item.Image = new Gtk.Image(img, Gtk.IconSize.Menu);
				item.Show();
				Append(item);
			}
		}
	}
}
