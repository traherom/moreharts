all : readme

test : readme
	evince readme.pdf

clean : clean-temp
	rm -f *.pdf

clean-temp :
	rm -f *.aux *.log
	
readme : readme.tex
	pdflatex readme.tex
	pdflatex readme.tex
	
