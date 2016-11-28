#include "def.h"
#include "memory.h"

Memory c_mem;

void Memory::HandleRequest(uint64_t addr, int bytes, int rw, char *content, int &hit, int &time) {
    //printf("get into memory!\n");
    //printf("addr = 0x%llx(%lld), bytes = %d, read = %d\n", addr, addr, bytes, read);

    hit = 1;
    time = latency_.hit_latency + latency_.bus_latency;
    stats_.access_time += time;
    stats_.access_counter++;

    if(rw == READ){
        memcpy(content, cmem_zero + addr, bytes);
    }else {
        memcpy(cmem_zero + addr, content, bytes);
    }
}


Memory::Memory(){
    cmem_zero = new byte[((unsigned long)1<<45)-1];
}
