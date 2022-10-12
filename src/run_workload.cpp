#include <bits/stdc++.h>
#include <immintrin.h>
#include <chrono>
#include "BOBHash32.h"
#include "RBF.h"
#include "REncoder.h"

using namespace std;
long long cache_hit = 0;
long long query_count = 0;
stack<pair<uint64_t, uint64_t>> psd;
RENCODER rencoder;
string KEY_TYPE;
int BPK;
int QL;
int IF_SELF_ADAPT;
int STEP;
int MAX_ITEM_NUM;
int rangeQueryNum;
vector<uint64_t> keys;
vector<pair<uint64_t, uint64_t>> range_queries;
set<uint64_t> key_set;
ofstream testData;

uint64_t stringToUINT64(const std::string s)
{
    uint64_t out = 0;
    memcpy(&out, s.c_str(), 8);
    out = __builtin_bswap64(out);
    return out;
}

void LoadKey()
{
    ifstream keyFile;
    keyFile.open("../data/key.txt");
    uint64_t key;
    string keystring;
    vector<string> kkk;
    for (int i = 0; i < MAX_ITEM_NUM; i++)
    {
        if (KEY_TYPE == "string")
        {
            keyFile >> keystring;
            kkk.push_back(keystring);
            key = stringToUINT64(keystring);
        }
        else
        {
            keyFile >> key;
        }
        keys[i] = key;
    }
    sort(keys.begin(), keys.end());
    // cout << keys.front() << endl;
    // cout << keys.back() << endl;
    for (int i = 0; i < MAX_ITEM_NUM; i++)
    {
        key_set.insert(keys[i]);
    }
    keyFile.close();
}
void LoadQuery()
{
    ifstream leftFile, rightFile;
    leftFile.open("../data/lower_bound.txt");
    rightFile.open("../data/upper_bound.txt");
    uint64_t lower;
    string lowerstring;
    uint64_t upper;
    string upperstring;
    for (int i = 0; i < rangeQueryNum; i++)
    {
        if (KEY_TYPE == "string")
        {
            leftFile >> lowerstring;
            lower = stringToUINT64(lowerstring);
            rightFile >> upperstring;
            upper = stringToUINT64(upperstring);
        }
        else
        {
            leftFile >> lower;
            rightFile >> upper;
        }
        upper -= 1;
        range_queries[i] = make_pair(lower, upper);
        // if (i==0)
        // {
        //     cout << range_queries[0].first << " " <<range_queries[0].second << endl;
        // }
    }
    random_shuffle(range_queries.begin(), range_queries.end());
    leftFile.close();
    rightFile.close();
}
void PrintEmptyRate()
{
    set<uint64_t>::iterator iter;
    int empty_cnt = 0;
    for (int i = 0; i < rangeQueryNum; i++)
    {
        iter = key_set.lower_bound(range_queries[i].first);
        if (!(iter != key_set.end() && (*iter) <= range_queries[i].second))
        {
            empty_cnt++;
        }
    }
    printf("Empty Rate: %lf\n", empty_cnt / 1.0 / rangeQueryNum);
}
void RunTest()
{
    int repeat = 1;
    int hash_num = 3;
    range_queries.clear();
    key_set.clear();
    LoadKey();
    LoadQuery();
    PrintEmptyRate();
    timespec time1, time2;
    long long resns, QueryTestNum = 0;
    double th;
    cout << "Insert " << MAX_ITEM_NUM << " items" << endl;
    uint64_t memory = (uint64_t)MAX_ITEM_NUM * BPK;
    cout << "Hash Num: " << hash_num << endl;
    cout << "Bits per Key: " << BPK << endl;
    rencoder.init(memory, hash_num, 64, QL);

    // Insertion
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
    pair<uint8_t *, size_t> ser = rencoder.serialize();

    // Range query (Filter Throughput)
    cache_hit = 0;
    query_count = 0;
    QueryTestNum = 0;
    auto start = chrono::high_resolution_clock::now();
    int res = 0;
    for (int k = 1; k <= repeat; k++)
        for (uint32_t i = 0; i < rangeQueryNum; i++)
        {
            uint64_t l = range_queries[i].first, r = range_queries[i].second;
            QueryTestNum++;
            res ^= rencoder.RangeQuery(l, r);
        }
    auto end = chrono::high_resolution_clock::now();
    uint64_t duration = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    th = (double)1000.0 * QueryTestNum / duration;
    printf("Filter Throughput:\t %lf Mops/s\n", th);
    printf("query cnt: %lld; cache hit: %lld; hit rate: %lf\n", query_count / repeat, cache_hit / repeat, cache_hit / 1.0 / query_count);

    // False Positive Rate
    double FPR = 0, FPRQSUM = 0;
    for (uint32_t i = 0; i < rangeQueryNum; i++)
    {
        uint64_t l = range_queries[i].first, r = range_queries[i].second;
        auto iter = key_set.lower_bound(range_queries[i].first);
        if (iter != key_set.end() && (*iter) <= range_queries[i].second)
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
    if (argc < 7)
    {
        cout << "error arg : <key_type> <key_num> <query_num> <BPK> <IF_SELF_ADAPT> <SELF_ADAPT_STEP/STORE_LEVEL>" << endl;
        return 0;
    }
    KEY_TYPE = argv[1];
    MAX_ITEM_NUM = atoi(argv[2]);
    rangeQueryNum = atoi(argv[3]);
    BPK = atoi(argv[4]);
    IF_SELF_ADAPT = atoi(argv[5]);
    if (IF_SELF_ADAPT)
    {
        STEP = atoi(argv[6]);
    }
    else
    {
        QL = atoi(argv[6]);
    }
    keys.resize(MAX_ITEM_NUM);
    range_queries.resize(rangeQueryNum);
    uint64_t memory = (uint64_t)MAX_ITEM_NUM * BPK;
    
    RunTest();

    return 0;
}