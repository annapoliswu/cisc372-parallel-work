CC = cc -pedantic -std=c11
MPICC = mpicc -pedantic -std=c11
HOST := $(shell hostname)
ifeq ($(HOST),cisc372)
RUN = srun -n 1
MPIRUN = srun
else
RUN = 
MPIRUN = mpiexec
endif

pascal: pascal.exe
	$(RUN) ./pascal.exe

pascal_mpi: pascal_mpi.exe
	$(MPIRUN) -n 4 ./pascal_mpi.exe

pascal.exe: pascal.c
	$(CC) -o pascal.exe pascal.c

pascal_mpi.exe: pascal_mpi.c
	$(MPICC) -o pascal_mpi.exe pascal_mpi.c

clean:
	rm -f *~ *.exe a.out
