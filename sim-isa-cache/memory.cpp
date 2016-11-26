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


memory::memory()//initiaiize
{
    mem_zero=(byte *)malloc(sizeof(char)*MEM_SIZE);
    memset(mem_zero,0,MEM_SIZE);
    
    trace_out.open("trace.txt",std::ios::out|std::ios::trunc);    //open a new file or clean the existed file
    if (!trace_out.is_open())
        printf("cannot open trace file!\n");
    else
        printf("open a trace file!\n");
    
    
    // realize cache
    CacheConfig cache_config[MAXLEVEL+1];
    StorageStats storage_stats;
    StorageLatency latency_m, latency_c[MAXLEVEL + 1];
    int levelNum = 3;
    
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

    
    extern Memory c_mem;
    extern Cache l[MAXLEVEL + 1];
    SetSettings(c_mem, l, storage_stats, cache_config, latency_m, latency_c, levelNum);

    
    
}

void memory::SetSettings(Memory& m, Cache* l, StorageStats& storage_stats, CacheConfig* cache_config, StorageLatency& latency_m, StorageLatency* latency_c, int levelNum)
{
    for(int i = 1; i < levelNum; i++)
        l[i].SetLower(&l[i + 1]);
    l[levelNum].SetLower(&m);
    
    // set storageStatus and Configulation of memory and cache
    m.SetStats(storage_stats);
    for(int i = 1; i <= levelNum; i++)
    {
        l[i].SetStats(storage_stats);
        l[i].SetConfig(cache_config[i]);
    }
    
    // set latency of memory and cache
    m.SetLatency(latency_m);
    
    for(int i = 1; i <= levelNum; i++)
    {
        l[i].SetLatency(latency_c[i]);
    }
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
    
    //reg8* p_u8=(reg8*)v_addr;
    //*p_u8=value;
    
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
    
    //reg16* p_u16=(reg16*)v_addr;
    //*p_u16=value;
    
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
    
    //reg32* p_u32=(reg32*)v_addr;
    //*p_u32=value;
    
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
    
    
    //reg64* p_u64=(reg64*)v_addr;
    //*p_u64=value;
    
    
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
    
    
    //reg64* p_u64=(reg64*)v_addr;
    
    char content[8];
    int hit,time;
    l[1].HandleRequest(addr, 8, READ, content, hit, time);  //read 8 bytes from addr, result stored in content pointer
    
    return *((reg64*)content);
    
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
    
    
    //reg32* p_u32=(reg32*)v_addr;
    
    char content[4];
    int hit,time;
    l[1].HandleRequest(addr, 4, READ, content, hit, time);  //read 4 bytes from addr, result stored in content pointer
    
    return *((reg32*)content);
    
    
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
    
    
    //reg16* p_u16=(reg16*)v_addr;
    
    char content[2];
    int hit,time;
    l[1].HandleRequest(addr, 2, READ, content, hit, time);  //read 4 bytes from addr, result stored in content pointer
    
    return *((reg16*)content);
    
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
    
    //reg8* p_u8=(reg8*)v_addr;
    
    char content[1];
    int hit,time;
    l[1].HandleRequest(addr, 1, READ, content, hit, time);  //read 4 bytes from addr, result stored in content pointer
    
    return *((reg8*)content);
    
    
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
    
    return *((f32*)content);
    
    //float* p_f32 = (float*)v_addr;
    //return *p_f32;
    
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
    return *((f64*)content);
    
    //double* p_f64 = (double*)v_addr;
    //return *p_f64;
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
    
    //float* fp = (float*)v_addr;
    //*fp = value;
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
    
    //double* fp = (double*)v_addr;
    //*fp = value;
    return true;
}
