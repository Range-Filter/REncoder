#include <bits/stdc++.h>
#include <immintrin.h>
#include "BOBHash32.h"
#include "RBF.h"
#include "REncoder.h"

using namespace std;

#define MAX_SET_SIZE (1024*1024)
long long cache_hit = 0;
long long query_count = 0;
stack<pair<uint64_t, uint64_t>> psr;
stack<pair<uint64_t, uint64_t>> psd;
string filename;
RENCODER rencoder;
int BPK;
int QL;
int IF_SELF_ADAPT;
int STEP;
int MAX_ITEM_NUM;
int rangeQueryNum;
vector<uint64_t> keys;
struct Lookup
{
    uint64_t key;
    uint64_t value;
};
vector<Lookup> Lookups;
vector<uint64_t> querys;
vector<uint64_t> uppers;
set<uint64_t> key_set;
map<uint64_t, bool> query_map;
vector<uint64_t> keybuffer;
vector<uint64_t> maxkeys;
ofstream testData;
ifstream intestData;
int pr = 0, testt = 0;

void LoadKey()
{
    intestData.open("../data/key.txt");
    uint64_t key;
    for (int i = 0; i < MAX_ITEM_NUM; i++)
    {
        intestData >> key;
        keys[i] = key;
        key_set.insert(keys[i]);
        if (key_set.size() % MAX_SET_SIZE == 0)
        {
            maxkeys.push_back(key);
        }
    }
    intestData.close();
}
void LoadQuery()
{
    intestData.open("../data/lower_bound.txt");
    uint64_t query;
    for (int i = 0; i < rangeQueryNum; i++)
    {
        intestData >> query;
        querys[i] = query;
    }
    intestData.close();
    intestData.open("../data/upper_bound.txt");
    uint64_t upper;
    for (int i = 0; i < rangeQueryNum; i++)
    {
        intestData >> upper;
        uppers[i] = upper - 1;
    }
    intestData.close();
}
void GenQueryMap()
{
    LoadKey();
    LoadQuery();
    set<uint64_t>::iterator iter;
    for (int i = 0; i < rangeQueryNum; i++)
    {
        testData << querys[i] << " ";
        iter = key_set.lower_bound(querys[i]);
        if (iter != key_set.end() && (*iter) <= uppers[i])
        {
            query_map.insert(map<uint64_t, bool>::value_type(querys[i], true));
        }
        else
        {
            query_map.insert(map<uint64_t, bool>::value_type(querys[i], false));
        }
    }
    testData.close();
}
uint64_t GetQueryLevel()
{
    uint64_t maxrsize = 0;
    for (uint32_t i = 0; i < rangeQueryNum; i++)
    {
        if ((size_t)rand() % 10000 < 10000 * 10000 / rangeQueryNum)
        {
            uint64_t l = querys[i], r = uppers[i];
            if (r - l > maxrsize)
            {
                maxrsize = r - l;
            }
        }
    }
    uint64_t level = log(maxrsize + 1) / log(2) + 1;
    return level;
}
void RunTest()
{
    Lookups.clear();
    key_set.clear();
    query_map.clear();
    GenQueryMap();
    timespec time1, time2;
    long long resns, kk, kkk = 1, QueryTestNum = 0;
    double th;
    cout << "Insert " << MAX_ITEM_NUM << " items" << endl;
    uint64_t query_level = GetQueryLevel();
    uint64_t memory = (uint64_t)MAX_ITEM_NUM * BPK;
    int hash_num = 3;
    cout << "Hash Num: " << hash_num << endl;
    cout << "Bits per Key: " << BPK << endl;
    rencoder.init(memory, hash_num, 64, QL);
    kk = kkk;

    // Insertion
    for (int k = 1; k <= kk; k++)
    {
        if (IF_SELF_ADAPT)
        {
            int true_level = rencoder.Insert_SelfAdapt(keys, STEP);
            cout << "L_s: " << true_level << endl;
        }
        else
        {
            for (int i = 0; i < MAX_ITEM_NUM; i++)
            {
                rencoder.Insert(keys[i]);
            }
            cout << "L_s: " << QL << endl;
        }
    }

    // Range query (Filter Throughput)
    cache_hit = 0;
    query_count = 0;
    clock_gettime(CLOCK_MONOTONIC, &time1);
    kk = kkk;
    QueryTestNum = 0;
    int ss = 0;
    for (int k = 1; k <= kk; k++)
        for (uint32_t i = 0; i < rangeQueryNum; i++)
        {
            uint64_t l = querys[i], r = uppers[i];
            QueryTestNum++;
            ss ^= rencoder.RangeQuery(l, r);
        }
    clock_gettime(CLOCK_MONOTONIC, &time2);
    resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
    th = (double)1000.0 * QueryTestNum / resns;
    printf("Filter Throughput:\t %lf Mops/s\n", th);

    // Range Query (Overall Throughput)
    double fp = 0, empty = 0;
    bool positive, full;
    clock_gettime(CLOCK_MONOTONIC, &time1);
    kk = kkk;
    QueryTestNum = 0;
    for (int k = 1; k <= kk; k++)
        for (uint32_t i = 0; i < rangeQueryNum; i++)
        {
            positive = false, full = false;
            uint64_t l = querys[i], r = uppers[i];
            QueryTestNum++;
            positive = rencoder.RangeQuery(l, r);
            if (positive)
            {
                size_t start;
                auto iter = lower_bound(maxkeys.begin(), maxkeys.end(), l);
                if (iter == maxkeys.begin())
                {
                    start = 0;
                }
                else
                {
                    iter--;
                    start = *iter;
                }
                auto iter2 = key_set.lower_bound(start);
                while (iter2 != key_set.end())
                {
                    if ((*iter2) >= l)
                    {
                        if ((*iter2) <= r)
                        {
                            full = true;
                        }
                        break;
                    }
                    iter2++;
                }
            }
            if (positive && !full)
            {
                ++fp;
            }

            if (!full)
            {
                ++empty;
            }
        }
    clock_gettime(CLOCK_MONOTONIC, &time2);
    resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
    th = (double)1000.0 * QueryTestNum / resns;
    printf("Overall Throughput:\t %lf Mops/s\n", th);

    // False Positive Rate
    double FPR = 0, FPRQSUM = 0;
    FPR = 0, FPRQSUM = 0;
    for (uint32_t i = 0; i < rangeQueryNum; i++)
    {
        uint64_t l = querys[i], r = uppers[i];
        if (query_map[querys[i]])
        {
            if (!rencoder.RangeQuery(l, r))
            {
                cout << "Range Query Error";
                exit(-1);
            }
            continue;
        }
        FPRQSUM++;
        if (rencoder.RangeQuery(l, r))
        {
            FPR++;
        }
    }
    printf("False Positive Rate:\t %lf\n", FPR / FPRQSUM);
}
int main(int argc, char *argv[])
{
    if (argc < 6)
    {
        cout << "error arg : <key_num> <query_num> <BPK> <IF_SELF_ADAPT> <SELF_ADAPT_STEP/STORE_LEVEL>" << endl;
        return 0;
    }
    MAX_ITEM_NUM = atoi(argv[1]);
    rangeQueryNum = atoi(argv[2]);
    BPK = atoi(argv[3]);
    IF_SELF_ADAPT = atoi(argv[4]);
    if (IF_SELF_ADAPT)
    {
        STEP = atoi(argv[5]);
    }
    else
    {
        QL = atoi(argv[5]);
    }
    keys.resize(MAX_ITEM_NUM);
    Lookups.resize(rangeQueryNum);
    querys.resize(rangeQueryNum);
    uppers.resize(rangeQueryNum);
    uint64_t memory = (uint64_t)MAX_ITEM_NUM * BPK;
    RunTest();

    return 0;
}