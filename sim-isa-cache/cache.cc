#include "cache.h"
#include "def.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG

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

Cache::Cache()
{
    config_.size = 32;
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
    }
}


void Cache::SetConfig(CacheConfig cc)
{
    config_ .size = cc.size;
    config_ .block_size = cc.block_size;
    config_ .associativity = cc.associativity;
    config_ .set_num = cc.set_num;
    config_ .write_through = cc.write_through;
    config_ .write_allocate = cc.write_allocate;
    
    printf("size = %d, block_size = %d, assc = %d, set_num = %d\n",
           config_ .size, config_ .block_size, config_ .associativity, config_ .set_num);
    printf("write_through(back) = %d, write_alloc(no-alloc) = %d\n", config_ .write_through, config_ .write_allocate);
    
    for(int i = 0; i < config_.set_num; i++) {
        delete [] cache_[i];
    }
    delete [] cache_;
    
    cache_ = new Block *[config_.set_num];
    for(int i = 0; i < config_.set_num; i++) {
        cache_[i] = new Block[config_.associativity];
        memset(cache_[i], 0, sizeof(Block)*config_.associativity);
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


void Cache::HandleRequest(uint64_t addr, int bytes, int read,
                          char *content, int &hit, int &time) {
    hit = 0;
    time = 0;
    int set;
    unsigned int tag, offset;
    int target = -1;
    int condition = -1;
    
    // Bypass
    if (!BypassDecision()) {
        PartitionAlgorithm();
        int offset_set = lg2(config_.block_size);
        int offset_tag = lg2(config_.set_num) + lg2(config_.block_size);
        set = (int)((addr & ONES((offset_tag-1),offset_set)) >> offset_set);
        tag = (unsigned int)((addr & ONES(63,offset_tag)) >> offset_tag);
        offset = (unsigned int)(addr & ONES((offset_tag-1), 0));
        
        printf("\naddr = 0x%llx(%lld), offset_set = %d, offset_tag = %d\n", addr, addr, offset_set, offset_tag);
        printf("set = 0x%x, tag = 0x%x, offset = 0x%x, read = %d\n", set, tag, offset, read);
        
        condition = ReplaceDecision(set, tag, target);
        printf("condition TAG = %d(0|1|2:hit|cold|conflict)\t", condition);
        hit = (condition == HIT) ? 1:0;
        stats_.access_counter++;
        if (hit == 0) {
            stats_.miss_num++;
        }
        
        switch (condition) {
            case CONFLICT_MISS:
                if((read == false) && (config_.write_allocate == 0)){
                   // write miss, write no allc, lower_->HandleRequest
                    int lower_hit, lower_time;
                    lower_->HandleRequest(addr, bytes, read, content, lower_hit, lower_time);
                    time += latency_.bus_latency + lower_time;
                    stats_.access_time += latency_.bus_latency;
                    //no block evicted, no fetch from lower layer
                    break;
                }
                target = ReplaceAlgorithm(set);
                printf("target = %d\n", target);

                // evict, mind dirty bit(dirty bit is only valid under writeback policy)
                stats_.replace_num++;
                cache_[set][target].valid_bit = 0;
                printf("hhh\n");
                if(config_.write_through == 0 && cache_[set][target].dirty_bit == 1){
                    int lower_hit, lower_time;
                    uint64_t victim_address = (tag<<offset_tag)||(set<<offset_set);
                    char victim_content[16]; //!!
                    lower_->HandleRequest(victim_address, config_.block_size, WRITE, victim_content,
                                          lower_hit, lower_time);
                }

            case COLD_MISS:
                // load
                if (PrefetchDecision()) {
                    PrefetchAlgorithm(); // change some load arguments, no need for Lab3-1
                }
                printf("target = %d\n", target);
                if(read == true || (read == false && config_.write_allocate == 1)){
                    cache_[set][target].valid_bit = 1;
                    cache_[set][target].tag = tag;
                    SetRPP(set, cache_[set][target]);
                }
                int lower_hit, lower_time;
                lower_->HandleRequest(addr, bytes, read, content, lower_hit, lower_time);
                time += latency_.bus_latency + lower_time;
                stats_.access_time += latency_.bus_latency;
                break;
                
            case HIT:
                // read hit, write hit(writeback or writethrough)
                // for HIT: 0|1 for back(set dirty bit, only change cache)|through(write both cache and lower level)
                printf("target = %d\n", target);
                if(read == true){
                    cache_[set][target].valid_bit = 1;
                    cache_[set][target].tag = tag;
                    SetRPP(set, cache_[set][target]);
                 }
                else{
                    // write back, write through use a buffer, so no latency need to plus
                    cache_[set][target].valid_bit = 1;
                    cache_[set][target].dirty_bit = 1;
                    cache_[set][target].tag = tag;
                    SetRPP(set, cache_[set][target]);
                }
                
                time += latency_.bus_latency + latency_.hit_latency;
                stats_.access_time += time;
                break;
        }
    }
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
            target = i;
        }
    }
    if(target != -1) return COLD_MISS;       // set has slots, target refers to the first slot in this set
    return CONFLICT_MISS;
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
#ifdef DEBUG
    for(int j = 0; j < config_.associativity; j++){
        printf("%llu\t", cache_[set][j].RPP_tag);
    }
    printf("\n");
    printf("victim = %d\n", victim);
#endif
    
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

