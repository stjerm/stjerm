// Name: MainWindow.cs
// Date: 2006 08 14
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
	public class MainWindow: Gtk.Window
	{
		private TermBook tbook;
		
		public MainWindow(): base("Stjerm")
		{
			this.DeleteEvent += this.On_Window_Delete;
			this.ExposeEvent += this.On_Expose;
			
			this.tbook = new TermBook();
			this.Add(tbook);
			this.SetSizeRequest(690, 440);
			this.ResetPositionSize();
			this.TypeHint = Gdk.WindowTypeHint.Normal;
			this.Decorated = false;
			this.KeepAbove = true;
			this.SkipPagerHint = true;
			this.SkipTaskbarHint = true;
			this.Stick();
			this.BorderWidth = 8;
			this.tbook.Show();
			this.ShowAll();
		}
		
		public void ResetPositionSize()
		{
			int sw = Gdk.Screen.Default.Width;
			int ww, wh;
			this.GetSize(out ww, out wh);
			this.Move(sw/2 - ww/2, 0);
		}
		
		private void On_Window_Delete(Object o, Gtk.DeleteEventArgs args)
		{
			// do nothing
		}
		
		private void On_Expose(Object o, Gtk.ExposeEventArgs args)
		{
			int width, height;
			this.GetSize(out width, out height);
			Gtk.Widget w = (Gtk.Widget)o;
			
			for (int i = 0; i < 5; i++)
			{
				w.GdkWindow.DrawRectangle(
								w.Style.BackgroundGC(Gtk.StateType.Selected),
								false,
								i, i,
								width - i * 2 -1, height - i * 2 - 1
				);
			}
			
			for (int i = 5; i < 8; i++)
			{
				w.GdkWindow.DrawRectangle(
								w.Style.BackgroundGC(Gtk.StateType.Active),
								false,
								i, i,
								width - i * 2 - 1, height - i * 2 - 1
				);
			}
		}
	}
}
