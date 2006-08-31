// Name: AboutDialog.cs
// Date: 2006 08 29
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
	public class AboutDialog: Gtk.AboutDialog
	{
		public AboutDialog()
		{
			this.Authors = StjermInfo.Authors;
			this.Comments = StjermInfo.Comments;
			this.Copyright = StjermInfo.Copyright;
			this.License = StjermInfo.License;
			this.Name = StjermInfo.Name;
			this.Version = StjermInfo.Version;
			this.Website = StjermInfo.Website;
			this.License = StjermInfo.License;
			this.Response += this.On_Response;
		}
		
		private void On_Response(Object o, Gtk.ResponseArgs args)
		{
			if (args.ResponseId == Gtk.ResponseType.Cancel ||
				args.ResponseId == Gtk.ResponseType.DeleteEvent)
			{
				this.Destroy();
			}
		}
	}
}
