/* lexname=xml lexpfx=SCE_H lextag=SCLEX_XML */

#define xml_style html_style

static const char* xml_words[]= {
  "",
  "",
  "",
  "",
  "",
  "ELEMENT DOCTYPE ATTLIST ENTITY NOTATION",
  NULL
};


static const char* xml_mask = "*.xml|*.xsl|*.svg|*.xul|*.xsd|*.dtd|*.xslt|*.axl|*.xrc|*.rdf";


static const char* xml_apps = "";


static LangStyle LangXML = {
  "xml",
  SCLEX_XML,
  xml_style,
  (char**)xml_words,
  (char*)xml_mask,
  (char*)xml_apps,
  0,
  TABS_DEFAULT,
  0
};


