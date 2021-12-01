#!/bin/sh


cd workload; ./workload 50000000 64 10000000 32 0.0 uniform uniform; cd ..

cd src; ./main 50000000 10000000 26 1 1; cd ..