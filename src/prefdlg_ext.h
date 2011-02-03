/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2009-2011 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License version 3 as
  published by the Free Software Foundation.

  This software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/



// Dialog box to modify preferences for the file dialog's filter list.
class FileFiltersDlg: public DescListDlg {
  virtual bool Verify(FXString&item);
  virtual void setText(const FXString str);
  virtual void RestoreAppDefaults();
public:
  virtual const FXString& getText();
  FileFiltersDlg(FXWindow* w, const FXString init);
};

