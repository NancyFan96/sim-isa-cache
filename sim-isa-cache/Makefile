CC = g++
PROC = sim

SRC=	sim.cpp memory.cpp register.cpp decode.cpp execute.cpp execute_F.cpp cache.cc memory.cc
HDR=	system.h sim.hpp memory.hpp register.hpp decode.hpp cache.h memory.h storage.h def.h


all: 	$(PROC)

sim: 	$(SRC) $(HDR)
	$(CC) -o $(PROC) *.cpp *.cc -Wall


clean:
	-rm $(PROC) 
