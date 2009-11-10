/* lexname=vhdl lexpfx=SCE_VHDL_ lextag=SCLEX_VHDL */

static StyleDef vhdl_style[] = {
  { "default", SCE_VHDL_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "comment", SCE_VHDL_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentlinebang", SCE_VHDL_COMMENTLINEBANG, COMMNT_FG, _DEFLT_BG, Italic },
  { "number", SCE_VHDL_NUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "string", SCE_VHDL_STRING, STRING_FG, _DEFLT_BG, Normal },
  { "operator", SCE_VHDL_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "identifier", SCE_VHDL_IDENTIFIER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "stringeol", SCE_VHDL_STRINGEOL, STREOL_FG, _DEFLT_BG, Normal },
  { "keyword", SCE_VHDL_KEYWORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "stdoperator", SCE_VHDL_STDOPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "attribute", SCE_VHDL_ATTRIBUTE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "stdfunction", SCE_VHDL_STDFUNCTION, _DEFLT_FG, _DEFLT_BG, Normal },
  { "stdpackage", SCE_VHDL_STDPACKAGE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "stdtype", SCE_VHDL_STDTYPE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "userword", SCE_VHDL_USERWORD, _WORD1_FG, _DEFLT_BG, Bold },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* vhdl_words[]= {
  "access after alias all architecture array assert attribute begin block body buffer bus case component configuration constant disconnect downto else elsif end entity exit file for function generate generic group guarded if impure in inertial inout is label library linkage literal loop map new next null of on open others out package port postponed procedure process pure range record register reject report return select severity shared signal subtype then to transport type unaffected units until use variable wait when while with",
  "abs and mod nand nor not or rem rol ror sla sll sra srl xnor xor",
  "left right low high ascending image value pos val succ pred leftof rightof base range reverse_range length delayed stable quiet transaction event active last_event last_active last_value driving driving_value simple_name path_name instance_name",
  "now readline read writeline write endfile resolved to_bit to_bitvector to_stdulogic to_stdlogicvector to_stdulogicvector to_x01 to_x01z to_UX01 rising_edge falling_edge is_x shift_left shift_right rotate_left rotate_right resize to_integer to_unsigned to_signed std_match to_01",
  "std ieee work standard textio std_logic_1164 std_logic_arith std_logic_misc std_logic_signed std_logic_textio std_logic_unsigned numeric_bit numeric_std math_complex math_real vital_primitives vital_timing",
  "boolean bit character severity_level integer real time delay_length natural positive string bit_vector file_open_kind file_open_status line text side width std_ulogic std_ulogic_vector std_logic std_logic_vector X01 X01Z UX01 UX01Z unsigned signed",
  "",
  NULL
};


static const char* vhdl_mask = "*.vhd|*.vhdl";


static const char* vhdl_apps = "";


static LangStyle LangVHDL = {
  "vhdl",
  SCLEX_VHDL,
  vhdl_style,
  (char**)vhdl_words,
  (char*)vhdl_mask,
  (char*)vhdl_apps,
  0,
  TABS_DEFAULT
};


