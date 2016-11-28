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
#include "def.h"



memory sim_mem;
extern Memory c_mem;


memory::memory()//initiaiize
{
    mem_zero=(byte *)malloc(sizeof(char)*MEM_SIZE);
    memset(mem_zero,0,MEM_SIZE);
    
    trace_out.open("trace.txt",std::ios::out|std::ios::trunc);    //open a new file or clean the existed file
    if (!trace_out.is_open())
        printf("cannot open trace file!\n");
    else
        printf("open a trace file!\n");
}

byte* memory::get_memory_p_address(memAddress offset)
{
    return (mem_zero+offset);
}

memAddress memory::get_memory_offset(byte *my_mem_Paddr){
    return (memAddress)(my_mem_Paddr-mem_zero); // 64bit - 64bit
}


//cache handler: HandleRequest(uint64_t addr, int bytes, int read,char *content, int &hit, int &time)
bool  memory::set_memory_8(memAddress addr, reg8 value)
{
    byte* v_addr=get_memory_p_address(addr);
    if(v_addr==NULL)
    {
        printf("invalid memory address\n");
        return false;
    }
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"w\t"<<trace_addr<<"\n";
    
    reg8* p_u8=(reg8*)v_addr;
    *p_u8=value;
    
    reg8* content = new reg8;
    *content = value;
    int hit,time;
    l[1].HandleRequest(addr, 1, WRITE, (char*)content, hit, time);   //write one byte to addr, L1 cache
    
    return true;
}
bool  memory::set_memory_16(memAddress addr, reg16 value)
{
    byte* v_addr=get_memory_p_address(addr);
    if(v_addr==NULL)
    {
        printf("invalid memory address\n");
        return false;
    }
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"w\t"<<trace_addr<<"\n";
    
    reg16* p_u16=(reg16*)v_addr;
    *p_u16=value;
    
    reg16* content = new reg16;
    *content = value;
    int hit,time;
    l[1].HandleRequest(addr, 2, WRITE, (char*)content, hit, time);   //write 2 bytes to addr
    
    return true;
}
bool  memory::set_memory_32(memAddress addr, reg32 value)
{
    byte* v_addr=get_memory_p_address(addr);
    if(v_addr==NULL)
    {
        printf("invalid memory address\n");
        return false;
    }
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"w\t"<<trace_addr<<"\n";
    
    reg32* p_u32=(reg32*)v_addr;
    *p_u32=value;
    
    reg32* content = new reg32;
    *content = value;
    int hit,time;
    l[1].HandleRequest(addr, 4, WRITE, (char*)content, hit, time);   //write 4 bytes to addr
    
    return true;
    
}
bool  memory::set_memory_64(memAddress addr, reg64 value)
{
    byte* v_addr=get_memory_p_address(addr);
    if(v_addr==NULL)
    {
        printf("unvalid memory address\n");
        return false;
    }
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"w\t"<<trace_addr<<"\n";
    
    
    reg64* p_u64=(reg64*)v_addr;
    *p_u64=value;
    
    
    reg64* content = new reg64;
    *content = value;
    int hit,time;
    l[1].HandleRequest(addr, 8, WRITE, (char*)content, hit, time);   //write 8 byte to addr
    
    return true;
}
reg64 memory::get_memory_64(memAddress addr)
{
    byte* v_addr=get_memory_p_address(addr);
    //printf("0x%lx\n", (reg64)v_addr);
    //printf("*v_addr = 0x%x", *v_addr);
    if(v_addr==NULL)
    {
        printf("invalid memory address\n");
        return -1;
    }
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"r\t"<<trace_addr<<"\n";
    
    
    reg64* p_u64=(reg64*)v_addr;
    
    char content[8];
    int hit,time;
    l[1].HandleRequest(addr, 8, READ, content, hit, time);  //read 8 bytes from addr, result stored in content pointer
    
    return (*p_u64);
    
}
reg32 memory::get_memory_32(memAddress addr)
{
    byte* v_addr=get_memory_p_address(addr);
    if(v_addr==NULL){
        printf("invalid memory address\n");
        return -1;
    }
    
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"r\t"<<trace_addr<<"\n";
    
    
    reg32* p_u32=(reg32*)v_addr;
    
    char content[4];
    int hit,time;
    l[1].HandleRequest(addr, 4, READ, content, hit, time);  //read 4 bytes from addr, result stored in content pointer
    
    return (*p_u32);
    
    
}
reg16 memory::get_memory_16(memAddress addr)
{
    byte* v_addr=get_memory_p_address(addr);
    if(v_addr==NULL){
        printf("invalid memory address\n");
        return -1;
    }
    
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"r\t"<<trace_addr<<"\n";
    
    
    reg16* p_u16=(reg16*)v_addr;
    
    char content[2];
    int hit,time;
    l[1].HandleRequest(addr, 2, READ, content, hit, time);  //read 4 bytes from addr, result stored in content pointer
    
    return (*p_u16);
    
}
reg8  memory::get_memory_8(memAddress addr)
{
    byte* v_addr=get_memory_p_address(addr);
    if(v_addr==NULL){
        printf("invalid memory address\n");
        return -1;
    }
    
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"r\t"<<trace_addr<<"\n";
    
    reg8* p_u8=(reg8*)v_addr;
    
    char content[1];
    int hit,time;
    l[1].HandleRequest(addr, 1, READ, content, hit, time);  //read 4 bytes from addr, result stored in content pointer
    
    return (*p_u8);
    
    
}

f32   memory::get_memory_f32(memAddress addr)
{
    byte* v_addr = get_memory_p_address(addr);
    if(v_addr==NULL)
    {
        printf("invalid memory address\n");
        return -1;
    }
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"r\t"<<trace_addr<<"\n";
    
    char content[4];
    int hit,time;
    l[1].HandleRequest(addr, 4, READ, content, hit, time);  //read 4 bytes from addr, result stored in content pointer
    
    
    float* p_f32 = (float*)v_addr;
    return *p_f32;
    
}
f64  memory::get_memory_f64(memAddress addr)
{
    byte* v_addr = get_memory_p_address(addr);
    if(v_addr==NULL)
    {
        printf("invalid memory address\n");
        return -1;
    }
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"r\t"<<trace_addr<<"\n";
    
    char content[8];
    int hit,time;
    l[1].HandleRequest(addr, 8, READ, content, hit, time);  //read 4 bytes from addr, result stored in content pointer
    
    
    double* p_f64 = (double*)v_addr;
    return *p_f64;
}

bool  memory::set_memory_f32(memAddress addr,f32 value)
{
    byte* v_addr=get_memory_p_address(addr);
    if(v_addr==NULL)
    {
        printf("invalid memory address\n");
        return false;
    }
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"w\t"<<trace_addr<<"\n";
    
    f32* content = new f32;
    *content = value;
    int hit,time;
    l[1].HandleRequest(addr, 4, WRITE, (char*)content, hit, time);   //write 4 bytes to addr
    
    float* fptr = (float*)v_addr;
    *fptr = value;
    return true;
}
bool  memory::set_memory_f64(memAddress addr,f64 value)
{
    byte* v_addr=get_memory_p_address(addr);
    if(v_addr==NULL)
    {
        printf("invalid memory address\n");
        return false;
    }
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"w\t"<<trace_addr<<"\n";
    
    f64* content = new f64;
    *content = value;
    int hit,time;
    l[1].HandleRequest(addr, 8, WRITE, (char*)content, hit, time);   //write 4 bytes to addr
    
    double* fptr = (double*)v_addr;
    *fptr = value;
    return true;
}
