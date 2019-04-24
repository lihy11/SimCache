

struct CacheConfig{
    int size;             //整个cachedata大小
    int associativity;     //每个set里面有几路
    int set_num; // Number of cache sets
    int data_size; //数据块大小
    int write_through; // 0|1 for back|through
    int write_allocate; // 0|1 for no-alc|alc
} cache_config;

struct CacheBlock{
    unsigned tag;       //标记位，识别地址
    unsigned dirty;      //污染位，在写回策略时使用
    unsigned valid;       //块是否有效
    unsigned* data;      //数据

};

struct CacheSet{
    struct CacheBlock* blocks;
};

void init_cache(int size,    //大小
                int asso,     //几路
                int set_num,    //共几个set
                int write_throuth,//  0|1 == back | through
                int write_alloc      //     0|1 == no-alc | alloc
                );
