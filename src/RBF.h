//Range Bloom filter

#include <cstdint>
extern long long cache_hit;
extern long long query_count;
class RangeBloomfilter
{
public:
    static const uint64_t
        MAX_BIT_NUM = 1ll << 34,
        MAX_HASH_NUM = 8;
    uint64_t
        hash_num, //number of hash function
        counter_num,
        bit_per_row;
    uint64_t memory;
    uint64_t BFcache4_key = -1;
    uint64_t *BFcache4_value;
    BOBHash32 hash[MAX_HASH_NUM];
    vector<size_t> seeds;
    uint8_t array[MAX_HASH_NUM][MAX_BIT_NUM / 8 + 64];

    void init(uint64_t _memory, //bit
              int _hash_num)
    {
        memory = _memory,
        hash_num = _hash_num,
        counter_num = ((memory / hash_num) >> 3);
        bit_per_row = counter_num << 3;
        if (bit_per_row > MAX_BIT_NUM)
        {
            cout << "RBF memory initialization fail 1.\n";
            exit(-1);
        }
        if (hash_num > MAX_HASH_NUM)
        {
            cout << "RBF memory initialization fail 2.\n";
            exit(-1);
        }

        for (int i = 0; i < hash_num; i++)
        {
            hash[i].initialize(rand() % MAX_PRIME32);
        }
        seeds.resize(hash_num);
        mt19937 gen(1337);
        for (size_t i = 0; i < hash_num; ++i)
        {
            seeds[i] = gen();
        }
        memset(array, 0, sizeof(array));
    }

    void insertbt(uint64_t x, uint64_t *bt)
    {
        __m512i_u bt_new, bt_old;
        bt_new = _mm512_loadu_si512((__m512i_u *)(bt));
        for (int i = 0; i < hash_num; i++)
        {
            uint64_t pos = hash[i].run((char *)&x, sizeof(uint64_t)) % bit_per_row;
            uint64_t t = pos >> 3;
            bt_old = _mm512_loadu_si512((__m512i_u *)(array[i] + t));
            bt_old = _mm512_or_si512(bt_new, bt_old);
            _mm512_storeu_si512((__m512i_u *)(array[i] + t), bt_old);
        }
    }

    inline uint64_t *querybt(uint64_t x)
    {
        static uint64_t ans[8] = {0};
        __m512i_u ans1, ans2;
        query_count++;
        if (BFcache4_key == x)
        {
            cache_hit++;
            return BFcache4_value;
        }
        for (int i = 0; i < 1; i++)
        {
            uint64_t pos = hash[i].run((char *)&x, sizeof(uint64_t)) % bit_per_row;
            uint64_t t = pos >> 3;
            ans1 = _mm512_loadu_si512((__m512i_u *)(array[i] + t));
        }
        for (int i = 1; i < hash_num; i++)
        {
            uint64_t pos = hash[i].run((char *)&x, sizeof(uint64_t)) % bit_per_row;
            uint64_t t = pos >> 3;
            ans2 = _mm512_loadu_si512((__m512i_u *)(array[i] + t));
            ans1 = _mm512_and_si512(ans1, ans2);
        }
        BFcache4_key = x;
        _mm512_storeu_si512((__m512i_u *)(ans), ans1);
        BFcache4_value = ans;
        return ans;
    }
};
