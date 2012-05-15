os := $(shell uname)
main = covert
compiler = gcc
flags = -W -Wall -pedantic
dflags = -g -DDEBUG
lib =
cmp = $(compiler) $(flags) $(inc) -c
lnk = $(compiler) $(flags) $(lib) -o $(main)
objects = main.o client.o server.o network.o util.o header.o

ifeq ($(os), Darwin)
	flags += -j8
endif

debug : flags += $(dflags)
debug : $(main)

$(main) : $(objects)
	$(lnk) $(objects)

main.o : main.c clntsrvr.h
	$(cmp) main.c

client.o : client.c clntsrvr.h network.h util.h
	$(cmp) client.c

server.o : server.c clntsrvr.h network.h util.h
	$(cmp) server.c
			
network.o : network.c network.h
	$(cmp) network.c

util.o : util.c util.h
	$(cmp) util.c

header.o : header.c header.h
	$(cmp) header.c

clean :
	rm $(main) *.o
