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
#include "popmenu.h"

FXDEFMAP(PopUpMnuCmd) PopUpMnuCmdMap[]={ FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,0,PopUpMnuCmd::onButtonRelease) };
FXIMPLEMENT(PopUpMnuCmd,FXMenuCommand,PopUpMnuCmdMap,ARRAYNUMBER(PopUpMnuCmdMap));



PopUpMnuCmd::PopUpMnuCmd(FXComposite* p,const FXString& text,FXIcon* ic,FXObject* tgt,FXSelector sel):
     FXMenuCommand(p,text,ic,tgt,sel), CreationTime(FXThread::time())
{ 
  CreationTime=FXThread::time();
}


long PopUpMnuCmd::onButtonRelease(FXObject*o,FXSelector sel,void*p)
{
  return (FXThread::time()-CreationTime)<500000000 ? 1 : FXMenuCommand::onButtonRelease(o,sel,p);
}
