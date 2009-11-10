/* lexname=docbook lexpfx=UNKNOWN lextag=UNKNOWN */
#define SCLEX_DOCBOOK SCLEX_HTML

#define docbook_style html_style

static const char* docbook_words[]= {
  "arch condition conformance id lang os remap role revision revisionflag security userlevel vendor xreflabel status label endterm linkend space width",
  "abbrev abstract accel ackno acronym action address affiliation alt anchor answer appendix appendixinfo application area areaset areaspec arg article articleinfo artpagenums attribution audiodata audioobject author authorblurb authorgroup authorinitials beginpage bibliocoverage bibliodiv biblioentry bibliography bibliographyinfo biblioid bibliomisc bibliomixed bibliomset bibliorelation biblioset bibliosource blockinfo blockquote book bookinfo bridgehead callout calloutlist caption caution chapter chapterinfo citation citebiblioid citerefentry citetitle city classname classsynopsis classsynopsisinfo cmdsynopsis co collab collabname colophon colspec command computeroutput confdates confgroup confnum confsponsor conftitle constant constraint constraintdef constructorsynopsis contractnum contractsponsor contrib copyright coref corpauthor corpname country database date dedication destructorsynopsis edition editor email emphasis entry entrytbl envar epigraph equation errorcode errorname errortext errortype example exceptionname fax fieldsynopsis figure filename firstname firstterm footnote footnoteref foreignphrase formalpara funcdef funcparams funcprototype funcsynopsis funcsynopsisinfo function glossary glossaryinfo glossdef glossdiv glossentry glosslist glosssee glossseealso glossterm graphic graphicco group guibutton guiicon guilabel guimenu guimenuitem guisubmenu hardware highlights holder honorific htm imagedata imageobject imageobjectco important index indexdiv indexentry indexinfo indexterm informalequation informalexample informalfigure informaltable initializer inlineequation inlinegraphic inlinemediaobject interface interfacename invpartnumber isbn issn issuenum itemizedlist itermset jobtitle keycap keycode keycombo keysym keyword keywordset label legalnotice lhs lineage lineannotation link listitem iteral literallayout lot lotentry manvolnum markup medialabel mediaobject mediaobjectco member menuchoice methodname methodparam methodsynopsis mm modespec modifier ousebutton msg msgaud msgentry msgexplan msginfo msglevel msgmain msgorig msgrel msgset msgsub msgtext nonterminal note objectinfo olink ooclass ooexception oointerface option optional orderedlist orgdiv orgname otheraddr othercredit othername pagenums para paramdef parameter part partinfo partintro personblurb personname phone phrase pob postcode preface prefaceinfo primary primaryie printhistory procedure production productionrecap productionset productname productnumber programlisting programlistingco prompt property pubdate publisher publishername pubsnumber qandadiv qandaentry qandaset question quote refclass refdescriptor refentry refentryinfo refentrytitle reference referenceinfo refmeta refmiscinfo refname refnamediv refpurpose refsect1 refsect1info refsect2 refsect2info refsect3 refsect3info refsection refsectioninfo refsynopsisdiv refsynopsisdivinfo releaseinfo remark replaceable returnvalue revdescription revhistory revision revnumber revremark rhs row sbr screen screenco screeninfo screenshot secondary secondaryie sect1 sect1info sect2 sect2info sect3 sect3info sect4 sect4info sect5 sect5info section sectioninfo see seealso seealsoie seeie seg seglistitem segmentedlist segtitle seriesvolnums set setindex setindexinfo setinfo sgmltag shortaffil shortcut sidebar sidebarinfo simpara simplelist simplemsgentry simplesect spanspec state step street structfield structname subject subjectset subjectterm subscript substeps subtitle superscript surname sv symbol synopfragment synopfragmentref synopsis systemitem table tbody term tertiary tertiaryie textdata textobject tfoot tgroup thead tip title titleabbrev toc tocback tocchap tocentry tocfront toclevel1 toclevel2 toclevel3 toclevel4 toclevel5 tocpart token trademark type ulink userinput varargs variablelist varlistentry varname videodata videoobject void volumenum warning wordasword xref year",
  "",
  "",
  "",
  "ELEMENT DOCTYPE ATTLIST ENTITY NOTATION",
  NULL
};


static const char* docbook_mask = "*.docbook";


static const char* docbook_apps = "";


static LangStyle LangDocbook = {
  "docbook",
  SCLEX_DOCBOOK,
  docbook_style,
  (char**)docbook_words,
  (char*)docbook_mask,
  (char*)docbook_apps,
  0,
  TABS_DEFAULT
};


