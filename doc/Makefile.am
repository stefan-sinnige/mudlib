# Conditionally build the documentation
if MANUAL
    manual_PDF = manual.pdf
else
    manual_PDF = skip
endif

manual_SOURCES = \
    manual.tex \
	bibliography.bib \
	mud-uml.sty \
	mudbook.cls

pdf: $(manual_PDF)
install-pdf: install-$(manual_PDF)

# Targets when skipping documentation
skip:
install-skip:

# Targets for manual creation and installation
manual.pdf: $(manual_SOURCES)
	$(PDFLATEX) manual.tex
	$(BIBER) manual.bcf
	$(PDFLATEX) manual.tex
	$(PDFLATEX) manual.tex

install-manual.pdf: $(manual_PDF)
	install -d $(pdfdir)
	install $(manual_PDF) $(pdfdir)

doxygen:

# vi: set ai ts=4 noexpandtab: 
