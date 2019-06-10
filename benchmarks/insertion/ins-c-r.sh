#/usr/bin/env bash

insertions=100000

out_dir="$(pwd)/c_r_out"
rm -rf "${out_dir}"
mkdir -vp "${out_dir}"

cd ../../c
pwd

echo -n "Compiling... "
gcc -O3 bench_rand_ins.c geode.c geohash.c timer.c -o bench_rand_ins
echo "Done"

for i in {1..100}; do
    echo -n "${i} ... "
    /usr/bin/time -v ./bench_rand_ins ${insertions} &> "${out_dir}/run.${i}.txt"
    echo "X"
done

grep 'wall clock' "${out_dir}"/run.*.txt | awk '{print $9}' | sed 's|0:||g' \
    > "${out_dir}/wall_clock.txt"
grep 'Maximum resident' "${out_dir}"/run.*.txt | awk '{print $7}' \
    > "${out_dir}/max_mem.txt"

txtstats "${out_dir}/wall_clock.txt"
txtstats "${out_dir}/max_mem.txt"
