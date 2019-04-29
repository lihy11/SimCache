# SimCache #

## 基本假设 ##
假设所有给出的地址符合4字节地址对齐的要求，因此每个cache block最小为4字节，大小必须为4字节的倍数。而给出的组相连associativity，set数量都是2,4,8,16等规则的数据

## 代码结构 ##
cache.c:模拟相关的cache操作，包括初始化，释放，替换，
memory.c：模拟内存相关操作，包括读内存和写内存

## 设计逻辑 ##
- 在运行开始需要调用cache初始化函数，提供相关数据的设置，配置cache的相关信息存储在cache_config中
#### 内存操作 ####
- 读内存：调用`memory_read(ADDR64 addr , int width)`函数，代表从addr地址读取宽度为8,16,32,64的数据。流程为：

```mermaid
graph LR
    start[开始] --> check[检查addr]
    check --> conditionA{addr是否在cache}
    conditionA -- YES --> findcondition{addr+width是否超出边界}
    conditionA -- NO --> notfind{将addr更新至cache}
    findcondition -- YES --> outlimit[读取超出边界部分]
    findcondition -- NO --> notoutlimit[读取当前数据]
    outlimit --> cut[拼接数据如果需要]
    notoutlimit --> cut
    cut --> return[返回数据]
```
- 写内存：调用`memory_write(addr,width,data)`函数将宽度为width（8,16,32,64）的数据data写入addr位置。流程如下：
```mermaid
graph TB
    start[写内存] --> check{addr是否在cache中}
    check -- YES --> check_through{是否是写透策略}
    check_through -- YES --> write_mem[写入内存]
    write_mem --> write_cache[写入cache]
    check_through -- NO --> write_cache
    check -- NO --> check_alloc{检查是否是写分配}
    check_alloc -- YES --> wandaloc[写并分配cache]
    check_alloc -- NO --> write_mem
```

