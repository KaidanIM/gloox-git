

final-report.pdf: final-report.tex fyp.mp final-report.bib
	mpost -interaction=nonstopmode 'fyp'
	latex -src -interaction=nonstopmode 'final-report.tex'
	bibtex 'final-report'
	latex -src -interaction=nonstopmode 'final-report.tex'
	latex -src -interaction=nonstopmode 'final-report.tex'
	dvipdfm 'final-report.dvi'

interim-report.pdf: interim-report.tex fyp.mp interim-report.bib
	mpost -interaction=nonstopmode 'fyp'
	latex -src -interaction=nonstopmode 'interim-report.tex'
	bibtex 'interim-report'
	latex -src -interaction=nonstopmode 'interim-report.tex'
	latex -src -interaction=nonstopmode 'interim-report.tex'
	dvipdfm 'interim-report.dvi'

all: final-report.pdf interim-report.pdf

clean:
	rm -rf *.pdf
	rm -rf *.ps
	rm -rf *.0
	rm -rf *.1
	rm -rf *.2
	rm -rf *.bbl
	rm -rf *.log
	rm -rf *.dvi
	rm -rf *.aux
	rm -rf *.blg
	rm -rf *.mpx
