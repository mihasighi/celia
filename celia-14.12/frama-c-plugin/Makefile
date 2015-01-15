##########################################################################
#                                                                        #
#  This file is part of Frama-C.                                         #
#                                                                        #
#  Copyright (C) 2009-2010 LIAFA                                         #
#                                                                        #
#  you can redistribute it and/or modify it under the terms of the GNU   #
#  Lesser General Public License as published by the Free Software       #
#  Foundation, version 2.1.                                              #
#                                                                        #
#  It is distributed in the hope that it will be useful,                 #
#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
#  GNU Lesser General Public License for more details.                   #
#                                                                        #
#  See the GNU Lesser General Public License version 2.1                 #
#  for more details (enclosed in the file licenses/LGPLv2.1).            #
#                                                                        #
##########################################################################

# Makefile for compiling Celia independently of Frama-C.
# Adapted from Jessie (C) INRIA Makefile.
#
# To be used independently of Frama-C, a version of Frama-C compatible with
# Celia has to be properly installed as long as the Celia-specific stuff.

# Do not use ?= to initialize both below variables
# (fixed efficiency issue, see GNU Make manual, Section 8.11)
ifndef FRAMAC_SHARE
FRAMAC_SHARE  :=$(shell frama-c -journal-disable -print-path)
endif
ifndef FRAMAC_LIBDIR
FRAMAC_LIBDIR :=$(shell frama-c -journal-disable -print-libpath)
endif

include ../Makefile.config

CDLIB=D

FXP_BYTE=$(CAMLLIB_PREFIX)/lib/camllib.cmo $(FIXPOINT_PREFIX)/lib/fixpoint.cmo 
APRON_BYTE=$(MLGMPIDL_PREFIX)/gmp.cmo $(APRON_ML)/apron.cmo $(APRON_ML)/box$(CDLIB).cmo $(CINV_DISTRIB)/shapes/shape.cmo

PLUGIN_DIR      ?= .
CINV_DISTRIB	?= $(PLUGIN_DIR)/..
CELIA_INCLUDES	?= -g -I /usr/lib -I $(MLGMPIDL_PREFIX) -I /usr/lib/ocaml/ocamlgraph -I $(APRON_C) -I $(APRON_ML) -I $(CAMLLIB_PREFIX)/lib -I $(FIXPOINT_PREFIX)/lib -I $(CINV_DISTRIB)/src -I $(SMTLIB2_PREFIX) -I $(CINV_DISTRIB)/shapes
#CLCMO           ?= $(APRON_BYTE) $(IP_BYTE) $(FP_BYTE) $(CINV_DISTRIB)/src/clim.cmo 
CLCMO           ?= gmp.cma apron.cma camllib.cma fixpoint.cma shape.cma clim.cma
#CLCMX           ?= $(APRON_BYTE:.cmo=.cmxa) $(IP_BYTE:.cmo=.cmx) $(FP_BYTE:.cmo=.cmxa) $(CINV_DISTRIB)/src/clim.cmx
CLCMX           ?= apron.cmxa $(CINV_DISTRIB)/src/clim.cmxa

LDFLAGS  = \
-L/usr/lib/ocaml \
-L/usr/lib/ocaml/ocamlgraph \
-L$(MLGMPIDL_PREFIX) \
-L$(APRON_ML) \
-L$(APRON_C) \
-L$(CAMLLIB_PREFIX)/lib \
-L$(FIXPOINT_PREFIX)/lib \
-L$(SMTLIB2_PREFIX) \
-L$(CINV_DISTRIB)/shapes

LDLIBS   = \
-lshape_caml \
-lshape$(DEBUG) \
-lsmtlib2parser \
-lfixpoint \
-lapron_caml -lapron$(DEBUG) \
-lcamllib \
-lgmp_caml -lmpfr -lgmp \
-lunix -lbigarray -lcamlidl -lgraph

LDEFLAGS=-ccopt "$(LDFLAGS)" -cclib "$(LDLIBS)"

PLUGIN_NAME:=Celia
PLUGIN_CMO:= celia_config celia_options common \
             rewrite \
             clim_check spl_rewrite cil2apron cil2shad \
	     interp_eqn register
PLUGIN_HAS_MLI:=yes
PLUGIN_BFLAGS:=$(CELIA_INCLUDES)
PLUGIN_OFLAGS:=$(CELIA_INCLUDES)
PLUGIN_EXTRA_BYTE+=$(CLCMO)
PLUGIN_EXTRA_OPT+=$(CLCMX)
PLUGIN_DOCFLAGS:=$(CELIA_INCLUDES)
PLUGIN_TESTS_DIRS:=celia
PLUGIN_NO_DEFAULT_TEST:=yes

ifeq ($(FRAMAC_MAKE),yes)
unexport $(FRAMAC_MAKE)

all::  $(CINV_DISTRIB)/Makefile $(CINV_DISTRIB)/.depend celia_all

.PHONY: celia_all $(CINV_DISTRIB)/depend $(CINV_DISTRIB)/clean 
celia_all: $(CINV_DISTRIB)/Makefile $(CINV_DISTRIB)/.depend
	$(MAKE) -C $(dir $<) -j 1 all-without-frama-c-plugin

$(CINV_DISTRIB)/depend: $(CINV_DISTRIB)/Makefile
	$(MAKE) -C $(dir $@) -j 1 depend FRAMAC=no

$(CINV_DISTRIB)/clean:
	$(MAKE) -C $(dir $@) -j 1 clean

$(PLUGIN_DIR)/.depend: $(CINV_DISTRIB)/.depend

depend:: $(CINV_DISTRIB)/depend

clean::$(CINV_DISTRIB)/clean

endif

$(PLUGIN_DIR)/interp_eqn.cmo: $(PLUGIN_DIR)/interp_eqn.cmi $(CINV_DISTRIB)/src/clim.cmo

$(PLUGIN_DIR)/interp_eqn.cmi: $(CINV_DISTRIB)/src/clim.cmi

$(PLUGIN_DIR)/interp_eqn.cmx: $(PLUGIN_DIR)/interp_eqn.cmi $(CINV_DISTRIB)/src/clim.cmx

$(CINV_DISTRIB)/src/clim.cmo: $(CINV_DISTRIB)/src/clim.cmi
$(CINV_DISTRIB)/src/clim.cmx: $(CINV_DISTRIB)/src/clim.cmi

$(PLUGIN_DIR)/celia_config.ml:
	echo "let celia_local = false" > $@

$(CINV_DISTRIB)/Makefile: $(CINV_DISTRIB)/Makefile.in $(CINV_DISTRIB)/config.status
	cd $(dir $@) && ./config.status

$(CINV_DISTRIB)/.depend: $(CINV_DISTRIB)/Makefile
	$(MAKE) -C $(dir $@) -j 1 .depend FRAMAC=no

$(CINV_DISTRIB)/config.status: $(CINV_DISTRIB)/configure
	if test -e $(dir $@)/config.status; then \
	  cd $(dir $@) && ./config.status -recheck; \
	else \
	 cd $(dir $@) && ./configure ; \
	fi

$(CINV_DISTRIB)/configure: $(CINV_DISTRIB)/configure.in
	cd $(dir $@) && autoconf

$(CINV_DISTRIB)/Makefile.in: ;
$(CINV_DISTRIB)/configure.in: ;

$(CINV_DISTRIB)/%: $(CINV_DISTRIB)/Makefile $(CINV_DISTRIB)/.depend
	$(MAKE) -C $(subst /$*,,$@) -j 1 $*

PLUGIN_GENERATED_LIST+=$(CELIA_HOME_DIR)/celia_config.ml

include $(FRAMAC_SHARE)/Makefile.dynamic
DYN_BLINKFLAGS+=$(CELIA_INCLUDES)
