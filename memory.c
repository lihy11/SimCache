#include "memory.h"

#include <stdio.h>
#include <string.h>
extern int miss_query;
extern int hit_query;
extern int mem_query;

void memory_read(ADDR64 addr,void* data,int bytes){

    struct CacheBlock* cache_block = get_block(addr);

    if(cache_block != NULL){     //命中
        hit_query ++;
        read_from_cache(cache_block,addr,data,bytes);
    }else{          //不命中
        miss_query ++;
        cache_block = update_cache(addr);
        read_from_cache(cache_block,addr,data,bytes);
    }
}

void memory_write(ADDR64 addr,void* data,int bytes){
    struct CacheBlock* cache_block = get_block(addr);
    if(cache_config.write_through == 1){     //写透策略
        request_2_mem_write(addr,data,bytes);     //写入内存
        if(cache_block == NULL){       //不命中
            miss_query ++;
            if(cache_config.write_allocate == 1){    //写分配
                cache_block = update_cache(addr);
                write_to_cache(cache_block,addr,data,bytes);
            }
        }else{        //命中
            hit_query ++;
            write_to_cache(cache_block,addr,data,bytes);
        }
    }else{      //写回策略
        if(cache_block == NULL){   //不命中
            miss_query ++;
            cache_block = update_cache(addr);
            cache_block->dirty = 1;
            write_to_cache(cache_block,addr,data,bytes);
        }else{             //命中
            hit_query ++;
            cache_block->dirty = 1;
            write_to_cache(cache_block,addr,data,bytes);
        }
    }
}

void request_2_mem_read(ADDR64 addr,void* data,int bytes){
    mem_query ++;
}
void request_2_mem_write(ADDR64 addr,void* data,int bytes){
    mem_query ++;
}
