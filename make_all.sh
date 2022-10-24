#!/bin/sh


cd workload_gen; make workload_gen; cd ..

cd src; make run_workload_simd; cd ..

cd src; make test; cd ..