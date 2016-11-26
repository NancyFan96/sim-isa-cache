#ifndef CACHE_MEMORY_H_
#define CACHE_MEMORY_H_

#include <stdint.h>
#include "def.h"
#include "storage.h"


class Memory: public Storage {
 public:
    Memory();
    ~Memory(){};

    // Main access process
    void HandleRequest(uint64_t addr, int bytes, int read,
                     char *content, int &hit, int &time);
    char * get_cmem_base(){return cmem_zero;}

 private:
  // Memory implement
    char * cmem_zero;

  DISALLOW_COPY_AND_ASSIGN(Memory);
};


#endif //CACHE_MEMORY_H_ 
