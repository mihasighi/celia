#ma-c $< -value -then -load-module ./Ast2dot -ast2dot -ast2dot-output $<.dot
#
# Makefile for the AST2DOT plugin
#

FRAMAC_SHARE :=$(shell frama-c.byte -print-path)
FRAMAC_LIBDIR :=$(shell frama-c.byte -print-libpath)
PLUGIN_NAME = Ast2dot
PLUGIN_CMO = ast2dot_options ast2dot_core ast2dot_register
PLUGIN_GUI_CMO = ast2dot_gui
include $(FRAMAC_SHARE)/Makefile.dynamic


%.dot: %.c
	frama-c -val $< 
	frama-c -load-module ./Ast2dot -ast2dot -ast2dot-output $*.dot $< 
	dot -O -Tpng $*.dot
