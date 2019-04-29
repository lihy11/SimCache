#include "cache.h"

typedef unsigned long long ADDR64;
typedef unsigned long long DATA64;
/*提供外部访问内存的接口*/
void memory_read(ADDR64 addr,void* data,int bytes);
void memory_write(ADDR64 addr,void* data,int bytes);
/*跳过cache直接操作内存的方法*/
void requset_2_mem_read(ADDR64 addr,void* data,int bytes);
void request_2_mem_write(ADDR64 addr,void* data,int bytes);