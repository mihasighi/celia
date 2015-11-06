
#
# MAIN NAME
#
NAME=gslist-alloc

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

all: mset ucons

mset: $(NAME)-mset.prop $(FILES)
	$(VALGRIND) $(CINV)/bin/frama-c-Celia.byte -celia -celia-cinv-opt $(NAME)-mset.prop $(FILES)
	mv pan $(NAME)-mset.pan

ucons: $(NAME)-ucons.prop $(NAME)-ucons-funs.prop $(FILES)
	$(VALGRIND) $(CINV)/bin/frama-c-Celia.byte -celia -celia-cinv-opt $(NAME)-ucons.prop $(FILES)
	mv pan $(NAME)-ucons.pan
	
clean:
	rm -rf *.shp pan* *.txt
