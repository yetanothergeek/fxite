/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2009-2014 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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

#include "color_funcs.h"




static long InvertColor(long rgb)
{
  long r,g,b;
  r=FXREDVAL(rgb);
  g=FXGREENVAL(rgb);
  b=FXBLUEVAL(rgb);
  r=255-r;
  g=255-g;
  b=255-b;
  if ((r>0)&&(r<0x80)) { r+=0x40; }
  if ((g>0)&&(g<0x80)) { g+=0x40; }
  if ((b>0)&&(b<0x80)) { b+=0x40; }
  return FXRGB(r,g,b);
}



static bool invert=false;

void ColorFuncs::InvertColors(bool inverted)
{ 
  invert=inverted;
}



bool ColorFuncs::ColorsInverted() {
  return invert;
}



void ColorFuncs::RgbToHex(FXColor rgb, ColorName &clr)
{
  if (invert) { rgb=InvertColor(rgb); }
  snprintf(clr, 8, "#%02x%02x%02x", FXREDVAL(rgb), FXGREENVAL(rgb), FXBLUEVAL(rgb));
}



long ColorFuncs::HexToRGB(const char* rgb)
{
  int r=0, g=0, b=0;
  sscanf(rgb+1,"%2x%2x%2x",&r,&g,&b);
  return invert? InvertColor(FXRGB(r, g, b)) : FXRGB(r, g, b);
}

