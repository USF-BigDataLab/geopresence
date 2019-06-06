#/usr/bin/env bash

out_dir="$(pwd)/c_out"
rm -rf "${out_dir}"
mkdir -vp "${out_dir}"

cd ../../c
pwd

gcc -O3 bench_ins.c geode.c geohash.c timer.c -o bench_ins

for i in {1..100}; do
    echo -n "${i} ... "
    /usr/bin/time -v ./bench_ins &> "${out_dir}/run.${i}.txt"
    echo "X"
done
