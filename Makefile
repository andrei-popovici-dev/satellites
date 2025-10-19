CC = gcc
Flags =-Wall -Wextra -Werror 

build : tema2

tema2 : $(wildcard *.c)
	$(CC) $(Flags) $^ -o tema2

run : 
	./tema2
	
clean : 
	rm tema2