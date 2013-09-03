/** FXScintilla source code edit control
 *
 *  PlatFOX.cxx - implementation of platform facilities on the FOX toolkit
 *
 *  Copyright 2001-2004 by Gilles Filippini <gilles.filippini@free.fr>
 *
 *  Adapted from the Scintilla source PlatGTK.cxx 
 *  Copyright 1998-2004 by Neil Hodgson <neilh@scintilla.org>
 *
 *  ====================================================================
 *
 *  This file is part of FXScintilla.
 * 
 *  FXScintilla is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2.1 of the License, or
 *  (at your option) any later version.
 *
 *  FXScintilla is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with FXScintilla; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 **/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#if !defined(WIN32) || defined(__CYGWIN__)
# if defined(__CYGWIN__)
#  include <windows.h>
#  ifdef PIC
#   define FOXDLL
#  endif
# endif
# include <sys/time.h>
#  include <fx.h>
#  include <fxkeys.h>
#  include <FXRootWindow.h>
#else
# if defined(__MINGW32__) && defined(PIC) && !defined(FOXDLL)
#   define FOXDLL
# endif
# pragma warning (disable : 4786)
# include <time.h>
# include <windows.h>
# include <fx.h>
# include <fxkeys.h>
#endif  // !defined(WIN32) || defined(__CYGWIN__)

#include <FX88591Codec.h>

#include "Platform.h"
#include "Scintilla.h"
#include "ScintillaWidget.h"
#include "FXScintilla.h"

#include <map>
using namespace std;

// X has a 16 bit coordinate space, so stop drawing here to avoid wrapping
static const int maxCoordinate = 32000;

Point Point::FromLong(long lpoint) {
  return Point(
    Platform::LowShortFromLong(lpoint), 
    Platform::HighShortFromLong(lpoint));
}

Font::Font() : fid(0) {}

Font::~Font() {}

#ifndef WIN32

void Font::Create(const FontParameters &fp) {
  Release();
  // If name of the font begins with a '-', assume, that it is
  // a full fontspec.
  if (fp.faceName[0] == '-') {
    fid = new FXFont(FXApp::instance(), fp.faceName);
  }
  else {
        fid = new FXFont(FXApp::instance(), fp.faceName, fp.size,
          fp.weight>400 ? FXFont::Bold : FXFont::Normal ,
      fp.italic ? FXFont::Italic : FXFont::Straight,
      fp.characterSet);
  }
  if (!fid) {
    // Font not available so substitute with the app default font.
    fid = FXApp::instance()->getNormalFont();
  }
  if (fid)
    fid->create();
}

#else // WIN32

static int CharacterSetCode(int characterSet) {
  switch (characterSet) {
  case SC_CHARSET_ANSI:
    return FONTENCODING_DEFAULT;
  case SC_CHARSET_DEFAULT:
    return FONTENCODING_DEFAULT;
  case SC_CHARSET_BALTIC:
    return FONTENCODING_BALTIC;
  case SC_CHARSET_CHINESEBIG5:
    return FONTENCODING_DEFAULT;
  case SC_CHARSET_EASTEUROPE:
    return FONTENCODING_EASTEUROPE;
  case SC_CHARSET_GB2312:
    return FONTENCODING_DEFAULT;
  case SC_CHARSET_GREEK:
    return FONTENCODING_GREEK;
  case SC_CHARSET_HANGUL:
    return FONTENCODING_DEFAULT;
  case SC_CHARSET_MAC:
    return FONTENCODING_DEFAULT;
  case SC_CHARSET_OEM:
    return FONTENCODING_DEFAULT;
  case SC_CHARSET_RUSSIAN:
    return FONTENCODING_RUSSIAN;
  case SC_CHARSET_CYRILLIC:
    return FONTENCODING_CYRILLIC;
  case SC_CHARSET_SHIFTJIS:
    return FONTENCODING_DEFAULT;
  case SC_CHARSET_SYMBOL:
    return FONTENCODING_DEFAULT;
  case SC_CHARSET_TURKISH:
    return FONTENCODING_TURKISH;
  case SC_CHARSET_JOHAB:
    return FONTENCODING_DEFAULT;
  case SC_CHARSET_HEBREW:
    return FONTENCODING_HEBREW;
  case SC_CHARSET_ARABIC:
    return FONTENCODING_ARABIC;
  case SC_CHARSET_VIETNAMESE:
    return FONTENCODING_CP1258;     /// Windows Vietnam
  case SC_CHARSET_THAI:
    return FONTENCODING_THAI;
  case SC_CHARSET_8859_15:
    return FONTENCODING_ISO_8859_15;
  default:
    return FONTENCODING_DEFAULT;
  }
}

void Font::Create(const FontParameters &fp) {
  Release();
  fid = new FXFont(FXApp::instance(), fp.faceName, fp.size,
          fp.weight>400 ? FXFont::Bold : FXFont::Normal ,
      fp.italic ? FXFont::Italic : FXFont::Straight,
      CharacterSetCode(fp.characterSet));
  if (!fid) {
    // Font not available so substitute with the app default font.
    fid = FXApp::instance()->getNormalFont();
  }
  if (fid)
    fid->create();
}

#endif // WIN32

void Font::Release() {
  if (fid)
    delete fid;
  fid = 0;
}

// ====================================================================
// Surface
// ====================================================================

class SurfaceImpl : public Surface {
  bool unicodeMode;
  FXDrawable *drawable;
  FXImage *ppixmap;
  static SurfaceImpl *s_dc_owner;
  FXDCWindow *_dc;
  FXDCWindow *dc();
  int x;
  int y;
  bool inited;
  bool createdDC;
  FX88591Codec asciiCodec;
  FXString codecBuffer;
public:
  SurfaceImpl();
  virtual ~SurfaceImpl();

  void Init(WindowID wid);
  void Init(SurfaceID sid, WindowID wid);
  void InitPixMap(int width, int height, Surface *surface_, WindowID wid);

  void Release();
  bool Initialised();
  void PenColour(ColourDesired fore);
  void BackColour(ColourDesired back);
  int LogPixelsY();
  int DeviceHeightFont(int points);
  void MoveTo(int x_, int y_);
  void LineTo(int x_, int y_);
  void Polygon(Point *pts, int npts, ColourDesired fore, ColourDesired back);
  void RectangleDraw(PRectangle rc, ColourDesired fore, ColourDesired back);
  void FillRectangle(PRectangle rc, ColourDesired back);
  void FillRectangle(PRectangle rc, Surface &surfacePattern);
  void RoundedRectangle(PRectangle rc, ColourDesired fore, ColourDesired back);
  void AlphaRectangle(PRectangle rc, int cornerSize, ColourDesired fill, int alphaFill,
    ColourDesired outline, int alphaOutline, int flags);
  void Ellipse(PRectangle rc, ColourDesired fore, ColourDesired back);
  void Copy(PRectangle rc, Point from, Surface &surfaceSource);

  void DrawTextBase(PRectangle rc, Font &font_, int ybase, const char *s, int len, ColourDesired fore);
  void DrawTextNoClip(PRectangle rc, Font &font_, XYPOSITION ybase, const char *s, int len, ColourDesired fore, ColourDesired back);
  void DrawTextClipped(PRectangle rc, Font &font_, XYPOSITION ybase, const char *s, int len, ColourDesired fore, ColourDesired back);
  void DrawTextTransparent(PRectangle rc, Font &font_, XYPOSITION ybase, const char *s, int len, ColourDesired fore);
  void MeasureWidths(Font &font_, const char *s, int len, XYPOSITION *positions);
  XYPOSITION WidthText(Font &font_, const char *s, int len);
  XYPOSITION WidthChar(Font &font_, char ch);
  XYPOSITION Ascent(Font &font_);
  XYPOSITION Descent(Font &font_);
  XYPOSITION InternalLeading(Font &font_);
  XYPOSITION ExternalLeading(Font &font_);
  XYPOSITION Height(Font &font_);
  XYPOSITION AverageCharWidth(Font &font_);

  void SetClip(PRectangle rc);
  void FlushCachedState();

  void SetUnicodeMode(bool unicodeMode_);
  virtual void SetDBCSMode(int /* codePage */) {}
  virtual void DrawRGBAImage(PRectangle rc, int width, int height, const unsigned char *pixelsImage);
};

SurfaceImpl * SurfaceImpl::s_dc_owner = NULL;

SurfaceImpl::SurfaceImpl() : unicodeMode(false), drawable(0), ppixmap(0), _dc(0),
x(0), y(0), inited(false) {}


SurfaceImpl::~SurfaceImpl() {
  Release();
}

FXDCWindow * SurfaceImpl::dc()
{
  if (s_dc_owner != this) {
    if (s_dc_owner) {
      delete s_dc_owner->_dc;
      s_dc_owner->_dc = NULL;
    }
    s_dc_owner = this;
    _dc = (drawable) ? new FXDCWindow(drawable) : NULL;
    // Ask for lines that do not paint the last pixel so is like Win32
    if (_dc)
      _dc->setLineCap(CAP_NOT_LAST);
  }
  return _dc;
    
}

void SurfaceImpl::Release() {
  drawable = 0;
  if (_dc) {
    delete _dc;
    _dc = 0;
    s_dc_owner = 0;
  }
  if (ppixmap)
    delete ppixmap;
  ppixmap = 0;
  x = 0;
  y = 0;
  createdDC = false;
  inited = false;
}

bool SurfaceImpl::Initialised() {
  return inited;
}

void SurfaceImpl::Init(WindowID) {
  Release();
  inited = true;
}

void SurfaceImpl::Init(SurfaceID sid, WindowID) {
  Release();
  drawable = reinterpret_cast<FXDrawable *>(sid);
  createdDC = true;
  inited = true;
}

void SurfaceImpl::InitPixMap(int width, int height, Surface*, WindowID) {
  Release();
  if (height > 0 && width > 0)
    ppixmap = new FXImage(FXApp::instance(), NULL, 0, width, height);
  else
    ppixmap = NULL;
  drawable = ppixmap;
  if (drawable)
    drawable->create();
  createdDC = true;
  inited = true;
}

#if defined(FOX_1_7) && ((FOX_MAJOR>1)||(FOX_MINOR>7)||(FOX_LEVEL>25))
# define SCI_TO_FOX_COLOR(cd) FXRGB(cd.GetBlue(), cd.GetGreen(), cd.GetRed())
#else
# define SCI_TO_FOX_COLOR(cd) FXRGB(cd.GetRed(), cd.GetGreen(), cd.GetBlue())
#endif

void SurfaceImpl::PenColour(ColourDesired fore) {
  if (dc()) {
    ColourDesired cd(fore.AsLong());
    _dc->setForeground(SCI_TO_FOX_COLOR(cd));
  }
}

void SurfaceImpl::BackColour(ColourDesired back) {
  if (dc()) {
    ColourDesired cd(back.AsLong());
    _dc->setBackground(SCI_TO_FOX_COLOR(cd));
  }
}

int SurfaceImpl::LogPixelsY() {
  return 72;
}

int SurfaceImpl::DeviceHeightFont(int points) {
  int logPix = LogPixelsY();
  return (points * logPix + logPix / 2) / 72;
}

void SurfaceImpl::MoveTo(int x_, int y_) {
  x = x_;
  y = y_;
}

void SurfaceImpl::LineTo(int x_, int y_) {
  if (dc()) {
    _dc->drawLine(x, y, x_, y_);
  }
  x = x_;
  y = y_;
}

void SurfaceImpl::Polygon(Point *pts, int npts, ColourDesired fore,
                      ColourDesired back) {
  if (dc()) {
    FXPoint gpts[20];
    if (npts < static_cast<int>((sizeof(gpts)/sizeof(gpts[0])))) {
      for (int i=0;i<npts;i++) {
        gpts[i].x = pts[i].x;
        gpts[i].y = pts[i].y;
      }
      gpts[npts].x = pts[0].x;
      gpts[npts].y = pts[0].y;
      PenColour(back);
      _dc->fillPolygon(gpts, npts);
      PenColour(fore);
      _dc->drawLines(gpts, npts + 1);
    }
  }
}

void SurfaceImpl::RectangleDraw(PRectangle rc, ColourDesired fore, ColourDesired back) {
  if (dc()) {
    PenColour(fore);
    BackColour(back);
    _dc->drawRectangle(rc.left, rc.top,
                      rc.right - rc.left + 1, rc.bottom - rc.top + 1);
  }
}

void SurfaceImpl::FillRectangle(PRectangle rc, ColourDesired back) {
  if (dc() && (rc.left < maxCoordinate)) {  // Protect against out of range
    // GTK+ rectangles include their lower and right edges
    rc.bottom--;
    rc.right--;
    PenColour(back);
    _dc->fillRectangle(rc.left, rc.top,
            rc.right - rc.left + 1, rc.bottom - rc.top + 1);
  }
}

void SurfaceImpl::FillRectangle(PRectangle rc, Surface &surfacePattern) {
  if (static_cast<SurfaceImpl &>(surfacePattern).drawable) {
    if (dc()) {
      // Tile pattern over rectangle
      // Currently assumes 8x8 pattern
      int widthPat = 8;
      int heightPat = 8;
      for (int xTile = rc.left; xTile < rc.right; xTile += widthPat) {
        int widthx = (xTile + widthPat > rc.right) ? rc.right - xTile : widthPat;
        for (int yTile = rc.top; yTile < rc.bottom; yTile += heightPat) {
          int heighty = (yTile + heightPat > rc.bottom) ? rc.bottom - yTile : heightPat;
          _dc->drawArea(static_cast<SurfaceImpl &>(surfacePattern).drawable,
                        0, 0,
                        widthx, heighty,
                        xTile, yTile);
        }
      }
    }
  } else {
    // Something is wrong so try to show anyway
    // Shows up black because colour not allocated
    FillRectangle(rc, ColourDesired(0));
  }
}

void SurfaceImpl::RoundedRectangle(PRectangle rc, ColourDesired fore, ColourDesired back) {
  if (((rc.right - rc.left) > 4) && ((rc.bottom - rc.top) > 4)) {
    // Approximate a round rect with some cut off corners
    Point pts[] = {
        Point(rc.left + 2, rc.top),
        Point(rc.right - 2, rc.top),
        Point(rc.right, rc.top + 2),
        Point(rc.right, rc.bottom - 2),
        Point(rc.right - 2, rc.bottom),
        Point(rc.left + 2, rc.bottom),
        Point(rc.left, rc.bottom - 2),
        Point(rc.left, rc.top + 2),
    };
    Polygon(pts, sizeof(pts) / sizeof(pts[0]), fore, back); 
  } else {
    RectangleDraw(rc, fore, back);
  }
}

// Plot a point into a guint32 buffer symetrically to all 4 qudrants
static void AllFour(FXImage *image, int width, int height, int x, int y, FXColor color) {
  image->setPixel(x, y, color);
  image->setPixel(width-1-x, y, color);
  image->setPixel(x, height-1-y, color);
  image->setPixel(width-1-x, height-1-y, color);
}

static unsigned int GetRed(unsigned int co) {
  return (co >> 16) & 0xff;
}

static unsigned int GetGreen(unsigned int co) {
  return (co >> 8) & 0xff;
}

static unsigned int GetBlue(unsigned int co) {
  return co & 0xff;
}

void SurfaceImpl::AlphaRectangle(PRectangle rc, int cornerSize, ColourDesired fill, int alphaFill,
    ColourDesired outline, int alphaOutline, int flags) {
  (void)flags;
  if (dc()) {
    int width = rc.Width();
    int height = rc.Height();
    // Ensure not distorted too much by corners when small
    cornerSize = Platform::Minimum(cornerSize, (Platform::Minimum(width, height) / 2) - 2);
    // Make a 32 bit deep image
    FXImage * image = new FXImage(FXApp::instance(), NULL, IMAGE_OWNED, width, height);

    FXColor valEmpty = 0;
    FXColor valFill = FXRGBA(GetRed(fill.AsLong()), GetGreen(fill.AsLong()), GetBlue(fill.AsLong()), alphaFill);
    FXColor valOutline = FXRGBA(GetRed(outline.AsLong()), GetGreen(outline.AsLong()), GetBlue(outline.AsLong()), alphaOutline);
    for (int pY=0; pY<height; pY++) {
      for (int pX=0; pX<width; pX++) {
        if ((pX==0) || (pX==width-1) || (pY == 0) || (pY == height-1)) {
          image->setPixel(pX, pY, valOutline);
        } else {
          image->setPixel(pX, pY, valFill);
        }
      }
    }
    for (int c=0;c<cornerSize; c++) {
      for (int pX=0;pX<c+1; pX++) {
        AllFour(image, width, height, pX, c-pX, valEmpty);
      }
    }
    for (int pX=1;pX<cornerSize; pX++) {
      AllFour(image, width, height, pX, cornerSize-pX, valOutline);
    }

    // Draw with alpha
    image->create();
    _dc->drawImage(image, rc.left, rc.top);
  }
}


void SurfaceImpl::Ellipse(PRectangle rc, ColourDesired fore, ColourDesired back) {
  if (dc()) {
    PenColour(back);
    _dc->fillArc(rc.left, rc.top,
          rc.right - rc.left, rc.bottom - rc.top,
          0, 32767);
    PenColour(fore);
    _dc->drawArc(rc.left, rc.top,
          rc.right - rc.left, rc.bottom - rc.top,
          0, 32767);
  }
}

void SurfaceImpl::Copy(PRectangle rc, Point from, Surface &surfaceSource) {
  if (dc() && static_cast<SurfaceImpl &>(surfaceSource).drawable) {
    _dc->drawArea(static_cast<SurfaceImpl &>(surfaceSource).drawable,
                 from.x, from.y,
                 rc.right - rc.left, rc.bottom - rc.top ,
                 rc.left, rc.top);
  }
}

void SurfaceImpl::DrawTextBase(PRectangle rc, Font &font_, int ybase, const char *s, int len, ColourDesired fore)
{
  if (dc()) {
    PenColour(fore);
    _dc->setFont(font_.GetID());
    const int segmentLength = 1000;
    int xbase = rc.left;
    if (codecBuffer.length()) { codecBuffer=FXString::null; }
    if (!unicodeMode) { // Fox uses UTF8 for text drawing, so we must convert any extended ASCII first.
      for (int p=0; p<len; p++) {
        if ((FXuchar)(s[p])>126) {
          codecBuffer.length(asciiCodec.mb2utflen(s,len));
          asciiCodec.mb2utf(&(codecBuffer.at(0)),codecBuffer.length(),s,len);
          len=codecBuffer.length();
          break;
        }
      }
    }
    while ((len > 0) && (xbase < maxCoordinate)) {
      int lenDraw = Platform::Minimum(len, segmentLength);
      _dc->drawText(xbase, ybase, codecBuffer.length()?codecBuffer.text():s, lenDraw);
      len -= lenDraw;
      if (len > 0) {
        xbase += font_.GetID()->getTextWidth(s, lenDraw);
      }
      s += lenDraw;
    }
  }
  if (codecBuffer.length()) { codecBuffer=FXString::null; }
}

void SurfaceImpl::DrawTextNoClip(PRectangle rc, Font &font_, XYPOSITION ybase, const char *s, int len,
                       ColourDesired fore, ColourDesired back) {
  if (dc()) {
    FillRectangle(rc, back);
    DrawTextBase(rc, font_, ybase, s, len, fore);
  }
}

// On GTK+, exactly same as DrawText NoClip
// <FIXME> what about FOX ? </FIXME>
void SurfaceImpl::DrawTextClipped(PRectangle rc, Font &font_, XYPOSITION ybase, const char *s, int len,
                       ColourDesired fore, ColourDesired back) {
  DrawTextNoClip(rc, font_, ybase, s, len, fore, back);
}

void SurfaceImpl::DrawTextTransparent(PRectangle rc, Font &font_, XYPOSITION ybase, const char *s, int len,
                                      ColourDesired fore) {
  DrawTextBase(rc, font_, ybase, s, len, fore);
}

void SurfaceImpl::MeasureWidths(Font &font_, const char *s, int len, XYPOSITION *positions) {
  if (font_.GetID()) {
    int totalWidth = 0;
    if (unicodeMode) {
      const char*p=s;
      for (FXint i=0; i<len; i++) {
        int cw=1; // number of bytes in this utf8 character
        FXuchar c=*p;
        if (c>127) { // no check for invalid bytes here, there's not much we could do about it anyway.
          if ((c>=194) && (c<=223)) {
            cw=2;
          } else if (c<=239) {
            cw=3;        
          } else if (c<=244) {
            cw=4;
          }
        }
        int width = font_.GetID()->getTextWidth(p, cw);
        totalWidth += width;
        positions[i] = totalWidth;
        p+=cw; // this many bytes consumed
        for (FXint j=1; j<cw; j++) { // assign the same position to each byte for multibyte chars
          positions[i+1]=positions[i];
          i++;
        }
      }
    } else {
      for (int i=0;i<len;i++) {
        int default_width=font_.GetID()->getTextWidth("8", 1);
#ifdef WIN32 // The width of chars > #239 are incorrect on Win32, so use default.
        int width = ((FXuchar)(s[i])<=239) ? font_.GetID()->getTextWidth(s + i, 1) : default_width;
#else
        int width = font_.GetID()->getTextWidth(s + i, 1);
#endif
        totalWidth += width?width:default_width;
        positions[i] = totalWidth;
      }
    }  
  } else {
    for (int i=0;i<len;i++) {
      positions[i] = i + 1;
    }
  }
}

XYPOSITION SurfaceImpl::WidthText(Font &font_, const char *s, int len) {
  if (font_.GetID())
    return font_.GetID()->getTextWidth(s, len);
  else
    return 1;
}

XYPOSITION SurfaceImpl::WidthChar(Font &font_, char ch) {
  if (font_.GetID())
    return font_.GetID()->getTextWidth(&ch, 1);
  else
    return 1;
}

XYPOSITION SurfaceImpl::Ascent(Font &font_) {
  if (!font_.GetID())
    return 1;
  return font_.GetID()->getFontAscent();
}

XYPOSITION SurfaceImpl::Descent(Font &font_) {
  if (!font_.GetID())
    return 1;
  return font_.GetID()->getFontDescent();
}

XYPOSITION SurfaceImpl::InternalLeading(Font &) {
  return 0;
}

XYPOSITION SurfaceImpl::ExternalLeading(Font &) {
  return 0;
}

XYPOSITION SurfaceImpl::Height(Font &font_) {
  if (!font_.GetID())
    return 1;
  return font_.GetID()->getFontHeight();
}

XYPOSITION SurfaceImpl::AverageCharWidth(Font &font_) {
  if (font_.GetID())
    return font_.GetID()->getTextWidth("n", 1);
  else
    return 1;
}

void SurfaceImpl::SetClip(PRectangle rc) {
  if (dc())
    _dc->setClipRectangle(rc.left, rc.top,
                       rc.right - rc.left, rc.bottom - rc.top);
}

void SurfaceImpl::FlushCachedState() {}

void SurfaceImpl::SetUnicodeMode(bool unicodeMode_) {
  unicodeMode=unicodeMode_;
}

Surface *Surface::Allocate(int technology) {
  (void)technology;
  return new SurfaceImpl;
}

void SurfaceImpl::DrawRGBAImage( PRectangle rc, int width, int height, 
                               const unsigned char *pixelsImage) {
  (void)rc;
  (void)width;
  (void)height;
  (void)pixelsImage;
}

Window::~Window() {}

void Window::Destroy() {
  if (wid)
    delete wid;
  wid = 0;
}

bool Window::HasFocus() {
  return wid->hasFocus();
}

PRectangle Window::GetPosition() {
  // Before any size allocated pretend its 1000 wide so not scrolled
  PRectangle rc(0, 0, 1000, 1000);
  if (wid) {
    rc.left = wid->getX();
    rc.top = wid->getY();
    rc.right = rc.left + wid->getWidth();
    rc.bottom = rc.top + wid->getHeight();
  }
  return rc;
}

void Window::SetPosition(PRectangle rc) {
  wid->position(rc.left, rc.top, rc.Width(), rc.Height());
}


void Window::SetPositionRelative(PRectangle rc, Window relativeTo) {
  int ox = relativeTo.GetID()->getX() + rc.left;
  int oy = relativeTo.GetID()->getY() + rc.top;
  if (ox < 0)
    ox = 0;
  if (oy < 0)
    oy = 0;

  /* do some corrections to fit into screen */
  int sizex = rc.right - rc.left;
  int sizey = rc.bottom - rc.top;
  int screenWidth = FXApp::instance()->getRootWindow()->getDefaultWidth();
  int screenHeight = FXApp::instance()->getRootWindow()->getDefaultHeight();
  if (sizex > screenWidth)
    ox = 0; /* the best we can do */
  else if (ox + sizex > screenWidth)
    ox = screenWidth - sizex;
  if (oy + sizey > screenHeight)
    oy = screenHeight - sizey;

  wid->position(ox, oy, rc.Width(), rc.Height());
}

PRectangle Window::GetClientPosition() {
  // On GTK+, the client position is the window position
  return PRectangle(0, 0, (wid) ? wid->getWidth() - 1 : 1000, (wid) ? wid->getHeight() -1 : 1000);
}

void Window::Show(bool show) {
  if (show) {
    wid->show();
    wid->raise();
  }
  else
    wid->hide();
}

void Window::InvalidateAll() {
  if (wid) {
    wid->update();
  }
}

void Window::InvalidateRectangle(PRectangle rc) {
  if (wid)
    wid->update(rc.left, rc.top, rc.right - rc.left + 1, rc.bottom - rc.top + 1);
}

void Window::SetFont(Font &) {
  // TODO
}

void Window::SetCursor(Cursor curs) {
  // We don't set the cursor to same value numerous times under FOX because
  // it stores the cursor in the window once it's set
  if (curs == cursorLast)
    return;
  FXDefaultCursor cursorID;
  cursorLast = curs;

  switch (curs) {
  case cursorText:
    cursorID = DEF_TEXT_CURSOR;
    break;
  case cursorArrow:
    cursorID = DEF_ARROW_CURSOR;
    break;
  case cursorUp:
    cursorID = DEF_MOVE_CURSOR;
    break;
  case cursorWait:
    cursorID = DEF_SWATCH_CURSOR;
    break;
  case cursorHand:
    // <FIXME/> Should be a hand cursor...
        cursorID = DEF_HAND_CURSOR; //JKP
//    cursorID = DEF_CROSSHAIR_CURSOR;
    break;
  case cursorReverseArrow:
    cursorID = DEF_RARROW_CURSOR;
    break;
  default:
    cursorID = DEF_ARROW_CURSOR;
    cursorLast = cursorArrow;
    break;
  }
  wid->setDefaultCursor(wid->getApp()->getDefaultCursor(cursorID));
}

void Window::SetTitle(const char *s) {
  static_cast<FXTopWindow *>(wid)->setTitle(s);
}


/*** JKP: FIXME: Ugly and not tested !!! ***/
PRectangle Window::GetMonitorRect(Point pt) {
  (void)pt;
  FXRootWindow *rootwin=wid->getApp()->getRootWindow(); //(id->getApp(),id->getVisual());
  FXint xpos=wid->getX();
  FXint ypos=wid->getY();
  return PRectangle(-xpos, -ypos, (-xpos) + rootwin->getDefaultWidth(),
                    (-ypos) + rootwin->getDefaultHeight());
}

// ====================================================================
// ListBoxFox
// ====================================================================

class ListBoxFox : public ListBox
{
  Window * Parent;
  FXList * list;
  map<int, FXXPMIcon *> * pixhash;
  int desiredVisibleRows;
  unsigned int maxItemCharacters;
  unsigned int aveCharWidth;
public:
  CallBackAction doubleClickAction;
  void *doubleClickActionData;

  ListBoxFox() : list(0), pixhash(NULL), desiredVisibleRows(5), maxItemCharacters(0),
    doubleClickAction(NULL), doubleClickActionData(NULL) {
  }
  virtual ~ListBoxFox() {
    ClearRegisteredImages();
  }
  virtual void Show(bool show=true);
  virtual void SetFont(Font &font);
  virtual void Create(Window &parent, int ctrlID, Point location, int lineHeight_, bool unicodeMode_, int technology_);
  virtual void SetAverageCharWidth(int width);
  virtual void SetVisibleRows(int rows);
  virtual int GetVisibleRows() const;
  virtual PRectangle GetDesiredRect();
  virtual int CaretFromEdge();
  virtual void Clear();
  virtual void Append(char *s, int type = -1);
  virtual int Length();
  virtual void Select(int n);
  virtual int GetSelection();
  virtual int Find(const char *prefix);
  virtual void GetValue(int n, char *value, int len);
  virtual void RegisterImage(int type, const char *xpm_data);
  virtual void ClearRegisteredImages();
  virtual void SetDoubleClickAction(CallBackAction action, void *data) {
    doubleClickAction = action;
    doubleClickActionData = data;
  }
  virtual void SetList(const char* list, char separator, char typesep);
  virtual void RegisterRGBAImage(int type, int width, int height, const unsigned char *pixelsImage);
  Window* GetParent() { return Parent; }
};


static int sListSortFunction(const FXListItem* item1, const FXListItem* item2) {
  return compare(item1->getText(), item2->getText());
}

class ListBoxList: public FXList {
private:
  FXDECLARE(ListBoxList)
  ListBoxList(){}
  ListBoxFox*listBox;
public: 
  ListBoxList(FXComposite*p, ListBoxFox*lbf):FXList(p, NULL, 0, LIST_BROWSESELECT|LAYOUT_FILL_X|LAYOUT_FILL_Y|SCROLLERS_TRACK|HSCROLLER_NEVER) {
    listBox=lbf;
    setSortFunc(sListSortFunction);
  }
  long onKeyPress(FXObject*o, FXSelector sel, void*p) {
    FXEvent*event = (FXEvent*)p;
    switch(event->code) {

      case KEY_Up:
      case KEY_KP_Up:
      case KEY_Down:
      case KEY_KP_Down:
      case KEY_Home:
      case KEY_KP_Home:
      case KEY_End:
      case KEY_KP_End: {
        return FXList::onKeyPress(o,sel,p);
      }
      case KEY_Page_Up:
      case KEY_KP_Page_Up: {
        setCurrentItem(current>visible?current-visible:0);
        return FXList::onKeyPress(o,sel,p);
      }
      case KEY_Page_Down:
      case KEY_KP_Page_Down: {
        setCurrentItem((current+visible)>(items.no()-1)?items.no()-1:current+visible);
        return FXList::onKeyPress(o,sel,p);
      }
      default: {
        return 0;
      }
    }
  }
  long onDoubleClicked(FXObject *o, FXSelector sel, void*p) {
    FXList::onDoubleClicked(o,sel,p);
    if (listBox->doubleClickAction) {
      listBox->doubleClickAction(listBox->doubleClickActionData);
      return 1;
    } else {
      FXEvent ev;
      ev.type=SEL_KEYPRESS;
      ev.code=KEY_Tab;
      return listBox->GetParent()->GetID()->handle(o,MKUINT(0, SEL_KEYPRESS),&ev);
    }
  }
};

FXDEFMAP(ListBoxList) ListBoxListMap[] = {
  FXMAPFUNC(SEL_KEYPRESS,0,ListBoxList::onKeyPress),
  FXMAPFUNC(SEL_DOUBLECLICKED,0,ListBoxList::onDoubleClicked),
};
FXIMPLEMENT(ListBoxList,FXList,ListBoxListMap,ARRAYNUMBER(ListBoxListMap))



// ====================================================================

void ListBoxFox::Create(Window &parent, int ctrlID, Point location, int lineHeight_, bool unicodeMode_, int technology_) {
  (void)ctrlID;
  (void)location;
  (void)lineHeight_;
  (void)unicodeMode_;
  (void)technology_;
  Parent=&parent;
  wid = new FXPacker(static_cast<FXComposite*>(parent.GetID()), FRAME_NONE|LAYOUT_EXPLICIT,0,0,0,0,1,1,1,1,0,0);
  list = new ListBoxList(static_cast<FXComposite*>(wid),this);
  wid->setBackColor(list->getTextColor());
  wid->create();
}

void ListBoxFox::SetFont(Font &scint_font) {
  list->setFont(scint_font.GetID());
}

void ListBoxFox::SetAverageCharWidth(int width) {
    aveCharWidth = width;
}

void ListBoxFox::SetVisibleRows(int rows) {
  list->setNumVisible(rows);
}

int ListBoxFox::GetVisibleRows() const {
  return list->getNumVisible();
}

PRectangle ListBoxFox::GetDesiredRect() {
  // Before any size allocated pretend its 100 wide so not scrolled
  PRectangle rc(0, 0, 100, 100);
  if (wid) {
    // Height
    int rows = Length();
    if ((rows == 0) || (rows > desiredVisibleRows))
      rows = desiredVisibleRows;
    list->setNumVisible(rows);
    rc.bottom = wid->getHeight();
    // Width
    int width = maxItemCharacters;
    if (width < 12)
      width = 12;
    rc.right = width * (aveCharWidth+aveCharWidth/3);
    if (Length() > rows)
      rc.right += list->verticalScrollBar()->getWidth();

    // <FIXME/>
/*    int rows = Length();
    if ((rows == 0) || (rows > desiredVisibleRows))
      rows = desiredVisibleRows;
    
    GtkRequisition req;
    int height;

    // First calculate height of the clist for our desired visible row count otherwise it tries to expand to the total # of rows
    height = (rows * GTK_CLIST(list)->row_height
              + rows + 1
              + 2 * (list->style->klass->ythickness 
                     + GTK_CONTAINER(list)->border_width));
    gtk_widget_set_usize(GTK_WIDGET(list), -1, height);

    // Get the size of the scroller because we set usize on the window
    gtk_widget_size_request(GTK_WIDGET(scroller), &req);
    rc.right = req.width;
    rc.bottom = req.height;
                
    gtk_widget_set_usize(GTK_WIDGET(list), -1, -1);
    int width = maxItemCharacters;
    if (width < 12)
      width = 12;
    rc.right = width * (aveCharWidth+aveCharWidth/3);
    if (Length() > rows)
      rc.right = rc.right + 16;*/
  }
  return rc;
    
}

void ListBoxFox::Show(bool show) {
  if (show) {
    list->selectItem(0);
    list->setCurrentItem(0);
    list->setFocus();
    FXPacker *shell=static_cast<FXPacker*>(wid);
    FXScintilla*sci=static_cast<FXScintilla*>(Parent->GetID());
    FXint x=shell->getX();
    FXint y=shell->getY();
    FXint w=list->getWidth();
    FXint h=shell->getDefaultHeight();
    FXint sci_w = sci->getWidth();
    FXint sci_h = sci->getHeight();
    sci_w -= sci->isVerticalScrollable()?sci->verticalScrollBar()->getWidth():0;
    sci_h -= sci->isHorizontalScrollable()?sci->horizontalScrollBar()->getHeight():0;
    x -= sci->getX();
    y -= sci->getY();
    if ( x+w > sci_w ) {
      x=sci_w-w;
    }
    if ( y+h > sci_h ) {
      FXint line_hgt=sci->sendMessage(SCI_TEXTHEIGHT,sci->sendMessage(SCI_LINEFROMPOSITION,sci->sendMessage(SCI_GETCURRENTPOS,0,0),0),0);
      y=y-(h+line_hgt);
    }
    shell->position(x,y,w,h);
    shell->show();
  }
}

int ListBoxFox::CaretFromEdge() {
  // <FIXME/> return 4 + GetWidth();
  return 0;
}

void ListBoxFox::Clear() {
  list->clearItems();
  maxItemCharacters = 0;
}

void ListBoxFox::Append(char *s, int type) {
  FXXPMIcon * icon = NULL;
  if ((type >= 0) && pixhash) {
    map<int, FXXPMIcon *>::iterator it = pixhash->find(type);
    if (it != pixhash->end())
      icon = (*it).second;
  }
  list->appendItem(s, icon);
  size_t len = strlen(s);
  if (maxItemCharacters < len)
          maxItemCharacters = len;
  if (list->getNumItems() <= desiredVisibleRows)
    list->setNumVisible(list->getNumItems());
}

int ListBoxFox::Length() {
  if (wid)
    return list->getNumItems();
  return 0;
}

void ListBoxFox::Select(int n) {
  // Case n==-1 handled by FXList
  list->setCurrentItem(n, true);
  list->selectItem(n);
  list->makeItemVisible(n);
}

int ListBoxFox::GetSelection() {
  // Returns -1 when no current item
  return list->getCurrentItem();
}

int ListBoxFox::Find(const char *prefix) {
  int count = Length();
  for (int i = 0; i < count; i++) {
    FXString text = list->getItemText(i);
    const char* s = text.text();
    if (s && (0 == strncmp(prefix, s, strlen(prefix)))) {
      return i;
    }
  }
  return - 1;
}

void ListBoxFox::GetValue(int n, char *value, int len) {
  FXString text = list->getItemText(n);
  if (text.length() && len > 0) {
    strncpy(value, text.text(), len);
    value[len - 1] = '\0';
  } else {
    value[0] = '\0';
  }
}

void ListBoxFox::RegisterImage(int type, const char *xpm_data)
{
  FXXPMIcon * icon = new FXXPMIcon(FXApp::instance(), &xpm_data);
  icon->create();
  if (!pixhash)
    pixhash = new map<int, FXXPMIcon *>;
  FXXPMIcon * old = (*pixhash)[type];
  if (old)
    delete old;
  (*pixhash)[type] = icon;
}

void ListBoxFox::ClearRegisteredImages()
{
  if (pixhash) {
    map<int, FXXPMIcon *>::iterator it;
    for (it = pixhash->begin(); it != pixhash->end(); it++) {
      delete (*it).second;
    }
    delete pixhash;
  }  
}

void ListBoxFox::SetList(const char* items, char separator, char typesep) {
  Clear();
  int count = strlen(items) + 1;
  char *words = new char[count];
  if (words) {
    memcpy(words, items, count);
    char *startword = words;
    char *numword = NULL;
    int i = 0;
    for (; words[i]; i++) {
      if (words[i] == separator) {
        words[i] = '\0';
        if (numword)
          *numword = '\0';
        Append(startword, numword?atoi(numword + 1):-1);
        startword = words + i + 1;
        numword = NULL;
      } else if (words[i] == typesep) {
        numword = words + i;
      }
    }
    if (startword) {
      if (numword)
        *numword = '\0';
      Append(startword, numword?atoi(numword + 1):-1);
    }
    delete []words;
  }
  list->sortItems();
}

void ListBoxFox::RegisterRGBAImage(int type, int width, int height, const unsigned char *pixelsImage) {

}


// ====================================================================
// ListBox
// ====================================================================

ListBox::ListBox()
{
}

ListBox::~ListBox()
{
}

ListBox * ListBox::Allocate()
{
  return new ListBoxFox();
}


// ====================================================================
// Menu
// ====================================================================

Menu::Menu() : mid(0) {}


void Menu::CreatePopUp() {
  Destroy();
  mid = new FXMenuPane(FXApp::instance()->getCursorWindow());
}

void Menu::Destroy() {
  if (mid)
    delete mid;
  mid = 0;
}

void Menu::Show(Point pt, Window &) {
  int screenHeight = FXApp::instance()->getRootWindow()->getDefaultHeight();
  int screenWidth = FXApp::instance()->getRootWindow()->getDefaultWidth();
  mid->create();
  if ((pt.x + mid->getWidth()) > screenWidth) {
    pt.x = screenWidth - mid->getWidth();
  }
  if ((pt.y + mid->getHeight()) > screenHeight) {
    pt.y = screenHeight - mid->getHeight();
  }
  mid->popup(NULL, pt.x - 4, pt.y - 2);
  mid->grabKeyboard();
  FXApp::instance()->runModalWhileShown(mid);
}

#ifndef WIN32

ElapsedTime::ElapsedTime() {
  timeval curTime;
  gettimeofday(&curTime, NULL);
  bigBit = curTime.tv_sec;
  littleBit = curTime.tv_usec;
}

double ElapsedTime::Duration(bool reset) {
  timeval curTime;
  gettimeofday(&curTime, NULL);
  long endBigBit = curTime.tv_sec;
  long endLittleBit = curTime.tv_usec;
  double result = 1000000.0 * (endBigBit - bigBit);
  result += endLittleBit - littleBit;
  result /= 1000000.0;
  if (reset) {
    bigBit = endBigBit;
    littleBit = endLittleBit;
  }
  return result;
}

#else  // WIN32

static bool initialisedET = false;
static bool usePerformanceCounter = false;
static LARGE_INTEGER frequency;

ElapsedTime::ElapsedTime() {
  if (!initialisedET) {
    usePerformanceCounter = ::QueryPerformanceFrequency(&frequency);
    initialisedET = true;
  }
  if (usePerformanceCounter) {
    LARGE_INTEGER timeVal;
    ::QueryPerformanceCounter(&timeVal);
    bigBit = timeVal.HighPart;
    littleBit = timeVal.LowPart;
  } else {
    bigBit = clock();
  }
}

double ElapsedTime::Duration(bool reset) {
  double result;
  long endBigBit;
  long endLittleBit;

  if (usePerformanceCounter) {
    LARGE_INTEGER lEnd;
    ::QueryPerformanceCounter(&lEnd);
    endBigBit = lEnd.HighPart;
    endLittleBit = lEnd.LowPart;
    LARGE_INTEGER lBegin;
    lBegin.HighPart = bigBit;
    lBegin.LowPart = littleBit;
    double elapsed = lEnd.QuadPart - lBegin.QuadPart;
    result = elapsed / static_cast<double>(frequency.QuadPart);
  } else {
    endBigBit = clock();
    endLittleBit = 0;
    double elapsed = endBigBit - bigBit;
    result = elapsed / CLOCKS_PER_SEC;
  }
  if (reset) {
    bigBit = endBigBit;
    littleBit = endLittleBit;
  }
  return result;
}
#endif  // WIN32

// ====================================================================
// Dynamic library handling.
// - fxdllXxx API with Fox >= 1.2
// ====================================================================

// Fox >= 1.2 has dynamic librarie handling


#include <FXDLL.h>

#ifdef FOX_1_6
class DynamicLibraryImpl : public DynamicLibrary {
protected:
  void * m;
public:
  DynamicLibraryImpl(const char *modulePath) {
    m = fxdllOpen(modulePath);
  }

  virtual ~DynamicLibraryImpl() {
    if (m != NULL)
      fxdllClose(m);
  }

  // Use lt_dlsym to get a pointer to the relevant function.
  virtual Function FindFunction(const char *name) {
    if (m != NULL) {
      return fxdllSymbol(m, name);
    } else
      return NULL;
  }

  virtual bool IsValid() {
    return m != NULL;
  }
};
#else
class DynamicLibraryImpl : public DynamicLibrary {
protected:
  FXDLL*dll;
public:
  DynamicLibraryImpl(const char *modulePath) {
      dll=new FXDLL();
    dll->load(modulePath);
  }

  virtual ~DynamicLibraryImpl() {
    dll->unload();
    delete dll;
  }


  // Use lt_dlsym to get a pointer to the relevant function.
  virtual Function FindFunction(const char *name) {
    if (dll->loaded()) {
      return dll->address(name);
    } else
      return NULL;
  }

  virtual bool IsValid() {
    return dll->loaded();
  }
};
#endif


DynamicLibrary *DynamicLibrary::Load(const char *modulePath) {
  return static_cast<DynamicLibrary *>( new DynamicLibraryImpl(modulePath) );
}


// ====================================================================
// Platform
// ====================================================================

ColourDesired Platform::Chrome() {
  return ColourDesired(0xe0, 0xe0, 0xe0);
}

ColourDesired Platform::ChromeHighlight() {
  return ColourDesired(0xff, 0xff, 0xff);
}

const char *Platform::DefaultFont() {
  static FXString fontName;
  fontName = FXApp::instance()->getNormalFont()->getName();
  return fontName.text();
}

int Platform::DefaultFontSize() {
  // Warning: FOX gives the font size in deci-point
  return FXApp::instance()->getNormalFont()->getSize() / 10;
}

unsigned int Platform::DoubleClickTime() {
  return 500;   // Half a second
}

bool Platform::MouseButtonBounce() {
  return true; // <FIXME/> same as gtk?
}

void Platform::DebugDisplay(const char *s) {
  printf("%s", s);
}

bool Platform::IsKeyDown(int) {
  // TODO: discover state of keys in GTK+/X
  return false;
}

/* These methods are now implemented in ScintillaFOX.cxx
long Platform::SendScintilla(WindowID w, unsigned int msg,
                             unsigned long wParam, long lParam) {
  return static_cast<FXScintilla *>(w)->sendMessage(msg, wParam, lParam);
}
long Platform::SendScintillaPointer(WindowID w, unsigned int msg,
                                    unsigned long wParam, void *lParam) {
  return static_cast<FXScintilla *>(w)->
    sendMessage(msg, wParam, reinterpret_cast<sptr_t>(lParam));
}
*/

bool Platform::IsDBCSLeadByte(int /*codePage*/, char /*ch*/) {
  return false;
}

int Platform::DBCSCharLength(int /*codePage*/, const char *s) {
  int bytes = mblen(s, MB_CUR_MAX);
  if (bytes >= 1)
    return bytes;
  else
    return 1;
}

int Platform::DBCSCharMaxLength() {
  return MB_CUR_MAX;
}

// These are utility functions not really tied to a platform

int Platform::Minimum(int a, int b) {
  if (a < b)
    return a;
  else
    return b;
}

int Platform::Maximum(int a, int b) {
  if (a > b)
    return a;
  else
    return b;
}

//#define TRACE

#ifdef TRACE
void Platform::DebugPrintf(const char *format, ...) {
  char buffer[2000];
  va_list pArguments;
  va_start(pArguments, format);
  vsprintf(buffer, format, pArguments);
  va_end(pArguments);
  Platform::DebugDisplay(buffer);
}
#else
void Platform::DebugPrintf(const char *, ...) {
}
#endif

// Not supported for GTK+
static bool assertionPopUps = true;

bool Platform::ShowAssertionPopUps(bool assertionPopUps_) {
  bool ret = assertionPopUps;
  assertionPopUps = assertionPopUps_;
  return ret;
}

void Platform::Assert(const char *c, const char *file, int line) {
  char buffer[2000];
  sprintf(buffer, "Assertion [%s] failed at %s %d", c, file, line);
  strcat(buffer, "\r\n");
  Platform::DebugDisplay(buffer);
  abort();
}

int Platform::Clamp(int val, int minVal, int maxVal) {
  if (val > maxVal)
    val = maxVal;
  if (val < minVal)
    val = minVal;
  return val;
}
