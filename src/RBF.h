// Range Bloom filter

#include <cstdint>

extern long long cache_hit;
extern long long query_count;

class RangeBloomfilter
{
public:
    static const uint64_t
        MAX_BIT_NUM = (1ll << 32) - 1,
        MAX_HASH_NUM = 20;
    uint64_t
        hash_num,
        counter_num,
        bit_per_row,
        memory;

    // Cache the result of query.
    uint64_t BFcache_key = -1;
#ifdef USE_SIMD
    uint64_t *BFcache_value;
#else
    uint32_t BFcache_value;
#endif
    BOBHash32 hash[MAX_HASH_NUM];
#ifdef USE_SIMD
    uint8_t **array;
#else
    uint32_t **array;
#endif

    // Create an uninitialized Range Bloom Filter.
    RangeBloomfilter()
    {
    }

    // Create an initialized Range Bloom Filter.
    RangeBloomfilter(uint64_t _hash_num, uint64_t _counter_num, uint64_t _bit_per_row, uint64_t _memory, uint8_t *_hash, uint8_t *_array)
    {
        hash_num = _hash_num;
        counter_num = _counter_num;
        bit_per_row = _bit_per_row;
        memory = _memory;
        for (int i = 0; i < hash_num; i++)
        {
            hash[i].initialize(((uint32_t *)_hash)[i]);
        }
#ifdef USE_SIMD
        array = new uint8_t *[hash_num];
        for (int i = 0; i < hash_num; i++)
        {
            array[i] = new uint8_t[counter_num];
            memcpy(array[i], _array + i * counter_num * sizeof(uint8_t), counter_num * sizeof(uint8_t));
        }
#else
        array = new uint32_t *[hash_num];
        for (int i = 0; i < hash_num; i++)
        {
            array[i] = new uint32_t[counter_num];
            memcpy(array[i], _array + i * counter_num * sizeof(uint32_t), counter_num * sizeof(uint32_t));
        }
#endif
    }

    // Initialize Range Bloom Filter.
    void init(uint64_t _memory, int _hash_num)
    {
        srand(921224);
        memory = _memory,
        hash_num = _hash_num;
#ifdef USE_SIMD
        counter_num = ((memory / hash_num) >> 3) + 1;
        bit_per_row = counter_num << 3;
#else
        counter_num = ((memory / hash_num) >> 5) + 1;
        bit_per_row = counter_num << 5;
#endif
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
#ifdef USE_SIMD
        array = new uint8_t *[hash_num];
        for (int i = 0; i < hash_num; i++)
        {
            array[i] = new uint8_t[counter_num]();
        }
#else
        array = new uint32_t *[hash_num];
        for (int i = 0; i < hash_num; i++)
        {
            array[i] = new uint32_t[counter_num]();
        }
#endif
    }

    // Serialize Range Bloom Filter and get its size.
    pair<uint8_t *, size_t> serialize()
    {
        size_t len = 4 * sizeof(uint64_t) + hash_num * sizeof(uint32_t);
#ifdef USE_SIMD
        len += hash_num * counter_num * sizeof(uint8_t);
#else
        len += hash_num * counter_num * sizeof(uint32_t);
#endif
        uint8_t *out = new uint8_t[len];
        uint8_t *pos = out;
        memcpy(pos, &hash_num, sizeof(uint64_t));
        pos += sizeof(uint64_t);
        memcpy(pos, &counter_num, sizeof(uint64_t));
        pos += sizeof(uint64_t);
        memcpy(pos, &bit_per_row, sizeof(uint64_t));
        pos += sizeof(uint64_t);
        memcpy(pos, &memory, sizeof(uint64_t));
        pos += sizeof(uint64_t);
        for (int i = 0; i < hash_num; i++)
        {
            uint32_t pri_num = hash[i].get_prime_num();
            memcpy(pos, &pri_num, sizeof(uint32_t));
            pos += sizeof(uint32_t);
        }
#ifdef USE_SIMD
        for (int i = 0; i < hash_num; i++)
        {
            memcpy(pos, array[i], counter_num * sizeof(uint8_t));
            pos += counter_num * sizeof(uint8_t);
        }
#else
        for (int i = 0; i < hash_num; i++)
        {
            memcpy(pos, array[i], counter_num * sizeof(uint32_t));
            pos += counter_num * sizeof(uint32_t);
        }
#endif
        // printf("RBF serialized size: %lu bytes\n", len);
        return {out, len};
    }

    // Deserialize to get Range Bloom Filter.
    static pair<RangeBloomfilter *, size_t> deserialize(uint8_t *ser)
    {
        uint8_t *pos = ser;
        uint64_t hash_num = ((uint64_t *)pos)[0];
        uint64_t counter_num = ((uint64_t *)pos)[1];
        uint64_t bit_per_row = ((uint64_t *)pos)[2];
        uint64_t memory = ((uint64_t *)pos)[3];
        pos += 4 * sizeof(uint64_t);
        size_t len = pos - ser + hash_num * sizeof(uint32_t);
#ifdef USE_SIMD
        len += hash_num * counter_num * sizeof(uint8_t);
#else
        len += hash_num * counter_num * sizeof(uint32_t);
#endif
        return {new RangeBloomfilter(hash_num, counter_num, bit_per_row, memory, pos, pos + hash_num * sizeof(uint32_t)), len};
    }

#ifdef USE_SIMD
    // Range Bloom Filter with SIMD.

    // Insert bitmap into Range Bloom Filter.
    void insertbt(uint64_t p, uint64_t *bt)
    {
        __m512i_u bt_new, bt_old;
        bt_new = _mm512_loadu_si512((__m512i_u *)(bt));

        // Hash the prefix(p) into indices of Range Bloom Filter and inlay the bitmap.
        for (int i = 0; i < hash_num; i++)
        {
            uint64_t pos = hash[i].run((char *)&p, sizeof(uint64_t)) % bit_per_row;
            uint64_t t = pos >> 3;
            bt_old = _mm512_loadu_si512((__m512i_u *)(array[i] + t));
            bt_old = _mm512_or_si512(bt_new, bt_old);
            _mm512_storeu_si512((__m512i_u *)(array[i] + t), bt_old);
        }
    }

    // Extract the bitmap from Range Bloom Filter.
    inline uint64_t *querybt(uint64_t p)
    {
        static uint64_t bt[8] = {0};
        __m512i_u bt1, bt2;
        query_count++;
        if (BFcache_key == p)
        {
            // Return the cached result.
            cache_hit++;
            return BFcache_value;
        }

        // Hash the prefix(p) into indices of Range Bloom Filter and extract the bitmap.
        for (int i = 0; i < 1; i++)
        {
            uint64_t pos = hash[i].run((char *)&p, sizeof(uint64_t)) % bit_per_row;
            uint64_t t = pos >> 3;
            bt1 = _mm512_loadu_si512((__m512i_u *)(array[i] + t));
        }
        for (int i = 1; i < hash_num; i++)
        {
            uint64_t pos = hash[i].run((char *)&p, sizeof(uint64_t)) % bit_per_row;
            uint64_t t = pos >> 3;
            bt2 = _mm512_loadu_si512((__m512i_u *)(array[i] + t));
            bt1 = _mm512_and_si512(bt1, bt2);
        }
        BFcache_key = p;
        _mm512_storeu_si512((__m512i_u *)(bt), bt1);
        BFcache_value = bt;
        return bt;
    }

#else
    // Range Bloom Filter without SIMD.

    // Insert bitmap into Range Bloom Filter.
    void insertbt(uint64_t p, uint32_t bt)
    {
        // Hash the prefix(p) into indices of Range Bloom Filter and inlay the bitmap.
        for (int i = 0; i < hash_num; i++)
        {
            uint64_t pos = hash[i].run((char *)&p, sizeof(uint64_t)) % bit_per_row;
            uint64_t t = pos >> 5;
            array[i][t] |= bt << (pos & 31);
            array[i][t] |= ((pos & 31) == 0 ? 0 : bt >> (32 - (pos & 31)));
            t = 0;
        }
    }

    // Extract the bitmap from Range Bloom Filter.
    inline uint32_t querybt(uint64_t p)
    {
        query_count++;
        if (BFcache_key == p)
        {
            cache_hit++;
            return BFcache_value;
        }

        // Hash the prefix(p) into indices of Range Bloom Filter and extract the bitmap.
        uint32_t bt = 0;
        for (int i = 0; i < 1; i++)
        {
            uint64_t pos = hash[i].run((char *)&p, sizeof(uint64_t)) % bit_per_row;
            uint64_t t = pos >> 5;
            bt = array[i][t] >> (pos & 31) | ((pos & 31) == 0 ? 0 : array[i][t] << (32 - (pos & 31)));
        }
        for (int i = 1; i < hash_num; i++)
        {
            uint64_t pos = hash[i].run((char *)&p, sizeof(uint64_t)) % bit_per_row;
            uint64_t t = pos >> 5;
            bt &= array[i][t] >> (pos & 31) | ((pos & 31) == 0 ? 0 : array[i][t] << (32 - (pos & 31)));
        }
        BFcache_key = p, BFcache_value = bt;
        return bt;
    }
#endif
};
