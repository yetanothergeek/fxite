/* lexname=metapost lexpfx=SCE_METAPOST_ lextag=SCLEX_METAPOST */

static StyleDef metapost_style[] = {
  { "default", SCE_METAPOST_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "special", SCE_METAPOST_SPECIAL, _DEFLT_FG, _DEFLT_BG, Normal },
  { "group", SCE_METAPOST_GROUP, _DEFLT_FG, _DEFLT_BG, Normal },
  { "symbol", SCE_METAPOST_SYMBOL, _DEFLT_FG, _DEFLT_BG, Normal },
  { "command", SCE_METAPOST_COMMAND, _DEFLT_FG, _DEFLT_BG, Normal },
  { "text", SCE_METAPOST_TEXT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "extra", SCE_METAPOST_EXTRA, _DEFLT_FG, _DEFLT_BG, Normal },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* metapost_words[]= {
  "btex verbatimtex etexcharcode day linecap linejoin miterlimit month pausing prologues showstopping time tracingcapsules tracingchoices tracingcommands tracingequations tracinglostchars tracingmacros tracingonline tracingoutput tracingrestores tracingspecs tracingstats tracingtitles truecorners warningcheck year false nullpicture pencircle true and angle arclength arctime ASCII bluepart boolean bot char color cosd cycle decimal directiontime floor fontsize greenpart hex infont intersectiontimes known length llcorner lrcorner makepath makepen mexp mlog normaldeviate not numeric oct odd or path pair pen penoffset picture point postcontrol precontrol redpart reverse rotated scaled shifted sind slanted sqrt str string subpath substring transform transformed ulcorner uniformdeviate unknown urcorner xpart xscaled xxpart xypart ypart yscaled yxpart yypart zscaled addto clip input interim let newinternal save setbounds shipout show showdependencies showtoken showvariable special begingroup endgroup of curl tension and controls reflectedabout rotatedaround interpath on off beginfig endfig def vardef enddef epxr suffix text primary secondary tertiary primarydef secondarydef tertiarydef top bottom ulft urt llft lrt randomseed also contour doublepath withcolor withpen dashed if else elseif fi for endfor forever exitif forsuffixes downto upto step until charlist extensible fontdimen headerbyte kern ligtable boundarychar chardp charext charht charic charwd designsize fontmaking charexists cullit currenttransform gfcorners grayfont hround imagerules lowres_fix nodisplays notransforms openit displaying currentwindow screen_rows screen_cols pixels_per_inch cull display openwindow numspecial totalweight autorounding fillin proofing tracingpens xoffset chardx granularity smoothing turningcheck yoffset chardy hppp tracingedges vppp extra_beginfig extra_endfig mpxbreak endahangle ahlength bboxmargin defaultpen defaultscale labeloffset background currentpen currentpicture cuttings defaultfont extra_beginfig extra_endfig beveled black blue bp butt cc cm dd ditto down epsilon evenly fullcircle green halfcircle identity in infinity left mitered mm origin pensquare pt quartercircle red right rounded squared unitsquare up white withdots abs bbox ceiling center cutafter cutbefore dir directionpoint div dotprod intersectionpoint inverse mod lft round rt unitvector whatever cutdraw draw drawarrow drawdblarrow fill filldraw drawdot loggingall pickup tracingall tracingnone undraw unfill unfilldraw buildcycle dashpattern decr dotlabel dotlabels drawoptions incr label labels max min thelabel z beginchar blacker capsule_end change_width define_blacker_pixels define_corrected_pixels define_good_x_pixels define_good_y_pixels define_horizontal_corrected_pixels define_pixels define_whole_blacker_pixels define_whole_pixels define_whole_vertical_blacker_pixels define_whole_vertical_pixels endchar extra_beginchar extra_endchar extra_setup font_coding_scheme font_extra_space",
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


