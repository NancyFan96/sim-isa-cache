CC = g++
PROC = sim-cache

SRC=	sim.cpp memory.cpp register.cpp decode.cpp execute.cpp execute_F.cpp cache.cc 
HDR=	system.h sim.hpp memory.hpp register.hpp decode.hpp cache.h storage.h 


all: 	$(PROC)

sim-cache: 	$(SRC) $(HDR)
	$(CC) -o $(PROC) *.cpp *.cc -Wall


clean:
	-rm $(PROC) 
