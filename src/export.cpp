/*
Large portions of this file were adapted from the SciTE text
editor source code.

SciTE is copyright 1998-2010 by Neil Hodgson <neilh@scintilla.org>
and is released under the following license:

All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation.

NEIL HODGSON DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS, IN NO EVENT SHALL NEIL HODGSON BE LIABLE FOR ANY
SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
OR PERFORMANCE OF THIS SOFTWARE.
*/


#include <stdio.h>
#include <string.h>

#include <fx.h>
#include <FX88591Codec.h>

#include "appname.h"
#include "scidoc.h"
#include "prefs.h"
#include "lang.h"
#include "export.h"

#define CharAt(where) sci->sendMessage(SCI_GETCHARAT, where, 0)

//---------- Save to HTML ----------

void SaveToHTML(SciDoc*sci, FILE *fp )
{
  sci->sendMessage(SCI_COLOURISE, 0, -1);
  int tabSize = 4;
  int lengthDoc = sci->GetTextLength();


  bool styleIsUsed[STYLE_MAX + 1];
  bool onlyStylesUsed=true;
  if (onlyStylesUsed) {
    int i;
    for (i = 0; i <= STYLE_MAX; i++) {
      styleIsUsed[i] = false;
    }
    // check the used styles
    for (i = 0; i < lengthDoc; i++) {
      styleIsUsed[sci->sendMessage(SCI_GETSTYLEAT,i,0) & 0x7F] = true;
    }
  } else {
    for (int i = 0; i <= STYLE_MAX; i++) {
      styleIsUsed[i] = true;
    }
  }
  styleIsUsed[STYLE_DEFAULT] = true;

  fputs( "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \
    \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n", fp);

  fputs("<html xmlns=\"http://www.w3.org/1999/xhtml\">\n", fp);
  fputs("<head>\n", fp);
  fprintf(fp, "<title>%s</title>\n", FXPath::name(sci->Filename()).text());
  // Probably not used by robots, but making a little advertisement for those looking
  // at the source code doesn't hurt...
  fputs("<meta name=\"Generator\" content=\""APP_NAME" HTML export\" />\n", fp);
  if ( sci->sendMessage(SCI_GETCODEPAGE,0,0) == SC_CP_UTF8) {
    fputs("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n", fp);
  }
  fputs("<style type=\"text/css\">\n", fp);

  char* bgColour=Settings::globalStyle()->bg;
  LangStyle* ls = sci->getLanguage();

  for (int istyle = 0; istyle <= STYLE_MAX; istyle++) {
    if ((istyle > STYLE_DEFAULT) && (istyle <= STYLE_LASTPREDEFINED)) { continue; }
    if (styleIsUsed[istyle]) {
      StyleDef*sd=GetStyleFromId(ls?ls->styles:Settings::globalStyle(), istyle);
     if (!sd) { sd=Settings::globalStyle(); }
      if (istyle == STYLE_DEFAULT) {
        fprintf(fp, "span {\n");
      } else {
        fprintf(fp, ".S%0d {\n", istyle);
      }
      if (sd->style & Italic) { fprintf(fp, "  font-style: italic;\n"); }
      if (sd->style & Bold) { fprintf(fp, "  font-weight: bold;\n"); }
      if (sd->fg[0]) {
        fprintf(fp, "  color: %s;\n", sd->fg);
      } else if (istyle == STYLE_DEFAULT) {
        fprintf(fp, "  color: #000000;\n");
      }
      if (sd->bg[0]) {
        if (istyle != STYLE_DEFAULT && (strcmp(bgColour,sd->bg)!=0)) {
          fprintf(fp, "  background: %s;\n", sd->bg);
          fprintf(fp, "  text-decoration: inherit;\n");
        }
      }
      fprintf(fp, "}\n");
    }
  }
  fputs("</style>\n", fp);
  fputs("</head>\n", fp);
  if (bgColour[0]) {
    fprintf(fp, "<body bgcolor=\"%s\">\n", bgColour);
  } else {
    fputs("<body>\n", fp);
  }

  int styleCurrent = sci->sendMessage(SCI_GETSTYLEAT,0,0);
  bool inStyleSpan = false;
  bool inFoldSpan = false;
  // Global span for default attributes
  fputs("<pre>", fp);

  if (styleIsUsed[styleCurrent]) {
    fprintf(fp, "<span class=\"S%0d\">", styleCurrent);
    inStyleSpan = true;
  }
  // Else, this style has no definition (beside default one):
  // no span for it, except the global one

  int column = 0;
  for (int i = 0; i < lengthDoc; i++) {
    char ch = sci->sendMessage(SCI_GETCHARAT,i,0);
    int style = sci->sendMessage(SCI_GETSTYLEAT,i,0);

    if (style != styleCurrent) {
      if (inStyleSpan) {
        fputs("</span>", fp);
        inStyleSpan = false;
      }
      if (ch != '\r' && ch != '\n') {  // No need of a span for the EOL
        if (styleIsUsed[style]) {
          fprintf(fp, "<span class=\"S%0d\">", style);
          inStyleSpan = true;
        }
        styleCurrent = style;
      }
    }
    if (ch == ' ') {
    fputc(' ', fp);
    column++;

    } else if (ch == '\t') {
      int ts = tabSize - (column % tabSize);
      for (int itab = 0; itab < ts; itab++) { fputc(' ', fp);  }
      column += ts;
    } else if (ch == '\r' || ch == '\n') {
      if (inStyleSpan) {
        fputs("</span>", fp);
        inStyleSpan = false;
      }
      if (inFoldSpan) {
        fputs("</span>", fp);
        inFoldSpan = false;
      }
      if ((ch == '\r') && (sci->sendMessage(SCI_GETCHARAT, i + 1, 0) == '\n')) {
        i++;  // CR+LF line ending, skip the "extra" EOL char
      }
      column = 0;
      styleCurrent = sci->sendMessage(SCI_GETSTYLEAT, i + 1, 0);
      fputc('\n', fp);
      if (styleIsUsed[styleCurrent] && (CharAt(i+1) != '\r') && (CharAt(i+1) != '\n')) {
        // We know it's the correct next style,
        // but no (empty) span for an empty line
        fprintf(fp, "<span class=\"S%0d\">", styleCurrent);
        inStyleSpan = true;
      }
    } else {
      switch (ch) {
      case '<':
        fputs("&lt;", fp);
        break;
      case '>':
        fputs("&gt;", fp);
        break;
      case '&':
        fputs("&amp;", fp);
        break;
      default:
        fputc(ch, fp);
      }
      column++;
    }
  }
  if (inStyleSpan) {
    fputs("</span>", fp);
  }
  fputs("</pre>", fp);
  fputs("\n</body>\n</html>\n", fp);
}

class Point {
public:
  int x;
  int y;
  explicit Point(int x_=0, int y_=0) : x(x_), y(y_) {}
  static Point FromLong(long lpoint);
};

class PRectangle {
public:
  int left;
  int top;
  int right;
  int bottom;
  PRectangle(int left_=0, int top_=0, int right_=0, int bottom_ = 0) :
    left(left_), top(top_), right(right_), bottom(bottom_) {
  }
  bool operator==(PRectangle &rc) {
    return (rc.left == left) && (rc.right == right) &&
      (rc.top == top) && (rc.bottom == bottom);
  }
  bool Contains(Point pt) {
    return (pt.x >= left) && (pt.x <= right) &&
      (pt.y >= top) && (pt.y <= bottom);
  }
  bool Contains(PRectangle rc) {
    return (rc.left >= left) && (rc.right <= right) &&
      (rc.top >= top) && (rc.bottom <= bottom);
  }
  bool Intersects(PRectangle other) {
    return (right > other.left) && (left < other.right) &&
      (bottom > other.top) && (top < other.bottom);
  }
  void Move(int xDelta, int yDelta) {
    left += xDelta;
    top += yDelta;
    right += xDelta;
    bottom += yDelta;
  }
  int Width() { return right - left; }
  int Height() { return bottom - top; }
  bool Empty() {
    return (Height() <= 0) || (Width() <= 0);
  }
};


static int IntFromHexDigit(int ch) {
  if ((ch >= '0') && (ch <= '9')) {
    return ch - '0';
  } else if (ch >= 'A' && ch <= 'F') {
    return ch - 'A' + 10;
  } else if (ch >= 'a' && ch <= 'f') {
    return ch - 'a' + 10;
  } else {
    return 0;
  }
}

int IntFromHexByte(const char *hexByte) {
  return IntFromHexDigit(hexByte[0]) * 16 + IntFromHexDigit(hexByte[1]);
}

//---------- Save to PDF ----------

/*
  PDF Exporter. Status: Beta
  Contributed by Ahmad M. Zawawi <zeus_go64@hotmail.com>
  Modifications by Darren Schroeder Feb 22, 2003; Philippe Lhoste 2003-10
  Overhauled by Kein-Hong Man 2003-11

  This exporter is meant to be small and simple; users are expected to
  use other methods for heavy-duty formatting. PDF elements marked with
  "PDF1.4Ref" states where in the PDF 1.4 Reference Spec (the PDF file of
  which is freely available from Adobe) the particular element can be found.

  Possible TODOs that will probably not be implemented: full styling,
  optimization, font substitution, compression, character set encoding.
*/
#define PDF_TAB_DEFAULT    8
#define PDF_FONT_DEFAULT  1  // Helvetica
#define PDF_FONTSIZE_DEFAULT  10
#define PDF_SPACING_DEFAULT  1.2
#define PDF_HEIGHT_DEFAULT  792  // Letter
#define PDF_WIDTH_DEFAULT  612
#define PDF_MARGIN_DEFAULT  72  // 1.0"
#define PDF_ENCODING    "WinAnsiEncoding"

struct PDFStyle {
  char fore[24];
  int font;
};

static const char *PDFfontNames[] = {
            "Courier", "Courier-Bold", "Courier-Oblique", "Courier-BoldOblique",
            "Helvetica", "Helvetica-Bold", "Helvetica-Oblique", "Helvetica-BoldOblique",
            "Times-Roman", "Times-Bold", "Times-Italic", "Times-BoldItalic"
        };

// ascender, descender aligns font origin point with page
static short PDFfontAscenders[] =  { 629, 718, 699 };
static short PDFfontDescenders[] = { 157, 207, 217 };
static short PDFfontWidths[] =     { 600,   0,   0 };

static inline void getPDFRGB(char* pdfcolour, const char* stylecolour) {
  // grab colour components (max string length produced = 18)
  for (int i = 1; i < 6; i += 2) {
    char val[20];
    // 3 decimal places for enough dynamic range
    int c = (IntFromHexByte(stylecolour + i) * 1000 + 127) / 255;
    if (c == 0 || c == 1000) {  // optimise
      sprintf(val, "%d ", c / 1000);
    } else {
      sprintf(val, "0.%03d ", c);
    }
    strcat(pdfcolour, val);
  }
}

void SaveToPDF(SciDoc*sci,  FILE *fp)
{
  // This class conveniently handles the tracking of PDF objects
  // so that the cross-reference table can be built (PDF1.4Ref(p39))
  // All writes to fp passes through a PDFObjectTracker object.
  class PDFObjectTracker {
  private:
    FILE *fp;
    int *offsetList, tableSize;
  public:
    int index;
    PDFObjectTracker(FILE *fp_) {
      fp = fp_;
      tableSize = 100;
      offsetList = new int[tableSize];
      index = 1;
    }
    ~PDFObjectTracker() {
      delete []offsetList;
    }
    void write(const char *objectData) {
      unsigned int length = strlen(objectData);
      // note binary write used, open with "wb"
      fwrite(objectData, sizeof(char), length, fp);
    }
    void write(int objectData) {
      char val[20];
      sprintf(val, "%d", objectData);
      write(val);
    }
    // returns object number assigned to the supplied data
    int add(const char *objectData) {
      // resize xref offset table if too small
      if (index > tableSize) {
        int newSize = tableSize * 2;
        int *newList = new int[newSize];
        for (int i = 0; i < tableSize; i++) {
          newList[i] = offsetList[i];
        }
        delete []offsetList;
        offsetList = newList;
        tableSize = newSize;
      }
      // save offset, then format and write object
      offsetList[index - 1] = ftell(fp);
      write(index);
      write(" 0 obj\n");
      write(objectData);
      write("endobj\n");
      return index++;
    }
    // builds xref table, returns file offset of xref table
    int xref() {
      char val[32];
      // xref start index and number of entries
      int xrefStart = ftell(fp);
      write("xref\n0 ");
      write(index);
      // a xref entry *must* be 20 bytes long (PDF1.4Ref(p64))
      // so extra space added; also the first entry is special
      write("\n0000000000 65535 f \n");
      for (int i = 0; i < index - 1; i++) {
        sprintf(val, "%010d 00000 n \n", offsetList[i]);
        write(val);
      }
      return xrefStart;
    }
  };

  // Object to manage line and page rendering. Apart from startPDF, endPDF
  // everything goes in via add() and nextLine() so that line formatting
  // and pagination can be done properly.
  class PDFRender {
  private:
    bool pageStarted;
    bool firstLine;
    int pageCount;
    int pageContentStart;
    double xPos, yPos;  // position tracking for line wrapping
    FXString pageData;  // holds PDF stream contents
    FXString segment;  // character data
    char *segStyle;    // style of segment
    bool justWhiteSpace;
    int styleCurrent, stylePrev;
    double leading;
    char *buffer;
  public:
    PDFObjectTracker *oT;
    PDFStyle *style;
    int fontSize;    // properties supplied by user
    int fontSet;
    int pageWidth, pageHeight;
    PRectangle pageMargin;
    //
    PDFRender() {
      pageStarted = false;
      pageCount = 0;
      style = NULL;
      buffer = new char[250];
      segStyle = new char[100];
    }
    ~PDFRender() {
      if (style) { delete []style; }
      delete []buffer;
      delete []segStyle;
    }
    //
    double fontToPoints(int thousandths) {
      return (double)fontSize * thousandths / 1000.0;
    }
    void setStyle(char *buff, int style_) {
      int styleNext = style_;
      if (style_ == -1) { styleNext = styleCurrent; }
      *buff = '\0';
      if (styleNext != styleCurrent || style_ == -1) {
        if (style[styleCurrent].font != style[styleNext].font
                || style_ == -1) {
          sprintf(buff, "/F%d %d Tf ",
                  style[styleNext].font + 1, fontSize);
        }
        if (strcmp(style[styleCurrent].fore, style[styleNext].fore) != 0
                || style_ == -1) {
          strcat(buff, style[styleNext].fore);
          strcat(buff, "rg ");
        }
      }
    }
    //
    void startPDF() {
      if (fontSize <= 0) {
        fontSize = PDF_FONTSIZE_DEFAULT;
      }
      // leading is the term for distance between lines
      leading = fontSize * PDF_SPACING_DEFAULT;
      // sanity check for page size and margins
      int pageWidthMin = (int)leading + pageMargin.left + pageMargin.right;
      if (pageWidth < pageWidthMin) {
        pageWidth = pageWidthMin;
      }
      int pageHeightMin = (int)leading + pageMargin.top + pageMargin.bottom;
      if (pageHeight < pageHeightMin) {
        pageHeight = pageHeightMin;
      }
      // start to write PDF file here (PDF1.4Ref(p63))
      // ASCII>127 characters to indicate binary-possible stream
      oT->write("%PDF-1.3\n%\xc7\xec\x8f\xa2\n");
      styleCurrent = STYLE_DEFAULT;

      // build objects for font resources; note that font objects are
      // *expected* to start from index 1 since they are the first objects
      // to be inserted (PDF1.4Ref(p317))
      for (int i = 0; i < 4; i++) {
        sprintf(buffer, "<</Type/Font/Subtype/Type1"
                "/Name/F%d/BaseFont/%s/Encoding/"
                PDF_ENCODING
                ">>\n", i + 1,
                PDFfontNames[fontSet * 4 + i]);
        oT->add(buffer);
      }
      pageContentStart = oT->index;
    }
    void endPDF() {
      if (pageStarted) {  // flush buffers
        endPage();
      }
      // refer to all used or unused fonts for simplicity
      int resourceRef = oT->add(
                  "<</ProcSet[/PDF/Text]\n"
                  "/Font<</F1 1 0 R/F2 2 0 R/F3 3 0 R"
                  "/F4 4 0 R>> >>\n");
      // create all the page objects (PDF1.4Ref(p88))
      // forward reference pages object; calculate its object number
      int pageObjectStart = oT->index;
      int pagesRef = pageObjectStart + pageCount;
      for (int i = 0; i < pageCount; i++) {
        sprintf(buffer, "<</Type/Page/Parent %d 0 R\n"
                "/MediaBox[ 0 0 %d %d"
                "]\n/Contents %d 0 R\n"
                "/Resources %d 0 R\n>>\n",
                pagesRef, pageWidth, pageHeight,
                pageContentStart + i, resourceRef);
        oT->add(buffer);
      }
      // create page tree object (PDF1.4Ref(p86))
      pageData = "<</Type/Pages/Kids[\n";
      for (int j = 0; j < pageCount; j++) {
        sprintf(buffer, "%d 0 R\n", pageObjectStart + j);
        pageData += buffer;
      }
      sprintf(buffer, "]/Count %d\n>>\n", pageCount);
      pageData += buffer;
      oT->add(pageData.text());
      // create catalog object (PDF1.4Ref(p83))
      sprintf(buffer, "<</Type/Catalog/Pages %d 0 R >>\n", pagesRef);
      int catalogRef = oT->add(buffer);
      // append the cross reference table (PDF1.4Ref(p64))
      int xref = oT->xref();
      // end the file with the trailer (PDF1.4Ref(p67))
      sprintf(buffer, "trailer\n<< /Size %d /Root %d 0 R\n>>"
              "\nstartxref\n%d\n%%%%EOF\n",
              oT->index, catalogRef, xref);
      oT->write(buffer);
    }
    void add(char ch, int style_) {
      if (!pageStarted) {
        startPage();
      }
      // get glyph width (TODO future non-monospace handling)
      double glyphWidth = fontToPoints(PDFfontWidths[fontSet]);
      xPos += glyphWidth;
      // if cannot fit into a line, flush, wrap to next line
      if (xPos > pageWidth - pageMargin.right) {
        nextLine();
        xPos += glyphWidth;
      }
      // if different style, then change to style
      if (style_ != styleCurrent) {
        flushSegment();
        // output code (if needed) for new style
        setStyle(segStyle, style_);
        stylePrev = styleCurrent;
        styleCurrent = style_;
      }
      // escape these characters
      if (ch == ')' || ch == '(' || ch == '\\') {
        segment += '\\';
      }
      if (ch != ' ') { justWhiteSpace = false; }
      segment += ch;  // add to segment data
    }
    void flushSegment() {
      if (segment.length() > 0) {
        if (justWhiteSpace) {  // optimise
          styleCurrent = stylePrev;
        } else {
          pageData += segStyle;
        }
        pageData += "(";
        pageData += segment;
        pageData += ")Tj\n";
      }
      segment.clear();
      *segStyle = '\0';
      justWhiteSpace = true;
    }
    void startPage() {
      pageStarted = true;
      firstLine = true;
      pageCount++;
      double fontAscender = fontToPoints(PDFfontAscenders[fontSet]);
      yPos = pageHeight - pageMargin.top - fontAscender;
      // start a new page
      sprintf(buffer, "BT 1 0 0 1 %d %d Tm\n",
              pageMargin.left, (int)yPos);
      // force setting of initial font, colour
      setStyle(segStyle, -1);
      strcat(buffer, segStyle);
      pageData = buffer;
      xPos = pageMargin.left;
      segment.clear();
      flushSegment();
    }
    void endPage() {
      pageStarted = false;
      flushSegment();
      // build actual text object; +3 is for "ET\n"
      // PDF1.4Ref(p38) EOL marker preceding endstream not counted
      char *textObj = new char[pageData.length() + 100];
      // concatenate stream within the text object
      sprintf(textObj, "<</Length %d>>\nstream\n%s"
              "ET\nendstream\n",
              static_cast<int>(pageData.length() - 1 + 3),
              pageData.text());
      oT->add(textObj);
      delete []textObj;
    }
    void nextLine() {
      if (!pageStarted) {
        startPage();
      }
      xPos = pageMargin.left;
      flushSegment();
      // PDF follows cartesian coords, subtract -> down
      yPos -= leading;
      double fontDescender = fontToPoints(PDFfontDescenders[fontSet]);
      if (yPos < pageMargin.bottom + fontDescender) {
        endPage();
        startPage();
        return;
      }
      if (firstLine) {
        // avoid breakage due to locale setting
        int f = (int)(leading * 10 + 0.5);
        sprintf(buffer, "0 -%d.%d TD\n", f / 10, f % 10);
        firstLine = false;
      } else {
        sprintf(buffer, "T*\n");
      }
      pageData += buffer;
    }
  };
  PDFRender pr;

  sci->sendMessage(SCI_COLOURISE, 0, -1);
  int tabSize = PDF_TAB_DEFAULT;

  pr.fontSize = 0;

  FXString propItem = "Courier";
  pr.fontSet = PDF_FONT_DEFAULT;
  if (propItem.length()) {
    if (propItem == "Courier")
      pr.fontSet = 0;
    else if (propItem == "Helvetica")
      pr.fontSet = 1;
    else if (propItem == "Times")
      pr.fontSet = 2;
  }

  pr.pageWidth = PDF_WIDTH_DEFAULT;
  pr.pageHeight = PDF_HEIGHT_DEFAULT;
  pr.pageMargin.left = PDF_MARGIN_DEFAULT;
  pr.pageMargin.right = PDF_MARGIN_DEFAULT;
  pr.pageMargin.top = PDF_MARGIN_DEFAULT;
  pr.pageMargin.bottom = PDF_MARGIN_DEFAULT;

  // collect all styles available for that 'language'
  // or the default style if no language is available...
  pr.style = new PDFStyle[STYLE_MAX + 1];

  LangStyle* ls = sci->getLanguage();
  if (ls) {
    StyleDef* sd;
    int i;
    for (i=0; (i<=STYLE_MAX); i++) {  // get keys
     sd=GetStyleFromId(ls->styles, i);
     if (!sd) { sd=Settings::globalStyle(); }

      pr.style[i].font = 0;
      if (sd->style & Italic) { pr.style[i].font |= 2; }
      if (sd->style & Bold) { pr.style[i].font |= 1; }
      pr.style[i].fore[0] = '\0';
      if (sd->fg[0]) {
        getPDFRGB(pr.style[i].fore, sd->fg);
      } else if (i == STYLE_DEFAULT) {
        strcpy(pr.style[i].fore, "0 0 0 ");
      }
      pr.fontSize = PDF_FONTSIZE_DEFAULT;
    }
    // patch in default foregrounds
    for (int j = 0; j <= STYLE_MAX; j++) {
      if (pr.style[j].fore[0] == '\0') {
        strcpy(pr.style[j].fore, pr.style[STYLE_DEFAULT].fore);
      }
    }
  } else {
    for (int i = 0; i <= STYLE_MAX; i++) {
      strcpy(pr.style[i].fore, "0 0 0 ");
    }
  }


  // initialise PDF rendering
  PDFObjectTracker ot(fp);
  pr.oT = &ot;
  pr.startPDF();

  // do here all the writing
  int lengthDoc = sci->GetTextLength();
  int lineIndex = 0;

  if (!lengthDoc) {  // enable zero length docs
    pr.nextLine();
  } else {
    FX88591Codec *codec = sci->GetUTF8() ? new FX88591Codec() : NULL;
    for (int i = 0; i < lengthDoc; i++) {
      char ch = sci->sendMessage(SCI_GETCHARAT,i,0);
      int style = sci->sendMessage(SCI_GETSTYLEAT,i,0);

      if (ch == '\t') {
        // expand tabs
        int ts = tabSize - (lineIndex % tabSize);
        lineIndex += ts;
        for (; ts; ts--) {  // add ts count of spaces
          pr.add(' ', style);  // add spaces
        }
      } else if (ch == '\r' || ch == '\n') {
        if (ch == '\r' && sci->sendMessage(SCI_GETCHARAT,i+1,0) == '\n') {
          i++;
        }
        // close and begin a newline...
        pr.nextLine();
        lineIndex = 0;
      } else {
        // write the character normally...
        if (codec) {
          long charlen=sci->sendMessage(SCI_POSITIONAFTER,i,0)-i;
          if ((charlen>1)&&(charlen<=8)) {
            // PDF doesn't like UTF-8, try conversion to single-byte ISO-8859
            char utf[9]="\0";
            char asc[2]="\0";
            Sci_TextRange tr;
            tr.chrg.cpMin=i;
            tr.chrg.cpMax=i+charlen;
            tr.lpstrText=utf;
            sci->sendMessage(SCI_GETTEXTRANGE, 0, reinterpret_cast<long>(&tr));
            codec->utf2mb(asc, sizeof(asc), tr.lpstrText, charlen);
            if (asc[0]&&!asc[1]) {
              // Conversion succeeded: use our single byte and move past the multi-bytes
              ch=asc[0];
              i+=charlen-1;
            }
          }
        }
        pr.add(ch, style);
        lineIndex++;
      }
    }
    if (codec) { delete codec; }
  }

  // write required stuff and close the PDF file
  pr.endPDF();

}

