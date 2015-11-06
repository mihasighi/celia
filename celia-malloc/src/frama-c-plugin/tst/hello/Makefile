#
#
# Makefile for the hello plugin
#

FRAMAC_SHARE :=$(shell frama-c.byte -print-path)
FRAMAC_LIBDIR :=$(shell frama-c.byte -print-libpath)
PLUGIN_NAME = Hello
PLUGIN_CMO = hello_options hello_print hello_world
include $(FRAMAC_SHARE)/Makefile.dynamic

