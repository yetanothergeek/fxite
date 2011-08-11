/* lexname=erlang lexpfx=SCE_ERLANG_ lextag=SCLEX_ERLANG */

static StyleDef erlang_style[] = {
  { "default", SCE_ERLANG_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "comment", SCE_ERLANG_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "variable", SCE_ERLANG_VARIABLE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "number", SCE_ERLANG_NUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "keyword", SCE_ERLANG_KEYWORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "string", SCE_ERLANG_STRING, STRING_FG, _DEFLT_BG, Normal },
  { "operator", SCE_ERLANG_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "atom", SCE_ERLANG_ATOM, _DEFLT_FG, _DEFLT_BG, Normal },
  { "functionname", SCE_ERLANG_FUNCTION_NAME, _DEFLT_FG, _DEFLT_BG, Normal },
  { "character", SCE_ERLANG_CHARACTER, STRING_FG, _DEFLT_BG, Normal },
  { "macro", SCE_ERLANG_MACRO, _DEFLT_FG, _DEFLT_BG, Normal },
  { "record", SCE_ERLANG_RECORD, _DEFLT_FG, _DEFLT_BG, Normal },
  { "preproc", SCE_ERLANG_PREPROC, _DEFLT_FG, _DEFLT_BG, Normal },
  { "nodename", SCE_ERLANG_NODE_NAME, _DEFLT_FG, _DEFLT_BG, Normal },
  { "commentfunction", SCE_ERLANG_COMMENT_FUNCTION, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentmodule", SCE_ERLANG_COMMENT_MODULE, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentdoc", SCE_ERLANG_COMMENT_DOC, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentdocmacro", SCE_ERLANG_COMMENT_DOC_MACRO, COMMNT_FG, _DEFLT_BG, Italic },
  { "atomquoted", SCE_ERLANG_ATOM_QUOTED, _DEFLT_FG, _DEFLT_BG, Normal },
  { "macroquoted", SCE_ERLANG_MACRO_QUOTED, _DEFLT_FG, _DEFLT_BG, Normal },
  { "recordquoted", SCE_ERLANG_RECORD_QUOTED, _DEFLT_FG, _DEFLT_BG, Normal },
  { "nodenamequoted", SCE_ERLANG_NODE_NAME_QUOTED, _DEFLT_FG, _DEFLT_BG, Normal },
  { "bifs", SCE_ERLANG_BIFS, _DEFLT_FG, _DEFLT_BG, Normal },
  { "modules", SCE_ERLANG_MODULES, _DEFLT_FG, _DEFLT_BG, Normal },
  { "modulesatt", SCE_ERLANG_MODULES_ATT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "unknown", SCE_ERLANG_UNKNOWN, STREOL_FG, _DEFLT_BG, Normal },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* erlang_words[]= {
  "after and andalso band begin bnot bor bsl bsr bxor case catch cond div end fun if let not of or orelse query receive rem try when xor",
  "erlang: abs adler32 adler32_combine erlang:append_element apply atom_to_binary atom_to_list binary_to_atom binary_to_existing_atom binary_to_list bitstring_to_list binary_to_term bit_size erlang:bump_reductions byte_size erlang:cancel_timer check_process_code concat_binary crc32 crc32_combine date decode_packet delete_module erlang:demonitor disconnect_node erlang:display element erase erlang:error exit float float_to_list erlang:fun_info erlang:fun_to_list erlang:function_exported garbage_collect get erlang:get_cookie get_keys erlang:get_stacktrace group_leader halt erlang:hash hd erlang:hibernate integer_to_list erlang:integer_to_list iolist_to_binary iolist_size is_alive is_atom is_binary is_bitstring is_boolean erlang:is_builtin is_float is_function is_integer is_list is_number is_pid is_port is_process_alive is_record is_reference is_tuple length link list_to_atom list_to_binary list_to_bitstring list_to_existing_atom list_to_float list_to_integer erlang:list_to_integer list_to_pid list_to_tuple load_module erlang:load_nif erlang:loaded erlang:localtime erlang:localtime_to_universaltime make_ref erlang:make_tuple erlang:max erlang:md5 erlang:md5_final erlang:md5_init erlang:md5_update erlang:memory erlang:min module_loaded erlang:monitor monitor_node node nodes now open_port erlang:phash erlang:phash2 pid_to_list port_close port_command erlang:port_command port_connect port_control erlang:port_call erlang:port_info erlang:port_to_list erlang:ports pre_loaded erlang:process_display process_flag process_info processes purge_module put erlang:raise erlang:read_timer erlang:ref_to_list register registered erlang:resume_process round self erlang:send erlang:send_after erlang:send_nosuspend erlang:set_cookie setelement size spawn spawn_link spawn_monitor spawn_opt split_binary erlang:start_timer statistics erlang:suspend_process erlang:system_flag erlang:system_info erlang:system_monitor erlang:system_profile term_to_binary throw time tl erlang:trace erlang:trace_delivered erlang:trace_info erlang:trace_pattern trunc tuple_size tuple_to_list erlang:universaltime erlang:universaltime_to_localtime unlink unregister whereis erlang:yield",
  "-define -else -endif -ifdef -ifndef -include -include_lib -undef",
  "-behavior -behaviour -compile -created -created_by -export -file -import -module -modified -modified_by -record -revision -spec -type -vsn",
  "@author @clear @copyright @deprecated @doc @docfile @end @equiv @headerfile @hidden @private @reference @see @since @spec @throws @title @todo @TODO @type @version",
  "@date @docRoot @link @module @package @section @time @type @version",
  NULL
};


static const char* erlang_mask = "*.erl|*.hrl";


static const char* erlang_apps = "";


static LangStyle LangErlang = {
  "erlang",
  SCLEX_ERLANG,
  erlang_style,
  (char**)erlang_words,
  (char*)erlang_mask,
  (char*)erlang_apps,
  0,
  TABS_DEFAULT,
  0
};


