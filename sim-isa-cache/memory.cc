#include "def.h"
#include "memory.h"
#include "system.h"

void Memory::HandleRequest(uint64_t addr, int bytes, int read,
                          char *content, int &hit, int &time) {
    // printf("get into memory!\n");
    //printf("addr = 0x%llx(%lld), bytes = %d, read = %d\n", addr, addr, bytes, read);
    // read
    if(read == READ){
        for(int i = 0; i < bytes; i++)
            content[i] = cmem_zero[addr+i];
     }
    // write
    else {
        for(int i = 0; i < bytes; i++)
          cmem_zero[addr+i] = content[i];
    }
    hit = 1;
    time = latency_.hit_latency + latency_.bus_latency;
    stats_.access_time += time;
    stats_.access_counter++;

}


Memory::Memory(){
    cmem_zero = new char[((unsigned long)1<<45)-1];
}
