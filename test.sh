#!/bin/sh

echo "Test #1"
cd workload_gen; ./workload_gen int 500000 64 100000 64 0.0 0.0 uniform uniform; cd ..
cd src; ./test int 500000 100000 26 1 1; cd ..

echo "Test #2"
cd workload_gen; ./workload_gen int 500000 64 100000 64 0.0 0.5 uniform uniform; cd ..
cd src; ./test int 500000 100000 26 1 1; cd ..

echo "Test #3"
cd workload_gen; ./workload_gen int 500000 64 100000 64 0.0 1.0 uniform uniform; cd ..
cd src; ./test int 500000 100000 26 1 1; cd ..

echo "Test #4"
cd workload_gen; ./workload_gen int 500000 64 100000 64 0.0 0.0 uniform uniform; cd ..
cd src; ./test int 500000 100000 26 0 8; cd ..

echo "Test #5"
cd workload_gen; ./workload_gen int 500000 64 100000 64 0.0 0.5 uniform uniform; cd ..
cd src; ./test int 500000 100000 26 0 8; cd ..

echo "Test #6"
cd workload_gen; ./workload_gen int 500000 64 100000 64 0.0 1.0 uniform uniform; cd ..
cd src; ./test int 500000 100000 26 0 8; cd ..

echo "Test #7"
cd workload_gen; ./workload_gen string 500000 64 100000 64 0.0 0.0 uniform uniform; cd ..
cd src; ./test string 500000 100000 26 1 1; cd ..

echo "Test #8"
cd workload_gen; ./workload_gen string 500000 64 100000 64 0.0 0.5 uniform uniform; cd ..
cd src; ./test string 500000 100000 26 1 1; cd ..

echo "Test #9"
cd workload_gen; ./workload_gen string 500000 64 100000 64 0.0 1.0 uniform uniform; cd ..
cd src; ./test string 500000 100000 26 1 1; cd ..

echo "Test #10"
cd workload_gen; ./workload_gen string 500000 64 100000 64 0.0 0.0 uniform uniform; cd ..
cd src; ./test string 500000 100000 26 0 8; cd ..

echo "Test #11"
cd workload_gen; ./workload_gen string 500000 64 100000 64 0.0 0.5 uniform uniform; cd ..
cd src; ./test string 500000 100000 26 0 8; cd ..

echo "Test #12"
cd workload_gen; ./workload_gen string 500000 64 100000 64 0.0 1.0 uniform uniform; cd ..
cd src; ./test string 500000 100000 26 0 8; cd ..