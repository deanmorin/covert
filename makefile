os := $(shell uname)
main = main
compiler = gcc
flags = -W -Wall -pedantic
dflags = -g -DDEBUG
lib =
cmp = $(compiler) $(flags) $(inc) -c
lnk = $(compiler) $(flags) $(lib) -o $(main)
objects = main.o network.o util.o header.o

ifeq ($(os), Darwin)
	flags += -j8
endif

debug : flags += $(dflags)
debug : $(main)

$(main) : $(objects)
	$(lnk) main.o network.o util.o header.o

main.o : main.c network.h
	$(cmp) main.c
			
network.o : network.c network.h
	$(cmp) network.c

util.o : util.c util.h
	$(cmp) util.c

header.o : header.c header.h
	$(cmp) header.c

clean :
	rm $(main) *.o
