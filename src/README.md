# Source Codes
Codes of REncoder.
## Build
- `make`
## Run
- `./run_workload(_simd) <key_type> <key_num> <query_num> <BPK> <IF_SELF_ADAPT> <SELF_ADAPT_STEP/STORE_LEVEL>`: 

For example: 

`./run_workload(_simd) int 50000000 10000000 26 1 1` means the key type is int, the number of keys is 50000000, the number of queries is 10000000, allocate 26 bits to each key, employ self-adapt insertion, the step of self-adapt insertion is 1;

`./run_workload(_simd) string 200000000 10000000 24 0 8` means the key type is string, the number of keys is 200000000, the number of queries is 10000000, allocate 24 bits to each key, employ regular insertion, the number of stored levels is 8; 