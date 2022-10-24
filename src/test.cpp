#include <bits/stdc++.h>
#include <immintrin.h>
#include <chrono>
#include "BOBHash32.h"
#include "RBF.h"
#include "REncoder.h"
using namespace std;

// For pretty print.
static const char *kGreen = "\033[0;32m";
static const char *kRed = "\033[0;31m";
static const char *kWhite = "\033[0;0m";

// Number of queries to RBF.
long long query_count = 0;
// Number of cache hits (the queried prefix is the same as the last queried one).
long long cache_hit = 0;

// Parameters of REncoder.
int BPK;
int HASH_NUM;
int STORED_LEVELS;
int IS_SELF_ADAPT;
int SELF_ADAPT_STEP;

RENCODER rencoder;

// Parameters of workload.
string KEY_TYPE;
int TOTAL_ITEM_NUM;
int RANGE_QUERY_NUM;

vector<uint64_t> keys;
vector<pair<uint64_t, uint64_t>> range_queries;
set<uint64_t> key_set;

// Convert string to unsigned 64-bit integer.
uint64_t stringToUINT64(const std::string s)
{
    uint64_t out = 0;
    memcpy(&out, s.c_str(), 8);
    out = __builtin_bswap64(out);
    return out;
}

// Load keys from file.
void LoadKey()
{
    ifstream keyFile;
    keyFile.open("../data/key.txt");
    uint64_t key;
    string keystring;
    for (int i = 0; i < TOTAL_ITEM_NUM; i++)
    {
        if (KEY_TYPE == "string")
        {
            keyFile >> keystring;
            key = stringToUINT64(keystring);
        }
        else
        {
            keyFile >> key;
        }
        keys[i] = key;
    }
    sort(keys.begin(), keys.end());
    for (int i = 0; i < TOTAL_ITEM_NUM; i++)
    {
        key_set.insert(keys[i]);
    }
    keyFile.close();
}

// Load range queries from file.
void LoadQuery()
{
    ifstream leftFile, rightFile;
    // Load the left and right boundaries of the range queries separately.
    leftFile.open("../data/lower_bound.txt");
    rightFile.open("../data/upper_bound.txt");
    uint64_t lower;
    string lowerstring;
    uint64_t upper;
    string upperstring;
    for (int i = 0; i < RANGE_QUERY_NUM; i++)
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
    }
    // Shuffle to make the queries random.
    random_shuffle(range_queries.begin(), range_queries.end());
    leftFile.close();
    rightFile.close();
}

// Get the proportion of empty queries in all queries.
double GetEmptyRate()
{
    set<uint64_t>::iterator iter;
    int empty_cnt = 0;
    for (int i = 0; i < RANGE_QUERY_NUM; i++)
    {
        iter = key_set.lower_bound(range_queries[i].first);
        if (!(iter != key_set.end() && (*iter) <= range_queries[i].second))
        {
            empty_cnt++;
        }
    }
    return empty_cnt / 1.0 / RANGE_QUERY_NUM;
}

// Run the workload and print the metrics.
void RunTest()
{
    range_queries.clear();
    key_set.clear();

    LoadKey();
    LoadQuery();
    double empty_rate = GetEmptyRate();

    // Initialize REncoder.
    uint64_t memory = (uint64_t)TOTAL_ITEM_NUM * BPK;
    rencoder.init(memory, HASH_NUM, 64, STORED_LEVELS);

    // Insert keys into REncoder.
    if (IS_SELF_ADAPT)
    {
        // Self-adaptively choose the optimal number of stored levels.
        int true_level = rencoder.Insert_SelfAdapt(keys, SELF_ADAPT_STEP);
        printf("Key Type = %6s\tEmpty Rate = %lf\tSelf-adapt = Yes\t", KEY_TYPE.c_str(), empty_rate);
    }
    else
    {
        // Choose the number of stored levels given by arguments.
        for (int i = 0; i < TOTAL_ITEM_NUM; i++)
        {
            rencoder.Insert(keys[i]);
        }
        printf("Key Type = %6s\tEmpty Rate = %lf\tSelf-adapt = No \t", KEY_TYPE.c_str(), empty_rate);
    }

    // Test for false negatives.
    for (uint32_t i = 0; i < RANGE_QUERY_NUM; i++)
    {
        uint64_t l = range_queries[i].first, r = range_queries[i].second;
        auto iter = key_set.lower_bound(range_queries[i].first);
        if (iter != key_set.end() && (*iter) <= range_queries[i].second)
        {
            if (!rencoder.RangeQuery(l, r))
            {
                printf("%sFail\n%s", kRed, kWhite);
                exit(-1);
            }
            continue;
        }
    }
    printf("%sPass\n%s", kGreen, kWhite);
}

int main(int argc, char *argv[])
{
    if (argc < 7)
    {
        cout << "error arg : <KEY_TYPE> <TOTAL_ITEM_NUM> <RANGE_QUERY_NUM> <BPK> <IS_SELF_ADAPT> <SELF_ADAPT_STEP/STORED_LEVELS>" << endl;
        return 0;
    }

    KEY_TYPE = argv[1];              // Type of keys.
    TOTAL_ITEM_NUM = atoi(argv[2]);  // Number of keys.
    RANGE_QUERY_NUM = atoi(argv[3]); // Number of range queries.
    BPK = atoi(argv[4]);             // Number of bits allocated for each key in REncoder.
    IS_SELF_ADAPT = atoi(argv[5]);   // Whether to insert keys self-adaptively.
    if (IS_SELF_ADAPT)
    {
        SELF_ADAPT_STEP = atoi(argv[6]); // Number of prefixes inserted for each key in each round.
    }
    else
    {
        STORED_LEVELS = atoi(argv[6]); // Number of stored levels given manually.
    }
    HASH_NUM = 3; // Number of hash functions used in Range Bloom Filter.
    keys.resize(TOTAL_ITEM_NUM);
    range_queries.resize(RANGE_QUERY_NUM);

    RunTest();

    return 0;
}