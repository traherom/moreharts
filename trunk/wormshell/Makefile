SRC = main.asm
OBJS = $(SRC:.asm=.o)
LSTS = $(SRC:.asm=.lst)

# Main program
worm : $(OBJS)
	gcc -nostdlib $(OBJS) -o worm

debug : $(OBJS) $(LSTS)
	gcc -nostdlib -ggdb $(OBJS) -o worm

%.lst : %.asm
	nasm -f elf -l $@ $< 

%.o : %.asm
	nasm -f elf $<

clean : 
	rm $(OBJS) $(LSTS) worm
	
run : worm
	./worm
	
