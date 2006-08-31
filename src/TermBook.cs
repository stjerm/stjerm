// Name: TermBook.cs
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
	public class TermBook: Gtk.Notebook
	{
		private System.Collections.ArrayList boxes;
		private System.Collections.ArrayList terms;
		private System.Collections.ArrayList scrolls;
		
		public TermBook() : base()
		{
			this.SetSizeRequest(650, 400);
			this.TabPos = Gtk.PositionType.Top;
			this.Scrollable = false;
			this.TabBorder = 0;
			
			this.boxes = new System.Collections.ArrayList();
			this.terms = new System.Collections.ArrayList();
			this.scrolls = new System.Collections.ArrayList();
			this.NewTab();
		}
		
		public void NewTab()
		{
			this.boxes.Add(new Gtk.HBox());
			Gtk.HBox box = (Gtk.HBox)this.boxes[this.boxes.Count-1];
			
			this.terms.Add(new STerm());
			STerm term = (STerm)this.terms[this.terms.Count-1];
			term.ChildExited += this.On_Term_Child_Exited;
			
			this.scrolls.Add(new Gtk.VScrollbar(term.Adjustment));
			Gtk.VScrollbar scroll = (Gtk.VScrollbar)
									this.scrolls[this.scrolls.Count-1];
			
			box.PackStart(term, true, true, 0);
			box.PackEnd(scroll, false, false, 0);
			term.Show();
			box.Show();
			scroll.Show();
						
			this.AppendPage(box, null);
			this.SetTabLabelPacking(box, true, false,
			                        Gtk.PackType.Start
			);
			this.CurrentPage = this.terms.Count - 1;
			
			term.GrabFocus();
		}
		
		public void CloseTab()
		{
			this.boxes.RemoveAt(this.CurrentPage);
			this.terms.RemoveAt(this.CurrentPage);
			this.scrolls.RemoveAt(this.CurrentPage);
			this.RemovePage(this.CurrentPage);
			
			if (this.terms.Count < 1)
			{
				this.NewTab();
			}
		}
		
		private void On_Term_Child_Exited(Object o, System.EventArgs args)
		{
			this.CloseTab();
		}
	}
}
