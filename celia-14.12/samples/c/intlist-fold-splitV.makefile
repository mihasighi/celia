#
# MAIN NAME
#
NAME=intlist-fold-splitV

#
# FILES
#
FILES= \
$(CINV)/samples/c/$(NAME).c

#
# DEBUG
#
VALGRIND=
ifeq ($(DEBUG),_debug)
        VALGRIND=valgrind
endif

TOOL=$(CINV)/frama-c-plugin/frama-c-Celia.byte

all: ucons

# WARNING: mset domain is not useful for this example
# TODO: multiset constraints generated are not good@
mset: $(NAME)-mset.prop $(FILES)
	$(VALGRIND) $(TOOL) -celia -celia-cinv-opt $(NAME)-mset.prop $(FILES)

ucons: $(NAME)-ucons.prop $(NAME)-ucons-funs.prop $(FILES)
	$(VALGRIND) $(TOOL) -celia -celia-cinv-opt $(NAME)-ucons.prop $(FILES)

clean:
	rm -rf *.shp pan* *.txt
