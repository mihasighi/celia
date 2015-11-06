#
# MAIN NAME
#
NAME=intlist-main-map-add2

#
# FILES
#
FILES= \
$(CINV)/samples/c/intlist-lib-init.c \
$(CINV)/samples/c/intlist-lib-printlist.c \
$(CINV)/samples/c/$(NAME).c

#
# TOOL
#
TOOL=$(CINV)/frama-c-plugin/frama-c-Celia.byte
#TOOL=$(CINV)/bin/frama-c-Celia.byte

#
# DEBUG
#
VALGRIND=
ifeq ($(DEBUG),_debug)
        VALGRIND=valgrind
endif

all: ucons

# WARNING: mset domain is not useful for this example
# TODO: multiset constraints generated are not good@
mset: $(NAME)-mset.prop $(FILES)
	$(VALGRIND) $(TOOL) -celia -celia-cinv-opt $(NAME)-mset.prop $(FILES)
	mv pan $(NAME)-mset.pan

ucons: $(NAME)-ucons.prop $(NAME)-ucons-funs.prop $(FILES)
	$(VALGRIND) $(TOOL) -celia -celia-cinv-opt $(NAME)-ucons.prop $(FILES)
	mv pan $(NAME)-ucons.pan

clean:
	rm -rf *.shp pan* *.txt
