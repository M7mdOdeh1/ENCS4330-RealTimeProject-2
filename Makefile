CC = gcc
CFLAGS = -g -Wall
UILIBS = -lglut -lGLU -lGL -lm  -lrt
LIBS = -lpthread

ARGS= arguments.txt
PRODUCTS= products.txt 

all:  project2

#customer: customer.c
#	$(CC) $(CFLAGS) $< -o $@

#cashier: cashier.c
#	$(CC) $(CFLAGS) $< -o $@


project2: project2.c utilities.c
	$(CC) $(CFLAGS) -o project2 project2.c utilities.c $(LIBS)



#gui: gui.c
#	$(CC) $(CFLAGS) $< -o $@ $(LIBS)
	
run: project2
	./project2 $(ARGS) $(PRODUCTS)

clean:
	rm -f project2
