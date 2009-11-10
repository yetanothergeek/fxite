/* lexname=tcl lexpfx=SCE_TCL_ lextag=SCLEX_TCL */

static StyleDef tcl_style[] = {
  { "default", SCE_TCL_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "comment", SCE_TCL_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentline", SCE_TCL_COMMENTLINE, COMMNT_FG, _DEFLT_BG, Italic },
  { "number", SCE_TCL_NUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "wordinquote", SCE_TCL_WORD_IN_QUOTE, _WORD1_FG, _DEFLT_BG, Bold },
  { "inquote", SCE_TCL_IN_QUOTE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "operator", SCE_TCL_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "identifier", SCE_TCL_IDENTIFIER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "substitution", SCE_TCL_SUBSTITUTION, _DEFLT_FG, _DEFLT_BG, Normal },
  { "subbrace", SCE_TCL_SUB_BRACE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "modifier", SCE_TCL_MODIFIER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "expand", SCE_TCL_EXPAND, _DEFLT_FG, _DEFLT_BG, Normal },
  { "word", SCE_TCL_WORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "word2", SCE_TCL_WORD2, _WORD2_FG, _DEFLT_BG, Bold },
  { "word3", SCE_TCL_WORD3, _WORD1_FG, _DEFLT_BG, Bold },
  { "word4", SCE_TCL_WORD4, _WORD1_FG, _DEFLT_BG, Bold },
  { "word5", SCE_TCL_WORD5, _WORD1_FG, _DEFLT_BG, Bold },
  { "word6", SCE_TCL_WORD6, _WORD1_FG, _DEFLT_BG, Bold },
  { "word7", SCE_TCL_WORD7, _WORD1_FG, _DEFLT_BG, Bold },
  { "word8", SCE_TCL_WORD8, _WORD1_FG, _DEFLT_BG, Bold },
  { "commentbox", SCE_TCL_COMMENT_BOX, COMMNT_FG, _DEFLT_BG, Italic },
  { "blockcomment", SCE_TCL_BLOCK_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* tcl_words[]= {
  "after append array auto_execok auto_import auto_load auto_load_index auto_qualify beep bgerror binary break case catch cd clock close concat continue dde default echo else elseif encoding eof error eval exec exit expr fblocked fconfigure fcopy file fileevent flush for foreach format gets glob global history http if incr info interp join lappend lindex linsert list llength load loadTk lrange lreplace lsearch lset lsort memory msgcat namespace open package pid pkg::create pkg_mkIndex Platform-specific proc puts pwd re_syntax read regexp registry regsub rename resource return scan seek set socket source split string subst switch tclLog tclMacPkgSearch tclPkgSetup tclPkgUnknown tell time trace unknown unset update uplevel upvar variable vwait while",
  "bell bind bindtags bitmap button canvas checkbutton clipboard colors console cursors destroy entry event focus font frame grab grid image Inter-client keysyms label labelframe listbox lower menu menubutton message option options pack panedwindow photo place radiobutton raise scale scrollbar selection send spinbox text tk tk_chooseColor tk_chooseDirectory tk_dialog tk_focusNext tk_getOpenFile tk_messageBox tk_optionMenu tk_popup tk_setPalette tkerror tkvars tkwait toplevel winfo wish wm",
  "tk_bisque tk_chooseColor tk_dialog tk_focusFollowsMouse tk_focusNext tk_focusPrev tk_getOpenFile tk_getSaveFile tk_messageBox tk_optionMenu tk_popup tk_setPalette tk_textCopy tk_textCut tk_textPaste tkButtonAutoInvoke tkButtonDown tkButtonEnter tkButtonInvoke tkButtonLeave tkButtonUp tkCancelRepeat tkCheckRadioDown tkCheckRadioEnter tkCheckRadioInvoke tkColorDialog tkColorDialog_BuildDialog tkColorDialog_CancelCmd tkColorDialog_Config tkColorDialog_CreateSelector tkColorDialog_DrawColorScale tkColorDialog_EnterColorBar tkColorDialog_HandleRGBEntry tkColorDialog_HandleSelEntry tkColorDialog_InitValues tkColorDialog_LeaveColorBar tkColorDialog_MoveSelector tkColorDialog_OkCmd tkColorDialog_RedrawColorBars tkColorDialog_RedrawFinalColor tkColorDialog_ReleaseMouse tkColorDialog_ResizeColorBars tkColorDialog_RgbToX tkColorDialog_SetRGBValue tkColorDialog_StartMove tkColorDialog_XToRgb tkConsoleAbout tkConsoleBind tkConsoleExit tkConsoleHistory tkConsoleInit tkConsoleInsert tkConsoleInvoke tkConsoleOutput tkConsolePrompt tkConsoleSource tkDarken tkEntryAutoScan tkEntryBackspace tkEntryButton1 tkEntryClosestGap tkEntryGetSelection tkEntryInsert tkEntryKeySelect tkEntryMouseSelect tkEntryNextWord tkEntryPaste tkEntryPreviousWord tkEntrySeeInsert tkEntrySetCursor tkEntryTranspose tkEventMotifBindings tkFDGetFileTypes tkFirstMenu tkFocusGroup_BindIn tkFocusGroup_BindOut tkFocusGroup_Create tkFocusGroup_Destroy tkFocusGroup_In tkFocusGroup_Out tkFocusOK tkGenerateMenuSelect tkIconList tkIconList_Add tkIconList_Arrange tkIconList_AutoScan tkIconList_Btn1 tkIconList_Config tkIconList_Create tkIconList_CtrlBtn1 tkIconList_Curselection tkIconList_DeleteAll tkIconList_Double1 tkIconList_DrawSelection tkIconList_FocusIn tkIconList_FocusOut tkIconList_Get tkIconList_Goto tkIconList_Index tkIconList_Invoke tkIconList_KeyPress tkIconList_Leave1 tkIconList_LeftRight tkIconList_Motion1 tkIconList_Reset tkIconList_ReturnKey tkIconList_See tkIconList_Select tkIconList_Selection tkIconList_ShiftBtn1 tkIconList_UpDown tkListbox tkListboxAutoScan tkListboxBeginExtend tkListboxBeginSelect tkListboxBeginToggle tkListboxCancel tkListboxDataExtend tkListboxExtendUpDown tkListboxKeyAccel_Goto tkListboxKeyAccel_Key tkListboxKeyAccel_Reset tkListboxKeyAccel_Set tkListboxKeyAccel_Unset tkListboxMotion tkListboxSelectAll tkListboxUpDown tkMbButtonUp tkMbEnter tkMbLeave tkMbMotion tkMbPost tkMenuButtonDown tkMenuDownArrow tkMenuDup tkMenuEscape tkMenuFind tkMenuFindName tkMenuFirstEntry tkMenuInvoke tkMenuLeave tkMenuLeftArrow tkMenuMotion tkMenuNextEntry tkMenuNextMenu tkMenuRightArrow tkMenuUnpost tkMenuUpArrow tkMessageBox tkMotifFDialog tkMotifFDialog_ActivateDList tkMotifFDialog_ActivateFEnt tkMotifFDialog_ActivateFList tkMotifFDialog_ActivateSEnt tkMotifFDialog_BrowseDList tkMotifFDialog_BrowseFList tkMotifFDialog_BuildUI tkMotifFDialog_CancelCmd tkMotifFDialog_Config tkMotifFDialog_Create tkMotifFDialog_FileTypes tkMotifFDialog_FilterCmd tkMotifFDialog_InterpFilter tkMotifFDialog_LoadFiles tkMotifFDialog_MakeSList tkMotifFDialog_OkCmd tkMotifFDialog_SetFilter tkMotifFDialog_SetListMode tkMotifFDialog_Update tkPostOverPoint tkRecolorTree tkRestoreOldGrab tkSaveGrabInfo tkScaleActivate tkScaleButton2Down tkScaleButtonDown tkScaleControlPress tkScaleDrag tkScaleEndDrag tkScaleIncrement tkScreenChanged tkScrollButton2Down tkScrollButtonDown tkScrollButtonDrag tkScrollButtonUp tkScrollByPages tkScrollByUnits tkScrollDrag tkScrollEndDrag tkScrollSelect tkScrollStartDrag tkScrollTopBottom tkScrollToPos tkTabToWindow tkTearOffMenu tkTextAutoScan tkTextButton1 tkTextClosestGap tkTextInsert tkTextKeyExtend tkTextKeySelect tkTextNextPara tkTextNextPos tkTextNextWord tkTextPaste tkTextPrevPara tkTextPrevPos tkTextPrevWord tkTextResetAnchor tkTextScrollPages tkTextSelectTo tkTextSetCursor tkTextTranspose tkTextUpDownLine tkTraverseToMenu tkTraverseWithinMenu",
  "",
  "expand",
  "",
  "",
  "",
  NULL
};


static const char* tcl_mask = "*.tcl|*.exp";


static const char* tcl_apps = "";


static LangStyle LangTCL = {
  "tcl",
  SCLEX_TCL,
  tcl_style,
  (char**)tcl_words,
  (char*)tcl_mask,
  (char*)tcl_apps,
  0,
  TABS_DEFAULT
};


