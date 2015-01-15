
# targets
# ##############
cinv: cinv.html cinv.pdf
	mkdir html
	mv cinv.html html/cinv

celia: celia.html celia.pdf
	mv celia.html html/celia


# generic rules
# ###############

.SUFFIXES: .texi .html .pdf 

.texi.html:
	makeinfo --html $<
	mv $*  $*.html

.texi.pdf:
	texi2pdf $<

clean:
	rm -f *.aux *.cp *.fn *.ky *.tp *.vr *.log *.toc *.pg *~ 
	rm -rf *.html
	

