##########################################################################
#                                                                        #
#  The CINV tool for dynamic programs verification                       #
#                                                                        #
#  This software is free software; you can redistribute it and/or        #
#  modify it under the terms of the GNU General Public                   #
#  License version 2, as published by the Free Software Foundation.      #
#                                                                        #
#  This software is distributed in the hope that it will be useful,      #
#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  #
#                                                                        #
#  See the GNU General Public License version 2 for more details         #
#  (enclosed in the file GPL).                                           #
#                                                                        #
##########################################################################

#
# sample Makefile for Objective Caml
# Copyright (C) 2007 Mihaela SIGHIREANU
#   from a first script by Jean-Christophe FILLIATRE
# 
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public
# License version 2, as published by the Free Software Foundation.
# 
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# 
# See the GNU Library General Public License version 2 for more details
# (enclosed in the file LGPL).

ifeq ($(VERBOSEMAKE),yes)
 QUIET = 
else    
 QUIET = yes
endif

# where to install the binaries
prefix=/usr/local
exec_prefix=${prefix}
BINDIR=${exec_prefix}/bin
LIBDIR=${exec_prefix}/lib

EXE=

# where to install the man page
MANDIR=${prefix}/share/man

# other variables
OCAMLC   = ocamlc
OCAMLOPT = ocamlopt
OCAMLDEP = ocamldep
OCAMLLEX = ocamllex.opt
OCAMLYACC= ocamlyacc
OCAMLLIB = /usr/lib/ocaml
OCAMLBEST= opt
OCAMLVERSION = 3.12.0

include ./Makefile.config

FRAMAC= yes
ifeq ($(FRAMAC),yes)
CELIA_PLUGIN_BYTE= celia_plugin.byte
CELIA_PLUGIN_OPT= celia_plugin.opt
CELIA_PLUGIN_BEST= celia.$(OCAMLBEST)
.PHONY: $(CELIA_PLUGIN_BYTE) $(CELIA_PLUGIN_OPT)
endif

INCLUDES = \
-I . \
-I src \
-I shapes \
-I $(SMTLIB2_PREFIX) \
-I $(MLGMPIDL_PREFIX)/lib \
-I $(APRON_PREFIX)/lib \
-I $(CAMLLIB_PREFIX)/lib \
-I $(FIXPOINT_PREFIX)/lib 

BFLAGS   = -g -dtypes $(INCLUDES)
OFLAGS   = -dtypes $(INCLUDES)

LDFLAGS  = \
-L/usr/lib \
-L$(OCAMLLIB) \
-L$(MLGMPIDL_PREFIX)/lib \
-L$(APRON_PREFIX)/lib \
-L$(CAMLLIB_PREFIX)/lib \
-L$(FIXPOINT_PREFIX)/lib \
-L$(SMTLIB2_PREFIX) \
-Lshapes 

COLIB=MPQ
OLIB=MPQ
CDLIB=D
DLIB=D

LDLIBS   = \
-lsmtlib2parser \
-lpolka$(COLIB)_caml -lpolka$(OLIB) \
-loct$(CDLIB)_caml -loct$(DLIB) \
-lbox$(CDLIB)_caml -lbox$(DLIB) \
-lapron_caml -lapron$(DEBUG) \
-lgmp_caml -lmpfr -lgmp \
-lunix -lbigarray -lcamlidl

LDLIBS_SHP = \
-lshape_caml -lshape \
-llsum -lmset -lucons 

GENERATED = src/cinv_version.ml shapes/shape_version.ml 

# main targets
##############

CINVTOOL = cinv
SL3DP = sl3dp
CELIA = frama-c-Celia.$(OCAMLBEST)

BINARY  = $(SL3DP) $(CELIA)

LIBRARY = \
lib/libshape.a

all: $(BINARY) check $(CELIA_PLUGIN_BEST)

opt: $(CELIA_PLUGIN_OPT)

byte: $(CELIA_PLUGIN_BYTE)

.PHONY: check

# sanity check: the prelude files do typecheck
check: $(BINARY) 

# OCAML
OCAMLCMA = unix.cma str.cma nums.cma bigarray.cma camllib.cma
OCAMLCMXA = $(OCAMLCMA:.cma=.cmxa)

# APRON
APRON_CMA = gmp.cma apron.cma box$(CDLIB).cma oct$(CDLIB).cma polka$(COLIB).cma ppl.cma 
APRON_CMXA = $(APRON_CMA:.cma=.cmxa)

# FIXPOINT
FXP_CMA = fixpoint.cma
FXP_CMXA = $(FXP_CMA:.cma=.cmxa)

#
# Celia
#
CLCML_EXPORT = src/option_misc.ml src/pp.ml \
	src/loc.ml src/output.ml \
	src/boolexpr.ml \
        src/clim_options.ml src/clim_stdlib.ml \
	src/clim_constructors.ml \
	src/clim_pervasives.ml \
	src/clim_output_misc.ml src/clim_poutput.ml \
        src/cinv_options.ml src/cinv_equation.ml src/cinv_solving.ml \
	src/shad.ml \
	src/cinv_frontend.ml
CLCMO_EXPORT = $(CMO_EXPORT) $(CLCML_EXPORT:.ml=.cmo)
CLCMX_EXPORT = $(CLCMO_EXPORT:.cmo=.cmx)
CLCMI_EXPORT = src/clim_ast.cmi src/clim_env.cmi src/clim.cmi $(CLCMO_EXPORT:.cmo=.cmi)

FXP_BYTE=$(CAMLLIB_PREFIX)/lib/camllib.cma $(FIXPOINT_PREFIX)/lib/fixpoint.cma
APRON_BYTE=$(APRON_PREFIX)/lib/gmp.cma $(APRON_PREFIX)/lib/apron.cma $(APRON_PREFIX)/lib/box$(CDLIB).cma

CLCMO = $(APRON_BYTE) $(FXP_BYTE) $(CLCMO_EXPORT) 
CLCMX = $(APRON_BYTE:.cmo=.cmxa) $(FXP_BYTE:.cmo=.cmxa) $(CLCMO_EXPORT:.cmo=.cmx)

$(CLCMX_EXPORT): OFLAGS:=$(OFLAGS) -for-pack Clim

CLLDFLAGS  = \
-L/usr/lib \
-L$(MLGMPIDL_PREFIX)/lib \
-L$(APRON_PREFIX)/lib \
-L$(FIXPOINT_PREFIX)/lib \
-L$(CAMLLIB_PREFIX)/lib \
-L$(OCAMLLIB) \
-L$(SMTLIB2_PREFIX) \
-L./shapes

CLLDLIBS   = \
$(LDLIBS_SHP) \
-lsmtlib2parser \
-lpolka$(COLIB)_caml -lpolka$(OLIB) \
-loct$(CDLIB)_caml -loct$(DLIB) \
-lbox$(CDLIB)_caml -lbox$(DLIB) \
-lapron_caml -lapron$(DEBUG) \
-lgmp_caml -lmpfr -lgmp \
-lunix -lbigarray -lcamlidl

CLALDFLAGS=-ccopt "$(CLLDFLAGS)" -cclib "$(CLLDLIBS)"

src/clim.cmi src/clim.cmo: $(CLCMO_EXPORT)
	$(OCAMLC) $(BFLAGS) -pack -o src/clim.cmo $^

src/clim.cmx src/clim.o: $(CLCMX_EXPORT)
	$(OCAMLOPT) $(OFLAGS) -pack -o $@ $^

src/clim.cma: src/clim.cmo
	$(OCAMLC) -a -g -o $@ $(CLALDFLAGS) $(INCLUDES) $(APRON_CMA) $(FXP_BYTE) $^

src/clim.cmxa: src/clim.cmx
	$(OCAMLOPT) -a -g -o $@ $(CLALDFLAGS) $^

clpc: src/clim.cmi src/clim.cmo src/clim.cmx src/clim.cma src/clim.cmxa

ifeq ($(OCAMLBEST),opt)
#CLLIB= src/clim.cmo src/clim.cmx src/clim.o 
CLLIB= src/clim.cmxa
else
#CLLIB= src/clim.cmo
CLLIB= src/clim.cma
endif

# 
# SL3DP
sl3dp: 
	make -C shapes $(SL3DP)
	mv $(SL3DP) bin/

# Frama-C plugin for Cinv
ifeq ($(FRAMAC),yes)
CELIA_PLUGIN_PATH=frama-c-plugin
$(CELIA_PLUGIN_BYTE): $(CLLIB) $(CLCMO) $(CMO)
	$(MAKE) -C $(CELIA_PLUGIN_PATH) depend
	$(MAKE) -C $(CELIA_PLUGIN_PATH) Celia.cmo

$(CELIA_PLUGIN_OPT): $(CLLIB) $(CLCMX) $(CMX)
	$(MAKE) -C $(CELIA_PLUGIN_PATH) depend
	$(MAKE) -C $(CELIA_PLUGIN_PATH)

install-plugin:
	$(MAKE) -C $(CELIA_PLUGIN_PATH) install
clean-plugin:
	$(MAKE) -C $(CELIA_PLUGIN_PATH) clean
endif


include Version

shapes/shape_version.ml: Version Makefile
	echo "let version = \""$(VERSION)"\"" >> $@
	echo "let date = \""`date`"\"" >> $@
	echo "let libdir = \""$(LIBDIR)/$(LIBRARY)"\"" >> $@

src/cinv_version.ml: Version Makefile
	echo "let version = \""$(VERSION)"\"" >> $@
	echo "let date = \""`date`"\"" >> $@
	echo "let libdir = \""$(LIBDIR)/$(CELIA)"\"" >> $@

doc:: doc/version.tex
# TODO	cd doc; make

# tests

.PHONY: tests

tests:: $(BINARY)
# TODO	cd tests; sh ./tests "../$(BINARY)"
# TODO	make -C examples yices

examples:: $(BINARY)
# TODO	make -C examples

# debugging


# installation
##############

install: install-binary install-lib install-man install-plugin

install-binary: $(BINARY)
	mkdir -p $(BINDIR)
	cp -f $(BINARY) $(BINDIR)/.

install-lib: 
	cp -f shapes/$(LIBRARY) $(LIBDIR)/

install-man:
	mkdir -p $(MANDIR)/man1
	cp -f doc/*.1 $(MANDIR)/man1

local-install: $(BINARY) 
	cp $(BINARY) $$HOME/bin/.

# doc

doc/manual.ps: doc/manual.tex doc/version.tex
	make -C doc manual.ps

doc/version.tex: Version Makefile.in
	echo '\\newcommand{\\cinvversion}'"{$(VERSION)}" > $@

doc/manual.html: doc/manual.tex doc/version.tex
	make -C doc manual.html

# special rules
###############

# CAMLP4=@CAMLP4O@ pa_extend.cmo pa_macro.cmo

# src/%.cmo: src/%.ml4
# 	$(OCAMLC) -c $(BFLAGS) -pp "$(CAMLP4) -DOCAML -impl" -impl $<

# src/%.cmx: src/%.ml4
# 	$(OCAMLOPT) -c $(OFLAGS) -pp "$(CAMLP4) -DOCAML -impl" -impl $<

# src/%.ml: src/%.ml4
# 	$(CAMLP4) pr_o.cmo -DOCAML -impl $< > $@

# generic rules
###############

.SUFFIXES: .mli .ml .cmi .cmo .cmx .mll .mly .v .vo .ml4

.mli.cmi:
	$(if $(QUIET),@echo 'Ocamlc   $<' &&) $(OCAMLC) $(BFLAGS) -c $<

.ml.cmo:
	$(if $(QUIET),@echo 'Ocamlc   $<' &&) $(OCAMLC) $(BFLAGS) -c $<

.ml.o:
	$(OCAMLOPT) $(OFLAGS) -c $<

.ml.cmx:
	$(if $(QUIET),@echo 'Ocamlopt $<' &&) $(OCAMLOPT) $(OFLAGS) -c $<

.mll.ml:
	$(OCAMLLEX) $<

.mly.ml:
	$(OCAMLYACC) -v $<

.mly.mli:
	$(OCAMLYACC) -v $<

.ml4.ml:
	$(CAMLP4) -impl $< > $@

otags:
	otags cpn/*.mli cpn/*.ml c/*.mli c/*.ml intf/*.mli intf/*.ml

wc:
	ocamlwc -p cpn/*.ml* jc/*.ml* c/*.ml* intf/*.ml* tools/*.ml*

# distrib
#########

NAME=$(CINVTOOL)-$(VERSION)
EXPORT=export/$(NAME)

FTP = $(HOME)/ftp/$(CINVTOOL)
WWW = $(HOME)/WWW/$(CINVTOOL)

FILES =src/*.ml* shapes/* \
       Version Makefile.in configure.in configure .depend \
       README INSTALL COPYING GPL CHANGES \
       doc/Makefile doc/manual.ps doc/$(CINVTOOL).1 \
       samples/Makefile* 

distrib export: source export-doc export-examples linux
	make -C $(HOME)/www/cinv install

source: doc/manual.ps export/$(NAME).tar.gz
	cp CHANGES export/$(NAME).tar.gz $(FTP)
	echo "<#def version>$(VERSION)</#def>" > $(HOME)/www/$(CINVTOOL)/version.prehtml
	echo "<#def cversion>$(CVERSION)</#def>" >> $(HOME)/www/$(CINVTOOL)/version.prehtml
	echo "<#def jcversion>$(JCVERSION)</#def>" >> $(HOME)/www/$(CINVTOOL)/version.prehtml

export/$(NAME).tar.gz:
	mkdir -p $(EXPORT)/bin
	cp --parents $(FILES) $(EXPORT)
	if test -d ocamlgraph; then \
	  cp --parents $(OCAMLGRAPHFILES) $(EXPORT); \
	fi
	cd $(EXPORT); rm -f $(GENERATED)
	cd export; tar cf $(NAME).tar $(NAME); gzip -f --best $(NAME).tar

tarball: 
	cd export; rm -rf $(NAME) $(NAME).tar.gz
	make export/$(NAME).tar.gz

EXFILES = examples/*/*.v examples/*/*.why

export-examples:
	cp --parents $(EXFILES) $(WWW)
	make -C $(WWW)/examples clean depend
	echo "*** faire make all dans $(WWW)/examples ***"

export-doc: doc/manual.ps doc/manual.html
	cp doc/manual.ps doc/manual.html $(WWW)/manual
	cp doc/logic_syntax.bnf $(WWW)/manual
	(cd $(WWW)/manual; hacha manual.html)

BINARYNAME = $(NAME)-$(OSTYPE)

linux: binary

BINARYFILES = $(FILES) bin/$(CELIA) bin/$(SL3DP)
# README INSTALL COPYING GPL \
# 	bin/$(CINVTOOL).static doc/$(CINVTOOL).1 doc/manual.ps \
# 	lib/$(CINVTOOL)/*.why

binary: $(BINARYFILES)
	mkdir -p export/$(BINARYNAME)
	cp --parents $(BINARYFILES) export/$(BINARYNAME)
	(cd export; tar czf $(BINARYNAME).tar.gz $(BINARYNAME))
	cp export/$(BINARYNAME).tar.gz $(FTP)

# file headers
##############
headers:
	headache -c doc/headache_config.txt -h doc/header.txt \
		Makefile.in configure.in README \
		*/*.ml */*.ml[ily4]

# myself
########

Makefile: Makefile.in config.status
	./config.status

config.status: configure
	./config.status --recheck

configure: configure.in
	autoconf 

# clean and depend
##################

clean: 
	rm -f bin/$(CELIA).* bin/$(SL3DP)
	rm -f $(GENERATED)
	make -C interproc-dist clean
	make -C apron-dist clean
	make -C smtlib2parser-dist clean
	make -C shapes clean
	make -C src clean
	make -C frama-c-plugin clean
	make -C samples clean
	make -C doc clean

distclean:: clean
	rm -f Makefile config.status config.cache config.log
	make -C interproc-dist distclean
	make -C apron-dist distclean
	make -C smtlib2parser-dist distclean
	make -C shapes distclean
	make -C src distclean
	make -C frama-c-plugin distclean
	make -C samples distclean
	make -C doc distclean

.depend depend:: $(GENERATED)
	rm -f .depend
	$(OCAMLDEP) -slash $(INCLUDES) src/*.ml src/*.mli > .depend
ifeq ($(FRAMAC),yes)
	$(MAKE) -C $(CELIA_PLUGIN_PATH) depend
endif


alldepend:
	make -C examples depend

include .depend
