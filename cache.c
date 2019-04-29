#include "cache.h"
#include <stdlib.h>
#include <string.h>
/*跳过cache直接操作内存的方法*/

extern void request_2_mem_read(ADDR64 addr,void* data,int bytes);
extern void request_2_mem_write(ADDR64 addr,void* data,int bytes);
/*提供外部访问内存的接口*/
extern void memory_read(ADDR64 addr,void* data,int bytes);
extern void memory_write(ADDR64 addr,void* data,int bytes);
extern int cache_query;

void init_cache(int size,    //大小
                int asso,     //几路
                int data_size,   //数据块大小
                int write_throuth,//  0|1 == back | through
                int write_alloc      //     0|1 == no-alc | alloc
                )
{

    size = size * 1024;
    int set_num = size/(asso * data_size);
    cache_config.size = size;
    cache_config.associativity = asso;
    cache_config.data_size = data_size;
    cache_config.set_num = set_num;
    cache_config.write_through = write_throuth;
    cache_config.write_allocate = write_alloc;

    //为cache分配内存空间
    cache = (struct CacheSet*)malloc(sizeof(struct CacheSet) * set_num);
    for(int i = 0; i < set_num; i ++){
        cache[i].blocks = (struct CacheBlock*)malloc(sizeof(struct CacheBlock) * asso);
        for(int j = 0; j < asso; j ++){
            cache[i].blocks[j].data = (unsigned char*)malloc(data_size);
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

struct CacheBlock* get_block(ADDR64 addr){
    unsigned set = get_set_pos(addr);
    ADDR64 tag = get_tag(addr);
    struct CacheBlock* cache_block = NULL;

    for(int i = 0; i < cache_config.associativity; i ++){
        if(cache[set].blocks[i].tag == tag){
            cache_block = &(cache[set].blocks[i]);
            break;
        }
    }
    return cache_block;
}

unsigned get_set_pos(ADDR64 addr){
    //获得代表set的位置
    int data_size = cache_config.data_size;
    int set_num = cache_config.set_num;

    unsigned bit = (data_size * set_num) -1 - (data_size -1);
    unsigned set_pos = (bit & addr)/data_size;
    return set_pos;
}
ADDR64 get_tag(ADDR64 addr){
    
    int data_size = cache_config.data_size;
    int set_num = cache_config.set_num;

    //获得tag
    ADDR64 tag = addr - (addr % (set_num * data_size));
    return tag/(set_num * data_size);
}

unsigned get_offset(ADDR64 addr){
    return addr & (cache_config.data_size-1);
}
/*
* 找到所属set
* 根据替换策略选择一个block
* 检查dirty位是否需要更新到内存
* 替换block
*/
struct CacheBlock* update_cache(ADDR64 addr){
    unsigned set = get_set_pos(addr);
    struct CacheBlock* block = choose_block(set);
    ADDR64 block_begin = addr - get_offset(addr);
    if(block->dirty == 1){
        request_2_mem_write(block_begin,block->data,cache_config.data_size);
    }
    block->tag = get_tag(addr);
    block->valid = 1;
    request_2_mem_read(block_begin,block->data,cache_config.data_size);
    //memcpy(block->data,block_begin,cache_config.data_size);
    return block;
}
void write_to_cache(struct CacheBlock* block,ADDR64 addr,void* data,int bytes){
    cache_query ++;
    unsigned offset = get_offset(addr);
    //写操作超出cache快边界
    if(bytes == 8 && (cache_config.data_size - offset < 8)){
        //memcpy(block->data+offset,data,4);
        memory_write(addr + 4,(char*)data + 4,4);
    }else{
        //memcpy(block->data+offset,data,bytes);
    }
}
void read_from_cache(struct CacheBlock* block,ADDR64 addr,void* data,int bytes){
    cache_query ++;
    unsigned offset = get_offset(addr);
    //cache是四字节对其，只有8字节读取可能超出一个cacheblock,考虑超出一个cache边界的情况
    if(bytes == 8 && (cache_config.data_size - offset < 8)){   
        //memcpy((char*)data,block->data+offset,4);
        memory_read(addr + 4,(char*)data + 4,4);
    }else{
        //memcpy((char*)data,block->data+offset,bytes);
    }
}
/*没有替换策略  TODO*/  
struct CacheBlock* choose_block(unsigned set){
    for(int i = 0; i < cache_config.associativity; i ++){
        if(cache[set].blocks[i].valid == 0){
            return &(cache[set].blocks[i]);
        }
    }
    return &(cache[set].blocks[0]);
}
