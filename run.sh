#!/bin/sh


cd workload_gen; ./workload_gen int 50000000 64 10000000 64 0.0 1 uniform uniform; cd ..

echo "=============================================================================="
cd src; ./run_workload_simd int 50000000 10000000 26 1 1 REncoder; cd ..
echo "=============================================================================="
cd src; ./run_workload_simd int 50000000 10000000 26 1 1 REncoderSS; cd ..
echo "=============================================================================="
cd src; ./run_workload_simd int 50000000 10000000 26 1 1 REncoderSE; cd ..