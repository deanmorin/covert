os := $(shell uname)
main = main
compiler = gcc
flags = -W -Wall -pedantic
dflags = -g -DDEBUG
lib =
cmp = $(compiler) $(flags) $(inc) -c
lnk = $(compiler) $(flags) $(lib) -o $(main)
objects = main.o network.o

ifeq ($(os), Darwin)
	flags += -j8
endif

debug : flags += $(dflags)
debug : $(main)

$(main) : $(objects)
	$(lnk) main.o network.o

main.o : main.c header.h network.h
	$(cmp) main.c
			
network.o : network.c network.h
	$(cmp) network.c

clean :
		rm $(main) *.o
