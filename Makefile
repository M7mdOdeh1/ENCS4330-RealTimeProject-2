CC = gcc
CFLAGS = -g -Wall
LIBS = -lglut -lGLU -lGL -lm -lpthread -lrt

ARGS= arguments.txt
PRODUCTS= products.txt 

all:  project2

#customer: customer.c
#	$(CC) $(CFLAGS) $< -o $@

#cashier: cashier.c
#	$(CC) $(CFLAGS) $< -o $@

project1: project2.c
	$(CC) $(CFLAGS) $< -o $@

#gui: gui.c
#	$(CC) $(CFLAGS) $< -o $@ $(LIBS)
	
run: project2
	./project2 $(ARGS) $(PRODUCTS)

clean:
	rm -f project2
