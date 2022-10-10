# cd ~/Range-Filter/REncoder/workload_gen
# ./workload_gen int 1000000 64 1000000 64 0.0 1 /share/SOSD_data/data/osm_cellids_200M_uint64 uniform
# cd ~/Range-Filter/Bloomier
# ./main int 1000000 1000000 22
# cd ~/Range-Filter/REncoder/src
# ./run_workload_simd int 1000000 1000000 95 1 1
# cd ~/Range-Filter/Rosetta/standalone/
# ./run_workload ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt 95 DST -1 64 0

# cd ~/Range-Filter/REncoder/workload_gen
# ./workload_gen int 1000000 64 1000000 64 0.0 1 /share/SOSD_data/data/books_200M_uint64 uniform
# cd ~/Range-Filter/Bloomier
# ./main int 1000000 1000000 22
# cd ~/Range-Filter/REncoder/src
# ./run_workload_simd int 1000000 1000000 106 1 1
# cd ~/Range-Filter/Rosetta/standalone/
# ./run_workload ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt 106 DST -1 64 0

# cd ~/Range-Filter/REncoder/workload_gen
# ./workload_gen int 1000000 64 1000000 64 0.0 1 /share/SOSD_data/data/fb_200M_uint64 uniform
# cd ~/Range-Filter/Bloomier
# ./main int 1000000 1000000 22
# cd ~/Range-Filter/REncoder/src
# ./run_workload_simd int 1000000 1000000 38 1 1
# cd ~/Range-Filter/Rosetta/standalone/
# ./run_workload ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt 38 DST -1 64 0

cd ~/Range-Filter/REncoder/workload_gen
./workload_gen int 1000000 64 1000000 64 0.0 1 /share/SOSD_data/data/wiki_ts_200M_uint64 uniform
cd ~/Range-Filter/Bloomier
./main int 1000000 1000000 22
cd ~/Range-Filter/REncoder/src
./run_workload_simd int 1000000 1000000 17 1 1
cd ~/Range-Filter/Rosetta/standalone/
./run_workload ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt 17 DST -1 64 0