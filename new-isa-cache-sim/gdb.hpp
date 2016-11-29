//
//  gdb.hpp
//  new-isa-cache-sim
//
//  Created by Nancy Fan on 11/28/16.
//  Copyright © 2016 Nancy Fan. All rights reserved.
//

#ifndef gdb_hpp
#define gdb_hpp

#include <stdio.h>
#include "system.h"

#define continue_run 0
#define step 1
#define set_breakpoint 2
#define delete_breakpoint 3
#define print_mem 4
#define print_reg 5
#define quit_gdb 6
#define undefined_gdb 7


bool gdb_mode_func();

extern bool IS_DYCOUNT;
extern bool GDB_MODE;
extern bool verbose;
extern bool IS_TO_EXIT;
extern bool IS_TO_DO;

extern int GDB_TYPE;
extern bool VALID_BREAKPOINT;
extern memAddress breakpoint;
extern bool IS_ENTER_STEP;
extern bool IS_FIRST_GDB;
extern bool IS_NOP;
extern bool WAIT;



#endif /* gdb_hpp */
