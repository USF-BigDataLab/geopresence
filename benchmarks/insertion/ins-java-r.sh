#/usr/bin/env bash

insertions=100000

out_dir="$(pwd)/java_r_out"
rm -rf "${out_dir}"
mkdir -vp "${out_dir}"

# Set to a specific JDK:
# export JAVA_HOME="${HOME}/jdk1.8.0_212"
# export PATH="${JAVA_HOME}/bin:${PATH}"

java -version

cd ../../java
pwd

mvn clean package

for i in {1..100}; do
    echo -n "${i} ... "
    /usr/bin/time -v java \
        -Xmx800M -Xms800M \
        -cp ./target/geogrid-1.0.jar \
            geogrid.RandomDriver ${insertions} \
                &> "${out_dir}/run.${i}.txt"
    echo "X"
done

grep 'wall clock' "${out_dir}"/run.*.txt | awk '{print $9}' | sed 's|0:||g' \
    > "${out_dir}/wall_clock.txt"
grep 'Maximum resident' "${out_dir}"/run.*.txt | awk '{print $7}' \
    > "${out_dir}/max_mem.txt"

txtstats "${out_dir}/wall_clock.txt"
txtstats "${out_dir}/max_mem.txt"
