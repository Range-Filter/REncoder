#!bin/bash
# #SIMD
# cd ~/Range-Filter/REncoder/workload
# ./workload int 50000000 64 10000000 64 0.0 0 uniform uniform
# cd ~/Range-Filter/REncoder/src
# for ((i=10; i<=26; i+=2))
# do
# echo './run_workload int 50000000 10000000 1 1' + $i
# ./run_workload int 50000000 10000000 $i 1 1
# done
# for ((i=10; i<=26; i+=2))
# do
# echo './run_workload_simd int 50000000 10000000 1 1' + $i
# ./run_workload_simd int 50000000 10000000 $i 1 1
# done
# cd ~/Range-Filter/Rosetta/standalone
# for ((i=10; i<=26; i+=2))
# do
# echo './run_workload ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt DST -1 64 0' + $i
# ./run_workload ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt $i DST -1 64 0
# done

#empty rate
# for e in 0 0.25 0.5 0.75 1; do
#     cd ~/Range-Filter/REncoder/workload
#     ./workload 50000000 64 10000000 64 0.0 $e uniform uniform
#     cd ~/Range-Filter/REncoder/src
#     for ((i=10; i<=26; i+=2))
#     do
#     echo './run_workload int 50000000 10000000 1 1' + $i
#     ./run_workload int 50000000 10000000 $i 1 1
#     done
#     for ((i=10; i<=26; i+=2))
#     do
#     echo './run_workload_simd int 50000000 10000000 1 1' + $i
#     ./run_workload_simd int 50000000 10000000 $i 1 1
#     done
#     cd ~/Range-Filter/Rosetta/standalone
#     for ((i=10; i<=26; i+=2))
#     do
#     echo './run_workload ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt DST -1 64 0' + $i
#     ./run_workload ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt $i DST -1 64 0
#     done
# done
# #empty rate2
# for e in 0 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 0.99; do
#     cd ~/Range-Filter/REncoder/workload
#     ./workload int 50000000 64 10000000 64 0.0 $e uniform uniform
#     cd ~/Range-Filter/REncoder/src
#     echo './run_workload int 50000000 10000000 22 1 1'
#     ./run_workload int 50000000 10000000 22 1 1
#     echo './run_workload_simd int 50000000 10000000 22 1 1'
#     ./run_workload_simd int 50000000 10000000 22 1 1
#     cd ~/Range-Filter/Rosetta/standalone
#     echo './run_workload ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt 22 DST -1 64 0'
#     ./run_workload ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt 22 DST -1 64 0
#     echo './run_workload ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt 22 SuRF 6 6'
#     ./run_workload ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt 22 SuRF 6 6
# done

# #uniform+uniform+2-64
# cd ~/Range-Filter/REncoder/workload
# ./workload int 50000000 64 10000000 64 0.0 1 uniform uniform
# cd ~/Range-Filter/REncoder/src
# for ((i=10; i<=26; i+=2))
# do
# echo './run_workload int 50000000 10000000 1 1' + $i
# ./run_workload int 50000000 10000000 $i 1 1
# done
# for ((i=10; i<=26; i+=2))
# do
# echo './run_workload_simd int 50000000 10000000 1 1' + $i
# ./run_workload_simd int 50000000 10000000 $i 1 1
# done
# cd ~/Range-Filter/Rosetta/standalone
# for ((i=10; i<=26; i+=2))
# do
# echo './run_workload ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt DST -1 64 0' + $i
# ./run_workload ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt $i DST -1 64 0
# done
# for ((i=0; i<=8; i+=1))
# do
# echo './run_workload ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt SuRF' + $i
# ./run_workload ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt 22 SuRF $i $i
# done

#TODO
# #uniform+uniform+point
# cd ~/Range-Filter/REncoder/workload
# ./workload int 50000000 64 10000000 64 0.0 1 uniform uniform
# cd ~/Range-Filter/REncoder/src
# for ((i=10; i<=26; i+=2))
# do
# echo './bf_workload int 50000000 10000000 ' + $i
# ./bf_workload int 50000000 10000000 $i
# done
# for ((i=10; i<=26; i+=2))
# do
# echo './run_workload_point int 50000000 10000000 1 1' + $i
# ./run_workload_point int 50000000 10000000 $i 1 1
# done
# for ((i=10; i<=26; i+=2))
# do
# echo './run_workload_simd_point int 50000000 10000000 1 1' + $i
# ./run_workload_simd_point int 50000000 10000000 $i 1 1
# done
# cd ~/Range-Filter/Rosetta/standalone
# for ((i=10; i<=26; i+=2))
# do
# echo './run_workload_point ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt DST -1 64 0' + $i
# ./run_workload_point ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt $i DST -1 64 0
# done
# for ((i=0; i<=8; i+=1))
# do
# echo './run_workload_point ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt SuRF' + $i
# ./run_workload_point ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt 22 SuRF $i $i
# done

# #uniform+uniform+fixsize
# for rsize in 2 4 8 16 32 64;do
#     cd ~/Range-Filter/REncoder/workload
#     ./workload int 50000000 64 10000000 $rsize 0.0 1 uniform uniform
#     cd ~/Range-Filter/REncoder/src
#     echo './run_workload int 50000000 10000000 22 1 1'
#     ./run_workload int 50000000 10000000 22 1 1
#     echo './run_workload_simd int 50000000 10000000 22 1 1'
#     ./run_workload_simd int 50000000 10000000 22 1 1
#     cd ~/Range-Filter/Rosetta/standalone
#     echo './run_workload ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt 22 DST -1 64 0'
#     ./run_workload ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt 22 DST -1 64 0
#     echo './run_workload ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt SuRF 6 6'
#     ./run_workload ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt 22 SuRF 6 6
# done

# #uniform+correlated+2-64
# cd ~/Range-Filter/REncoder/workload
# ./workload int 50000000 64 10000000 64 0.0 1 uniform correlated
# cd ~/Range-Filter/REncoder/src
# for ((i=10; i<=26; i+=2))
# do
# echo './run_workload int 50000000 10000000 1 1' + $i
# ./run_workload int 50000000 10000000 $i 1 1
# done
# for ((i=10; i<=26; i+=2))
# do
# echo './run_workload_simd int 50000000 10000000 1 1' + $i
# ./run_workload_simd int 50000000 10000000 $i 1 1
# done
# cd ~/Range-Filter/Rosetta/standalone
# for ((i=10; i<=26; i+=2))
# do
# echo './run_workload ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt DST -1 64 0' + $i
# ./run_workload ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt $i DST -1 64 0
# done
# for ((i=0; i<=8; i+=1))
# do
# echo './run_workload ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt SuRF' + $i
# ./run_workload ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt 22 SuRF $i $i
# done

#uniform+uniform+2-64+string
cd ~/Range-Filter/REncoder/src
for ((i=10; i<=26; i+=2))
do
echo './run_workload string 50000000 10000000 1 1' + $i
./run_workload string 50000000 10000000 $i 1 1
done
for ((i=10; i<=26; i+=2))
do
echo './run_workload_simd string 50000000 10000000 1 1' + $i
./run_workload_simd string 50000000 10000000 $i 1 1
done
cd ~/Range-Filter/Rosetta/standalone
for ((i=10; i<=26; i+=2))
do
echo './run_workload_string ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt DST -1 64 0' + $i
./run_workload_string ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt $i DST -1 64 0
done
for ((i=0; i<=7; i+=1))
do
echo './run_workload_string ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt SuRF' + $i
./run_workload_string ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt 22 SuRF $i $i
done

# #zipf+uniform+2-64
# cd ~/Range-Filter/REncoder/workload
# echo './workload int 50000000 64 10000000 64 0.0 1 zipfian uniform'
# ./workload int 50000000 64 10000000 64 0.0 1 zipfian uniform
# cd ~/Range-Filter/REncoder/src
# for ((i=10; i<=26; i+=2))
# do
# echo './run_workload int 50000000 10000000 1 1' + $i
# ./run_workload int 50000000 10000000 $i 1 1
# done
# for ((i=10; i<=26; i+=2))
# do
# echo './run_workload_simd int 50000000 10000000 1 1' + $i
# ./run_workload_simd int 50000000 10000000 $i 1 1
# done
# cd ~/Range-Filter/Rosetta/standalone
# for ((i=10; i<=26; i+=2))
# do
# echo './run_workload ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt DST -1 64 0' + $i
# ./run_workload ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt $i DST -1 64 0
# done
# for ((i=1; i<=9; i+=1))
# do
# echo './run_workload ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt SuRF' + $i
# ./run_workload ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt 22 SuRF $i $i
# done


# #normal+uniform+2-64
# cd ~/Range-Filter/REncoder/workload
# echo './workload int 50000000 64 10000000 64 0.0 1 normal uniform'
# ./workload int 50000000 64 10000000 64 0.0 1 normal uniform
# cd ~/Range-Filter/REncoder/src
# for ((i=10; i<=26; i+=2))
# do
# echo './run_workload int 50000000 10000000 1 1' + $i
# ./run_workload int 50000000 10000000 $i 1 1
# done
# for ((i=10; i<=26; i+=2))
# do
# echo './run_workload_simd int 50000000 10000000 1 1' + $i
# ./run_workload_simd int 50000000 10000000 $i 1 1
# done
# cd ~/Range-Filter/Rosetta/standalone
# for ((i=10; i<=26; i+=2))
# do
# echo './run_workload ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt DST -1 64 0' + $i
# ./run_workload ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt $i DST -1 64 0
# done
# for ((i=0; i<=6; i+=1))
# do
# echo './run_workload ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt SuRF' + $i
# ./run_workload ~/Range-Filter/REncoder/data/key.txt ~/Range-Filter/REncoder/data/lower_bound.txt ~/Range-Filter/REncoder/data/upper_bound.txt 22 SuRF $i $i
# done