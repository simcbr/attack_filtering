SOURCES = recordMath.c transfile.c transdir.c test.c prepdir.c
OBJECTS = $(SOURCES:.c=.o)
#OBJECTS = $(addsuffix .o, $(basename $(SOURCES)))

CC = gcc

CFLAGS = -g

PROG=prep

$(PROG): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

$(OBJECTS): $(SOURCES)
	$(CC) $(CFLAGS) -c $(SOURCES)

depend:
	makedepend -Y $(SOURCES)

clean:
	rm -f *.o 

