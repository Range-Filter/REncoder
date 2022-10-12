#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <unordered_set>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <random>
#include <limits>
#include <assert.h> /* assert */
#include <iostream>
#include <iomanip>
#include <map>
#include <random>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <string>
#include <string.h>
#include <unordered_map>
#include <iterator>
#include <algorithm>
#include <chrono>

using namespace std;
std::string UINT64Tostring(uint64_t s)
{
    char out[8];
    s = __builtin_bswap64(s);
    memcpy(out, &s, 8);
    return out;
}
// std::vector<unsigned long long> keys;
multimap<unsigned long long, unsigned long long> lower_bound_keys;
multimap<string, string> str_lower_bound_keys;
long double zipfian_sum;
std::vector<long double> zipfian_sum_prob; // Sum of probabilities

enum distribution_type
{
    uniform,
    normal,
    zipfian,
    correlated,
    file
};

void zipf_clear()
{
    zipfian_sum_prob.clear();
    lower_bound_keys.clear();
    str_lower_bound_keys.clear();
}
void zipf_prepare(double skew_degree, unsigned long long no_of_keys)
{
    // cout<<"preparing zipfian functions ..."<<endl;
    for (unsigned long long i = 1; i <= no_of_keys; i++)
        zipfian_sum += (1.0 / pow((double)i, skew_degree));
    zipfian_sum = 1.0 / zipfian_sum;
    long double sum_prob = 0.0;
    zipfian_sum_prob.reserve(no_of_keys);
    for (unsigned long long i = 1; i <= (unsigned long long)no_of_keys; i++)
    {
        sum_prob += zipfian_sum / pow((double)i, skew_degree);
        zipfian_sum_prob.push_back(sum_prob);
    }
    // cout<<"finish preparing zipfian functions"<<endl;
}

unsigned long long zipf_rank_gen(unsigned long long no_of_keys, double skew_degree, std::mt19937 &gen,
                                 uniform_int_distribution<unsigned long long> &uni_dist)
{
    (void)skew_degree;
    unsigned long long zipf_value;
    double z; // Uniform random number (0 < z < 1)

    do
    {
        unsigned long long number = uni_dist(gen);
        z = number / double(no_of_keys);
    } while ((z == 0) || (z == 1));
    //    for (unsigned long long i=1; i<=no_of_keys; i++)
    //    {
    //        if (zipfian_sum_prob[i-1] >= z)
    //        {
    //            zipf_value = i;
    //            break;
    //        }
    //    }
    //    cout<<zipf_value<<endl;
    // binary search
    std::vector<long double>::iterator first_pos_greater_than_z;
    first_pos_greater_than_z = std::lower_bound(zipfian_sum_prob.begin(), zipfian_sum_prob.end(), z);
    zipf_value = first_pos_greater_than_z - zipfian_sum_prob.begin() + 1;
    //    cout<<zipf_value<<endl;
    return zipf_value;
}

vector<unsigned long long> generateUniqueUnsortedKeys(unsigned long long no_of_keys, int key_length, bool print)
{
    // cout<<"generating unique unsorted keys ..."<<endl;
    std::random_device rd;     // Will be used to obtain a seed for the random number engine
    std::mt19937_64 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    // printf("%lu %lu\n", (unsigned long long)0ULL, (unsigned long long)((1ULL<<key_length)-100));
    uniform_int_distribution<unsigned long long> uni_dist(0, (1ULL << key_length) - 100);
    vector<unsigned long long> unsort_keys;
    unsort_keys.reserve(no_of_keys);
    // if the no_of_keys is very close to key space size, then we need to rethink the following code;
    unordered_map<unsigned long long, bool> hash;
    while (unsort_keys.size() < no_of_keys)
    {
        // unsigned long long number = gen();
        unsigned long long number = uni_dist(gen);
        if (print)
        {
            std::cout << number << std::endl;
        }
        if (hash.find(number) == hash.end())
        {
            unsort_keys.push_back(number);
            hash[number] = true;
        }
    }
    // cout<<"finish generating unique unsorted keys"<<endl;
    return unsort_keys;
}

std::vector<unsigned long long> generateKeysUniform(unsigned long long no_of_keys, int key_length, bool write_to_file, bool print, string data_type)
{
    std::vector<unsigned long long> keys;
    std::random_device rd;     // Will be used to obtain a seed for the random number engine
    std::mt19937_64 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    uniform_int_distribution<unsigned long long> uni_dist(0, (1ULL << key_length) - 100);
    keys.reserve(no_of_keys);
    while (keys.size() < no_of_keys)
    {
        // unsigned long long number = gen();
        unsigned long long number = uni_dist(gen);
        if (print)
        {
            std::cout << number << std::endl;
        }
        keys.push_back(number);
    }
    sort(keys.begin(), keys.end());
    // copy keys to file
    if (write_to_file)
    {
        const char *dir = "../data";
        mkdir(dir, 0777);
        std::ofstream output_file(dir + std::string("/key.txt"));
        if (data_type == "string")
        {
            vector<string> str_keys;
            for (unsigned long long key : keys)
            {
                str_keys.push_back(UINT64Tostring(key));
            }
            std::ostream_iterator<string> output_iterator(output_file, "\n");
            std::copy(str_keys.begin(), str_keys.end(), output_iterator);
        }
        else
        {
            std::ostream_iterator<unsigned long long> output_iterator(output_file, "\n");
            std::copy(keys.begin(), keys.end(), output_iterator);
        }
        output_file.close();
    }
    return keys;
}

std::vector<unsigned long long> generateKeysNormal(unsigned long long no_of_keys, int key_length, double standard_deviation, bool write_to_file, bool print, string data_type)
{
    std::vector<unsigned long long> keys;
    double mean = (1ULL << (key_length - 1));
    std::normal_distribution<double> nor_dist = std::normal_distribution<double>(mean, standard_deviation);
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    keys.reserve(no_of_keys);
    std::unordered_set<unsigned long long> key_set;
    double upper_b = mean * 2.0 + 1;
    while (keys.size() < no_of_keys)
    {
        // unsigned long long number = gen();
        unsigned long long number = (unsigned long long)nor_dist(generator);
        // cout << number << endl;
        if ((number >= 0.0) && (number <= upper_b))
        {
            if (print)
            {
                std::cout << number << std::endl;
            }
            if(key_set.find(number) == key_set.end())
            {
                key_set.insert(number);
                keys.push_back(number);
            }        
        }
    }
    sort(keys.begin(), keys.end());
    // copy keys to file
    if (write_to_file)
    {
        const char *dir = "../data";
        mkdir(dir, 0777);
        std::ofstream output_file(dir + std::string("/key.txt"));
        if (data_type == "string")
        {
            vector<string> str_keys;
            for (unsigned long long key : keys)
            {
                str_keys.push_back(UINT64Tostring(key));
            }
            std::ostream_iterator<string> output_iterator(output_file, "\n");
            std::copy(str_keys.begin(), str_keys.end(), output_iterator);
        }
        else
        {
            std::ostream_iterator<unsigned long long> output_iterator(output_file, "\n");
            std::copy(keys.begin(), keys.end(), output_iterator);
        }
        output_file.close();
    }
    return keys;
}

std::vector<unsigned long long> generateKeysZipf(unsigned long long no_of_keys, int key_length, double skew_degree, bool write_to_file, bool print, string data_type)
{
    zipf_clear();
    zipf_prepare(skew_degree, no_of_keys);
    std::vector<unsigned long long> keys;
    // Generate a uniform random number in (0,1)
    std::random_device rd;  // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    uniform_int_distribution<unsigned long long> uni_dist(0, no_of_keys);
    std::vector<unsigned long long> ranks = generateUniqueUnsortedKeys(no_of_keys, key_length, print);
    keys.reserve(no_of_keys);
    while (keys.size() < no_of_keys)
    {
        // unsigned long long number = gen();
        unsigned long long rank = zipf_rank_gen(no_of_keys, skew_degree, gen, uni_dist);
        unsigned long long number = ranks[rank];
        if (print)
        {
            std::cout << number << std::endl;
        }
        keys.push_back(number);
    }
    sort(keys.begin(), keys.end());
    // copy keys to file
    if (write_to_file)
    {
        const char *dir = "../data";
        mkdir(dir, 0777);
        std::ofstream output_file(dir + std::string("/key.txt"));
        if (data_type == "string")
        {
            vector<string> str_keys;
            for (unsigned long long key : keys)
            {
                str_keys.push_back(UINT64Tostring(key));
            }
            std::ostream_iterator<string> output_iterator(output_file, "\n");
            std::copy(str_keys.begin(), str_keys.end(), output_iterator);
        }
        else
        {
            std::ostream_iterator<unsigned long long> output_iterator(output_file, "\n");
            std::copy(keys.begin(), keys.end(), output_iterator);
        }
        output_file.close();
    }
    return keys;
}

void generateRangeQueries(int key_length, unsigned long long num_queries, unsigned long long max_range_size, double ratio_between_point_range, std::vector<unsigned long long> &keys, distribution_type query_dist_type, unsigned long long correlation_degree, bool print, double empty_rate, string data_type)
{
    (void)key_length;

    vector<unsigned long long> range_lefts;
    random_shuffle(keys.begin(), keys.end());
    if (query_dist_type == correlated)
    {
        range_lefts = keys;
        random_shuffle(range_lefts.begin(), range_lefts.end());
        range_lefts.resize(num_queries);
    }
    if (query_dist_type == uniform)
    {
        range_lefts = generateKeysUniform(num_queries, key_length, false, false, data_type);
    }
    if (query_dist_type == normal)
    {
        double standard_deviation = pow(2, key_length) * 0.01; // need to change
        range_lefts = generateKeysNormal(num_queries, key_length, standard_deviation, false, false, data_type);
    }

    unsigned long long range_size;
    unsigned long long left, no_of_querys, no_of_keys, max_range = 0;
    no_of_querys = range_lefts.size() - 1;
    no_of_keys = keys.size();
    srand(time(NULL));
    if (print)
    {
        std::cout << "\n\nRange queries" << std::endl;
    }
    std::random_device rd;  // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    uniform_int_distribution<unsigned long long> uni_dist(0, no_of_querys);
    int empty_queries = num_queries * empty_rate;
    int exist_queries = num_queries - empty_queries;
    for (unsigned long long i = 1; i <= empty_queries; i++) // all range queries
    {
        // left = keys[uni_dist(gen)]+1;
        if (query_dist_type == correlated)
        {
            // left = range_lefts[uni_dist(gen)] + correlation_degree;
            left = range_lefts[i - 1] + correlation_degree;
        }
        else
            // left = range_lefts[uni_dist(gen)];
            left = range_lefts[i - 1];
        double random_from_zero_to_one = uni_dist(gen) * 1.0 / no_of_querys;
        if (random_from_zero_to_one < ratio_between_point_range)
        {
            range_size = 1;
        }
        else
        {
            range_size = rand() % max_range_size + 1;
            if (range_size < 2)
                range_size = 2; // minimum range size is 2
        }
        // range_size = max_range_size;
        if (range_size > max_range)
        {
            max_range = range_size;
        }
        lower_bound_keys.insert(pair<unsigned long, unsigned long>(left, left + range_size));
        if (print)
        {
            std::cout << "query " << i + 1 << ": [" << left << ", " << left + range_size << "] (" << range_size << ")"
                      << std::endl;
        }
    }

    for (unsigned long long i = 1; i <= exist_queries; i++) // all range queries
    {
        double random_from_zero_to_one = uni_dist(gen) * 1.0 / no_of_querys;
        if (random_from_zero_to_one < ratio_between_point_range)
        {
            range_size = 1;
        }
        else
        {
            range_size = rand() % max_range_size + 1;
            if (range_size < 2)
                range_size = 2; // minimum range size is 2
        }
        // range_size = max_range_size;
        if (range_size > max_range)
        {
            max_range = range_size;
        }
        left = keys[rand() % no_of_keys] - rand() % range_size;
        lower_bound_keys.insert(pair<unsigned long, unsigned long>(left, left + range_size));
        if (print)
        {
            std::cout << "query " << i + 1 << ": [" << left << ", " << left + range_size << "] (" << range_size << ")"
                      << std::endl;
        }
    }
    // copy range bounds to file
    std::ofstream output_file1(std::string("../data/lower_bound.txt"));
    std::ofstream output_file2(std::string("../data/upper_bound.txt"));
    if (data_type == "string")
    {

        for (auto lower_bound_key : lower_bound_keys)
        {
            str_lower_bound_keys.insert({UINT64Tostring(lower_bound_key.first), UINT64Tostring(lower_bound_key.second)});
        }
        multimap<string, string>::iterator m_it;
        for (m_it = str_lower_bound_keys.begin(); m_it != str_lower_bound_keys.end(); m_it++)
        {
            // output_file1ULL<< m_it->first << "\t" << m_it->second << "\n";
            // std::cout << m_it->first << " " << m_it->second << std::endl;
            output_file1 << m_it->first << "\n";
            output_file2 << m_it->second << "\n";
        }
    }
    else
    {
        multimap<unsigned long long, unsigned long long>::iterator m_it;
        for (m_it = lower_bound_keys.begin(); m_it != lower_bound_keys.end(); m_it++)
        {
            // output_file1ULL<< m_it->first << "\t" << m_it->second << "\n";
            // std::cout << m_it->first << " " << m_it->second << std::endl;
            output_file1 << m_it->first << "\n";
            output_file2 << m_it->second << "\n";
        }
    }
    output_file1.close();
    output_file2.close();
}

int main(int argc, char *argv[])
{
    // default values
    unsigned long long entries_num = 1000000; // number of keys to insert
    int entries_length = 32;                  // number of bits per key,  Note: DO NOT TRY MORE THAN 63
    unsigned long long no_of_queries = 1000000;
    unsigned long long max_range_size = pow(2, 12); // max range is 4096
                                                    // change the distribution type
    string data_type = "int";
    distribution_type data_dist_type = normal;
    distribution_type query_dist_type = normal;
    string datafilename;
    double ratio_between_point_range = 0.0;
    double empty_rate = 1;
    unsigned long long correlation_degree = 32;
    // obtain values from parameters
    std::map<string, distribution_type> hash_string_to_enum;
    // uniform, normal, zipfian, adversary
    hash_string_to_enum["uniform"] = uniform;
    hash_string_to_enum["normal"] = normal;
    hash_string_to_enum["correlated"] = correlated;
    hash_string_to_enum["zipfian"] = zipfian;
    if (argc >= 10 && argc <= 11)
    {
        // argv[1] : entries_num
        // argv[2] : entries_length
        // argv[3] : no_of_queries
        // argv[4] : max_range_size
        // argv[5] : the ratio between point/range queries
        // argv[6] : data_dist_type
        // argv[7] : query_dist_type
        data_type = argv[1];
        entries_num = strtoul(argv[2], NULL, 0);
        entries_length = stoi(argv[3]);
        no_of_queries = strtoul(argv[4], NULL, 0);
        max_range_size = strtoul(argv[5], NULL, 0);
        ratio_between_point_range = atof(argv[6]);
        empty_rate = atof(argv[7]);
        if (hash_string_to_enum.count(argv[8]))
        {
            data_dist_type = hash_string_to_enum[argv[8]];
        }
        else
        {
            data_dist_type = file;
            datafilename = argv[8];
        }
        query_dist_type = hash_string_to_enum[argv[9]];
        if (query_dist_type == correlated)
        {
            if (argc == 11)
            {
                correlation_degree = strtoul(argv[10], NULL, 0);
            }
            else
            {
                cout << "warning: not enough parameters input... using the default ones" << endl;
            }
        }
    }
    else
    {
        cout << "warning: parameters setting not correct" << endl;
        exit(-1);
    }

    if (data_dist_type == uniform)
    {
        // generate data
        vector<unsigned long long> keys = generateKeysUniform(entries_num, entries_length, true, false, data_type);
        // generate range queries
        generateRangeQueries(entries_length, no_of_queries, max_range_size, ratio_between_point_range, keys, query_dist_type, correlation_degree, false, empty_rate, data_type);
        // generate data for normal distribution
    }
    else if (data_dist_type == normal)
    {

        // generate standard deviation for normal distribution
        double standard_deviation = pow(2, entries_length) * 1e-4; // need to change
        cout << standard_deviation << endl;
        vector<unsigned long long> keys = generateKeysNormal(entries_num, entries_length, standard_deviation, true, false, data_type);
        // generate range queries for normal distribution
        generateRangeQueries(entries_length, no_of_queries, max_range_size, ratio_between_point_range, keys, query_dist_type, correlation_degree, false, empty_rate, data_type);
    }
    else if (data_dist_type == zipfian)
    {
        double skewness_degree = 1;
        vector<unsigned long long> keys = generateKeysZipf(entries_num, entries_length, skewness_degree, true, false, data_type);
        // generate range queries for zipfian distribution
        generateRangeQueries(entries_length, no_of_queries, max_range_size, ratio_between_point_range, keys, query_dist_type, correlation_degree, false, empty_rate, data_type);
    }
    else if (data_dist_type == file)
    {
        ifstream in(datafilename, ios::binary);
        if (!in.is_open())
        {
            cerr << "unable to open file" << endl;
            exit(EXIT_FAILURE);
        }
        uint64_t size;
        in.read(reinterpret_cast<char *>(&size), sizeof(uint64_t));
        vector<unsigned long long> orikeys(size);
        in.read(reinterpret_cast<char *>(orikeys.data()), size * sizeof(uint64_t));
        sort(orikeys.begin(), orikeys.end());
        cout << orikeys.front() << " "<< orikeys.back() << endl;
        // random_shuffle(orikeys.begin(), orikeys.end());
        unordered_set<unsigned long long> key_set;
        vector<unsigned long long> keys;
        for (unsigned long long key : orikeys)
        {
            if(key_set.find(key) != key_set.end())
            {
                continue;
            }
            key_set.insert(key);
            keys.push_back(key);
            if(keys.size() == entries_num)
            {
                break;
            }
        }
        // sort(keys.begin(), keys.end());
        const char *dir = "../data";
        mkdir(dir, 0777);
        std::ofstream output_file(dir + std::string("/key.txt"));
        std::ostream_iterator<unsigned long long> output_iterator(output_file, "\n");
        std::copy(keys.begin(), keys.end(), output_iterator);
        output_file.close();
        generateRangeQueries(entries_length, no_of_queries, max_range_size, ratio_between_point_range, keys, query_dist_type, correlation_degree, false, empty_rate, data_type);
    }
    // cout << (1ULL << entries_length) - 100 << endl;
    return 0;
}
