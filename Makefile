CC=g++
FILES=ims.cpp
FLAGS=-lm -lsimlib -o
OUTPUT=ims

make:
	$(CC) $(FILES) $(FLAGS) $(OUTPUT)
