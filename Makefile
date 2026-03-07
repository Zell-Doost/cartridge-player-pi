CC=  gcc  # gcc or g++

CFLAGS=-g -Wall 
LDFLAGS=
LIBS=-lm

# subdirectory for objects
O=build

# not too sophisticated dependency
OBJS=				\
		$(O)/cpu.o		

all:	 $(O)/pccpu

clean:
	rm -f *.o *~ *.flc
	rm -f linux/*

$(O)/pccpu:	$(OBJS) $(O)/main.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(O)/main.o \
	-o $(O)/pccpu $(LIBS)

$(O)/%.o:	%.c
	$(CC) $(CFLAGS) -c $< -o $@
