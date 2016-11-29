//
//  gdb.cpp
//  new-isa-cache-sim
//
//  Created by Nancy Fan on 11/28/16.
//  Copyright © 2016 Nancy Fan. All rights reserved.
//

#include "gdb.hpp"
#include "system.h"
#include "memory.hpp"
#include "register.hpp"
#include <string.h>

bool IS_DYCOUNT = false;
bool GDB_MODE =false;
bool verbose = false;
bool IS_TO_EXIT = false;
bool IS_TO_DO = false;

int GDB_TYPE;
bool VALID_BREAKPOINT=false;
memAddress breakpoint = 0;
bool IS_ENTER_STEP = false;
bool IS_FIRST_GDB = false;
bool IS_NOP = false;
bool WAIT = false;

extern memAddress main_virtual_address;


//gdb mode execute function
bool gdb_mode_func()
{
    char cmd[20];
    memAddress break_addr=0;
    memAddress debug_mem=0;
    reg32 mem_content=0;
    if(IS_FIRST_GDB)
    {
        printf(">\n");
        printf("> main function start at %lx\n",main_virtual_address);
        printf("> select a mode you want to run with:\n");
        printf("> break: set breakpoint\n");
        printf("> delete: delete breakpoint\n");
        printf("> continue: continue running\n");
        printf("> step: step mode\n");
        printf("> memory: print memory content\n");
        printf("> register: print register file info\n");
        printf("> quit: quit gdb mode\n>\n>\n");
    }
    
    printf("> ");
    scanf("%s",cmd);        //read command
    fflush(stdin);          //clean stdin buffer
    char cmd_char = 'a';
    if (strcmp(cmd,"break")==0)
        cmd_char = 'b';
    else if(strcmp(cmd,"delete")==0)
        cmd_char = 'd';
    else if(strcmp(cmd,"continue")==0)
        cmd_char = 'c';
    else if(strcmp(cmd,"step")==0)
        cmd_char = 's';
    else if(strcmp(cmd,"memory")==0)
        cmd_char = 'm';
    else if(strcmp(cmd,"register")==0)
        cmd_char = 'r';
    else if(strcmp(cmd,"quit")==0)
        cmd_char = 'q';
    
    switch (cmd_char)
    {
        case 'b':
            GDB_TYPE = set_breakpoint;
            printf("> set breakpoint address\n");
            printf("> ");
            scanf("%lx",&break_addr);   //set breakpoint addresss
            VALID_BREAKPOINT=true;
            breakpoint=break_addr;
            return true;
        case 'd':
            GDB_TYPE = delete_breakpoint;
            printf("> delete breakpoint(%lx)\n", breakpoint);
            VALID_BREAKPOINT = false;
            WAIT = true;
            IS_NOP = true;
            return true;
        case 'c':
            GDB_TYPE = continue_run;
            printf("> continue running...\n");
            return true;
        case 's':
            if(GDB_TYPE != step)
                IS_ENTER_STEP = true;
            if(IS_ENTER_STEP)
            {
                printf(">\n");
                printf("> --------------------step mode--------------------\n");
                IS_ENTER_STEP = false;
            }
            GDB_TYPE = step;
            VALID_BREAKPOINT=true;
            breakpoint=sim_regs.getPC();
            return true;
        case 'm':
            GDB_TYPE = print_mem;
            IS_NOP = true;
            WAIT = true;
            printf("> set memory address\n");
            printf("> ");
            scanf("%lx",&debug_mem);
            for(int row=0;row<4;++row){
                printf("> ");
                for(int col=0;col<4;++col){
                    mem_content=sim_mem.get_memory_32(debug_mem);
                    printf("0x%08x",mem_content);
                    printf("    ");
                    debug_mem+=4;
                }
                printf("\n");
            }
            return true;
        case 'r':
            GDB_TYPE = print_reg;
            IS_NOP = true;
            WAIT = true;
            sim_regs.readReg();
            sim_regs.readFloatReg();
            return true;
        case 'q':
            GDB_MODE=false;
            GDB_TYPE = quit_gdb;
            verbose = false;
            return false;
        default:
            printf("> invalid command\n");
            //GDB_MODE=false;
            GDB_TYPE = undefined_gdb;
            IS_NOP = true;
            verbose = false;
            return true;
    }
    
}
