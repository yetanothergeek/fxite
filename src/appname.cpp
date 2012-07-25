/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2009-2012 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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


#include <fx.h>
#include <Scintilla.h>
#include <FXScintilla.h>
#include <lua.h>

#include "fxasq.h"
#include "intl.h"
#include "appname.h"

static const char *Scintilla_Copyright =" \
The Scintilla editing control and the PDF/HTML export code are\n\
copyright 1998-2012 by Neil Hodgson <neilh@scintilla.org>\n\
and are released under the following license:\n\
\n\
All Rights Reserved\n\
Permission to use, copy, modify, and distribute this software and its\n\
documentation for any purpose and without fee is hereby granted,\n\
provided that the above copyright notice appear in all copies and that\n\
both that copyright notice and this permission notice appear in\n\
supporting documentation.\n\
Neil Hodgson disclaims all warranties with regard to this\n\
software, including all implied warranties of merchantability\n\
and fitness, in no event shall Neil Hodgson be liable for any\n\
special, indirect or consequential damages or any damages\n\
whatsoever resulting from loss of use, data or profits,\n\
whether in an action of contract, negligence or other\n\
tortious action, arising out of or in connection with the use\n\
or performance of this software.\n\
\n\
For more information, visit http://scintilla.org/\
";

#ifdef FOX_1_6
// Fox 1.6 message box won't accept strings longer than ~1000 chars,
// so split the license across two dialogs...
static const char* Lua_License ="\
The Lua scripting engine is released under the following license:\n\
\n\
Copyright (C) 1994-2008 Lua.org, PUC-Rio.\n\
Permission is hereby granted, free of charge, to any person obtaining a copy\n\
of this software and associated documentation files (the \"Software\"), to deal\n\
in the Software without restriction, including without limitation the rights\n\
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n\
copies of the Software, and to permit persons to whom the Software is\n\
furnished to do so, subject to the following conditions:\n\
";
static const char* Lua_License_2 ="\
The preceeding copyright notice and this permission notice shall be included in\n\
all copies or substantial portions of the Software.\n\
The software is provided \"AS IS\", without warranty of any kind, express or\n\
implied, including but not limited to the warranties of merchantability,\n\
fitness for a particular purpose and noninfringement.  In no event shall the\n\
authors or copyright holders be liable for any claim, damages or other\n\
liability, whether in an action of contract, tort or otherwise, arising from,\n\
out of or in connection with the software or the use or other dealings in\n\
the software.\n\
\n\
For more information, visit http://www.lua.org/license.html .\
";
#else
static const char* Lua_License ="\
The Lua scripting engine is released under the following license:\n\
\n\
Copyright (C) 1994-2008 Lua.org, PUC-Rio.\n\
Permission is hereby granted, free of charge, to any person obtaining a copy\n\
of this software and associated documentation files (the \"Software\"), to deal\n\
in the Software without restriction, including without limitation the rights\n\
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n\
copies of the Software, and to permit persons to whom the Software is\n\
furnished to do so, subject to the following conditions:\n\
The above copyright notice and this permission notice shall be included in\n\
all copies or substantial portions of the Software.\n\
The software is provided \"AS IS\", without warranty of any kind, express or\n\
implied, including but not limited to the warranties of merchantability,\n\
fitness for a particular purpose and noninfringement.  In no event shall the\n\
authors or copyright holders be liable for any claim, damages or other\n\
liability, whether in an action of contract, tort or otherwise, arising from,\n\
out of or in connection with the software or the use or other dealings in\n\
the software.\n\
\n\
For more information, visit http://www.lua.org/license.html .\
";
#endif

static const char* Fox_LGPL="\
   This software uses the FOX Toolkit Library, released   \n\
under the GNU Lesser General Public License and \n\
the FOX Library License addendum.\n\
\n\
For more details, visit http://www.fox-toolkit.org\
";

#define App_About "\
Free eXtensIble Text Editor (FXiTe) %s %s\n\n\
Copyright (c) 2009-2012 Jeffrey Pohlmeyer\n\
<%s>\n\n\
%s\
GNU GENERAL PUBLIC LICENSE Version 3\n\n\
%s FOX-%d.%d.%d; FXScintilla-%s; %s\n\
"

void AppAbout::VersionInfo()
{
  fxmessage(App_About, _("Version"), VERSION, "yetanothergeek@gmail.com",
    _("This program is free software, under the terms of the\n"),
    _("Running"),
    fxversion[0],fxversion[1],fxversion[2],
    FXScintilla::version().text(),
    LUA_RELEASE);
}

void AppAbout::AboutBox()
{
  int i=0;
  while (1) {
    const char *btns[]={
      _(" About &Scintilla "),
      _(" About &Lua "),
      _(" About &FOX "),
      _(" &Close   "),NULL};
    const char*btn[]={_("    &Close    "), NULL};
    FxAsqWin*dlg=NULL;
    FXString msg;
    int rv=-1;
    dlg=new FxAsqWin(_("About "EXE_NAME), btns, i++);
    msg.format(App_About, _("Version"), VERSION, "yetanothergeek@gmail.com",
      _("  This program is free software, under the terms of the  \n"),
      _("Running"),
      fxversion[0],fxversion[1],fxversion[2],
      FXScintilla::version().text(),
      LUA_RELEASE);
    dlg->Label(msg.text(), JUSTIFY_CENTER_X);
    rv=dlg->Run(NULL);
    delete dlg;
    switch (rv) {
      case 0: {
        dlg=new FxAsqWin(_("About Scintilla"), btn);
        dlg->Label(Scintilla_Copyright);
        dlg->Run();
        delete dlg;
        break;
      }
      case 1: {
        dlg=new FxAsqWin(_("About Lua"), btn);
        dlg->Label(Lua_License);
  #ifdef FOX_1_6
        dlg->Label(Lua_License_2);
  #endif
        dlg->Run();
        delete dlg;
        break;
      }
      case 2: {
        dlg=new FxAsqWin(_("About FOX Toolkit"), btn);
        dlg->Label(Fox_LGPL,JUSTIFY_CENTER_X);
        dlg->Run();
        delete dlg;
        break;
      }
      default: { return; }
    }
  }
}


