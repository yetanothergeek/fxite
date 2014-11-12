/* lexname=metapost lexpfx=SCE_METAPOST_ lextag=SCLEX_METAPOST */

static StyleDef metapost_style[] = {
  { "command", SCE_METAPOST_COMMAND, _DEFLT_FG, _DEFLT_BG, Normal },
  { "default", SCE_METAPOST_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "extra", SCE_METAPOST_EXTRA, _DEFLT_FG, _DEFLT_BG, Normal },
  { "group", SCE_METAPOST_GROUP, _DEFLT_FG, _DEFLT_BG, Normal },
  { "special", SCE_METAPOST_SPECIAL, _DEFLT_FG, _DEFLT_BG, Normal },
  { "symbol", SCE_METAPOST_SYMBOL, _DEFLT_FG, _DEFLT_BG, Normal },
  { "text", SCE_METAPOST_TEXT, _DEFLT_FG, _DEFLT_BG, Normal },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* metapost_words[]= {
  "ASCII abs addto ahangle ahlength also and angle arclength arctime autorounding background bbox bboxmargin beginchar beginfig begingroup beveled black blacker blue bluepart boolean bot bottom boundarychar bp btex buildcycle butt capsule_end cc ceiling center change_width char charcode chardp chardx chardy charexists charext charht charic charlist charwd clip cm color contour controls cosd cull cullit curl currentpen currentpicture currenttransform currentwindow cutafter cutbefore cutdraw cuttings cycle dashed dashpattern day dd decimal decr def defaultfont defaultpen defaultscale define_blacker_pixels define_corrected_pixels define_good_x_pixels define_good_y_pixels define_horizontal_corrected_pixels define_pixels define_whole_blacker_pixels define_whole_pixels define_whole_vertical_blacker_pixels define_whole_vertical_pixels designsize dir directionpoint directiontime display displaying ditto div dotlabel dotlabels dotprod doublepath down downto draw drawarrow drawdblarrow drawdot drawoptions else elseif end endchar enddef endfig endfor endgroup endif epsilon epxr etex evenly exitif extensible extra_beginchar extra_beginfig extra_endchar extra_endfig extra_setup false fi fill filldraw fillin floor font_coding_scheme font_extra_space fontdimen fontmaking fontsize for forever forsuffixes fullcircle gfcorners granularity grayfont green greenpart halfcircle headerbyte hex hppp hround identity if imagerules in incr infinity infont input interim interpath intersectionpoint intersectiontimes inverse kern known label labeloffset labels left length let lft ligtable linecap linejoin llcorner llft loggingall lowres_fix lrcorner lrt makepath makepen max mexp min mitered miterlimit mlog mm mod month mpxbreak newinternal nodisplays normaldeviate not notransforms nullpicture numeric numspecial oct odd of off on openit openwindow or origin pair path pausing pen pencircle penoffset pensquare pickup picture pixels_per_inch point postcontrol precontrol primary primarydef prologues proofing pt quartercircle randomseed red redpart reflectedabout reverse right rotated rotatedaround round rounded rt save scaled screen_cols screen_rows secondary secondarydef setbounds shifted shipout show showdependencies showstopping showtoken showvariable sind slanted smoothing special sqrt squared step str string subpath substring suffix tension tertiary tertiarydef text thelabel time top totalweight tracingall tracingcapsules tracingchoices tracingcommands tracingedges tracingequations tracinglostchars tracingmacros tracingnone tracingonline tracingoutput tracingpens tracingrestores tracingspecs tracingstats tracingtitles transform transformed true truecorners turningcheck ulcorner ulft undraw unfill unfilldraw uniformdeviate unitsquare unitvector unknown until up upto urcorner urt vardef verbatimtex vppp warningcheck whatever white withcolor withdots withpen xoffset xpart xscaled xxpart xypart year yoffset ypart yscaled yxpart yypart z zscaled",
  "",
  NULL
};


static const char* metapost_mask = "*.mp|*.mpx";


static const char* metapost_apps = "";


static LangStyle LangMetapost = {
  "metapost",
  SCLEX_METAPOST,
  metapost_style,
  (char**)metapost_words,
  (char*)metapost_mask,
  (char*)metapost_apps,
  0,
  TABS_DEFAULT,
  0
};


