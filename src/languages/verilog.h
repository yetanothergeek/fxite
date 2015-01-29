/* lexname=verilog lexpfx=SCE_V_ lextag=SCLEX_VERILOG */

static StyleDef verilog_style[] = {
  { "comment", SCE_V_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentline", SCE_V_COMMENTLINE, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentlinebang", SCE_V_COMMENTLINEBANG, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentword", SCE_V_COMMENT_WORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "default", SCE_V_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "identifier", SCE_V_IDENTIFIER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "inout", SCE_V_INOUT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "input", SCE_V_INPUT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "number", SCE_V_NUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "operator", SCE_V_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "output", SCE_V_OUTPUT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "portconnect", SCE_V_PORT_CONNECT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "preprocessor", SCE_V_PREPROCESSOR, PREPRC_FG, _DEFLT_BG, Normal },
  { "string", SCE_V_STRING, STRING_FG, _DEFLT_BG, Normal },
  { "stringeol", SCE_V_STRINGEOL, STREOL_FG, _DEFLT_BG, Normal },
  { "user", SCE_V_USER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "word", SCE_V_WORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "word2", SCE_V_WORD2, _WORD2_FG, _DEFLT_BG, Bold },
  { "word3", SCE_V_WORD3, _WORD1_FG, _DEFLT_BG, Bold },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* verilog_words[]= {
  "always and assign automatic begin buf bufif0 bufif1 case casex casez cell cmos config deassign default defparam design disable edge else end endcase endconfig endfunction endgenerate endmodule endprimitive endspecify endtable endtask event for force forever fork function generate genvar highz0 highz1 if ifnone incdir include initial inout input instance integer join large liblist library localparam macromodule medium module nand negedge nmos nor noshowcancelled not notif0 notif1 or output parameter pmos posedge primitive pull0 pull1 pulldown pullup pulsestyle_ondetect pulsestyle_onevent rcmos real realtime reg release repeat rnmos rpmos rtran rtranif0 rtranif1 scalared showcancelled signed small specify specparam strong0 strong1 supply0 supply1 table task time tran tranif0 tranif1 tri tri0 tri1 triand trior trireg unsigned use uwire vectored wait wand weak0 weak1 while wire wor xnor xor",
  "$async$and$array $async$and$plane $async$nand$array $async$nand$plane $async$nor$array $async$nor$plane $async$or$array $async$or$plane $bitstoreal $countdrivers $display $displayb $displayh $displayo $dist_chi_square $dist_erlang $dist_exponential $dist_normal $dist_poisson $dist_t $dist_uniform $dumpall $dumpfile $dumpflush $dumplimit $dumpoff $dumpon $dumpportsall $dumpportsflush $dumpportslimit $dumpportsoff $dumpportson $dumpvars $fclose $fdisplayh $fdisplay $fdisplayf $fdisplayb $feof $ferror $fflush $fgetc $fgets $finish $fmonitorb $fmonitor $fmonitorf $fmonitorh $fopen $fread $fscanf $fseek $fsscanf $fstrobe $fstrobebb $fstrobef $fstrobeh $ftel $fullskew $fwriteb $fwritef $fwriteh $fwrite $getpattern $history $hold $incsave $input $itor $key $list $log $monitorb $monitorh $monitoroff $monitoron $monitor $monitoro $nochange $nokey $nolog $period $printtimescale $q_add $q_exam $q_full $q_initialize $q_remove $random $readmemb $readmemh $readmemh $realtime $realtobits $recovery $recrem $removal $reset_count $reset $reset_value $restart $rewind $rtoi $save $scale $scope $sdf_annotate $setup $setuphold $sformat $showscopes $showvariables $showvars $signed $skew $sreadmemb $sreadmemh $stime $stop $strobeb $strobe $strobeh $strobeo $swriteb $swriteh $swriteo $swrite $sync$and$array $sync$and$plane $sync$nand$array $sync$nand$plane $sync$nor$array $sync$nor$plane $sync$or$array $sync$or$plane $test$plusargs $time $timeformat $timeskew $ungetc $unsigned $value$plusargs $width $writeb $writeh $write $writeo",
  "$acos $acosh $asin $asinh $assertfailoff $assertfailon $assertkill $assertnonvacuouson $assertoff $asserton $assertpassoff $assertpasson $assertvacuousoff $async$and$array $async$and$plane $async$nand$array $async$nand$plane $async$nor$array $async$nor$plane $async$or$array $async$or$plane $atan $atan2 $atanh $bits $bitstoreal $bitstoshortreal $cast $ceil $changed $changed_gclk $changing_gclk $clog2 $cos $cosh $countdrivers $countones $coverage_control $coverage_get $coverage_get_max $coverage_merge $coverage_save $dimensions $display $displayb $displayh $displayo $dist_chi_square $dist_erlang $dist_exponential $dist_normal $dist_poisson $dist_t $dist_uniform $dumpall $dumpfile $dumpflush $dumplimit $dumpoff $dumpon $dumpports $dumpportsall $dumpportsflush $dumpportslimit $dumpportsoff $dumpportson $dumpvars $error $exit $exp $falling_gclk $fatal $fclose $fdisplay $fdisplayb $fdisplayf $fdisplayh $fdisplayo $fell $fell_gclk $feof $ferror $fflush $fgetc $fgets $finish $floor $fmonitor $fmonitorb $fmonitorf $fmonitorh $fmonitoro $fopen $fread $fscanf $fseek $fsscanf $fstrobe $fstrobeb $fstrobebb $fstrobef $fstrobeh $fstrobeo $ftel $ftell $fullskew $future_gclk $fwrite $fwriteb $fwritef $fwriteh $fwriteo $get_coverage $getpattern $high $history $hold $hypot $increment $incsave $info $input $isunbounded $isunknown $itor $key $left $list $ln $load_coverage_db $log $log10 $low $monitor $monitorb $monitorh $monitoro $monitoroff $monitoron $nochange $nokey $nolog $onehot $onehot0 $past $past_gclk $period $pow $printtimescale $q_add $q_exam $q_full $q_initialize $q_remove $random $readmemb $readmemh $realtime $realtobits $recovery $recrem $removal $reset $reset_count $reset_value $restart $rewind $right $rising_gclk $root $rose $rose_gclk $rtoi $sampled $save $scale $scope $sdf_annotate $set_coverage_db_name $setup $setuphold $sformat $sformatf $shortrealtobits $showscopes $showvariables $showvars $signed $sin $sinh $size $skew $sqrt $sreadmemb $sreadmemh $sscanf $stable $stable_gclk $steady_gclk $stime $stop $strobe $strobeb $strobeh $strobeo $swrite $swriteb $swriteh $swriteo $sync$and$array $sync$and$plane $sync$nand$array $sync$nand$plane $sync$nor$array $sync$nor$plane $sync$or$array $sync$or$plane $system $tan $tanh $test$plusargs $time $timeformat $timeskew $typename $ungetc $unit $unpacked_dimensions $unsigned $urandom $urandom_range $value$plusargs $warning $width $write $writeb $writeh $writememb $writememh $writeo",
  "synopsys parallel_case infer_mux TODO",
  "synopsys parallel_case infer_mux TODO",
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
  TABS_DEFAULT,
  0
};


