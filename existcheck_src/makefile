SOURCES = transfile.c recordMath.c existcheck.c
OBJECTS = $(SOURCES:.c=.o)
#OBJECTS = $(addsuffix .o, $(basename $(SOURCES)))

CC = gcc

CFLAGS = -g

PROG=existcheck

$(PROG): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

$(OBJECTS): $(SOURCES)
	$(CC) $(CFLAGS) -c $(SOURCES)

depend:
	makedepend -Y $(SOURCES)

clean:
	rm -f *.o 

