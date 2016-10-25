

CC=mpicc -cc=icc

CFLAGS=-O3 -qopenmp -xMIC-AVX512

zero: zero.o zero_compression.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	-rm *.o zero
