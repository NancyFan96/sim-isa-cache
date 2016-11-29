//
//  memory.cpp
//  RISCV_ISA_SIM
//
//  Read and write functions for Memory
//
//  Created by Nancy Fan, Kejing Yang,Yao Lu Wang  on 16/11/4.
//  Copyright © 2016年 Nancy Fan. All rights reserved.
//

#include <string.h>
#include "system.h"
#include "memory.hpp"
#include "memory.h"
#include "cache.h"



extern Memory c_mem;
extern Cache l[MAXLEVEL + 1];

memory sim_mem;


memory::memory()//initiaiize
{
#ifndef CACHE
    mem_zero=(byte *)malloc(sizeof(char)*MEM_SIZE);
    memset(mem_zero,0,MEM_SIZE);
#endif
    
#ifdef TRACE
    trace_out.open("trace.txt",std::ios::out|std::ios::trunc);    //open a new file or clean the existed file
    if (!trace_out.is_open())
        printf("cannot open trace file!\n");
    else
        printf("open a trace file!\n");
#endif
}

byte* memory::get_memory_p_address(memAddress offset)
{
#ifdef CACHE
    return ((byte*)c_mem.get_cmem_base()+offset);
#endif

#ifndef CACHE
    return (sim_mem.mem_zero+offset);//!!
#endif
}


//cache handler: HandleRequest(uint64_t addr, int bytes, int read,char *content, int &hit, int &time)
bool  memory::set_memory_8(memAddress addr, reg8 value)
{
#ifdef TRACE
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"w\t"<<trace_addr<<"\n";
#endif
   
    reg8* content = new reg8;
    *content = value;
    int hit = 0,time = 0;
    l[1].HandleRequest(addr, 1, WRITE, (char*)content, hit, time);   //write one byte to addr, L1 cache

#ifndef CACHE
    byte* v_addr=get_memory_p_address(addr);
    *(reg8*)v_addr=value;
#endif
    
    return true;
}

bool  memory::set_memory_16(memAddress addr, reg16 value)
{
#ifdef TRACE
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"w\t"<<trace_addr<<"\n";
#endif

    reg16* content = new reg16;
    *content = value;
    int hit = 0,time = 0;
    l[1].HandleRequest(addr, 2, WRITE, (char*)content, hit, time);   //write 2 bytes to addr
    
#ifndef CACHE
    byte* v_addr=get_memory_p_address(addr);
    *(reg16*)v_addr=value;
#endif
    
    return true;
}

bool  memory::set_memory_32(memAddress addr, reg32 value)
{
#ifdef TRACE
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"w\t"<<trace_addr<<"\n";
#endif

    reg32* content = new reg32;
    *content = value;
    int hit = 0,time = 0;
    l[1].HandleRequest(addr, 4, WRITE, (char*)content, hit, time);   //write 4 bytes to addr
    
#ifndef CACHE
    byte* v_addr=get_memory_p_address(addr);
    *(reg32*)v_addr=value;
#endif
    
    return true;
}

bool  memory::set_memory_64(memAddress addr, reg64 value)
{
#ifdef TRACE
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"w\t"<<trace_addr<<"\n";
#endif

    reg64* content = new reg64;
    *content = value;
    int hit = 0,time = 0;
    l[1].HandleRequest(addr, 8, WRITE, (char*)content, hit, time);   //write 8 byte to addr
    
#ifndef CACHE
    byte* v_addr=get_memory_p_address(addr);
    *(reg64*)v_addr = value;
#endif
    
    return true;
}

reg64 memory::get_memory_64(memAddress addr)
{
#ifdef TRACE
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"w\t"<<trace_addr<<"\n";
#endif

    reg64 value;
    
    char content[8];
    int hit,time;
    l[1].HandleRequest(addr, 8, READ, content, hit, time);  //read 8 bytes from addr, result stored in content pointer
    value = *(reg64 *)content;
    
#ifndef CACHE
    byte* v_addr=get_memory_p_address(addr);
    value = *(reg64*)v_addr;
#endif
    
    return value;
}

reg32 memory::get_memory_32(memAddress addr)
{
#ifdef TRACE
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"w\t"<<trace_addr<<"\n";
#endif

    reg32 value;
    
    char content[4];
    int hit,time;
    l[1].HandleRequest(addr, 4, READ, content, hit, time);  //read 4 bytes from addr, result stored in content pointer
    value = *(reg32 *)content;

#ifndef CACHE
    byte* v_addr=get_memory_p_address(addr);
    value = *(reg32*)v_addr;
#endif

    return value;
}

reg16 memory::get_memory_16(memAddress addr)
{
#ifdef TRACE
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"w\t"<<trace_addr<<"\n";
#endif

    reg16 value;

    char content[2];
    int hit,time;
    l[1].HandleRequest(addr, 2, READ, content, hit, time);  //read 4 bytes from addr, result stored in content pointer
    value = *(reg16 *)content;

#ifndef CACHE
    byte* v_addr=get_memory_p_address(addr);
    value = *(reg16*)v_addr;
#endif
    
    return value;
}

reg8  memory::get_memory_8(memAddress addr)
{
#ifdef TRACE
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"w\t"<<trace_addr<<"\n";
#endif
    
    reg8 value;
    
    char content[1];
    int hit,time;
    l[1].HandleRequest(addr, 1, READ, content, hit, time);  //read 4 bytes from addr, result stored in content pointer
    value = *(reg8 *)content;

#ifndef CACHE
    byte* v_addr=get_memory_p_address(addr);
    value =*(reg8*)v_addr;
#endif

    return value;
}

f32   memory::get_memory_f32(memAddress addr)
{
#ifdef TRACE
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"w\t"<<trace_addr<<"\n";
#endif

    f32 value;
    
    char content[4];
    int hit,time;
    l[1].HandleRequest(addr, 4, READ, content, hit, time);  //read 4 bytes from addr, result stored in content pointer
    value = *(f32 *)content;

#ifndef CACHE
    byte* v_addr = get_memory_p_address(addr);
    value = *(f32 *)v_addr;
#endif

    return value;
    
}
f64  memory::get_memory_f64(memAddress addr)
{
#ifdef TRACE
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"w\t"<<trace_addr<<"\n";
#endif

    f64 value;
    
    char content[8];
    int hit,time;
    l[1].HandleRequest(addr, 8, READ, content, hit, time);  //read 4 bytes from addr, result stored in content pointer
    value = *(f64 *)content;

#ifndef CACHE
    byte* v_addr = get_memory_p_address(addr);
    value = *(f64 *)v_addr;
#endif

    return value;
}

bool  memory::set_memory_f32(memAddress addr,f32 value)
{
#ifdef TRACE
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"w\t"<<trace_addr<<"\n";
#endif

    f32* content = new f32;
    *content = value;
    int hit,time;
    l[1].HandleRequest(addr, 4, WRITE, (char*)content, hit, time);   //write 4 bytes to addr

#ifndef CACHE
    byte* v_addr=get_memory_p_address(addr);
    *(f32 *)v_addr = value;
#endif
    
    return true;
}
bool  memory::set_memory_f64(memAddress addr,f64 value)
{
#ifdef TRACE
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"w\t"<<trace_addr<<"\n";
#endif
    
    f64* content = new f64;
    *content = value;
    int hit,time;
    l[1].HandleRequest(addr, 8, WRITE, (char*)content, hit, time);   //write 4 bytes to addr

#ifndef CACHE
    byte* v_addr=get_memory_p_address(addr);
    *(f64 *)v_addr = value;
#endif

    return true;
}
