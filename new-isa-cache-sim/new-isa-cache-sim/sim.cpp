//
//  sim.cpp
//  RISCV_ISA_SIM
//
//  Created by Nancy Fan, Kejing Yang,Yao Lu Wang on 16/11/5.
//  Copyright © 2016年 Nancy Fan. All rights reserved.
//

/*-------------------------------------------------
 sim.cpp contains several parts of RISCV simulator.
 0. help - print some help info to use this simulator
 1. load_program - read an ELF file and load program.
 2. fetch - fetch an instruction
 3. decode - decode an instruction
 4. execute - simulate instruction executing process
 5. exit_program - program exits, free the simulator memory space
 6. FLAG verbose - if set, print verbose info
 
 An extra GDB mode is also provided. This include
 1. gdb_mode_func - wait and process a gdb instruction
 2. Sepecial FLAGS like
    -- VALID_BREAKPOINT
    -- IS_ENTER_STEP
    -- IS_FIRST_GDB
    -- IS_NOP
    -- WAIT
 
 An extra count function is also provided. This include
 1. print_ins_cnt_init - set clear
 2. print_ins_cnt - print result table
 This function count every single instruction that has been executed
 and their ratio of the entire program
 
 --------------------------------------------------*/

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <cstring>
#include <string.h>
#include <map>
#include "system.h"
#include "register.hpp"
#include "memory.hpp"
#include "decode.hpp"
#include "sim.hpp"
#include "gdb.hpp"

#ifdef USE_COUNT
#include "count.hpp"
#endif

using namespace std;



memAddress currentPC;
memAddress main_virtual_address = 0;

#ifdef USE_COUNT
extern std::map<std::string,int> COUNTS;
extern long int COUNT_INS;
#endif

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

extern Memory c_mem;
extern Cache l[MAXLEVEL + 1];


void help()
{
    printf("This is a simulator to execute riscv ELF!\n");
    printf("    Usage: ./sim <filename> [--verbose|--debug]\n");
    printf("           ./sim --help\n");
    printf("Multiple ELFs is NOT supported!\n\n");
    
}

bool load_program(char const *file_name)
{
    /*-----------read ELF file-----------*/
    FILE * file_in=NULL;
    unsigned long file_size;
    
    file_in=fopen(file_name,"rb");
    if (file_in==NULL) {
        if(GDB_MODE)    printf("> ");
        printf("cannot open the file : %s \n",file_name);
        return false;
    }
    fseek(file_in,0,SEEK_END);                                              //set file pointer to the end of file
    file_size=ftell(file_in);                                               //get file size
    fseek(file_in,0,SEEK_SET);                                              //set file pointer to the begining of file
    
    unsigned char* file_buffer;
    file_buffer=(unsigned char*)malloc(sizeof(unsigned char)*file_size);    //read file into file_buffer
    if(fread(file_buffer,sizeof(char),file_size,file_in)<file_size)
    {
        if(GDB_MODE)    printf("> ");
        printf("Fail to read the entire file!\n");
        return false;
        
    }
    else
    {
        if(GDB_MODE)    printf("> ");
        printf("Read file successfully!\n");
    }
    
    /*copy segments in ELF file to simulator memory*/
    
    Elf64_Ehdr* elf_header;
    elf_header=(Elf64_Ehdr*)file_buffer;
    //locate to first section
    Elf64_Shdr* sec_header=(Elf64_Shdr*)((unsigned char*)elf_header+elf_header->e_shoff);                       
    Elf64_Half sec_header_entry_size=elf_header->e_shentsize;
    
    //locate to section .text, runtime virtual address
    sec_header=(Elf64_Shdr*)((unsigned char*)sec_header+sec_header_entry_size);                     
    memAddress program_entry_offset=(memAddress)(sec_header->sh_addr);                              
#ifdef CACHE
    byte*  cur_p_C_mem=sim_mem.get_memory_p_address(program_entry_offset);
#endif
#ifndef CACHE
    byte*  cur_p_mem=sim_mem.get_memory_p_address(program_entry_offset);
#endif
    Elf64_Half seg_num=elf_header->e_phnum;                                                     


    //locate to the first entry of program header table    
    Elf64_Phdr* seg_header = (Elf64_Phdr*)((unsigned char*)elf_header + elf_header->e_phoff);
    //Program header table entry size                   
    Elf64_Half  seg_header_entry_size=elf_header->e_phentsize;          
    
    for(int cnt=0;cnt<seg_num;++cnt)
    {
        unsigned char* seg_in_file=(unsigned char*)elf_header+seg_header->p_offset;
        Elf64_Xword seg_size_in_mem=seg_header->p_memsz;                                           //segment size
    
#ifdef CACHE
        memcpy(cur_p_C_mem,seg_in_file,seg_size_in_mem);
        cur_p_C_mem =(byte*)cur_p_C_mem + seg_size_in_mem;
#endif
#ifndef CACHE
        memcpy(cur_p_mem,seg_in_file,seg_size_in_mem);                                            //copy segment to sim_mem
        cur_p_mem =(byte*)cur_p_mem + seg_size_in_mem;
#endif
        seg_header=(Elf64_Phdr*)((unsigned char*)seg_header+seg_header_entry_size);
    }
#ifdef DEBUG
    char * begin = c_mem.get_cmem_base() + 0x10000;
    reg32 mem_content = 0;
    for (int i = 0; i < 16 ; i++) {
        mem_content = *((reg32*)begin);
        printf("%x ",mem_content);
        begin+=4;
    }
#endif
    /*------------- end of segments copy----------------*/
    
    Elf64_Half  sec_num=elf_header->e_shnum;                                                      
    sec_header=(Elf64_Shdr*)((unsigned char*)elf_header+elf_header->e_shoff);                     //locate to first section
    sec_header_entry_size=elf_header->e_shentsize;
    //locate to section header of string table
    Elf64_Shdr* strtab_sec_header=(Elf64_Shdr*)((unsigned char*)sec_header+sec_header_entry_size*(sec_num-1)); 
    //read string table offset offset 
    Elf64_Off strtab_offset=strtab_sec_header->sh_offset;   
    //point to the beginning of string table                                                    
    unsigned char* p_strtab=(unsigned char*)elf_header+strtab_offset;                                           
    
    
    for(int cnt=0;cnt<sec_num;++cnt)                                                                            
    {
        Elf64_Word section_type=sec_header->sh_type;
        if(section_type==2)                                                                                     
        {// if section type==SYMTAB
            Elf64_Off symtab_offset=sec_header->sh_offset;
            Elf64_Xword   sh_size=sec_header->sh_size;
            //number of entries in symbol table, locate to the first entry
            long int symtab_num=sh_size/sizeof(Elf64_Sym);                                                      
            Elf64_Sym* p_symtab=(Elf64_Sym*)((unsigned char*)elf_header+symtab_offset);                         
            for(int entry_cnt=0;entry_cnt<symtab_num;++entry_cnt)
            {
                Elf64_Word    st_name=p_symtab->st_name;
                Elf64_Addr    st_value=p_symtab->st_value;
                if(st_name!=0)
                {
                    unsigned char* p_name=p_strtab+st_name;
                    char* test_name=(char*)malloc(sizeof(char)*20);
                    int name_cnt=0;
                    while (*p_name!='\0'&&name_cnt<20)
                    {
                        test_name[name_cnt]=*p_name;
                        p_name++;
                        name_cnt++;
                    }
                    test_name[name_cnt]='\0';
                    if(strcmp(test_name,"main")==0)
                    {
                        main_virtual_address=(memAddress)st_value;
                        break;
                    }
                    
                }//end of if
                p_symtab++;
            }//end of for
            break;
        }//end of if
        sec_header=(Elf64_Shdr*)((unsigned char*)sec_header+sec_header_entry_size);//next section
    }
    

    fclose(file_in);                                //close ELF file
    free(file_buffer);                              //free buffer
    
    /* ---- init regs ------*/
    if((byte *)main_virtual_address==NULL)
        printf("main virtual address invalid!\n");
    printf("main_virtual_address = 0x%lx\n", (reg64)main_virtual_address);
    sim_regs.setPC(program_entry_offset);          //start from program entry
    sim_regs.writeReg(rzero, 0);
    sim_regs.writeReg(sp, STACK_TOP);

    return true;
}

void exit_program(){
    free(sim_mem.mem_zero);
}

ins fetch(){
    ins inst;
    if(IS_NOP){
        inst = sim_mem.get_memory_32(currentPC);
    }
    else{
        currentPC = sim_regs.getPC();
        inst = sim_mem.get_memory_32(currentPC);
        sim_regs.incPC();
    }
    return inst;
}




void initial_cache_mem(StorageStats& storage_stats, StorageLatency& latency_m, StorageLatency* latency_c, CacheConfig* cache_config, int& levelNum){
    // Level	Capacity	Associativity	Line Size(Bytes)	Write Update Policy Access time Random cycle time
    // L1	32 KB(32768)	8 ways	64	WriteBack, 1.9ns-4cycle, 0.79ns-2cycle
    // L2	256 KB(262144)	8 ways	64	WriteBack, 2.13ns-5cycle, 0.83ns-2cycle
    // LLC	8 MB(8192kB=8388608)	8 ways	64	WriteBack, 5.3ns-11cycle, 1.86ns-4cycle
    
    storage_stats.access_counter = 0;
    storage_stats.miss_num = 0;
    storage_stats.access_time = 0;
    storage_stats.replace_num = 0;
    storage_stats.fetch_num = 0;
    storage_stats.prefetch_num = 0;
    
    latency_m.bus_latency = 6;
    latency_m.hit_latency = 100;

    
    cache_config[1].size = 32;
    cache_config[1].associativity = 8;
    cache_config[1].block_size = 64;
    cache_config[1].set_num = 64;
    cache_config[1].write_through = 0;
    cache_config[1].write_allocate = 0;
    latency_c[1].bus_latency = 2;
    latency_c[1].hit_latency = 4;
    
    cache_config[2].size = 256;
    cache_config[2].associativity = 8;
    cache_config[2].block_size = 64;
    cache_config[2].set_num = 512;
    cache_config[2].write_through = 0;
    cache_config[2].write_allocate = 0;
    latency_c[2].bus_latency = 2;
    latency_c[2].hit_latency = 5;
    
    cache_config[3].size = 8192;
    cache_config[3].associativity = 8;
    cache_config[3].block_size = 64;
    cache_config[3].set_num = 16448;
    cache_config[3].write_through = 0;
    cache_config[3].write_allocate = 0;
    latency_c[3].bus_latency = 4;
    latency_c[3].hit_latency = 11;
    
    for(int i = 1; i < levelNum; i++)
        l[i].SetLower(&l[i + 1]);
    l[levelNum].SetLower(&c_mem);
    
    // set storageStatus and Configulation of memory and cache
    c_mem.SetStats(storage_stats);
    for(int i = 1; i <= levelNum; i++)
    {
        l[i].SetStats(storage_stats);
        l[i].SetConfig(cache_config[i]);
    }
    
    // set latency of memory and cache
    c_mem.SetLatency(latency_m);
    
    for(int i = 1; i <= levelNum; i++)
    {
        l[i].SetLatency(latency_c[i]);
    }

    
}

int main(int argc, char * argv[]){
    const char * file_name;
    
    if(argc < 2 || argc > 3 || strcmp(argv[1],"--help") == 0){
        help();
        return 0;
    }
    file_name = argv[1];
    if(argc == 3) {
        if (strcmp(argv[2], "--debug") == 0){
            printf("> (gdb mode)\n>\n");
            IS_FIRST_GDB = true;
            IS_NOP = true;
            VALID_BREAKPOINT=true;
            breakpoint=currentPC;      //break point <- current PC; fetched but not executed!
            GDB_MODE=true;
            WAIT = true;
        }
        else if (strcmp(argv[2], "--verbose") == 0)
            verbose = true;
        else{
            printf("\nINVALID COMMAND!\n");
            help();
            return 0;
        }
    }
    
    if(load_program(file_name)==true){
        if(GDB_MODE)    printf("> ");
        printf("sp = %lx  PC = %lx\n", sim_regs.readReg(sp),sim_regs.getPC());
        if(GDB_MODE)    printf("> ");
        cout << "EXCUTE "<< file_name << endl;
        if(GDB_MODE)    printf("> ");
        printf("\n");
    }
    else{
        cout << "LOAD ERROR!" << endl;
        return -1;
    }

#ifdef USE_COUNT
    print_ins_cnt_init();
#endif
    IS_DYCOUNT = false;

    // realize cache
    CacheConfig cache_config[MAXLEVEL+1];
    StorageStats storage_stats;
    StorageLatency latency_m, latency_c[MAXLEVEL + 1];
    int levelNum = 3;
    initial_cache_mem(storage_stats, latency_m, latency_c, cache_config, levelNum);    

    
    while(1){
        ins inst = fetch();
        instruction fetched_inst;
        
        if(GDB_MODE&&VALID_BREAKPOINT&&breakpoint == currentPC){
            verbose = true;
            WAIT = true;
            
        }
        
        if(!IS_NOP){
            if(verbose){
                printf("> -------------AFTER LASTST INSTRUCTION(VERBOSE)--------------------\n");
                printf("> currentPC = %lx\n", currentPC);
                printf("> instruction = %x\n", inst);
            }
            if(fetched_inst.decode(inst) == true){
                if(currentPC == main_virtual_address){
                    IS_DYCOUNT = true;
                    printf("currentPC = 0x%lx\n", currentPC);
                }
                fetched_inst.execute();
                //printf("sp = 0x%lx, fp = 0x%lx------------------\n", sim_regs.readReg(sp), sim_regs.readReg(s0));
                if(IS_DYCOUNT) {COUNT_INS++;}
                //else printf("no count\n");
                if(IS_TO_EXIT){
                    IS_DYCOUNT = false;
                    break;
                }  
            }else{
                cout << "DECODE ERROR!" << endl;
                printf("> currentPC = %lx\n", currentPC);
                printf("> instruction = %x\n", inst);
                printf("current register(current instruction not run yet)...\n");
                sim_regs.readReg();
                sim_regs.readFloatReg();
                return -1;
            }
        }
        else
            IS_NOP = false;

        /*-----------gdb mode------------*/
        if(GDB_MODE && WAIT){
            if(gdb_mode_func()==false){
                printf("> quit gdb mode\n\n");
                return -1;
            }//enter gdb mode function
            
            if(IS_FIRST_GDB)    IS_FIRST_GDB = false;
            
            if(GDB_TYPE!=print_mem && GDB_TYPE != print_reg && GDB_TYPE != delete_breakpoint){
                verbose = false;
                WAIT = false;
            }
        }

    }
    
    exit_program();

#ifdef USE_COUNT
    //print COUNTS
    //print_ins_cnt();
#endif
    cout << "\nBYE "<< file_name<< " !"<< endl<< endl;
    
    StorageStats s;
    for(int i = 1; i <= levelNum; i++){
        l[i].GetStats(s);
        printf("\n\n---------------L%d---------------\n", i);
        printf("Total L%d access counter: %d   <<---\n", i, s.access_counter);
        printf("Total L%d miss num: %d\n", i, s.miss_num);
        printf("Total L%d hit num: %d\n", i, s.access_counter - s.miss_num);
        printf("L%d miss rate: %lf           ##\n", i, (double)s.miss_num/s.access_counter);
        printf("Total L%d replace num: %d\n", i, s.replace_num);
        printf("Total L%d fetch num: %d        --->>\n", i, s.fetch_num);
        printf("Total L%d prefetch num: %d\n", i, s.prefetch_num);
        printf("Total L%d access time: %d cycle\n", i, s.access_time);
    }
    c_mem.GetStats(s);
    printf("\n\n---------------M---------------\n");
    printf("Total M access counter: %d\n", s.access_counter);
    printf("Total Memory access time: %d cycle\n\n\n", s.access_time);

    
    return 0;
}
