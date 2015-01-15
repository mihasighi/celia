#
# MAIN NAME
#
NAME=intlist-main-map-add2

#
# FILES
#
FILES= \
$(CINV)/samples/c/intlist-map-add2.c \
$(CINV)/samples/c/intlist-lib-init.c \
$(CINV)/samples/c/intlist-lib-printlist.c \
$(CINV)/samples/c/$(NAME).c

#
# DEBUG
#
VALGRIND=
ifeq ($(DEBUG),_debug)
        VALGRIND=valgrind
endif

#CELIA=$(CINV)/bin/frama-c-Celia.byte
CELIA=./frama-c-Celia.byte

all: ucons

# WARNING: mset domain is not useful for this example
# TODO: multiset constraints generated are not good@
mset: $(NAME)-mset.prop $(FILES)
	$(VALGRIND) $(CELIA) -celia -celia-cinv-opt $(NAME)-mset.prop $(FILES)

ucons: $(NAME)-ucons.prop $(NAME)-ucons-funs.prop $(FILES)
	$(VALGRIND) $(CELIA) -celia -celia-cinv-opt $(NAME)-ucons.prop $(FILES)

clean:
	rm -rf *.shp pan* *.txt
