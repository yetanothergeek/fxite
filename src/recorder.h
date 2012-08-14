/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2009 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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


typedef void (*PlaybackFunc) (int message, uptr_t wParam, const sptr_t lParam, void* user_data);
typedef void (*TranslateFunc) (const char* text, void* user_data);


struct MacroMessage;


class MacroRecorder: public FXObject {
  FXObjectListOf<MacroMessage> list;
public:
  void record(int message, uptr_t wParam, sptr_t lParam);
  void playback(PlaybackFunc callback, void* user_data);
  void translate(TranslateFunc callback, void* user_data);
  const FXString translate();
  void clear();
  MacroRecorder();
  ~MacroRecorder();
};

