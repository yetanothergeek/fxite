/** FXScintilla source code edit control
 *
 *  ScintillaFOX.cxx - FOX toolkit specific subclass of ScintillaBase
 *
 *  Copyright 2001-2004 by Gilles Filippini <gilles.filippini@free.fr>
 *
 *  Adapted from the Scintilla source ScintillaGTK.cxx 
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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <string>
#include <vector>

#if !defined(WIN32) || defined(__CYGWIN__)
# if defined(__CYGWIN__)
#  include <windows.h>
#  ifdef PIC
#   define FOXDLL
#  endif
# endif
# include <fx.h>
# include <fxkeys.h>
#else
# if defined(__MINGW32__) 
#  include <windows.h>
#  if defined(PIC) && !defined(FOXDLL)
#    define FOXDLL
#  endif
# endif
# include <fx.h>
# include <fxkeys.h>
#endif  // !defined(WIN32) || defined(__CYGWIN__)

#include "Platform.h"

#include "XPM.h"

#include "Scintilla.h"
#include "ScintillaWidget.h"
#ifdef SCI_LEXER
# include "SciLexer.h"
# include "PropSet.h"
# include "PropSetSimple.h"
# include "Accessor.h"
# include "KeyWords.h"
# include "ExternalLexer.h"
#endif
#include "SplitVector.h"
#include "Partitioning.h"
#include "RunStyles.h"
#include "ContractionState.h"
#include "SVector.h"
#include "CellBuffer.h"
#include "CallTip.h"
#include "KeyMap.h"
#include "Indicator.h"
#include "LineMarker.h"
#include "Style.h"
#include "AutoComplete.h"
#include "ViewStyle.h"
#include "CharClassify.h"
#include "Decoration.h"
#include "Document.h"
#include "Selection.h"
#include "PositionCache.h"
#include "Editor.h"
//#include "SString.h"
#include "ScintillaBase.h"

#ifdef WIN32
#ifdef FOXDLL
#undef FXAPI
#define FXAPI __declspec(dllexport)
#endif
#endif
#include "FXScintilla.h"

#ifdef _MSC_VER
// Constant conditional expressions are because of GTK+ headers
#pragma warning(disable: 4127)
#endif

#include <FX88591Codec.h>

#include "version.h"

// ====================================================================
// ScintillaFOX
// ====================================================================
class ScintillaFOX : public ScintillaBase {
  friend class FXScintilla;
protected:
  FXScintilla & _fxsc;
  bool tryDrag;
  bool dragWasDropped;
  Point ptMouseLastBeforeDND;

  // Because clipboard access is asynchronous, copyText is created by Copy
  SelectionText copyText;
  SelectionText primary;
  
  // Vertical scrollbar handling
  int vsbMax;
  int vsbPage;
  
  // Flag to prevent infinite loop with moveContents
  bool inMoveContents;
public:
  ScintillaFOX(FXScintilla & fxsc);
  virtual ~ScintillaFOX();
  virtual sptr_t WndProc(unsigned int iMessage, uptr_t wParam, sptr_t lParam);
private:
  int TargetAsUTF8(char *text);
  int EncodedFromUTF8(char *utf8, char *encoded);
  // Virtual methods from ScintillaBase
  virtual void Initialise();
  virtual void Finalise();
  virtual void DisplayCursor(Window::Cursor c);
  virtual void SetVerticalScrollPos();
  virtual void SetHorizontalScrollPos();
  virtual void CopyToClipboard(const SelectionText &selectedText);
  virtual void Copy();
  virtual void Paste();
  virtual void ClaimSelection();
  virtual void NotifyChange();
  virtual void NotifyParent(SCNotification scn);
  virtual void SetTicking(bool on);
  virtual bool SetIdle(bool on);
  virtual void SetMouseCapture(bool on);
  virtual bool HaveMouseCapture();
  virtual bool PaintContains(PRectangle rc);
  virtual sptr_t DefWndProc(unsigned int iMessage, uptr_t wParam, sptr_t lParam);
  virtual void CreateCallTipWindow(PRectangle rc);
  virtual void AddToPopUp(const char * label, int cmd = 0, bool enabled = true);
  virtual void StartDrag();
  virtual bool ValidCodePage(int codePage) const;
  //
  static sptr_t DirectFunction(ScintillaFOX *sciThis, 
    unsigned int iMessage, uptr_t wParam, sptr_t lParam);
  // Paint
  void SyncPaint(PRectangle rc);
  void FullPaint();
  void UnclaimSelection();
  void ReceivedSelection(FXDNDOrigin origin, int atPos);
  void NotifyKey(int key, int modifiers);
  void NotifyURIDropped(const char *list);
  
  // From ScintillaBase
  virtual bool ModifyScrollBars(int nMax, int nPage);
  virtual void ReconfigureScrollBars();
  virtual PRectangle GetClientRectangle();
  virtual int KeyDefault(int key, int modifiers);

  // Popup flag
  bool getDisplayPopupMenu() const { return displayPopupMenu; }
};

ScintillaFOX::ScintillaFOX(FXScintilla & fxsc) :
  _fxsc(fxsc),
  tryDrag(false),
  dragWasDropped(false),
  vsbMax(0),
  vsbPage(0),
  inMoveContents(false)
{
  wMain = &_fxsc;
  Initialise();
}

ScintillaFOX::~ScintillaFOX()
{
  Finalise();
}

int ScintillaFOX::TargetAsUTF8(char *text) {
  // TODO
  // Fail
  return 0;
}

int ScintillaFOX::EncodedFromUTF8(char *utf8, char *encoded) {
  // TODO
  // Fail
  return 0;
}

bool ScintillaFOX::ValidCodePage(int codePage) const {
  return codePage == 0 || codePage == SC_CP_UTF8 || codePage == SC_CP_DBCS;
}

sptr_t ScintillaFOX::WndProc(unsigned int iMessage, uptr_t wParam, sptr_t lParam) {
  switch (iMessage) {

  case SCI_GRABFOCUS:
    _fxsc.setFocus();
    break;

  case SCI_GETDIRECTFUNCTION:
    return reinterpret_cast<sptr_t>(DirectFunction);
  
  case SCI_GETDIRECTPOINTER:
    return reinterpret_cast<sptr_t>(this);

#ifdef SCI_LEXER
  case SCI_LOADLEXERLIBRARY:
    LexerManager::GetInstance()->Load(reinterpret_cast<const char*>(wParam));
    break;
#endif
  case SCI_TARGETASUTF8:
    return TargetAsUTF8(reinterpret_cast<char*>(lParam));

  case SCI_ENCODEDFROMUTF8:
    return EncodedFromUTF8(reinterpret_cast<char*>(wParam), 
      reinterpret_cast<char*>(lParam));

  default:
    return ScintillaBase::WndProc(iMessage,wParam,lParam);
  }
  return 0l;
}

void ScintillaFOX::ClaimSelection()
{
  // Acquire selection

// JKP  1.79 if (currentPos != anchor) {
// JKP 2.00 ? if (SelectionStart().Position() != SelectionEnd().Position()) {
  if (!sel.Empty()) { // <- JKP 2.00
    _fxsc.acquireSelection(&FXWindow::stringType,1);
    primarySelection = true;
    primary.Free();
  }
  else {
//  _fxsc.releaseSelection(); // JKP:  Does not work for 2.00 - Do we need somethong else?
  }
}

void ScintillaFOX::UnclaimSelection()
{
  //Platform::DebugPrintf("UnclaimPrimarySelection\n");
  if (!_fxsc.hasSelection()) {
    primary.Free();
    primarySelection = false;
    FullPaint();
  }
}


#define InUTF8Mode(sf) (sf->CodePage()==SC_CP_UTF8)

// JKP: Heavily reworked to fix middle-click-paste when current document has the X-selection.
// JKP: Still more reworking for 2.00!
void ScintillaFOX::ReceivedSelection(FXDNDOrigin origin, int atPos)
{
  FXuchar *data=NULL;
  FXuint len=0;
  if (pdoc->IsReadOnly()) { return; }
  if (InUTF8Mode(this)&&_fxsc.getDNDData(origin, FXWindow::utf8Type, data, len)) {
    /* If the data and the scintilla are both UTF-8, then no conversion is needed. */
  } else {
    if(_fxsc.getDNDData(origin, FXWindow::stringType, data, len)) {
      if (InUTF8Mode(this)) {
        for (FXuint i=0; i<len; i++) {
          if ((FXuchar)(data[i])>126) { // We are in UTF-8 mode, but the data is extended ASCII
            FX88591Codec asciiCodec;
            FXString codecBuffer;
            codecBuffer.length(asciiCodec.mb2utflen((FXchar*)data,len));
            asciiCodec.mb2utf(&(codecBuffer.at(0)),codecBuffer.length(),(FXchar*)data,len);
            FXRESIZE(&data,FXuchar,codecBuffer.length());
            memcpy(data, codecBuffer.text(),codecBuffer.length());
            len=codecBuffer.length();
            break;
          }
        }
      } 
    } else { // Type of DND data must be something we don't know how to deal with.
      return;
    }
  }
  FXRESIZE(&data,FXuchar,len+1);
  data[len]='\0';
  bool isRectangular;
  SelectionText selText;

#ifdef WIN32
  len=0;
  while(data[len]) { len++; }
  static CLIPFORMAT cfColumnSelect = 
    static_cast<CLIPFORMAT>(::RegisterClipboardFormat("MSDEVColumnSelect"));
  isRectangular = ::IsClipboardFormatAvailable(cfColumnSelect) != 0;
#else // !WIN32
  isRectangular = ((len > 2) && (data[len - 1] == 0 && data[len - 2] == '\n'));
#endif // WIN32

  selText.Copy((char*)data,len,CodePage(),0,isRectangular,false);
  pdoc->BeginUndoAction();

  if(_fxsc.hasSelection() && (origin == FROM_CLIPBOARD)) { ClearSelection(); }

  SelectionPosition selStart = SelectionStart();

  if (selText.rectangular) {
    PasteRectangular(selStart, selText.s, selText.len);
  } else {
    selStart = SelectionPosition(InsertSpace(selStart.Position(), selStart.VirtualSpace()));
    if (pdoc->InsertString(atPos>0?atPos:selStart.Position(),selText.s, selText.len)) {
      if (atPos<0) {
        SetEmptySelection( (atPos>0?atPos:selStart.Position()) + selText.len );
      } else {  
        SetEmptySelection( (atPos>0?atPos:selStart.Position()) + selText.len );
        FullPaint();
      }
    }
  }
  pdoc->EndUndoAction();
  EnsureCaretVisible();
//  _fxsc.setDNDData(origin, FXWindow::stringType, data, len);
  FXFREE(&data);
}


void ScintillaFOX::NotifyKey(int key, int modifiers) {
  SCNotification scn;
  scn.nmhdr.code = SCN_KEY;
  scn.ch = key;
  scn.modifiers = modifiers;

  NotifyParent(scn);
}

void ScintillaFOX::NotifyURIDropped(const char *list) {
  SCNotification scn;
  scn.nmhdr.code = SCN_URIDROPPED;
  scn.text = list;

  NotifyParent(scn);
}

int ScintillaFOX::KeyDefault(int key, int modifiers) {
  if (!(modifiers & SCI_CTRL) && !(modifiers & SCI_ALT) && (key < 256)) {
    //NotifyKey(key, modifiers);
    //return 0;
    AddChar(key);
    return 1;
  } else {
    // Pass up to container in case it is an accelerator
    NotifyKey(key, modifiers);
    return 0;
  }
  //Platform::DebugPrintf("SK-key: %d %x %x\n",key, modifiers);
}

// ********************************************************************

void ScintillaFOX::Initialise()
{
  SetTicking(true);
}

void ScintillaFOX::Finalise()
{
  SetTicking(false);
  ScintillaBase::Finalise();
}

void ScintillaFOX::DisplayCursor(Window::Cursor c) {
  if (cursorMode == SC_CURSORNORMAL)
    wMain.SetCursor(c);
  else
    wMain.SetCursor(static_cast<Window::Cursor>(cursorMode));
}

void ScintillaFOX::SetVerticalScrollPos()
{
  if (inMoveContents) return;
  // Update the scrollbar position only if this method is not called by
  // moveContents (to prevent an infinite loop becaude setPosition
  // triggers moveContents).
  // BTW scrollbar should be up to date when in movecontents.
  DwellEnd(true);
#ifdef FOX_1_6
    _fxsc.setPosition(_fxsc.getXPosition(), -topLine * vs.lineHeight);
#else
    FXint gpx,gpy;
    _fxsc.getPosition(gpx,gpy);
    _fxsc.setPosition(gpx, -topLine * vs.lineHeight);
#endif
}

void ScintillaFOX::SetHorizontalScrollPos()
{
  if (inMoveContents) return;
  // Update the scrollbar position only if this method is not called by
  // moveContents (to prevent an infinite loop becaude setPosition
  // triggers moveContents).
  // BTW scrollbar should be up to date when in movecontents.
  DwellEnd(true);
#ifdef FOX_1_6
  _fxsc.setPosition(-xOffset, _fxsc.getYPosition());
#else
  FXint gpx,gpy;
  _fxsc.getPosition(gpx,gpy);
  _fxsc.setPosition(-xOffset, gpy);
#endif
}

void ScintillaFOX::CopyToClipboard(const SelectionText &selectedText) {
  if (_fxsc.acquireClipboard(&FXWindow::stringType, 1)) {
    copyText.Copy(selectedText);
  }
}

void ScintillaFOX::Copy()
{
  if (_fxsc.hasSelection()) {
    if (_fxsc.acquireClipboard(&FXWindow::stringType, 1)) {
      CopySelectionRange(&copyText);
    }
  }
  else {
    _fxsc.releaseClipboard();
  }
}

void ScintillaFOX::Paste()
{
  ReceivedSelection(FROM_CLIPBOARD, -1);
}

void ScintillaFOX::NotifyChange()
{
  _fxsc.handle(&_fxsc, MKUINT(0, SEL_CHANGED), NULL);
}

void ScintillaFOX::NotifyParent(SCNotification scn)
{
  scn.nmhdr.hwndFrom = wMain.GetID();
  scn.nmhdr.idFrom = GetCtrlID();
  _fxsc.handle(&_fxsc, MKUINT(0, SEL_COMMAND), &scn);
}

void ScintillaFOX::SetTicking(bool on)
{
  if (timer.ticking != on) {
    timer.ticking = on;
    if (timer.ticking) {
      FXApp::instance()->addTimeout(&_fxsc, _fxsc.ID_TICK, timer.tickSize);
    } else {
      FXApp::instance()->removeTimeout(&_fxsc, _fxsc.ID_TICK);
    }
  }
  timer.ticksToWait = caret.period;
}

bool ScintillaFOX::SetIdle(bool on) {
  if (on) {
    // Start idler, if it's not running.
    if (idler.state == false) {
      idler.state = true;
      FXApp::instance()->addChore(&_fxsc, FXScintilla::ID_IDLE);
    }
  } else {
    // Stop idler, if it's running
    if (idler.state == true) {
      idler.state = false;
      FXApp::instance()->removeChore(&_fxsc, FXScintilla::ID_IDLE);
    }
  }
  return true;
}

void ScintillaFOX::SetMouseCapture(bool on)
{
  if (mouseDownCaptures) {
    if (on) {
      _fxsc.grab();
    } else {
      _fxsc.ungrab();
    }
  }
}

bool ScintillaFOX::HaveMouseCapture()
{
  return _fxsc.grabbed();
}

bool ScintillaFOX::PaintContains(PRectangle rc) {
  bool contains = true;
  if (paintState == painting) {
    if (!rcPaint.Contains(rc)) {
      contains = false;
    }
  }
  return contains;
}

void ScintillaFOX::StartDrag()
{
  tryDrag = true;
  dragWasDropped = false;
}

sptr_t ScintillaFOX::DefWndProc(unsigned int, uptr_t, sptr_t)
{
  return 0;
}


// JKP: A new class, overrides the onPaint() method to 
// let a CallTip object take over the painting.
class CallTipWindow:public FXFrame {
  FXDECLARE(CallTipWindow)
  CallTipWindow(){}
protected:
  CallTip*ct;
public:
  long onPaint(FXObject*o, FXSelector sel, void*p);
  CallTipWindow( FXComposite *p, CallTip*_ct):FXFrame(p,FRAME_NONE|LAYOUT_FILL),ct(_ct) {}
};

FXDEFMAP(CallTipWindow) CallTipWindowMap[] = {
  FXMAPFUNC(SEL_PAINT,0,CallTipWindow::onPaint)
};

FXIMPLEMENT(CallTipWindow,FXFrame,CallTipWindowMap,ARRAYNUMBER(CallTipWindowMap));

long CallTipWindow::onPaint(FXObject*o, FXSelector sel, void*p)
{
  long rv=FXFrame::onPaint(o,sel,p);  
  Surface *surfaceWindow = Surface::Allocate();
  if (surfaceWindow) {
    surfaceWindow->Init(this, this);
    surfaceWindow->SetUnicodeMode(SC_CP_UTF8 == ct->codePage);
    surfaceWindow->SetDBCSMode(ct->codePage);
    ct->PaintCT(surfaceWindow);
    surfaceWindow->Release();
    delete surfaceWindow;
  }
  return rv;
}


void ScintillaFOX::CreateCallTipWindow(PRectangle  rc )
{
  // Gilles says: <FIXME/>
  // JKP says: OK, I'll try....
  if (!ct.wCallTip.GetID()) {
    FXHorizontalFrame*w=new FXHorizontalFrame(&_fxsc,FRAME_NONE,rc.left, rc.top, 
      (rc.right-rc.left), (rc.bottom-rc.top),0,0,0,0,0,0);
    CallTipWindow*c=new CallTipWindow(w, &ct);
    w->create();
    ct.wCallTip=w;
    ct.wDraw=c;
  }
}

void ScintillaFOX::AddToPopUp(const char * label, int cmd, bool enabled)
{
  if (label[0]) {
    FXMenuCommand * item = new FXMenuCommand(static_cast<FXComposite *>(popup.GetID()), label, NULL, &_fxsc, SCID(cmd));
    if (!enabled)
      item->disable();
  }
  else
    new FXMenuSeparator(static_cast<FXComposite *>(popup.GetID()));
}

sptr_t ScintillaFOX::DirectFunction(
    ScintillaFOX *sciThis, unsigned int iMessage, uptr_t wParam, sptr_t lParam) {
  return sciThis->WndProc(iMessage, wParam, lParam);
}

PRectangle ScintillaFOX::GetClientRectangle() {
  // Have to call FXScrollArea::getViewportXxxx instead of getViewportXxxx
  // to prevent infinite loop
#ifdef FOX_1_6
  PRectangle rc(0, 0, _fxsc.FXScrollArea::getViewportWidth(), _fxsc.FXScrollArea::getViewportHeight());
  if (_fxsc.horizontalScrollBar()->shown())
    rc.bottom -= _fxsc.horizontalScrollBar()->getDefaultHeight();
  if (_fxsc.verticalScrollBar()->shown())
    rc.right -= _fxsc.verticalScrollBar()->getDefaultWidth();
#else
  PRectangle rc(0, 0, _fxsc.FXScrollArea::getVisibleWidth(), _fxsc.FXScrollArea::getVisibleHeight());
#endif
  return rc;
}

void ScintillaFOX::ReconfigureScrollBars()
{
  if (horizontalScrollBarVisible)
    _fxsc.setScrollStyle(_fxsc.getScrollStyle() & ~HSCROLLER_NEVER);
  else
    _fxsc.setScrollStyle(_fxsc.getScrollStyle() | HSCROLLER_NEVER);
  _fxsc.recalc();
}

bool ScintillaFOX::ModifyScrollBars(int nMax, int nPage)
{
  bool modified = false;
  // There was no vertical scrollbar if
  //   !vsbMax || (vsbMax < vsbPage)
  bool noVScroll = !vsbMax || (vsbMax < vsbPage);
  // There won't be a vertical scrollbar if
  //   !nMax || (nMax < nPage)
  bool noVScrollNew = !nMax || (nMax < nPage);
  if ((nMax != vsbMax) || (nPage != vsbPage)) {
    vsbMax = nMax;
    vsbPage = nPage;
    // Layout if vertical scrollbar should appear or change
    if (noVScroll != noVScrollNew || !noVScroll) {
      _fxsc.layout();
      if (noVScrollNew)
        ChangeSize();  // Force scrollbar recalc
      modified = true;
    }
  }
  // Vertical scrollbar
  int line = vs.lineHeight;
  if (_fxsc.verticalScrollBar()->getLine() != line) {
    _fxsc.verticalScrollBar()->setLine(line);
    modified = true;
  }
  // Horizontal scrollbar
  PRectangle rcText = GetTextRectangle();
  unsigned int pageWidth = rcText.Width();
  if ((_fxsc.horizontalScrollBar()->getPage() != int(pageWidth)) ||
      (_fxsc.horizontalScrollBar()->getLine() != 10)) {
    _fxsc.horizontalScrollBar()->setPage(pageWidth);
    _fxsc.horizontalScrollBar()->setLine(10);
    modified = true;
  }
  return modified;
}

// --------------------------------------------------------------------
// Synchronously paint a rectangle of the window.
void ScintillaFOX::SyncPaint(PRectangle rc) {
  paintState = painting;
  rcPaint = rc;
  PRectangle rcClient = GetClientRectangle();
  paintingAllText = rcPaint.Contains(rcClient);
  //Platform::DebugPrintf("ScintillaGTK::SyncPaint %0d,%0d %0d,%0d\n",
  //  rcPaint.left, rcPaint.top, rcPaint.right, rcPaint.bottom);
  Surface *sw = Surface::Allocate();
  if (sw) {
    sw->Init(wMain.GetID(), wMain.GetID());
    Paint(sw, rcPaint);
    sw->Release();
    delete sw;
  }
  if (paintState == paintAbandoned) {
    // Painting area was insufficient to cover new styling or brace highlight positions
    FullPaint();
  }
  paintState = notPainting;
}

// Redraw all of text area. This paint will not be abandoned.
void ScintillaFOX::FullPaint() {
  paintState = painting;
  rcPaint = GetClientRectangle();
  //Platform::DebugPrintf("ScintillaGTK::FullPaint %0d,%0d %0d,%0d\n",
  //  rcPaint.left, rcPaint.top, rcPaint.right, rcPaint.bottom);
  paintingAllText = true;
  if (wMain.GetID()) {
    Surface *sw = Surface::Allocate();
    if (sw) {
      sw->Init(wMain.GetID(), wMain.GetID());
      Paint(sw, rcPaint);
      sw->Release();
      delete sw;
    }
  }
  paintState = notPainting;
}

// ====================================================================
long Platform::SendScintilla(
    WindowID w, unsigned int msg, unsigned long wParam, long lParam) {
  return static_cast<FXScintilla *>(w)->sendMessage(msg, wParam, lParam);
}

long Platform::SendScintillaPointer(WindowID w, unsigned int msg,
                                    unsigned long wParam, void *lParam) {
  return static_cast<FXScintilla *>(w)->
    sendMessage(msg, wParam, reinterpret_cast<sptr_t>(lParam));
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// FXScintilla
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

FXDEFMAP(FXScintilla) FXScintillaMap[]={
  FXMAPFUNCS(SEL_COMMAND, FXScrollArea::ID_LAST, FXScintilla::idLast, FXScintilla::onScintillaCommand),
  FXMAPFUNC(SEL_COMMAND, 0, FXScintilla::onCommand),
  FXMAPFUNC(SEL_CHANGED, 0, FXScintilla::onChanged),
  FXMAPFUNC(SEL_PAINT, 0, FXScintilla::onPaint),
  FXMAPFUNC(SEL_CONFIGURE,0,FXScintilla::onConfigure),
  FXMAPFUNC(SEL_TIMEOUT,FXScintilla::ID_TICK,FXScintilla::onTimeoutTicking),
  FXMAPFUNC(SEL_FOCUSIN,0,FXScintilla::onFocusIn),
  FXMAPFUNC(SEL_FOCUSOUT,0,FXScintilla::onFocusOut),
  FXMAPFUNC(SEL_MOTION,0,FXScintilla::onMotion),
  FXMAPFUNC(SEL_LEFTBUTTONPRESS,0,FXScintilla::onLeftBtnPress),
  FXMAPFUNC(SEL_LEFTBUTTONRELEASE,0,FXScintilla::onLeftBtnRelease),
  FXMAPFUNC(SEL_RIGHTBUTTONPRESS,0,FXScintilla::onRightBtnPress),
  FXMAPFUNC(SEL_MIDDLEBUTTONPRESS,0,FXScintilla::onMiddleBtnPress),
  FXMAPFUNC(SEL_BEGINDRAG,0,FXScintilla::onBeginDrag),
  FXMAPFUNC(SEL_DRAGGED,0,FXScintilla::onDragged),
  FXMAPFUNC(SEL_ENDDRAG,0,FXScintilla::onEndDrag),
  FXMAPFUNC(SEL_DND_ENTER,0,FXScintilla::onDNDEnter),
  FXMAPFUNC(SEL_DND_LEAVE,0,FXScintilla::onDNDLeave),
  FXMAPFUNC(SEL_DND_DROP,0,FXScintilla::onDNDDrop),
  FXMAPFUNC(SEL_DND_MOTION,0,FXScintilla::onDNDMotion),
  FXMAPFUNC(SEL_DND_REQUEST,0,FXScintilla::onDNDRequest),
  FXMAPFUNC(SEL_SELECTION_LOST,0,FXScintilla::onSelectionLost),
  FXMAPFUNC(SEL_SELECTION_GAINED,0,FXScintilla::onSelectionGained), // JKP
  FXMAPFUNC(SEL_SELECTION_REQUEST,0,FXScintilla::onSelectionRequest),
  FXMAPFUNC(SEL_CLIPBOARD_LOST,0,FXScintilla::onClipboardLost),
  FXMAPFUNC(SEL_CLIPBOARD_GAINED,0,FXScintilla::onClipboardGained), // JKP
  FXMAPFUNC(SEL_CLIPBOARD_REQUEST,0,FXScintilla::onClipboardRequest),
  FXMAPFUNC(SEL_KEYPRESS,0,FXScintilla::onKeyPress),
  FXMAPFUNC(SEL_CHORE,FXScintilla::ID_IDLE,FXScintilla::onChoreIdle),
};

FXIMPLEMENT(FXScintilla,FXScrollArea,FXScintillaMap,ARRAYNUMBER(FXScintillaMap))

FXScintilla::FXScintilla()
{
}

FXScintilla::FXScintilla(FXComposite * p, FXObject * tgt, FXSelector sel,
    FXuint opts, FXint x, FXint y, FXint w, FXint h) :
  FXScrollArea(p, opts, x, y, w, h)
{
  flags|=FLAG_ENABLED;
  target = tgt;
  message = sel;
  _scint = new ScintillaFOX(*this);
}

FXScintilla::~FXScintilla()
{
  delete _scint;
}

void FXScintilla::create()
{
  FXScrollArea::create();
  _scint->wMain.SetCursor(Window::cursorArrow);
  if(!textType){textType=getApp()->registerDragType(textTypeName);}
  if(!utf8Type){utf8Type=getApp()->registerDragType(utf8TypeName);}
  if(!urilistType){urilistType=getApp()->registerDragType(urilistTypeName);}
  dropEnable();
}


bool FXScintilla::canFocus() const
{
  return true;
}

long FXScintilla::onScintillaCommand(FXObject *, FXSelector sel, void *)
{
  _scint->Command(FXSELID(sel)-SCID(0));
  return 1;
}

long FXScintilla::onCommand(FXObject *, FXSelector sel, void * ptr)
{
  if (target)
    return target->handle(this, MKUINT(message, FXSELTYPE(sel)), ptr);
  return 0;
}

long FXScintilla::onChanged(FXObject *, FXSelector sel, void * ptr)
{
  if (target)
    return target->handle(this, MKUINT(message, FXSELTYPE(sel)), ptr);
  return 0;
}

long FXScintilla::onPaint(FXObject *, FXSelector, void * ptr)
{
  FXEvent   *ev=(FXEvent*)ptr;
  PRectangle rcPaint(ev->rect.x, ev->rect.y, ev->rect.x + ev->rect.w - 1, ev->rect.y + ev->rect.h - 1);
  _scint->SyncPaint(rcPaint);
  return 1;
}

long FXScintilla::onTimeoutTicking(FXObject *, FXSelector, void *)
{
#ifdef FOX_1_6
  FXApp::instance()->addTimeout(this, ID_TICK, _scint->timer.tickSize);
#else
  FXApp::instance()->addTimeout(this, ID_TICK, _scint->timer.tickSize*1000000);
#endif
  _scint->Tick();
  return 1;
}

long FXScintilla::onChoreIdle(FXObject *, FXSelector, void *)
{
  // Idler will be automatically stoped, if there is nothing
  // to do while idle.
  bool ret = _scint->Idle();
  if (ret == false) {
    _scint->SetIdle(false);
  }
  return 1;
}

long FXScintilla::onFocusIn(FXObject * sender, FXSelector sel, void * ptr)
{
  FXScrollArea::onFocusIn(sender, sel, ptr);
  _scint->SetFocusState(true);
  return 1;
}

long FXScintilla::onFocusOut(FXObject * sender, FXSelector sel, void * ptr)
{
  FXScrollArea::onFocusOut(sender, sel, ptr);
  _scint->SetFocusState(false);
  return 1;
}

long FXScintilla::onMotion(FXObject *, FXSelector, void * ptr)
{
  FXEvent * ev = static_cast<FXEvent *>(ptr);
  if (_scint->tryDrag) {
    _scint->tryDrag = false;
    handle(this, MKUINT(0, SEL_BEGINDRAG), 0);
  }
  if (isDragging()) {
    return handle(this, MKUINT(0, SEL_DRAGGED), ptr);
  }
  else {
    Point pt(ev->win_x, ev->win_y);
    _scint->ButtonMove(pt);
  }
  return 1;
}

long FXScintilla::onLeftBtnPress(FXObject *, FXSelector, void * ptr)
{
//  if (FXScrollArea::onLeftBtnPress(sender, sel, ptr))
//    return 1;
  handle(this, MKUINT(0, SEL_FOCUS_SELF), ptr);
  setFocus();
  FXEvent * ev = static_cast<FXEvent *>(ptr);
  Point pt;
  pt.x = ev->win_x;
  pt.y = ev->win_y;
    //sciThis->ButtonDown(pt, event->time,
    //  event->state & GDK_SHIFT_MASK,
    //  event->state & GDK_CONTROL_MASK,
    //  event->state & GDK_MOD1_MASK);
    // Instead of sending literal modifiers use control instead of alt
    // This is because all the window managers seem to grab alt + click for moving
  _scint->ButtonDown(pt, ev->time,
      ev->state & SHIFTMASK,
      ev->state & CONTROLMASK,
      ev->state & CONTROLMASK);
  return 1;
}

long FXScintilla::onLeftBtnRelease(FXObject *, FXSelector, void * ptr)
{
  if (isDragging())
    return handle(this, MKUINT(0, SEL_ENDDRAG), ptr);
  FXEvent * ev = static_cast<FXEvent *>(ptr);
  Point pt(ev->win_x, ev->win_y);
  if (!_scint->HaveMouseCapture()) {
    if (_scint->tryDrag) {
      _scint->tryDrag = false;
      _scint->SetEmptySelection(_scint->PositionFromLocation(pt));
      _scint->SetDragPosition(SelectionPosition(invalidPosition));
    }
    return 1;
  }
  _scint->ButtonUp(pt, ev->time, (ev->state & CONTROLMASK) != 0);
  return 1;
}

long FXScintilla::onRightBtnPress(FXObject *, FXSelector sel, void * ptr)
{
//  if (FXScrollArea::onRightBtnPress(sender, sel, ptr))
//    return 1;
  if (target && target->handle(this, MKUINT(message, FXSELTYPE(sel)), ptr))
    return 1;
  if (!_scint->getDisplayPopupMenu())
    return 0;
  FXEvent * ev = static_cast<FXEvent *>(ptr);
  _scint->ContextMenu(Point(ev->root_x, ev->root_y));
  return 1;
}

long FXScintilla::onMiddleBtnPress(FXObject *, FXSelector, void * ptr)
{
//  if (FXScrollArea::onMiddleBtnPress(sender, sel, ptr))
//    return 1;
  int pos;
  Point pt;
  pt.x = ((FXEvent *)ptr)->win_x;
  pt.y = ((FXEvent *)ptr)->win_y;
  pos=_scint->PositionFromLocation(pt);
  _scint->ReceivedSelection(FROM_SELECTION, pos); 
//  _scint->currentPos = pos; // JKP: Delay asignment of currentPos until AFTER ReceivedSelection()
//  _scint->anchor = pos; // JKP
  return 1;
}

// ********************************************************************
// Keyboard
// ********************************************************************

// Map the keypad keys to their equivalent functions
static int KeyTranslate(int keyIn) {
  switch (keyIn) {
    case KEY_ISO_Left_Tab:
      return SCK_TAB;
    case KEY_KP_Down:
      return SCK_DOWN;
    case KEY_KP_Up:
      return SCK_UP;
    case KEY_KP_Left:
      return SCK_LEFT;
    case KEY_KP_Right:
      return SCK_RIGHT;
    case KEY_KP_Home:
      return SCK_HOME;
    case KEY_KP_End:
      return SCK_END;
    case KEY_KP_Page_Up:
      return SCK_PRIOR;
    case KEY_KP_Page_Down:
      return SCK_NEXT;
    case KEY_KP_Delete:
      return SCK_DELETE;
    case KEY_KP_Insert:
      return SCK_INSERT;
    case KEY_KP_Enter:
      return SCK_RETURN;
      
    case KEY_Down:
      return SCK_DOWN;
    case KEY_Up:
      return SCK_UP;
    case KEY_Left:
      return SCK_LEFT;
    case KEY_Right:
      return SCK_RIGHT;
    case KEY_Home:
      return SCK_HOME;
    case KEY_End:
      return SCK_END;
    case KEY_Page_Up:
      return SCK_PRIOR;
    case KEY_Page_Down:
      return SCK_NEXT;
    case KEY_Delete:
      return SCK_DELETE;
    case KEY_Insert:
      return SCK_INSERT;
    case KEY_Escape:
      return SCK_ESCAPE;
    case KEY_BackSpace:
      return SCK_BACK;
    case KEY_Tab:
      return SCK_TAB;
    case KEY_Return:
      return SCK_RETURN;
    case KEY_KP_Add:
      return SCK_ADD;
    case KEY_KP_Subtract:
      return SCK_SUBTRACT;
    case KEY_KP_Divide:
      return SCK_DIVIDE;
    default:
      return keyIn;
  }
}

long FXScintilla::onKeyPress(FXObject* sender,FXSelector sel,void* ptr)
{
  // Try handling it in base class first
  if (FXScrollArea::onKeyPress(sender,sel,ptr)) return 1;

  FXEvent *event=(FXEvent*)ptr;
  bool shift = (event->state & SHIFTMASK) != 0;
  bool ctrl = (event->state & CONTROLMASK) != 0;
  bool alt = (event->state & ALTMASK) != 0;
  // <FIXME> Workaround for event->code doesn't hold the correct
  // KEY_xxx under WIN32
#ifndef WIN32
  int key = (event->code) ? event->code : event->text[0];
#else
  if (!event->text[0] && (event->code < 256))
    return 1;
  int key = (((unsigned int)(event->text[0]) >= 32) && !ctrl && !alt) ?
    event->text[0] : event->code;
#endif
  // </FIXME>
  if (ctrl && (key < 128))
    key = toupper(key);
  else if (!ctrl && (key >= KEY_KP_Multiply && key <= KEY_KP_9))
    key &= 0x7F;
  // Hack for keys over 256 and below command keys but makes Hungarian work.
  // This will have to change for Unicode
  else if ((key >= 0x100) && (key < 0x1000))
    key &= 0xff;
  else  
    key = KeyTranslate(key);

  bool consumed = false;
  bool added = _scint->KeyDown(key, shift, ctrl, alt, &consumed) != 0;
  if (!consumed)
    consumed = added;
  //Platform::DebugPrintf("SK-key: %d %x %x\n",event->code, event->state, consumed);
  if (event->code == 0xffffff && event->text.length() > 0) {
    _scint->ClearSelection();
    if (_scint->pdoc->InsertCString(_scint->CurrentPosition(), (const char*)event->text.text())) {
      _scint->MovePositionTo(_scint->CurrentPosition() + event->text.length());
    }
    consumed = true;
  }
  return consumed ? 1 : 0;
}

// ********************************************************************
// Clipboard
// ********************************************************************

// We lost the clipboard somehow
long FXScintilla::onClipboardLost(FXObject* sender,FXSelector sel,void* ptr){
  FXScrollArea::onClipboardLost(sender,sel,ptr);
  //Platform::DebugPrintf("Clipboard lost\n");
  _scint->copyText.Free();

  return 1;
}

// Somebody wants our clipboard
long FXScintilla::onClipboardRequest(FXObject* sender,FXSelector sel,void* ptr){
  FXEvent *event=(FXEvent*)ptr;
  FXuchar *cbdata;
  FXDragType types[]={utf8Type,stringType,0};

  // Try handling it in base class first
  if(FXScrollArea::onClipboardRequest(sender,sel,ptr)) return 1;

  for (FXDragType *dt=InUTF8Mode(_scint)?types:types+1; *dt; dt++) {
    if(event->target==*dt){
      // <FIXME> Framework taken from FXTextField.cpp - Should have a look to FXText.cpp too!
      size_t len=strlen(_scint->copyText.s);
      FXCALLOC(&cbdata,FXuchar,len+1);
      memcpy(cbdata,_scint->copyText.s,len);
  #ifndef WIN32
      setDNDData(FROM_CLIPBOARD,*dt,cbdata,len);
  #else
      setDNDData(FROM_CLIPBOARD,*dt,cbdata,len+1);
  #endif
      // </FIXME>
      return 1;
    }
  }
  return 0;
}

// ********************************************************************
// Drag'n drop
// ********************************************************************

// Start a drag operation
long FXScintilla::onBeginDrag(FXObject* sender,FXSelector sel,void* ptr){
  _scint->SetMouseCapture(true);
  if (FXScrollArea::onBeginDrag(sender,sel,ptr)) return 1;
  beginDrag(&textType,1);
  setDragCursor(getApp()->getDefaultCursor(DEF_DNDSTOP_CURSOR));
  return 1;
}


// End drag operation
long FXScintilla::onEndDrag(FXObject* sender,FXSelector sel,void* ptr){
  _scint->inDragDrop = _scint->ddNone; // <-JKP: Fix for mouse not releasing.
  _scint->SetMouseCapture(false);
  if (FXScrollArea::onEndDrag(sender,sel,ptr)) return 1;
  endDrag((didAccept()!=DRAG_REJECT));
  setDragCursor(getApp()->getDefaultCursor(DEF_TEXT_CURSOR));
  _scint->SetDragPosition(SelectionPosition(invalidPosition));
  return 1;
}


// Dragged stuff around
long FXScintilla::onDragged(FXObject* sender,FXSelector sel,void* ptr) {
  FXEvent* event=(FXEvent*)ptr;
  FXDragAction action;
  if(FXScrollArea::onDragged(sender,sel,ptr)) return 1;
  action=DRAG_COPY;
  if (!_scint->pdoc->IsReadOnly()) {
    if (isDropTarget()) action=DRAG_MOVE;
    if (event->state&CONTROLMASK) action=DRAG_COPY;
    if (event->state&SHIFTMASK) action=DRAG_MOVE;
  }
  handleDrag(event->root_x,event->root_y,action);

  switch (didAccept()) {
    case DRAG_MOVE:
      setDragCursor(getApp()->getDefaultCursor(DEF_DNDMOVE_CURSOR));
      break;
    case DRAG_COPY:
      setDragCursor(getApp()->getDefaultCursor(DEF_DNDCOPY_CURSOR));
      break;
    default:
      setDragCursor(getApp()->getDefaultCursor(DEF_DNDSTOP_CURSOR));
      break;
  }
  return 1;
}


// Handle drag-and-drop enter
long FXScintilla::onDNDEnter(FXObject* sender,FXSelector sel,void* ptr) {
  FXScrollArea::onDNDEnter(sender,sel,ptr);
  return 1;
}


// Handle drag-and-drop leave
long FXScintilla::onDNDLeave(FXObject* sender,FXSelector sel,void* ptr) {
  stopAutoScroll();
  FXScrollArea::onDNDLeave(sender,sel,ptr);
  return 1;
}

// Handle drag-and-drop motion
long FXScintilla::onDNDMotion(FXObject* sender,FXSelector sel,void* ptr){
  FXEvent* ev=(FXEvent*)ptr;
  FXDragAction action;
  FXint pos;
  // Scroll into view
  if(startAutoScroll(ev, TRUE)) return 1;

  // Handled elsewhere
  if(FXScrollArea::onDNDMotion(sender,sel,ptr)) return 1;

  // Correct drop type
  if (offeredDNDType(FROM_DRAGNDROP,textType) ){

    // Is target editable?
    if (!_scint->pdoc->IsReadOnly()) {
      action=inquireDNDAction();
      // Check for legal DND action
      if (action==DRAG_COPY || action==DRAG_MOVE) {
        Point npt(ev->win_x, ev->win_y);
        pos = _scint->PositionFromLocation(npt);
        if (!_scint->inDragDrop) {
          _scint->inDragDrop = _scint->ddDragging; /*** or ddInitial ??? ***/
          _scint->ptMouseLastBeforeDND = _scint->ptMouseLast;
        }
        _scint->ptMouseLast = npt;
        _scint->SetDragPosition(SelectionPosition(pos));

        // JKP: Changed the logic here...
        if ( (!_scint->PositionInSelection(pos)) || (_scint->pdoc->Length()==0) ) {
          acceptDrop(DRAG_ACCEPT);
          setFocus();
        }
      }
    }
    return 1;
  }

  // Didn't handle it here
  return 0;
}


// Handle drag-and-drop drop
long FXScintilla::onDNDDrop(FXObject* sender,FXSelector sel,void* ptr){
  FXuchar *dnddata,*junk;
  FXuint len,dum;

  // Stop scrolling
  stopAutoScroll();

  // Try handling it in base class first
  if(FXScrollArea::onDNDDrop(sender,sel,ptr)) return 1;

  // Should really not have gotten this if non-editable
  if (!_scint->pdoc->IsReadOnly()) {

    // Try handle here
    if(getDNDData(FROM_DRAGNDROP,textType,dnddata,len)){
      _scint->dragWasDropped = true;
      FXRESIZE(&dnddata,FXchar,len+1); dnddata[len]='\0';

      // Need to ask the source to delete his copy
      if(inquireDNDAction()==DRAG_MOVE){
        getDNDData(FROM_DRAGNDROP,deleteType,junk,dum);
        FXASSERT(!junk);
      }

      // Insert the new text
      bool isRectangular = ((len > 0) &&
          (dnddata[len] == 0 && dnddata[len-1] == '\n'));
      _scint->DropAt(_scint->posDrop, (const char *)(dnddata), false, isRectangular);
    
      FXFREE(&dnddata);
    }
    else if (getDNDData(FROM_DRAGNDROP,urilistType,dnddata,len)) {
      _scint->dragWasDropped = true;
      FXRESIZE(&dnddata,FXchar,len+1); dnddata[len]='\0';
      _scint->NotifyURIDropped((FXchar *)dnddata);
    }
    return 1;
  }
  return 0;
}

// Service requested DND data
long FXScintilla::onDNDRequest(FXObject* sender,FXSelector sel,void* ptr) {
  FXEvent *event=(FXEvent*)ptr;

  // Perhaps the target wants to supply its own data
  if(FXScrollArea::onDNDRequest(sender,sel,ptr)) return 1;

  // Return dragged text
  if(event->target==textType){
    if (_scint->primary.s == NULL) {
      _scint->CopySelectionRange(&_scint->primary);

    }
        if (_scint->primary.s) { /* JKP: This will crash if _scint->primary.s is NULL, so we test it first !!! */
          setDNDData(FROM_DRAGNDROP,stringType,(FXuchar *)strdup(_scint->primary.s),strlen(_scint->primary.s));
        } else { setDNDData(FROM_DRAGNDROP,stringType,NULL,0); }
    return 1;
    }

  // Delete dragged text
  if(event->target==deleteType){
    if (!_scint->pdoc->IsReadOnly()) {
      if (isDragging()) {
        int selStart = _scint->SelectionStart().Position();
        int selEnd = _scint->SelectionEnd().Position();
        if (_scint->posDrop.Position() > selStart) {
          if (_scint->posDrop.Position() > selEnd)
            _scint->posDrop.SetPosition(_scint->posDrop.Position() - (selEnd-selStart));
          else
            _scint->posDrop.SetPosition(selStart);
          _scint->posDrop.SetPosition(_scint->pdoc->ClampPositionIntoDocument(_scint->posDrop.Position()));
        }
      }
      _scint->ClearSelection();
      }
    return 1;
    }

  return 0;
}

// ********************************************************************
// Selection
// ********************************************************************

// We lost the selection somehow
long FXScintilla::onSelectionLost(FXObject* sender,FXSelector sel,void* ptr){
  FXbool hadselection=hasSelection();  
  FXScrollArea::onSelectionLost(sender,sel,ptr);
  if (hadselection) {
    _scint->UnclaimSelection();
  }
  return 1;
}


// Somebody wants our selection
long FXScintilla::onSelectionRequest(FXObject* sender,FXSelector sel,void* ptr){
  FXEvent *event=(FXEvent*)ptr;
  FXDragType types[]={utf8Type,stringType,0};
  // Perhaps the target wants to supply its own data for the selection
  if (FXScrollArea::onSelectionRequest(sender,sel,ptr)) { return 1; }

  for (FXDragType *dt=InUTF8Mode(_scint)?types:types+1; *dt; dt++) {
    // Return text of the selection
    if (event->target==*dt) {
      if (_scint->primary.s == NULL) {
        _scint->CopySelectionRange(&_scint->primary);
      }
      if (_scint->primary.s) {
        setDNDData(FROM_SELECTION,*dt,(FXuchar *)strdup(_scint->primary.s),strlen(_scint->primary.s));
        return 1;
      }
    }
  }
  return 0;
}

// ********************************************************************
// Scrolling
// ********************************************************************

FXint FXScintilla::getViewportWidth()
{
  return _scint->GetTextRectangle().Width();
  //return FXScrollArea::getViewportWidth();
}

FXint FXScintilla::getViewportHeight()
{
  //return (_scint) ? _scint->GetTextRectangle().Height() : FXScrollArea::getViewportHeight();
#ifdef FOX_1_6
  return FXScrollArea::getViewportHeight();
#else
  return FXScrollArea::getVisibleHeight();
#endif
}

FXint FXScintilla::getContentWidth()
{
  return FXMAX(_scint->scrollWidth, 0);
}

FXint FXScintilla::getContentHeight()
{
  return
    _scint->cs.LinesDisplayed() * _scint->vs.lineHeight +
    // This part is a kind of magic, to have the correct scrollbar
    // behavior regarding the last line of the text
    (getViewportHeight() + _scint->vs.maxDescent) % _scint->vs.lineHeight;
}

void FXScintilla::moveContents(FXint x,FXint y)
{
  _scint->inMoveContents = true;
  bool moved = false;
  int line = (-y + _scint->vs.lineHeight / 2) / _scint->vs.lineHeight;

#ifdef FOX_1_6
    if (line != getYPosition()/_scint->vs.lineHeight)
    {
    moved = true;
    _scint->ScrollTo(line);
  }
    if (x != getXPosition())
    {
    moved = true;
    _scint->HorizontalScrollTo(-x);
  }
#else
  FXint gpx,gpy;
  getPosition(gpx,gpy);
  if (line != gpy/_scint->vs.lineHeight) 
    {
    moved = true;
    _scint->ScrollTo(line);
  }
  getPosition(gpx,gpy);
  if (x != gpx) 
    {
    moved = true;
    _scint->HorizontalScrollTo(-x);
  }

#endif

  if (moved) {
    FXScrollArea::moveContents(x, -line * _scint->vs.lineHeight);
  }
  _scint->inMoveContents = false;
}

long FXScintilla::onConfigure(FXObject *sender, FXSelector sel, void * ptr)
{
  long ret = FXScrollArea::onConfigure(sender, sel, ptr);
  _scint->ChangeSize();
  return ret;
}

// ********************************************************************
// Messenging
// ********************************************************************

void FXScintilla::setScintillaID(int sid)
{
  _scint->ctrlID = sid;
}

sptr_t FXScintilla::sendMessage(unsigned int iMessage, uptr_t wParam, sptr_t lParam) {
  return _scint->WndProc(iMessage, wParam, lParam);
}


