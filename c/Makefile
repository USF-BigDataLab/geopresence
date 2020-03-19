CFLAGS += -Wall -std=c11

all: bench_rand_ins bench_ins

bench_rand_ins: bench_rand_ins.c geode.c timer.c geohash.c
	gcc -Wall bench_rand_ins.c geode.c timer.c geohash.c -o bench_rand_ins

bench_ins: bench_ins.c geode.c timer.c geohash.c
	gcc -Wall bench_ins.c geode.c timer.c geohash.c grid_queries.c -o bench_ins 

clean:
	rm -f bench_rand_ins bench_ins
