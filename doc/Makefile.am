if ACHART_MANPAGE

man_MANS = \
	acharts.1

acharts.1.txt.xml: acharts.1.txt
	asciidoc --doctype=manpage --backend=docbook --out-file=$@ $<

acharts.1: acharts.1.txt.xml
	xmlto man $<

endif

EXTRA_DIST = \
	acharts.1.txt

CLEANFILES = acharts.1 acharts.1.txt.xml
