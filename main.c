#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "cache.h"

typedef ADDR64 ADDR64;

void memory_read(ADDR64 addr);
void memory_write(ADDR64 addr);
unsigned get_set_pos(ADDR64 addr);
ADDR64 get_tag(ADDR64 addr);
void update_cache(ADDR64 addr);

int total_query = 0;
int miss_query = 0;
int hit_query = 0;

int main(){
    
    char path[200];
    printf("输入trace绝对路径\n");
    scanf("%s",path);
    int fd = open(path,O_RDWR);
   
    int t = 0;
    char label = 'd';
    ADDR64 addr = 0;
    dup2(fd,0);
    do{
        printf("%c  %x\n",label,addr);
        t = scanf("%c   %d\n",&label,&addr); 
        total_query ++;
        if(label == 'r'){
            memory_read(addr);
        }else if (label == 'w'){
            memory_write(addr);
        }
        
    }while(t > 0);
    return 0;
}

void memory_read(ADDR64 addr){
    unsigned set = get_set_pos(addr);
    ADDR64 tag = get_tag(addr);
    struct CacheBlock* cache_block = NULL;

    for(int i = 0; i < cache_config.associativity; i ++){
        if(cache[set].blocks[i].tag == tag){
            cache_block = &(cache[set].blocks[i]);
            break;
        }
    }

    if(cache_block != NULL){
        hit_query ++;
    }else{
        miss_query ++;
        update_cache(addr);
    }
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
    return tag;
}

void update_cache(ADDR64 addr)