
#TOOL=$(CINV)/frama-c-plugin/frama-c-Celia.byte
TOOL=$(CINV)/bin/frama-c-Celia.byte

%.pan: %.c %-ucons.prop %-ucons-funs.prop clean
	$(TOOL) -celia -celia-sl3 -celia-cinv-opt $*-ucons.prop $*.c
	mv pan $*.pan
	$(CINV)/com/pan2html.sh $* ucons

clean:
	rm -rf pan 
