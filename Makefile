CC=g++
FILES=ims.cpp
FLAGS=-lm -g -lsimlib -o
OUTPUT=ims

make:
	$(CC) $(FILES) $(FLAGS) $(OUTPUT)
