CC = g++
PROC = sim-cache

OBJECTS = sim.o gdb.o count.o \
			memory.o cache.o register.o \
			decode.o execute.o execute_F.o 

MAIN = sim.o gdb.o count.o
GDB = gdb.o
COUNT = count.o
MEMORY = memory.o cache.o 
CACHE = cache.o
REGISTER = register.o
EXECUTE = decode.o execute.o execute_F.o


sim-cache: 	$(OBJECTS)
	$(CC) -o $(PROC) $(OBJECTS) -Wall

$(OBJECTS): system.h

$(CACHE): storage.h cache.h 

$(MEMORY) $(EXECUTE) $(MAIN): memory.hpp 

$(REGISTER) $(EXECUTE) $(MAIN): register.hpp

$(EXECUTE) $(MAIN): decode.hpp 

$(GDB) $(MAIN): gdb.hpp

$(COUNT) $(MAIN): count.hpp

$(MAIN) $(MAIN): sim.hpp	

.PHONY : clean
clean:
	-rm $(PROC) *.o
