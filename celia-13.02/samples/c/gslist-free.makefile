#
# MAIN NAME
#
NAME=gslist-free

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

all: ucons

mset: $(NAME)-mset.prop $(FILES)
	$(VALGRIND) $(CINV)/bin/frama-c-Celia.byte -celia -celia-cinv-opt $(NAME)-mset.prop $(FILES)
	mv pan $(NAME)-mset

ucons: $(NAME)-ucons.prop $(NAME)-ucons-funs.prop $(FILES)
	$(VALGRIND) $(CINV)/bin/frama-c-Celia.byte -celia -celia-cinv-opt $(NAME)-ucons.prop $(FILES)
	mv pan $(NAME)-ucons
	
clean:
	rm -rf *.shp pan* *.txt
