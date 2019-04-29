
typedef unsigned long long ADDR64;
typedef unsigned long long DATA64;

struct CacheConfig{
    int size;             //整个cachedata大小.字节表示
    int associativity;     //每个set里面有几路
    int set_num; // Number of cache sets
    int data_size; //数据块大小,字节表示
    int write_through; // 0|1 for back|through
    int write_allocate; // 0|1 for no-alc|alc
} cache_config;

struct CacheBlock{
    unsigned tag;       //标记位，识别地址
    unsigned dirty;      //污染位，在写回策略时使用
    unsigned valid;       //块是否有效
    unsigned char* data;      //数据

};

struct CacheSet{
    struct CacheBlock* blocks;
};

void init_cache(int size,    //大小   kb计量
                int asso,     //几路
                int data_size,   //数据块大小
                int write_throuth,//  0|1 == back | through
                int write_alloc      //     0|1 == no-alc | alloc
                );
void free_cache();
struct CacheSet* cache;


struct CacheBlock* get_block(ADDR64 addr);
unsigned get_set_pos(ADDR64 addr);
ADDR64 get_tag(ADDR64 addr);
unsigned get_offset(ADDR64 addr);
struct CacheBlock* update_cache(ADDR64 addr);

void write_to_cache(struct CacheBlock* block,ADDR64 addr,void* data,int bytes);
void read_from_cache(struct CacheBlock* block,ADDR64 addr,void* data,int bytes);

struct CacheBlock* choose_block(unsigned set);