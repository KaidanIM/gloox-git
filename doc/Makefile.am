
TEX=aim-objectives.tex conclusions.tex final-planning.tex results.tex \
    approach.tex deliverables.tex final-report.tex introduction.tex revised-planning.tex \
    background.tex environment.tex interim-planning.tex

final-report.pdf: ${TEX} fyp.mp final-report.bib
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

presentation.pdf: presentation.tex fyp.mp
	mpost -interaction=nonstopmode 'fyp'
	latex -src -interaction=nonstopmode 'presentation.tex'
	latex -src -interaction=nonstopmode 'presentation.tex'
	latex -src -interaction=nonstopmode 'presentation.tex'
	dvipdfm 'presentation.dvi'

all: final-report.pdf interim-report.pdf presentation.pdf

clean:
	rm -rf *.pdf
	rm -rf *.ps
	rm -rf *.0
	rm -rf *.1
	rm -rf *.2
	rm -rf *.3
	rm -rf *.4
	rm -rf *.5
	rm -rf *.6
	rm -rf *.7
	rm -rf *.8
	rm -rf *.9
	rm -rf *.10
	rm -rf *.11
	rm -rf *.bbl
	rm -rf *.log
	rm -rf *.dvi
	rm -rf *.aux
	rm -rf *.blg
	rm -rf *.mpx
