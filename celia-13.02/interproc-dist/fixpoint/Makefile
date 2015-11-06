include Makefile.config

#---------------------------------------
# Directories
#---------------------------------------

SRCDIR = $(shell pwd)
#
# Installation directory prefix
#
PREFIX = $(FIXPOINT_PREFIX)

#---------------------------------------
# CAML part
#---------------------------------------

OCAMLINC = -I $(CAMLLIB_PREFIX)/lib

MLMODULES = fixpointType fixpointStd fixpointGuided fixpointDyn fixpoint
MLSRC = $(MLMODULES:%=%.mli) $(MLMODULES:%=%.ml)

INT = $(MLMODULES:%=%.cmi)
OBJ = $(MLMODULES:%=%.cmo)
OBJx = $(MLMODULES:%=%.cmx)

MLLIB_TOINSTALL = $(INT) $(MLMODULES:%=%.mli) fixpoint.cma
MLLIB_TOINSTALLx = $(OBJSx) fixpoint.cmxa fixpoint.a

MLMODULES_aux = print time sette hashhe ilist sHGraph

MLMODULES_tex = Print Time Sette Hashhe Ilist SHGraph FixpointType FixpointStd FixpointGuided FixpointDyn Fixpoint

#---------------------------------------
# Rules
#---------------------------------------

# Global rules
all: byte opt

byte: $(INT) $(OBJ) fixpoint.cma 
opt: $(INT) $(OBJx) fixpoint.cmxa

install:
	$(INSTALLd) $(PREFIX)/lib
	for i in $(MLLIB_TOINSTALL) $(MLLIB_TOINSTALLx); do \
		if test -f $$i; then $(INSTALL) $$i $(PREFIX)/lib; fi; \
	done

distclean: clean
	(cd $(PREFIX)/lib; /bin/rm -f $(MLLIB_TOINSTALL) $(MLLIB_TOINSTALLx))

clean:
	/bin/rm -f *.[aoc] *.cm[ioxa] *.cmxa *.annot *.dot
	/bin/rm -f *.idx *.ilg *.ind *.out *.log *.aux *.bbl *.blg *.toc *.dvi *.ps ocamldoc.tex $(MLMODULES_tex:%=%.tex) ocamldoc.sty Makefile.depend *.pdf
	/bin/rm -fr html

mostlyclean: clean

tar: $(MLSRC) Makefile
	(cd ..; tar zcvf $(HOME)/fixpoint.tgz $(^:%=fixpoint/%))

dist: $(MLSRC) Makefile Makefile.config.model fixpoint.pdf example.ml example.mli fixpointdoc.tex html README
	(cd ..; tar zcvf $(HOME)/fixpoint.tgz $(^:%=fixpoint/%))

# CAML rules
fixpoint.cma: $(OBJ)
	$(OCAMLC) $(OCAMLFLAGS) $(OCAMLINC) -a -o $@ $^
fixpoint.cmxa: $(OBJx)
	$(OCAMLOPT) $(OCAMLOPTFLAGS) $(OCAMLINC) -a -o $@ $^

example: fixpoint.cma example.cmo
	$(OCAMLC) $(OCAMLFLAGS) $(OCAMLINC) -o $@ camllib.cma $^

# TEX rules

dvi: fixpointdoc.dvi

.PHONY: fixpointdoc.dvi html

fixpoint.pdf: fixpointdoc.dvi
	$(DVIPDF) $^ $@

fixpointdoc.dvi: $(INT) $(MLMODULES:%=%.mli)
	$(OCAMLDOC) -latextitle 1,chapter -latextitle 2,section -latextitle 3,subsection -latextitle 4,subsubsection -latextitle 5,paragraph -sepfiles -latex -o ocamldoc.tex $(OCAMLINC) $(MLMODULES:%=%.mli) $(MLMODULES_aux:%=$(CAMLLIB_PREFIX)/lib/%.mli)
	$(LATEX) fixpointdoc
	$(MAKEINDEX) fixpointdoc
	$(LATEX) fixpointdoc
	$(LATEX) fixpointdoc

html: $(MLMODULES:%=%.mli) $(MLMODULES_aux:%=$(CAMLLIB_PREFIX)/lib/%.mli)
	mkdir -p html
	$(OCAMLDOC) $(OCAMLINC) -intro fixpoint.odoc -html -d html -colorize-code $^

homepage: html fixpoint.pdf
	latex index && bibtex index && latex index && latex index
	hyperlatex index
	scp -r index.html html fixpoint.pdf example.ml Changes presentation-fixpoint.pdf \
		johns:/home/wwwpop-art/people/bjeannet/bjeannet-forge/fixpoint
	ssh johns chmod -R ugoa+rx /home/wwwpop-art/people/bjeannet/bjeannet-forge/fixpoint

#--------------------------------------------------------------
# IMPLICIT RULES AND DEPENDENCIES
#--------------------------------------------------------------

.SUFFIXES: .ml .mli .cmi .cmo .cmx .dvi .tex .ps

#-----------------------------------
# CAML
#-----------------------------------

%.cmi: %.mli
	$(OCAMLC) $(OCAMLFLAGS) $(OCAMLINC) -c $<

%.cmo: %.ml %.cmi
	$(OCAMLC) $(OCAMLFLAGS) $(OCAMLINC) -c $<

%.cmx: %.ml %.cmi
	$(OCAMLOPT) $(OCAMLOPTFLAGS) $(OCAMLINC) -c $<

%.pdf: %.dvi
	$(DVIPDF) $^ $@

depend:
	$(OCAMLDEP) $(OCAMLINC) $(MLSRC) >Makefile.depend

Makefile.depend:
	$(OCAMLDEP) $(OCAMLINC) $(MLSRC) >Makefile.depend


-include Makefile.depend
