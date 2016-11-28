#include "cache.h"
#include "def.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define MAXLEVEL 3

inline int lg2(const int x)
{
    int num = 0;
    int xx = x;
    while(xx != 1)
    {
        xx = xx >> 1;
        num ++;
    }
    return num;
}

Cache l[MAXLEVEL + 1];

Cache::Cache()
{
    config_.size = 32;//KB
    config_.block_size = 64;
    config_.associativity = 8;
    config_.set_num = 64;
    config_.write_through = 0;
    config_.write_allocate = 0;
    lower_ = NULL;
    cache_ = new Block *[config_.set_num];
    for(int i = 0; i < config_.set_num; i++) {
        cache_[i] = new Block[config_.associativity];
        memset(cache_[i], 0, sizeof(Block)*config_.associativity);
        for(int j = 0; j < config_.associativity; j++){
            cache_[i][j].block_content = new char[config_.block_size];
        }
    }
}


void Cache::SetConfig(CacheConfig cc)
{
    int old_set_num = config_.set_num;
    config_ .size = cc.size;
    config_ .block_size = cc.block_size;
    config_ .associativity = cc.associativity;
    config_ .set_num = cc.set_num;
    config_ .write_through = cc.write_through;
    config_ .write_allocate = cc.write_allocate;
    
    printf("size = %d, block_size = %d, assc = %d, set_num = %d\n",
           config_ .size, config_ .block_size, config_ .associativity, config_ .set_num);
    printf("write_through(back) = %d, write_alloc(no-alloc) = %d\n", config_ .write_through, config_ .write_allocate);
    
    for(int i = 0; i < old_set_num; i++) {
        delete [] cache_[i];
    }
    delete [] cache_;
    
    cache_ = new Block *[config_.set_num];
    for(int i = 0; i < config_.set_num; i++) {
        cache_[i] = new Block[config_.associativity];
        memset(cache_[i], 0, sizeof(Block)*config_.associativity);
        for(int j = 0; j < config_.associativity; j++){
            cache_[i][j].block_content = new char[config_.block_size];
        }
    }
}

void Cache::GetConfig(CacheConfig cc)
{
    cc.size = config_.size;
    cc.block_size = config_.block_size;
    cc.associativity = config_.associativity;
    cc.set_num = config_.set_num;
    cc.write_through = config_.write_through;
    cc.write_allocate = config_.write_allocate;
}


void Cache::HandleRequest(uint64_t addr, int bytes, int rw, char *content, int &hit, int &time) {
    int offset_set = lg2(config_.block_size);
    int offset_tag = lg2(config_.set_num) + lg2(config_.block_size);
    unsigned int set = (unsigned int)GETSET(addr, offset_tag, offset_set);
    unsigned int tag = (unsigned int)GETTAG(addr, offset_tag);
    unsigned int offset = (unsigned int)GETOFFSET(addr, offset_set);
    
    printf("\naddr = 0x%llx(%lld), offset_set = %d, offset_tag = %d\n", addr, addr, offset_set, offset_tag);
    printf("set = 0x%x, tag = 0x%x, offset = 0x%x, read = %d\n", set, tag, offset, rw);
    
    // process asking for multiblocks
    int overbytes = offset + bytes - config_.block_size;
    if(overbytes > 0){
        int first_bytes = config_.block_size - offset;
        HandleRequest(addr, first_bytes, rw, content, hit, time);
        HandleRequest(addr + first_bytes, overbytes, rw, content + first_bytes, hit, time);
        return;
    }
    
    // Bypass
    if (!BypassDecision()) {
        PartitionAlgorithm();
        
        int target = -1;
        int condition = ReplaceDecision(set, tag, target);
        hit = (condition == HIT) ? 1:0;
        stats_.access_counter++;
        printf("condition TAG = %d(0|1|2:hit|cold|conflict)\t", condition);

        if(condition == HIT){
            hit = 1;
            time += latency_.bus_latency + latency_.hit_latency;
            stats_.access_time += latency_.bus_latency + latency_.hit_latency;
            SetRPP(set, cache_[set][target]);
            
            if(rw == WRITE){
                memcpy(cache_[set][target].block_content + offset, content, bytes);
                if(config_.write_through){ // new fetch to lower cache because of write policy
                    int lower_hit = 0, lower_time = 0;
                    lower_->HandleRequest(addr, bytes, rw, content, lower_hit, lower_time);
                    stats_.fetch_num++;
                }
                else{
                    cache_[set][target].dirty_bit = TRUE;
                }
            }else{
                memcpy(content, cache_[set][target].block_content + offset, bytes);
            }
            
            return;
        }
        else{// MISS
            hit = 0;
            stats_.miss_num++;
            time += latency_.bus_latency;
            stats_.access_time += latency_.bus_latency;
            
            if(rw == WRITE && config_.write_allocate == 0){// write no alloc
                lower_->HandleRequest(addr, bytes, rw, content, hit, time);
                stats_.fetch_num++;
                return;
            }
            
            if(target == -1){// do an evict,
                target = ReplaceAlgorithm(set);     // chose a victim
                
                stats_.replace_num++;
                cache_[set][target].valid_bit = 0;
                if(config_.write_through == 0 && cache_[set][target].dirty_bit == 1){ // otherwise, just cast the block
                    int lower_hit = 0, lower_time = 0;
                    uint64_t victim_address = (tag<<offset_tag)||(set<<offset_set);
                    char *victim_content = new char[config_.block_size];
                    memcpy(victim_content, cache_[set][target].block_content, config_.block_size);
                    lower_->HandleRequest(victim_address, config_.block_size, WRITE, victim_content, lower_hit, lower_time);
                    stats_.fetch_num++;
                }
            }
            
            // load a block into current cache from lower
            char * new_block_content = new char[config_.block_size];
            int lower_hit;
            lower_->HandleRequest(addr - offset, config_.block_size, READ, new_block_content, lower_hit, time);
            stats_.fetch_num++;
            memcpy(cache_[set][target].block_content, new_block_content, config_.block_size);
            cache_[set][target].valid_bit = 1;
            cache_[set][target].dirty_bit = 0;         // -1 means invalid
            cache_[set][target].tag = tag;
            SetRPP(set, cache_[set][target]);
            
            // other types of miss, now restart with a implict hit and r/w
            if(rw == WRITE && config_.write_allocate ==1){
                memcpy(cache_[set][target].block_content+offset, content, bytes);
                if(config_.write_through == 0)
                    cache_[set][target].dirty_bit = 1;
            }else{// rw == READ
                memcpy(content, cache_[set][target].block_content+offset, bytes);
            }
            
            
        }// MISS
    }// NO bypassing
}



int Cache::BypassDecision() {
    return FALSE;
}

void Cache::PartitionAlgorithm() {
}

int Cache::ReplaceDecision(const int set, const unsigned int tag, int & target) {
    target = -1;
    for(int i = 0; i < config_.associativity; i++){
        if(cache_[set][i].valid_bit == true && cache_[set][i].tag == tag){
            target = i;
            return HIT;
        }
        if(target == -1 && cache_[set][i].valid_bit == false){
            target = i;             // set has slots, target refers to the first slot in this set
        }
    }
    return MISS;
}

int Cache::ReplaceAlgorithm(const int set){
    int victim = -1;
    uint64_t min = UINT64_MAX;
    for(int i = 0; i < config_.associativity; i++){
        if(cache_[set][i].RPP_tag < min){
            min = cache_[set][i].RPP_tag;
            victim = i;
        }
    }
/*
 for(int j = 0; j < config_.associativity; j++){
        printf("%llu\t", cache_[set][j].RPP_tag);
    }
    printf("\n");
    printf("victim = %d\n", victim);
*/
    
    
    
    return victim;
}

void Cache::SetRPP(const int set_id, Block & block){
    block.RPP_tag = clock();
}

int Cache::PrefetchDecision() {
    return FALSE;
}

void Cache::PrefetchAlgorithm() {
}

