/** FXScintilla source code edit control
 *
 *  FXScintilla.h - Interface for the FOX Scintilla widget
 *
 *  Copyright 2001-2004 by Gilles Filippini <gilles.filippini@free.fr>
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

#ifndef FXScintilla_H
#define FXScintilla_H

class ScintillaFOX;

#define SCID(id) (FXScrollArea::ID_LAST+id)

class FXAPI FXScintilla : public FXScrollArea
{
  friend class ScintillaFOX;
  FXDECLARE(FXScintilla)
protected:
  ScintillaFOX * _scint;
protected:
  FXScintilla();
public:
  enum {
    idLast = SCID(100),
    ID_TICK,
    ID_IDLE,
    ID_LAST
  };
public:
  static FXString version();
  long onScintillaCommand(FXObject *, FXSelector, void*);
  long onCommand(FXObject *, FXSelector, void*);
  long onChanged(FXObject *, FXSelector, void*);
  long onPaint(FXObject *, FXSelector, void *);
  long onConfigure(FXObject *, FXSelector, void *);
  long onTimeoutTicking(FXObject *, FXSelector, void *);
  long onChoreIdle(FXObject *, FXSelector, void *);
  long onFocusIn(FXObject *, FXSelector, void *);
  long onFocusOut(FXObject *, FXSelector, void *);
  long onMotion(FXObject *, FXSelector, void *);
  long onLeftBtnPress(FXObject *, FXSelector, void *);
  long onLeftBtnRelease(FXObject *, FXSelector, void *);
  long onRightBtnPress(FXObject *, FXSelector, void *);
  long onMiddleBtnPress(FXObject *, FXSelector, void *);
  long onBeginDrag(FXObject *, FXSelector, void *);
  long onDragged(FXObject *, FXSelector, void *);
  long onEndDrag(FXObject *, FXSelector, void *);
  long onDNDEnter(FXObject *, FXSelector, void *);
  long onDNDLeave(FXObject *, FXSelector, void *);
  long onDNDMotion(FXObject *, FXSelector, void *);
  long onDNDDrop(FXObject *, FXSelector, void *);
  long onDNDRequest(FXObject *, FXSelector, void *);
  long onSelectionLost(FXObject *, FXSelector, void *);
  long onSelectionRequest(FXObject *, FXSelector, void *);
  long onClipboardLost(FXObject *, FXSelector, void *);
  long onClipboardRequest(FXObject *, FXSelector, void *);
  long onKeyPress(FXObject *, FXSelector, void *);
//  long onSelectionGained(FXObject *, FXSelector, void *); // JKP
//  long onClipboardGained(FXObject *, FXSelector, void *); // JKP
public:
  /// Constructor
  FXScintilla(FXComposite * p, FXObject * tgt = NULL, FXSelector sel = 0,
    FXuint opts = 0, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0);

  // From FXScrollArea
  virtual void create();
  virtual bool canFocus() const;

  virtual FXint getViewportWidth();
  virtual FXint getViewportHeight();
  virtual FXint getContentWidth();
  virtual FXint getContentHeight();
  virtual void moveContents(FXint x,FXint y);

  /// Scintilla messaging
  void setScintillaID(int sid);
  sptr_t sendMessage(unsigned int iMessage, uptr_t wParam, sptr_t lParam);

  /// Destructor
  ~FXScintilla();
};

#endif // FXScintilla_H
