#/usr/bin/env bash

out_dir="$(pwd)/c_out"
rm -rf "${out_dir}"
mkdir -vp "${out_dir}"

cd ../../c
pwd

echo -n "Compiling... "
make
echo "Done"

for i in {1..100}; do
    echo -n "Run ${i}... "
    row=""
    for j in `seq 10000 10000 100000`; do
        elem=`./bench_query ${j}`
        row+="$elem"
        if [ $j -ne 100000 ]; then
            row+=","
        fi
    done
    echo "X"
    echo $row >> "${out_dir}/run.${i}.txt"
done
