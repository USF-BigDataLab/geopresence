#/usr/bin/env bash

out_dir="$(pwd)/java_out"
rm -rf "${out_dir}"
mkdir -vp "${out_dir}"

export JAVA_HOME="${HOME}/jdk1.8.0_212"
export PATH="${JAVA_HOME}/bin:${PATH}"

java -version

cd ../../java
pwd

mvn clean package

for i in {1..100}; do
    echo -n "${i} ... "
    /usr/bin/time -v java \
        -Xmx800M -Xms800M \
        -cp ./target/geogrid-1.0.jar \
            geogrid.Driver ../datasets/geohashes.txt \
                &> "${out_dir}/run.${i}.txt"
    echo "X"
done

grep 'wall clock' "${out_dir}"/run.*.txt | awk '{print $9}' | sed 's|0:||g' \
    > "${out_dir}/wall_clock.txt"

txtstats "${out_dir}/wall_clock.txt"
