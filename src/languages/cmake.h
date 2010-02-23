/* lexname=cmake lexpfx=SCE_CMAKE_ lextag=SCLEX_CMAKE */

static StyleDef cmake_style[] = {
  { "default", SCE_CMAKE_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "comment", SCE_CMAKE_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "stringdq", SCE_CMAKE_STRINGDQ, STRING_FG, _DEFLT_BG, Normal },
  { "stringlq", SCE_CMAKE_STRINGLQ, STRING_FG, _DEFLT_BG, Normal },
  { "stringrq", SCE_CMAKE_STRINGRQ, STRING_FG, _DEFLT_BG, Normal },
  { "commands", SCE_CMAKE_COMMANDS, _DEFLT_FG, _DEFLT_BG, Normal },
  { "parameters", SCE_CMAKE_PARAMETERS, _DEFLT_FG, _DEFLT_BG, Normal },
  { "variable", SCE_CMAKE_VARIABLE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "userdefined", SCE_CMAKE_USERDEFINED, _DEFLT_FG, _DEFLT_BG, Normal },
  { "whiledef", SCE_CMAKE_WHILEDEF, _DEFLT_FG, _DEFLT_BG, Normal },
  { "foreachdef", SCE_CMAKE_FOREACHDEF, _DEFLT_FG, _DEFLT_BG, Normal },
  { "ifdefinedef", SCE_CMAKE_IFDEFINEDEF, _DEFLT_FG, _DEFLT_BG, Normal },
  { "macrodef", SCE_CMAKE_MACRODEF, _DEFLT_FG, _DEFLT_BG, Normal },
  { "stringvar", SCE_CMAKE_STRINGVAR, STRING_FG, _DEFLT_BG, Normal },
  { "number", SCE_CMAKE_NUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* cmake_words[]= {
  "add_custom_command add_custom_target add_definitions add_dependencies add_executable add_library add_subdirectory add_test aux_source_directory build_command build_name cmake_minimum_required configure_file create_test_sourcelist else elseif enable_language enable_testing endforeach endif endmacro endwhile exec_program execute_process export_library_dependencies file find_file find_library find_package find_path find_program fltk_wrap_ui foreach get_cmake_property get_directory_property get_filename_component get_source_file_property get_target_property get_test_property if include include_directories include_external_msproject include_regular_expression install install_files install_programs install_targets link_directories link_libraries list load_cache load_command macro make_directory mark_as_advanced math message option output_required_files project qt_wrap_cpp qt_wrap_ui remove remove_definitions separate_arguments set set_directory_properties set_source_files_properties set_target_properties set_tests_properties site_name source_group string subdir_depends subdirs target_link_libraries try_compile try_run use_mangled_mesa utility_source variable_requires vtk_make_instantiator vtk_wrap_java vtk_wrap_python vtk_wrap_tcl while write_file",
  "ABSOLUTE ABSTRACT ADDITIONAL_MAKE_CLEAN_FILES ALL AND APPEND ARGS ASCII BEFORE CACHE CACHE_VARIABLES CLEAR COMMAND COMMANDS COMMAND_NAME COMMENT COMPARE COMPILE_FLAGS COPYONLY DEFINED DEFINE_SYMBOL DEPENDS DOC EQUAL ESCAPE_QUOTES EXCLUDE EXCLUDE_FROM_ALL EXISTS EXPORT_MACRO EXT EXTRA_INCLUDE FATAL_ERROR FILE FILES FORCE FUNCTION GENERATED GLOB GLOB_RECURSE GREATER GROUP_SIZE HEADER_FILE_ONLY HEADER_LOCATION IMMEDIATE INCLUDES INCLUDE_DIRECTORIES INCLUDE_INTERNALS INCLUDE_REGULAR_EXPRESSION LESS LINK_DIRECTORIES LINK_FLAGS LOCATION MACOSX_BUNDLE MACROS MAIN_DEPENDENCY MAKE_DIRECTORY MATCH MATCHALL MATCHES MODULE NAME NAME_WE NOT NOTEQUAL NO_SYSTEM_PATH OBJECT_DEPENDS OPTIONAL OR OUTPUT OUTPUT_VARIABLE PATH PATHS POST_BUILD POST_INSTALL_SCRIPT PREFIX PREORDER PRE_BUILD PRE_INSTALL_SCRIPT PRE_LINK PROGRAM PROGRAM_ARGS PROPERTIES QUIET RANGE READ REGEX REGULAR_EXPRESSION REPLACE REQUIRED RETURN_VALUE RUNTIME_DIRECTORY SEND_ERROR SHARED SOURCES STATIC STATUS STREQUAL STRGREATER STRLESS SUFFIX TARGET TOLOWER TOUPPER VAR VARIABLES VERSION WIN32 WRAP_EXCLUDE WRITE APPLE MINGW MSYS CYGWIN BORLAND WATCOM MSVC MSVC_IDE MSVC60 MSVC70 MSVC71 MSVC80 CMAKE_COMPILER_2005 OFF ON ",
  "",
  NULL
};


static const char* cmake_mask = "CMakeLists.txt|*.cmake|*.cmake.in|*.ctest|*.ctest.in";


static const char* cmake_apps = "";


static LangStyle LangCMAKE = {
  "cmake",
  SCLEX_CMAKE,
  cmake_style,
  (char**)cmake_words,
  (char*)cmake_mask,
  (char*)cmake_apps,
  0,
  TABS_DEFAULT
};


