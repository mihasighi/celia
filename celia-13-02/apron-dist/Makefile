include Makefile.config

REQUESTED = \
mlgmpidl/Makefile.config \
apron/Makefile.config

all: $(REQUESTED)
ifneq ($(HAS_OCAML),)
	(cd mlgmpidl; make all install)
endif
	(cd apron; make all)

install:
	(cd apron; make install)

rebuild: $(REQUESTED)
ifneq ($(HAS_OCAML),)
	(cd mlgmpidl; make rebuild install)
endif
	(cd apron; make rebuild)

clean: $(REQUESTED)
	(cd mlgmpidl; make clean)
	(cd apron; make clean)

mostlyclean: $(REQUESTED)
	(cd mlgmpidl; make mostlyclean)
	(cd apron; make mostlyclean)
	/bin/rm -f $(REQUESTED)

uninstall: $(REQUESTED)
	(cd mlgmpidl; make uninstall)
	(cd apron; make uninstall)

distclean: uninstall

doc: 
ifneq ($(HAS_OCAML),)
	(cd mlgmpidl; make mlgmpidl.pdf html)
endif
	(cd apron; make doc)

mlgmpidl/Makefile.config: Makefile.config Makefile
	$(SED) -e '1 aHAS_MPFR=1\n' Makefile.config >$@

apron/Makefile.config: Makefile.config Makefile
	cp -f $< $@
