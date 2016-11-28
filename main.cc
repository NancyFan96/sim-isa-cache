#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdio.h"
#include "def.h"
#include "cache.h"
#include "memory.h"


#define MAXLEVEL 3

void check(bool tmp, const char* message)
{
    if(!tmp)
    {
        printf("%s\n", message);
        printf("program exits with error\n");
        exit(0);
    }
}

// check if it's the power of 2
bool is2power(int tmp)
{
    if(tmp == 0)
        return false;
    while((tmp & 1) == 0)
        tmp = (unsigned int)tmp >> 1;
    return ((tmp >> 1) == 0);
}
void Initial(StorageStats& storage_stats, StorageLatency& latency_m, StorageLatency* latency_c, CacheConfig* cache_config, int& levelNum)
{
    storage_stats.access_counter = 0;
    storage_stats.miss_num = 0;
    storage_stats.access_time = 0;
    storage_stats.replace_num = 0;
    storage_stats.fetch_num = 0;
    storage_stats.prefetch_num = 0;
    
    latency_m.bus_latency = 6;
    latency_m.hit_latency = 100;
    
    for(int i = 1; i <= MAXLEVEL; i++)
    {
        latency_c[i].bus_latency = 3;
        latency_c[i].hit_latency = 10;
    }
    
    for(int i = 1; i <= MAXLEVEL; i++)
    {
        cache_config[i].size = 32;
        cache_config[i].associativity = 8;
        cache_config[i].write_through = 0;
        cache_config[i].write_allocate = 0;
        cache_config[i].block_size = 64;
        cache_config[i].set_num = 64;
    }
    
    levelNum = 1;
}

void GetSettings(int& argc, char *argv[], int& levelNum, CacheConfig* cache_config, StorageLatency* latency_c, FILE*& input)
{
    int j = 0,argCount = 0;
    for(argc--, argv++; argc > 0 ; argc -= argCount, argv += argCount)
    {
        argCount = 1;
        if(!strcmp(*argv, "-f")) // set cache level num
        {
            check(argc > 1, "no input file following -f");
            input = fopen(*(argv + 1), "r");
            check(input != NULL, "invalid input file");
            argCount = 2;
        }
        if(!strcmp(*argv, "-l")) // set cache level num
        {
            check(argc > 1, "no level num following -l");
            levelNum = atoi(*(argv + 1));
            check(levelNum <= MAXLEVEL && levelNum > 0, "invalid cache level num");
            argCount = 2;
        }
        else if(!strcmp(*argv, "-s")) // set cache size
        {
            check(argc > levelNum, "no enough size num following -s");
            for(int i = 1; i <= levelNum; i++)
            {
                cache_config[i].size = atoi(*(argv + i));
                check(is2power(cache_config[i].size), "invalid cache size");
            }
            argCount = levelNum + 1;
        }
        else if(!strcmp(*argv, "-a")) // set cache associativity
        {
            check(argc > levelNum, "no enough associativity num following -s");
            for(int i = 1; i <= levelNum; i++)
            {
                cache_config[i].associativity = atoi(*(argv + i));
                check(is2power(cache_config[i].associativity), "invalid associativity");
            }
            argCount = levelNum + 1;
        }
        else if(!strcmp(*argv, "-p")) // set cache policy
        {
            check(argc > levelNum, "no enough policy num following -p");
            for(int i = 1; i <= levelNum; i++)
            {
                j = atoi(*(argv + i));
                check(j < 4 && j >= 0, "invalid policy");
                cache_config[i].write_through = j & 1;  // least significant bit represents whether to write through
                cache_config[i].write_allocate = (j & 2) >> 1; // second least significant bit represents whether to write allocate
            }
            argCount = levelNum + 1;
        }
        else if(!strcmp(*argv, "-b")) // set block size
        {
            check(argc > levelNum, "no enough block size num following -p");
            for(int i = 1; i <= levelNum; i++)
            {
                cache_config[i].block_size = atoi(*(argv + i));
                cache_config[i].set_num = (double)cache_config[i].size*1024/(cache_config[i].block_size*cache_config[i].associativity);
                check(is2power(cache_config[i].block_size), "invalid block size");
            }
            argCount = levelNum + 1;
        }
        else if(!strcmp(*argv, "-h")) // set hit latency
        {
            check(argc > levelNum, "no enough hit latency num following -p");
            for(int i = 1; i <= levelNum; i++)
            {
                latency_c[i].hit_latency = atoi(*(argv + i));
                check(latency_c[i].hit_latency > 0, "invalid cache hit latency");
            }
            argCount = levelNum + 1;
        }
    }
    check(input != NULL, "input file not provided");
    for(int i = 1; i <= levelNum; i++)
    {
        check(cache_config[i].block_size * cache_config[i].associativity *  cache_config[i].set_num== cache_config[i].size * 1024,
              "invalid cache setting, as cache size can't match the associativity and block size");
    }
}

void SetSettings(Memory& m, Cache* l, StorageStats& storage_stats, CacheConfig* cache_config, StorageLatency& latency_m, StorageLatency* latency_c, int levelNum)
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

void help(){
    printf("\nThis is a simulator for cache!\n");
    printf("    Usage: ./sim -f [filename]\n");
    printf("                 -l [cache level] -s [cache size] -a [way num] -b [block size] \n");
    printf("                 -p [policy1...l:0|1|2|3:(write_back&write_non_allocate)|(through&non_allc)|(back&alloc)|(through&alloc)]\n");
    printf("                 -h [l1 hitlatency] [l2 hitlatency]...\n");
    printf("                 --help\n\n");
}

int main(int argc,char *argv[])
{
    if(argc <= 2|| strcmp(argv[1],"--help") == 0){
        help();
    }
    
    int levelNum = 0;
    CacheConfig cache_config[MAXLEVEL+1];
    StorageStats storage_stats;
    StorageLatency latency_m, latency_c[MAXLEVEL + 1];
    Initial(storage_stats, latency_m, latency_c, cache_config, levelNum);
    
    FILE* input = NULL;
    GetSettings(argc, argv, levelNum, cache_config, latency_c, input);
    
    Memory m;
    Cache l[MAXLEVEL + 1];
    SetSettings(m, l, storage_stats, cache_config, latency_m, latency_c, levelNum);
    
    int hit, time;
    char content[64];
    char ch_wORr;
    uint64_t addr;
    while(fscanf(input, "%c\t%llu\n", &ch_wORr, &addr) != EOF)
    {
        printf("\n\b%c %llu\t\t", ch_wORr, addr);
        int bl_wORr = (ch_wORr == 'w' ? 0 : 1);
        printf("r/w-%d, addr-%llu", bl_wORr, addr);
        l[1].HandleRequest(addr, 1, bl_wORr, content, hit, time);
        printf("Request access time: %dns\n", time);
    }
    StorageStats s;
    for(int i = 1; i <= levelNum; i++){
        l[i].GetStats(s);
        printf("\n\nTotal L1 access time: %dns\n", s.access_time);
        printf("Total L1 access counter: %d\n", s.access_counter);
        printf("Total L1 miss num: %d\n", s.miss_num);
        printf("Total L1 hit num: %d\n", s.access_counter - s.miss_num);
        printf("L1 miss rate: %lf\n", (double)s.miss_num/s.access_counter);
        printf("Total L1 access time: %dns\n\n", s.access_time);
    }
    m.GetStats(s);
    printf("Total Memory access time: %dns\n", s.access_time);
    fclose(input);
    return 0;
}
