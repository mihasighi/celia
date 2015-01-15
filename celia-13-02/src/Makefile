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
# Copyright (C) 2011 Mihaela SIGHIREANU
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

include ../Makefile.config

SHAD_PREFIX = $(CINV)/shad

INCLUDES = \
-I . \
-I $(CINV)/shapes \
-I $(SMTLIB2_PREFIX) \
-I $(MLGMPIDL_PREFIX)/lib \
-I $(APRON_PREFIX)/lib \
-I $(CAMLLIB_PREFIX)/lib \
-I $(SHAD_PREFIX)/lib \
-I $(FIXPOINT_PREFIX)/lib 

BFLAGS   = -g -dtypes $(INCLUDES)
OFLAGS   = -dtypes $(INCLUDES)

GENERATED = cinv_version.ml 

# main targets
##############

all: depend clim.cma 

# OCAML
OCAMLCMA = unix.cma str.cma nums.cma bigarray.cma camllib.cma
OCAMLCMXA = $(OCAMLCMA:.cma=.cmxa)

# APRON
APRON_CMA = gmp.cma apron.cma box$(CDLIB).cma oct$(CDLIB).cma polka$(COLIB).cma ppl.cma 
APRON_CMXA = $(APRON_CMA:.cma=.cmxa)

# SHAD
SHAD_CMA = shad.cma
SHAD_CMXA = $(SHAD_CMA:.cma=.cmxa)

# FIXPOINT
FXP_CMA = fixpoint.cma
FXP_CMXA = $(FXP_CMA:.cma=.cmxa)

#
# Clim
#
CLCML_EXPORT = option_misc.ml pp.ml \
	loc.ml output.ml \
	boolexpr.ml \
        clim_options.ml clim_stdlib.ml \
	clim_constructors.ml \
	clim_pervasives.ml \
	clim_output_misc.ml clim_poutput.ml \
        cinv_options.ml cinv_environment.ml cinv_equation.ml cinv_solving.ml \
	cinv_frontend.ml
CLCMO_EXPORT = $(CMO_EXPORT) $(CLCML_EXPORT:.ml=.cmo)
CLCMX_EXPORT = $(CLCMO_EXPORT:.cmo=.cmx)
CLCMI_EXPORT = clim_ast.cmi clim_env.cmi clim.cmi $(CLCMO_EXPORT:.cmo=.cmi)

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
-L$(CINV)/shapes

COLIB=MPQ
OLIB=MPQ
CDLIB=D
DLIB=D

CLLDLIBS   = \
-lshape_caml -lshape$(DEBUG) \
-lsmtlib2parser \
-lpolka$(COLIB)_caml -lpolka$(OLIB) \
-loct$(CDLIB)_caml -loct$(DLIB) \
-lbox$(CDLIB)_caml -lbox$(DLIB) \
-lapron_caml -lapron$(DEBUG) \
-lgmp_caml -lmpfr -lgmp \
-lunix -lbigarray -lcamlidl

CLALDFLAGS=-ccopt "$(CLLDFLAGS)" -cclib "$(CLLDLIBS)"

clim.cmi: $(CLCMI_EXPORT)

clim.cmo: $(CLCMO_EXPORT)
	$(OCAMLC) $(BFLAGS) -pack -o clim.cmo $^

clim.cmx clim.o: $(CLCMX_EXPORT)
	$(OCAMLOPT) $(OFLAGS) -pack -o $@ $^

clim.cma: clim.cmo
	$(OCAMLC) -a -g -o $@ $(CLALDFLAGS) $(INCLUDES) $(APRON_CMA) $(FXP_BYTE) $^

clim.cmxa: clim.cmx
	$(OCAMLOPT) -a -g -o $@ $(CLALDFLAGS) $^

clpc: clim.cmi clim.cmo clim.cmx clim.cma clim.cmxa

ifeq ($(OCAMLBEST),opt)
CLLIB= clim.cmxa
else
CLLIB= clim.cma
endif

include ../Version

cinv_version.ml: ../Version Makefile
	echo "let version = \""$(VERSION)"\"" >> $@
	echo "let date = \""`date`"\"" >> $@
	echo "let libdir = \""$(LIBDIR)/$(CINVTOOL)"\"" >> $@

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

# clean and depend
##################

clean: 
	rm -f *.cm[ioxa] *.o *~ *.annot
	rm -f $(GENERATED)

.depend depend:: $(GENERATED)
	rm -f .depend
	$(OCAMLDEP) -slash $(INCLUDES) *.ml *.mli > .depend

include .depend
