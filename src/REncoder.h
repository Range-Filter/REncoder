// #include "sBF.h"

using namespace std;

// Use the stack to store the prefixes in verification stage.
stack<pair<uint64_t, uint64_t>> prefixes;

// Record last inserted prefix to avoid duplicate insertion.
uint64_t last_p = 0;
bool first_insert = true;

class RENCODER
{
public:
    static const int
        MAX_KEY_LENGTH = 64;
    uint64_t
        hash_num,
        memory,
        L, // Length of key.
        stored_level,
        start_level,
        end_level;
    RangeBloomfilter *rbf;

    // Create an uninitialized REncoder.
    RENCODER()
    {
    }

    // Create an initialized REncoder.
    RENCODER(uint64_t _hash_num, uint64_t _memory, uint64_t _L, uint64_t _stored_level, RangeBloomfilter *_rbf)
    {
        hash_num = _hash_num;
        memory = _memory;
        L = _L;
        stored_level = _stored_level;
        memory = _memory;
        rbf = _rbf;
    }

    // Initialize REncoder.
    void init(uint64_t _memory,
              uint64_t _hash_num,
              uint64_t _L,
              uint64_t _stored_level = 64,
              uint64_t _start_level = 1,
              uint64_t _end_level = -1)
    {
        memory = _memory,
        hash_num = _hash_num,
        L = _L;
        stored_level = _stored_level;
        start_level = _start_level;
        end_level = _end_level;
        if (MAX_KEY_LENGTH < _L)
        {
            cout << "REncoder memory initialization fail 1.\n";
            exit(-1);
        }

        // Initialize Range Bloom Filter.
        rbf = new RangeBloomfilter();
        rbf->init(_memory, hash_num);
    }

    // Serialize REncoder and get its size.
    pair<uint8_t *, size_t> serialize()
    {
        size_t len = 6 * sizeof(uint64_t);
        auto ser = rbf->serialize();
        len += ser.second;
        uint8_t *out = new uint8_t[len];
        uint8_t *pos = out;
        memcpy(pos, &hash_num, sizeof(uint64_t));
        pos += sizeof(uint64_t);
        memcpy(pos, &memory, sizeof(uint64_t));
        pos += sizeof(uint64_t);
        memcpy(pos, &L, sizeof(uint64_t));
        pos += sizeof(uint64_t);
        memcpy(pos, &stored_level, sizeof(uint64_t));
        pos += sizeof(uint64_t);
        memcpy(pos, &start_level, sizeof(uint64_t));
        pos += sizeof(uint64_t);
        memcpy(pos, &end_level, sizeof(uint64_t));
        pos += sizeof(uint64_t);
        memcpy(pos, ser.first, ser.second);
        delete[] ser.first;
        return {out, len};
    }

    // Deserialize to get REncoder.
    static pair<RENCODER *, size_t> deserialize(uint8_t *ser)
    {
        uint8_t *pos = ser;
        uint64_t hash_num = ((uint64_t *)pos)[0];
        uint64_t memory = ((uint64_t *)pos)[1];
        uint64_t L = ((uint64_t *)pos)[2];
        uint64_t stored_level = ((uint64_t *)pos)[3];
        uint64_t start_level = ((uint64_t *)pos)[4];
        uint64_t end_level = ((uint64_t *)pos)[5];
        pos += 6 * sizeof(uint64_t);
        pair<RangeBloomfilter *, size_t> tmp = RangeBloomfilter::deserialize(pos);
        size_t len = pos - ser + tmp.second;
        return {new RENCODER(hash_num, memory, L, stored_level, tmp.first), len};
    }

    // Self-adaptively insert keys. The number of stored levels is determined
    // during insertion and returned.
    int Insert_SelfAdapt(vector<uint64_t> keys, int step)
    {
        int num = keys.size();
        double onerate = 0;
        double pre_onerate = 0;
        int begin_level;
        if (end_level == -1)
        {
            begin_level = start_level;
        }
        else
        {
            begin_level = end_level - step + 1;
        }
        while (true)
        {
            stored_level = begin_level + step;
            for (int i = 0; i < num; i++)
            {
                Insert(keys[i], begin_level);
            }
            for (int i = 0; i < rbf->counter_num; i++)
            {
                uint64_t array = rbf->array[0][i];
#ifdef USE_SIMD
                for (int j = 0; j < 8; j++)
                {
                    onerate += (array & 1);
                    array >>= 1;
                }
#else
                for (int j = 0; j < 32; j++)
                {
                    onerate += (array & 1);
                    array >>= 1;
                }
#endif
            }
            onerate /= rbf->bit_per_row;
            if (0.5 - onerate < 0.065 || onerate >= 0.5 || stored_level == 65)
            {
                break;
            }
            if (end_level == -1)
            {
                begin_level = stored_level;
            }
            else
            {
                begin_level = begin_level - step;
                if (begin_level < 1)
                {
                    begin_level += step;
                    break;
                }
            }
            pre_onerate = onerate;
            onerate = 0;
        }
        if (end_level == -1)
        {
            stored_level -= 1;
        }
        else
        {
            start_level = begin_level;
            stored_level = end_level;
        }
        return stored_level;
    }

#ifdef USE_SIMD
    // Insertion with SIMD.

    // Insert prefixes between begin_level and stored_level.
    void Insert(uint64_t p, uint64_t begin_level)
    {
        uint64_t bt[8] = {0};
        uint64_t p0 = p;
        p >>= (begin_level - 1) / 8 * 8;
        if (first_insert)
        {
            first_insert = false;
        }
        else if (p == last_p)
        {
            return;
        }
        last_p = p;
        int si = (begin_level - 1) / 8 * 8 + 8;
        int ei = (stored_level - 2) / 8 * 8 + 8;
        for (int i = si; i <= ei; i += 8)
        {
            int sl = begin_level - i + 8 > 0 ? begin_level - i + 8 : 1;
            int el = stored_level - 1 - i + 8;
            // Encode the prefix to a bitmap.
            memset(bt, 0, sizeof(bt));
            uint64_t u = p & 0x000000FFU;
            bt[7 - u / 64] |= sl <= 1 && el >= 1 ? (1ul << u) : 0;
            u >>= 1;
            bt[3 - u / 64] |= sl <= 2 && el >= 2 ? (1ul << u) : 0;
            u >>= 1;
            bt[1] |= sl <= 3 && el >= 3 ? (1ul << u) : 0;
            u >>= 1;
            u |= 0xFFFFFFC0U;
            bt[0] |= sl <= 4 && el >= 4 ? (1ul << u) : 0;
            bt[0] |= sl <= 5 && el >= 5 ? (1ul << (u >> 1)) : 0;
            bt[0] |= sl <= 6 && el >= 6 ? (1ul << (u >> 2)) : 0;
            bt[0] |= sl <= 7 && el >= 7 ? (1ul << (u >> 3)) : 0;
            bt[0] |= sl <= 8 && el >= 8 ? (1ul << (u >> 4)) : 0;
            rbf->insertbt((p >> 8) + (L - i >> 3) * 1000000007, bt);
            p >>= 8;
        }
    }

    // Insert prefixes between 1 and stored_level (given manually).
    void Insert(uint64_t p)
    {
        uint64_t level = stored_level;
        uint64_t bt[8] = {0};
        uint64_t p0 = p;
        for (int i = 8; i <= L; i += 8)
        {
            // Encode the prefix to a bitmap.
            memset(bt, 0, sizeof(bt));
            uint64_t u = p & 0x000000FFU;
            if (level <= 8)
            {
                bt[7 - u / 64] |= level >= 1 ? (1ul << u) : 0;
                u >>= 1;
                bt[3 - u / 64] |= level >= 2 ? (1ul << u) : 0;
                u >>= 1;
                bt[1] |= level >= 3 ? (1ul << u) : 0;
                u >>= 1;
                u |= 0xFFFFFFC0U;
                bt[0] |= level >= 4 ? (1ul << u) : 0;
                bt[0] |= level >= 5 ? (1ul << (u >> 1)) : 0;
                bt[0] |= level >= 6 ? (1ul << (u >> 2)) : 0;
                bt[0] |= level >= 7 ? (1ul << (u >> 3)) : 0;
                bt[0] |= level >= 8 ? (1ul << (u >> 4)) : 0;
                rbf->insertbt((p >> 8) + (L - i >> 3) * 1000000007, bt);
                break;
            }
            else
            {
                bt[7 - u / 64] |= (1ul << u);
                u >>= 1;
                bt[3 - u / 64] |= (1ul << u);
                u >>= 1;
                bt[1] |= (1ul << u);
                u >>= 1;
                u |= 0xFFFFFFC0U;
                bt[0] |= (1ul << u);
                bt[0] |= (1ul << (u >> 1));
                bt[0] |= (1ul << (u >> 2));
                bt[0] |= (1ul << (u >> 3));
                bt[0] |= (1ul << (u >> 4));
            }
            level -= 8;
            rbf->insertbt((p >> 8) + (L - i >> 3) * 1000000007, bt);
            p >>= 8;
        }
    }

#else
    // Insertion without SIMD.

    // Insert prefixes between begin_level and stored_level.
    void Insert(uint64_t p, uint64_t begin_level)
    {
        uint32_t bt = 0;
        uint64_t p0 = p;
        p >>= (begin_level - 1) / 4 * 4;
        if (first_insert)
        {
            first_insert = false;
        }
        else if (p == last_p)
        {
            return;
        }
        last_p = p;
        int si = (begin_level - 1) / 4 * 4 + 4;
        int ei = (stored_level - 2) / 4 * 4 + 4;
        for (int i = si; i <= ei; i += 4)
        {
            int sl = begin_level - i + 4 > 0 ? begin_level - i + 4 : 1;
            int el = stored_level - 1 - i + 4;
            // Encode the prefix to a bitmap.
            bt = 0;
            uint64_t u = (p & 0x0000000FU) | 0xFFFFFFE0U;
            bt |= sl <= 1 && el >= 1 ? (1u << u) : 0;
            bt |= sl <= 2 && el >= 2 ? (1u << (u >> 1)) : 0;
            bt |= sl <= 3 && el >= 3 ? (1u << (u >> 2)) : 0;
            bt |= sl <= 4 && el >= 4 ? (1u << (u >> 3)) : 0;
            rbf->insertbt((p >> 4) + (L - i >> 2) * 1000000007, bt);
            p >>= 4;
        }
    }

    // Insert prefixes between 1 and stored_level (given manually).
    void Insert(uint64_t p)
    {
        uint64_t level = stored_level;
        uint32_t bt = 0;
        uint64_t p0 = p;
        for (int i = 4; i <= L; i += 4)
        {
            // Encode the prefix to a bitmap.
            bt = 0;
            uint64_t u = (p & 0x0000000FU) | 0xFFFFFFE0U;
            if (level <= 4)
            {
                bt |= level >= 1 ? (1u << u) : 0;
                bt |= level >= 2 ? (1u << (u >> 1)) : 0;
                bt |= level >= 3 ? (1u << (u >> 2)) : 0;
                bt |= level >= 4 ? (1u << (u >> 3)) : 0;
                rbf->insertbt((p >> 4) + (L - i >> 2) * 1000000007, bt);
                break;
            }
            else
            {
                bt |= (1u << u);
                bt |= (1u << (u >> 1));
                bt |= (1u << (u >> 2));
                bt |= (1u << (u >> 3));
            }
            level -= 4;
            rbf->insertbt((p >> 4) + (L - i >> 2) * 1000000007, bt);
            p >>= 4;
        }
    }
#endif

    // Check if there are any keys in [low,high].
    bool RangeQuery(uint64_t low, uint64_t high, uint64_t p = 0, uint64_t l = 1)
    {
        // Point query.
        if (low == high)
        {
            for (int i = 2; i <= L + 1; i++)
            {
                if (!QueryRBF(i - 1, (low >> L - i + 1)))
                {
                    return false;
                }
            }
            return true;
        }

        // Determine the left and right boundaries of the query.
        bool leftBorder = low == 0;
        bool rightBorder = high + 1 == 0;
        if (!leftBorder)
        {
            low -= 1;
        }
        if (!rightBorder)
        {
            high += 1;
        }

        // Determine the length of common prefix of low and high.
        uint64_t diff = low ^ high;
        uint64_t difflen;
        for (difflen = 0; difflen <= L; difflen += 1)
        {
            if (diff == 0)
            {
                break;
            }
            diff >>= 1;
        }
        l = (L - difflen) + 1;

        // Query for extra-stored prefixes to further reduce FPR.
        for (int i = 2; i <= l; i++)
        {
            if (!QueryRBF(i - 1, (low >> L - i + 1)))
            {
                return false;
            }
        }

        // Query [0,2^64-1].
        if (leftBorder && rightBorder)
        {
            if (verify(0, l + 1))
            {
                return true;
            }
            return verify(1ll << (L - l), l + 1);
        }

        // Query [low,2^64-1].
        else if (rightBorder)
        {
            for (int i = l; i <= L; i++)
            {
                if (i != l && !QueryRBF(i - 1, low >> L - i + 1))
                {
                    return false;
                }
                if (!(low >> L - i & 1))
                {
                    p = (i == 1 ? 0 : low >> L - i + 1 << L - i + 1) + (1ll << L - i);
                    if (verify(p, i + 1))
                    {
                        return true;
                    }
                }
                if (low + 1 >> L - i != low >> L - i)
                {
                    break;
                }
            }
        }

        // Query [0,high].
        else if (leftBorder)
        {
            for (int i = l; i <= L; i++)
            {
                if (i != l && !QueryRBF(i - 1, high >> L - i + 1))
                {
                    return false;
                }
                if (high >> L - i & 1)
                {
                    p = i == 1 ? 0 : high >> L - i + 1 << L - i + 1;
                    if (verify(p, i + 1))
                    {
                        return true;
                    }
                }
                if (high >> L - i << L - i == high)
                {
                    break;
                }
            }
        }

        // Query [low,high].
        else
        {
            // Verify the sub-ranges in the left part (close to low).
            for (int i = l + 1; i <= L; i++)
            {
                if (!QueryRBF(i - 1, low >> L - i + 1))
                {
                    break;
                }
                if (!(low >> L - i & 1))
                {
                    p = (i == 1 ? 0 : low >> L - i + 1 << L - i + 1) + (1ll << L - i);
                    if (verify(p, i + 1))
                    {
                        return true;
                    }
                }
                if (low + 1 >> L - i != low >> L - i)
                {
                    break;
                }
            }

            // Verify the sub-ranges in the right part (close to high).
            for (int i = l + 1; i <= L; i++)
            {
                if (!QueryRBF(i - 1, high >> L - i + 1))
                {
                    break;
                }
                if (high >> L - i & 1)
                {
                    p = i == 1 ? 0 : high >> L - i + 1 << L - i + 1;
                    if (verify(p, i + 1))
                    {
                        return true;
                    }
                }
                if (high >> L - i << L - i == high)
                {
                    break;
                }
            }
        }
        return false;
    }

    // Check if there are any keys in a sub-range.
    // p is the prefix corresponding to the sub-range, l is the length of the prefix.
    bool verify(uint64_t p, uint64_t l)
    {
        while (!prefixes.empty())
        {
            prefixes.pop();
        }
        prefixes.push(make_pair(p, l));
        while (!prefixes.empty())
        {
            p = prefixes.top().first;
            l = prefixes.top().second;
            prefixes.pop();
            if (!QueryRBF(l - 1, (p >> L - l + 1)))
            {
                // The current sub-range is empty.
                continue;
            }
            if (l > L)
            {
                // Reach a leaf node.
                return true;
            }
            prefixes.push(make_pair(p + (1ll << (L - l)), l + 1));
            prefixes.push(make_pair(p, l + 1));
        }
        return false;
    }

#ifdef USE_SIMD
    // Query Range Bloom Filter with SIMD.

    // Query Range Bloom Filter for the existence of the prefix.
    bool QueryRBF(uint64_t plen, uint64_t p)
    {
        if (L - plen >= stored_level || L - plen < start_level - 1)
        {
            return true;
        }
        uint64_t level = (plen - 1) & 7;
        uint64_t mask = 0xF << (level + 2);
        uint64_t *bt = rbf->querybt((p >> level + 1) + (plen - level - 1 >> 3) * 1000000007);
        uint32_t res;

        // Get the bit from the bitmap.
        if (level < 5)
        {
            res = (bt[0] >> (p & (~(1ll << level + 1)) | mask)) & 1;
        }
        else
        {
            p = p & ((1u << level + 1) - 1);
            res = (bt[(1u << level - 4) - 1 - (p >> 6)] >> p) & 1;
        }
        return res;
    }
#else
    // Query Range Bloom Filter without SIMD.

    // Query Range Bloom Filter for the existence of the prefix.
    bool QueryRBF(uint64_t plen, uint64_t p)
    {
        if (L - plen >= stored_level || L - plen < start_level - 1)
        {
            return true;
        }
        uint64_t level = (plen - 1) & 3;
        uint64_t mask = 0xF << (level + 2);

        // Get the bit from the bitmap.
        uint32_t res = (rbf->querybt((p >> level + 1) + (plen - level - 1 >> 2) * 1000000007) >> (p & (~(1ll << level + 1)) | mask)) & 1;
        return res;
    }
#endif
};