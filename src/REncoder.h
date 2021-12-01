// #include "sBF.h"

using namespace std;

extern int testt;
extern stack<pair<uint64_t, uint64_t>> psr;
extern stack<pair<uint64_t, uint64_t>> psd;
class RENCODER
{
public:
    static const int
        MAX_BF_NUM = 64;
    int
        count,
        hash_num;
    uint64_t memory;
    int k_tree = 4;
    uint64_t L;
    RangeBloomfilter rbf;
    int query_level;
    void init(uint64_t _memory,
              int _hash_num,
              uint64_t _L,
              int _query_level = 64)
    {
        count = 0;
        memory = _memory,
        hash_num = _hash_num,
        L = _L;
        query_level = _query_level;
        if (MAX_BF_NUM < _L)
        {
            cout << "REncoder memory initialization fail 1.\n";
            exit(-1);
        }
        rbf.init(_memory, hash_num);
    }
    void setQueryLevel(int _query_level)
    {
        query_level = _query_level;
    }
    int Insert_SelfAdapt(vector<uint64_t> keys, int step)
    {
        int num = keys.size();
        double onerate = 0;
        double pre_onerate = 0;
        int start_level = 1;
        while (true)
        {
            query_level = start_level + step;
            for (int i = 0; i < num; i++)
            {
                Insert(keys[i], start_level);
            }
            for (int i = 0; i < rbf.counter_num; i++)
            {
                uint64_t array = rbf.array[0][i];
                for (int j = 0; j < 8; j++)
                {
                    onerate += (array & 1);
                    array >>= 1;
                }
            }
            onerate /= rbf.bit_per_row;
            if (0.5 - onerate < 0.065 || onerate >= 0.5 || query_level==65)
            {
                break;
            }
            start_level = query_level;
            pre_onerate = onerate;
            onerate = 0;
        }
        query_level -= 1;
        return query_level;
    }
    void Insert(uint64_t p, uint64_t start_level)
    {
        uint64_t bt[8] = {0};
        uint64_t p0 = p;
        p >>= (start_level - 1) / 8 * 8;
        int si = (start_level - 1) / 8 * 8 + 8;
        int ei = (query_level - 2) / 8 * 8 + 8;
        for (int i = si; i <= ei; i += 8)
        {
            int sl = start_level - i + 8 > 0 ? start_level - i + 8 : 1;
            int el = query_level - 1 - i + 8;
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
            rbf.insertbt((p >> 8) + (L - i >> 3) * 1000000007, bt);
            p >>= 8;
        }
    }
    void Insert(uint64_t p)
    {
        uint64_t level = query_level;
        uint64_t bt[8] = {0};
        uint64_t p0 = p;
        for (int i = 8; i <= L; i += 8)
        {
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
                rbf.insertbt((p >> 8) + (L - i >> 3) * 1000000007, bt);
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
            rbf.insertbt((p >> 8) + (L - i >> 3) * 1000000007, bt);
            p >>= 8;
        }
    }

    bool RangeQuery(uint64_t low, uint64_t high, uint64_t p = 0, uint64_t l = 1)
    {
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
        for (int i = 2; i <= l; i++)
        {
            if (!QueryRBF(i - 1, (low >> L - i + 1)))
            {
                return false;
            }
        }
        if (leftBorder && rightBorder)
        {
            if (doubt(0, l + 1))
            {
                return true;
            }
            return doubt(1ll << (L - l), l + 1);
        }
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
                    if (doubt(p, i + 1))
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
                    if (doubt(p, i + 1))
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
        else
        {
            for (int i = l + 1; i <= L; i++)
            {
                if (!QueryRBF(i - 1, low >> L - i + 1))
                {
                    break;
                }
                if (!(low >> L - i & 1))
                {
                    p = (i == 1 ? 0 : low >> L - i + 1 << L - i + 1) + (1ll << L - i);
                    if (doubt(p, i + 1))
                    {
                        return true;
                    }
                }
                if (low + 1 >> L - i != low >> L - i)
                {
                    break;
                }
            }
            for (int i = l + 1; i <= L; i++)
            {
                if (!QueryRBF(i - 1, high >> L - i + 1))
                {
                    break;
                }
                if (high >> L - i & 1)
                {
                    p = i == 1 ? 0 : high >> L - i + 1 << L - i + 1;
                    if (doubt(p, i + 1))
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
    bool doubt(uint64_t p, uint64_t l)
    {
        while (!psd.empty())
        {
            psd.pop();
        }
        psd.push(make_pair(p, l));
        while (!psd.empty())
        {
            p = psd.top().first;
            l = psd.top().second;
            psd.pop();
            if (!QueryRBF(l - 1, (p >> L - l + 1)))
            {
                continue;
            }
            if (l > L)
            {
                return true;
            }
            psd.push(make_pair(p + (1ll << (L - l)), l + 1));
            psd.push(make_pair(p, l + 1));
        }
        return false;
    }
    bool QueryRBF(uint64_t plen, uint64_t p)
    {
        if (L - plen >= query_level)
        {
            return true;
        }
        uint64_t level = (plen - 1) & 7;
        uint64_t mask = 0xF << (level + 2);
        uint64_t *bt = rbf.querybt((p >> level + 1) + (plen - level - 1 >> 3) * 1000000007);
        uint32_t res;
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
};