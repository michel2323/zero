

CFLAGS=-O3 -fopenmp

zero: zero.o zero_compression.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	-rm *.o zero
