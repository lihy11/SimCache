#include "memory.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

typedef unsigned long long  ADDR64;
int test(int cache_size,int block_size,int asso,int write_through,int write_alloc);
void read_trace();
struct Query{
    char op;
    ADDR64 addr;
};
struct Query querys[50000];
int total_query = 0;
int miss_query = 0;
int hit_query = 0;

int mem_query = 0;
int cache_query = 0;
int main(){

	char path[200];
	printf("输入trace绝对路径\n");     //  /home/lhy/Desktop/SimCache/1.trace
	scanf("%s",path);
	int fd = open(path,O_RDWR);
	dup2(fd,0);
	read_trace();
	close(fd);
	int cache_size = 32*1024;
	int block_size = 256;
	int asso = 8;
	test(cache_size,block_size,asso,0,0);
	printf("no write through and no write alloc\n total memory query is %d,total cache query is %d\n",mem_query,cache_query);
	test(cache_size,block_size,asso,1,0);
	printf("write through and no write alloc\n total memory query is %d,total cache query is %d\n",mem_query,cache_query);
	test(cache_size,block_size,asso,1,1);
	printf("write through and write alloc\n total memory query is %d,total cache query is %d\n",mem_query,cache_query);
//	printf("cache_size | block_size | asso | total | miss | hit | miss_rate|\n");
// 	for(;cache_size <= 32*1024; cache_size *= 2){
//         printf("%d\n",cache_size);
// 		for(block_size=4;block_size <= 1024; block_size *= 2){
// 			miss_query = 0;
// 			hit_query = 0;
// 			test(cache_size,block_size,asso);
// 			if(cache_config.set_num == 0)
// 				continue;
// //            printf("%f\n",(float)miss_query/(float)total_query);
// // 			printf("%d | %d | %d | %d | %d | %d | %f |\n",
// // 					cache_size,block_size,asso,total_query,miss_query,hit_query,
// // 					(float)miss_query/(float)total_query);
// 		}
// 	}
// 	printf("-----------------------------\n");
// 	cache_size = 32;
// 	block_size = 64;
// 	asso = 1;
// 	for(cache_size=32;cache_size <= 32*1024; cache_size *= 2){
//         printf("%d\n",cache_size);
// 		for(asso=1;asso <= 32; asso *= 2){
// 			miss_query = 0;
// 			hit_query = 0;
// 			test(cache_size,block_size,asso);
// 			if(cache_config.set_num == 0)
// 				continue;
//             printf("%f\n",(float)miss_query/(float)total_query);
// // 			printf("%d | %d | %d | %d | %d | %d | %f |\n",
// // 					cache_size,block_size,asso,total_query,miss_query,hit_query,
// // 					(float)miss_query/(float)total_query);
// 		}
// 		//printf("------------\n");
// 	}
}

int test(int cache_size,int block_size,int asso,int write_through,int write_alloc){
    
	init_cache(cache_size,asso,block_size,write_through,write_alloc);
	if(cache_config.set_num == 0){
		return 0;
	}
    int t = 0;
    char data[8] = {};

    for(int i = 0; i < total_query ; i ++){
        if(querys[i].op == 'r'){
            memory_read(querys[i].addr,data,1);
        }else if(querys[i].op == 'w'){
             memory_write(querys[i].addr,data,1);
        }
    }
    free_cache();
}

void read_trace(){
    int t = 0;
    char label = 'd';
    ADDR64 addr = 0;
    int i = 0;
    do{
        t = scanf("%c %d \n",&label,&addr);
        if(label == 'r'){
            total_query ++;
            querys[i].op = 'r';
            querys[i].addr = addr;
            i ++;
   //         printf("%c  %x\n",label,addr);
        }else if (label == 'w'){
            total_query ++;
            querys[i].op = 'w';
            querys[i].addr = addr;
            i ++;
    //        printf("%c  %x\n",label,addr);
        }
    }while(t > 0);
}
