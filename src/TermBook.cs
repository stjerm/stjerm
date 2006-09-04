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
using System.Collections;

namespace Stjerm
{
	public class TermBook: Gtk.Notebook
	{
		private ArrayList boxes;
		private ArrayList terms;
		private ArrayList scrolls;
		
		public TermBook() : base()
		{
			SetSizeRequest(650, 400);
			TabPos = Gtk.PositionType.Top;
			Scrollable = false;
			TabBorder = 0;
			
			boxes = new ArrayList();
			terms = new ArrayList();
			scrolls = new ArrayList();
			NewTab();
		}
		
		public void NewTab()
		{
			boxes.Add(new Gtk.HBox());
			Gtk.HBox box = (Gtk.HBox)boxes[boxes.Count-1];
			
			terms.Add(new STerm());
			STerm term = (STerm)terms[terms.Count-1];
			term.ChildExited += On_Term_Child_Exited;
			
			scrolls.Add(new Gtk.VScrollbar(term.Adjustment));
			Gtk.VScrollbar scroll = (Gtk.VScrollbar)
									scrolls[scrolls.Count-1];
			
			box.PackStart(term, true, true, 0);
			box.PackEnd(scroll, false, false, 0);
			term.Show();
			box.Show();
			scroll.Show();
						
			AppendPage(box, null);
			SetTabLabelPacking(box, true, false,
			                   Gtk.PackType.Start
			);
			CurrentPage = terms.Count - 1;
			
			term.GrabFocus();
		}
		
		public void CloseTab()
		{
			boxes.RemoveAt(CurrentPage);
			terms.RemoveAt(CurrentPage);
			scrolls.RemoveAt(CurrentPage);
			RemovePage(CurrentPage);
			
			if (terms.Count < 1)
			{
				NewTab();
			}
		}
		
		private void On_Term_Child_Exited(Object o, System.EventArgs args)
		{
			CloseTab();
		}
	}
}
