#/usr/bin/env bash

out_dir="$(pwd)/java_out"
rm -rf "${out_dir}"
mkdir -vp "${out_dir}"

cd ../../java
pwd

echo -n "Compiling... "
mvn clean package
echo "Done"

for i in {1..100}; do
    echo -n "${i} ... "
    java -Xmx1600M -Xms1600M \
        -cp ./target/geogrid-1.0.jar \
            geogrid.QueryBenchmark 10000 \
                &> "${out_dir}/run.${i}.txt"

    echo "X"
    awk "NR==5" "${out_dir}/run.${i}.txt" >> "${out_dir}/wall_clock.txt"
done
