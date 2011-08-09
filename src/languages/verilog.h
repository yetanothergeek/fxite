/* lexname=verilog lexpfx=SCE_V_ lextag=SCLEX_VERILOG */

static StyleDef verilog_style[] = {
  { "default", SCE_V_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "comment", SCE_V_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentline", SCE_V_COMMENTLINE, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentlinebang", SCE_V_COMMENTLINEBANG, COMMNT_FG, _DEFLT_BG, Italic },
  { "number", SCE_V_NUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "word", SCE_V_WORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "string", SCE_V_STRING, STRING_FG, _DEFLT_BG, Normal },
  { "word2", SCE_V_WORD2, _WORD2_FG, _DEFLT_BG, Bold },
  { "word3", SCE_V_WORD3, _WORD1_FG, _DEFLT_BG, Bold },
  { "preprocessor", SCE_V_PREPROCESSOR, PREPRC_FG, _DEFLT_BG, Normal },
  { "operator", SCE_V_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "identifier", SCE_V_IDENTIFIER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "stringeol", SCE_V_STRINGEOL, STREOL_FG, _DEFLT_BG, Normal },
  { "user", SCE_V_USER, _DEFLT_FG, _DEFLT_BG, Normal },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* verilog_words[]= {
  "always and assign automatic begin buf bufif0 bufif1 case casex casez cell cmos config deassign default defparam design disable edge else end endcase endconfig endfunction endgenerate endmodule endprimitive endspecify endtable endtask event for force forever fork function generate genvar highz0 highz1 if ifnone incdir include initial inout input instance integer join large liblist library localparam macromodule medium module nand negedge nmos nor noshowcancelled not notif0 notif1 or output parameter pmos posedge primitive pull0 pull1 pulldown pullup pulsestyle_ondetect pulsestyle_onevent rcmos real realtime reg release repeat rnmos rpmos rtran rtranif0 rtranif1 scalared showcancelled signed small specify specparam strong0 strong1 supply0 supply1 table task time tran tranif0 tranif1 tri tri0 tri1 triand trior trireg unsigned use uwire vectored wait wand weak0 weak1 while wire wor xnor xor",
  "",
  "$async$and$array $async$and$plane $async$nand$array $async$nand$plane $async$nor$array $async$nor$plane $async$or$array $async$or$plane $bitstoreal $countdrivers $display $displayb $displayh $displayo $dist_chi_square $dist_erlang $dist_exponential $dist_normal $dist_poisson $dist_t $dist_uniform $dumpall $dumpfile $dumpflush $dumplimit $dumpoff $dumpon $dumpportsall $dumpportsflush $dumpportslimit $dumpportsoff $dumpportson $dumpvars $fclose $fdisplayh $fdisplay $fdisplayf $fdisplayb $feof $ferror $fflush $fgetc $fgets $finish $fmonitorb $fmonitor $fmonitorf $fmonitorh $fopen $fread $fscanf $fseek $fsscanf $fstrobe $fstrobebb $fstrobef $fstrobeh $ftel $fullskew $fwriteb $fwritef $fwriteh $fwrite $getpattern $history $hold $incsave $input $itor $key $list $log $monitorb $monitorh $monitoroff $monitoron $monitor $monitoro $nochange $nokey $nolog $period $printtimescale $q_add $q_exam $q_full $q_initialize $q_remove $random $readmemb $readmemh $readmemh $realtime $realtobits $recovery $recrem $removal $reset_count $reset $reset_value $restart $rewind $rtoi $save $scale $scope $sdf_annotate $setup $setuphold $sformat $showscopes $showvariables $showvars $signed $skew $sreadmemb $sreadmemh $stime $stop $strobeb $strobe $strobeh $strobeo $swriteb $swriteh $swriteo $swrite $sync$and$array $sync$and$plane $sync$nand$array $sync$nand$plane $sync$nor$array $sync$nor$plane $sync$or$array $sync$or$plane $test$plusargs $time $timeformat $timeskew $ungetc $unsigned $value$plusargs $width $writeb $writeh $write $writeo",
  "",
  "",
  NULL
};


static const char* verilog_mask = "*.v|*.vh";


static const char* verilog_apps = "";


static LangStyle LangVerilog = {
  "verilog",
  SCLEX_VERILOG,
  verilog_style,
  (char**)verilog_words,
  (char*)verilog_mask,
  (char*)verilog_apps,
  0,
  TABS_DEFAULT
};


