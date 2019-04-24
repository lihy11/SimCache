#include "cache.h"
#include <stdlib.h>

void init_cache(int size,    //大小
                int asso,     //几路
                int set_num,    //共几个set
                int write_throuth,//  0|1 == back | through
                int write_alloc      //     0|1 == no-alc | alloc
                )
{
    cache_config.size = size;
    cache_config.associativity = asso;
    cache_config.set_num = set_num;
    cache_config.write_through = write_throuth;
    cache_config.write_allocate = write_alloc;
    cache_config.data_size = size/(asso * set_num);
    //为cache分配内存空间
    cache = (struct CacheSet*)malloc(sizeof(struct CacheSet) * set_num);
    for(int i = 0; i < set_num; i ++){
        cache[i].blocks = (struct CacheBlock*)malloc(sizeof(struct CacheBlock) * asso);
        for(int j = 0; j < asso; j ++){
            cache[i].blocks[j].data = (unsigned*)malloc(cache_config.data_size);
            cache[i].blocks[j].tag = cache[i].blocks[j].dirty = cache[i].blocks[j].valid = 0;
        }
    }

}

void free_cache(){
    for(int i = 0; i < cache_config.set_num; i ++){
        for(int j = 0; j < cache_config.associativity; j ++){
            free(cache[i].blocks[j].data);
        }
        free(cache[i].blocks);
    }
    free(cache);
}