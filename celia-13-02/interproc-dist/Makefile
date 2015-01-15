include Makefile.config

REQUESTED = \
camllib/Makefile.config \
fixpoint/Makefile.config 

all:
	(cd camllib; make all install)
	(cd fixpoint; make all install)

install: all
	(cd camllib; make install)
	(cd fixpoint; make install)

clean:
	(cd camllib; make clean)
	(cd fixpoint; make clean)

distclean:
	(cd camllib; make distclean)
	(cd fixpoint; make distclean)

mostlyclean:
	(cd camllib; make mostlyclean)
	(cd fixpoint; make mostlyclean)
	/bin/rm -f $(REQUESTED)

camllib/Makefile.config: Makefile.config
	cp $^ $@
fixpoint/Makefile.config: Makefile.config
	cp $^ $@
