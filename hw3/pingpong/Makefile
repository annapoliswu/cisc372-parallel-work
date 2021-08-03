CC = cc -pedantic -std=c11
MPICC = mpicc -pedantic -std=c11
LINKS = -lm

HOST := $(shell hostname)
ifeq ($(HOST),cisc372)
RUN = srun
else
RUN = mpiexec
endif

all : pingpong run

pingpong : pingpong.c
	$(MPICC) -o pingpong pingpong.c $(LINKS)

run : pingpong
	$(RUN) -n 20 ./pingpong

clean :
	rm -f pingpong *~ a.out
