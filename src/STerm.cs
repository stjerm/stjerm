// Name: STerm.cs
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
	public class STerm: Vte.Terminal
	{
		public PopupMenu menu;
		
		public STerm() : base()
		{
			SetColors(new Gdk.Color(0, 0, 0), new Gdk.Color(255, 255, 255), null, 0);
			Font = Pango.FontDescription.FromString("Mono 10");
			
			menu = new PopupMenu();
			foreach (Gtk.ImageMenuItem item in menu.MenuItems)
			{
				item.Activated += On_MenuItem_Activated;
			}
			
			string[] envv = new string[Environment.GetEnvironmentVariables().Count];
			int i = 0;
			foreach (DictionaryEntry e in Environment.GetEnvironmentVariables())
			{
				if ((string)e.Key == "" || (string)e.Value == "")
				{
					continue;
				}
				string tmp = String.Format("{0}={1}", e.Key, e.Value);
				envv[i] = tmp;
				i++;
			}
			ForkCommand(Environment.GetEnvironmentVariable("SHELL"),
					    Environment.GetCommandLineArgs(), envv,
						Environment.CurrentDirectory, false, true, true
			);
		}
		
		private void Copy()
		{
			CopyClipboard();
		}
		
		private void Paste()
		{
			PasteClipboard();
		}
		
		private void On_MenuItem_Activated(Object o, EventArgs args)
		{
			Gtk.ImageMenuItem item = (Gtk.ImageMenuItem)o;
			switch (item.Name)
			{
				case "New Tab":
					((TermBook)Parent.Parent).NewTab();
					break;
				case "Close Tab":
					((TermBook)Parent.Parent).CloseTab();
					break;
				case "Copy":
					Copy();
					break;
				case "Paste":
					Paste();
					break;
				case "Reset Position and Size":
					((MainWindow)Parent.Parent.Parent).ResetPositionSize();
					break;
				case "About":
					AboutDialog d = new AboutDialog();
					d.Run();
					break;
				case "Quit":
					AppStjerm.Exit(0);
					break;
				default:
					System.Console.WriteLine("ERROR: unknown menu item: {0}",
											 item.Name
					);
					AppStjerm.Exit(1);
					break;
			}
		}
		
		protected override bool OnButtonPressEvent(Gdk.EventButton evnt)
		{
			bool ret = base.OnButtonPressEvent(evnt);
			if (evnt.Button == 3)
			{
				menu.Popup(null, null, null, 3, Gtk.Global.CurrentEventTime);
			}
			return ret;
		}
	}
}
