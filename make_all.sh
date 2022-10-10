#!/bin/sh


cd workload_gen; make workload_gen; cd ..

cd src; make run_workload_simd; cd ..