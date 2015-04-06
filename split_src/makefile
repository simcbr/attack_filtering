SOURCES = transfile.c recordMath.c split.c
OBJECTS = $(SOURCES:.c=.o)
#OBJECTS = $(addsuffix .o, $(basename $(SOURCES)))

CC = gcc

CFLAGS = -g

PROG=split

$(PROG): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

$(OBJECTS): $(SOURCES)
	$(CC) $(CFLAGS) -c $(SOURCES)

depend:
	makedepend -Y $(SOURCES)

clean:
	rm -f *.o 

