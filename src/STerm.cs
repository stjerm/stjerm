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

namespace Stjerm
{
	public class STerm: Vte.Terminal
	{
		public PopupMenu menu;
		
		public STerm() : base()
		{
			this.SetColors(new Gdk.Color(0, 0, 0), new Gdk.Color(255, 255, 255), null, 0);
			this.Font = Pango.FontDescription.FromString("Mono 10");
			
			Gtk.TargetEntry te = new Gtk.TargetEntry();
			te.Flags = Gtk.TargetFlags.Widget;
			te.Target = "text/uri-list";
			Gtk.TargetEntry[] tes = new Gtk.TargetEntry[] {te};
			Gtk.Drag.DestSet(this, Gtk.DestDefaults.All, tes, Gdk.DragAction.Ask);
			this.DragDrop += this.On_DragDrop;
			this.MotionNotifyEvent += this.On_Motion;
			
			this.menu = new PopupMenu();
			foreach (Gtk.ImageMenuItem item in this.menu.MenuItems)
			{
				item.Activated += this.On_MenuItem_Activated;
			}
			
			string[] envv = new
						string[Environment.GetEnvironmentVariables().Count];
			int i = 0;
			foreach (System.Collections.DictionaryEntry e in
					 Environment.GetEnvironmentVariables())
			{
				if ((string)e.Key == "" || (string)e.Value == "")
				{
					continue;
				}
				string tmp = String.Format("{0}={1}", e.Key, e.Value);
				envv[i] = tmp;
				i++;
			}
			this.ForkCommand(Environment.GetEnvironmentVariable("SHELL"),
							 Environment.GetCommandLineArgs(), envv,
							 Environment.CurrentDirectory, false, true, true
			);
		}
		
		private void Copy()
		{
			this.CopyClipboard();
		}
		
		private void Paste()
		{
			this.PasteClipboard();
		}
		
		private void On_MenuItem_Activated(Object o, System.EventArgs args)
		{
			Gtk.ImageMenuItem item = (Gtk.ImageMenuItem)o;
			switch (item.Name)
			{
				case "New Tab":
					((TermBook)this.Parent.Parent).NewTab();
					break;
				case "Close Tab":
					((TermBook)this.Parent.Parent).CloseTab();
					break;
				case "Copy":
					this.Copy();
					break;
				case "Paste":
					this.Paste();
					break;
				case "Reset Position and Size":
					((MainWindow)this.Parent.Parent.Parent).ResetPositionSize();
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
		
		private void On_DragDrop(Object o, Gtk.DragDropArgs args)
		{
			System.Console.WriteLine("dragdrop");
		}
		
		private void On_Motion(Object o, Gtk.MotionNotifyEventArgs args)
		{
			System.Console.WriteLine("motion");
		}
		
		protected override bool OnButtonPressEvent(Gdk.EventButton evnt)
		{
			bool ret = base.OnButtonPressEvent(evnt);
			if (evnt.Button == 3)
			{
				this.menu.Popup(null, null, null, 3, Gtk.Global.CurrentEventTime);
			}
			return ret;
		}
	}
}
