all : readme

test : all
	evince *.pdf

clean : clean-all
	rm -f *.pdf
	
clean-temp: 
	rm -f *.aux *.log
	
readme :
	pdflatex readme.tex
	
