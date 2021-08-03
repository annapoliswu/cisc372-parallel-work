CC = cc -pedantic -std=c11
MPICC = mpicc -pedantic -std=c11
LINKS = -lm

HOST := $(shell hostname)
ifeq ($(HOST),cisc372)
RUN = srun
else
RUN = mpiexec
endif

all : pi pi_mpi testmpi 

pi : pi.c
	$(CC) -o pi pi.c $(LINKS)

pi_mpi : pi_mpi.c
	$(MPICC) -o pi_mpi pi_mpi.c $(LINKS)

testmpi : pi_mpi
	$(RUN) -n 20 ./pi_mpi

clean :
	rm -f pi pi_mpi *~ a.out
