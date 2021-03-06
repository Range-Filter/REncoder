# Source Codes
Codes for generating dataset.
## Build
- `make`
## Run
- `./workload <key_num> <key_length> <query_num> <max_range_size> <ratio_of_point_query> <key_dist_type> <query_dist_type> (<correlation_degree>)`

For example: 

`./workload 50000000 64 10000000 32 0.0 uniform uniform` means the number of keys is 50000000, the length of keys is 64 bits, the number of queries is 10000000, the max range size is 32, ratio of point query is 0 (all range queries), the distribution of keys is uniform, the distribution of queries is uniform;

`./workload 50000000 64 10000000 32 0.0 uniform correlated 32` means the number of keys is 50000000, the length of keys is 64 bits, the number of queries is 10000000, the max range size is 32, ratio of point queries is 0 (all range queries), the distribution of keys is uniform, the distribution of queries is correlated and the correlation degree is 32 (lower bound of query = key + 32);

`./workload 200000000 64 10000000 32 0.0 ./datapath uniform` means the number of keys is 50000000, the length of keys is 64 bits, the number of queries is 10000000, the max range size is 32, ratio of point queries is 0 (all range queries), the keys are from file "./datapath", the distribution of queries is uniform;