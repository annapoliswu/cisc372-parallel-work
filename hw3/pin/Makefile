CC = cc -pedantic -std=c11
MPICC = mpicc -pedantic -std=c11
LINKS = -lm
SMALL  =        100
MEDIUM =   10000000
LARGE = 10000000000

HOST := $(shell hostname)
ifeq ($(HOST),cisc372)
RUN = srun
else
RUN = mpiexec
endif

all : pin pin_mpi seq_small seq_medium

pin : pin.c
	$(CC) -o pin pin.c $(LINKS)

pin_mpi : pin_mpi.c
	$(MPICC) -o pin_mpi pin_mpi.c $(LINKS)

seq_small : pin
	./pin $(SMALL)

seq_medium: pin
	./pin $(MEDIUM)

# Add more rules here for running the MPI version ...

mpi_small : pin_mpi
	$(RUN) -n 4 ./pin_mpi $(SMALL)

mpi_medium4 : pin_mpi
	$(RUN) -n 4 ./pin_mpi $(MEDIUM)

mpi_medium8 : pin_mpi
	$(RUN) -n 8 ./pin_mpi $(MEDIUM)

mpi_large : pin_mpi
	$(RUN) -n 40 ./pin_mpi $(LARGE)

clean :
	rm -f pin pin_mpi *~ a.out
