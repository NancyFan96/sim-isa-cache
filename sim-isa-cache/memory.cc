#include "def.h"
#include "memory.h"

void Memory::HandleRequest(uint64_t addr, int bytes, int read,
                          char *content, int &hit, int &time) {
    printf("get into memory!\n");
    //printf("addr = 0x%llx(%lld), bytes = %d, read = %d\n", addr, addr, bytes, read);
    // read
    if(read == TRUE){
        //for(int i = 0; i < bytes; i++)
          //  content[i] = this->mem_zero[addr+i];
        hit = 1;
        time = latency_.hit_latency + latency_.bus_latency;
        stats_.access_time += time;
     }
    // write
    else {
        //for(int i = 0; i < bytes; i++)
        //    this->mem_zero[addr+i] = content[i];
        hit = 1;
        time = latency_.hit_latency + latency_.bus_latency;
        stats_.access_time += time;
 
    }
}


Memory::Memory(){
    mem_zero = new byte[1];
}
