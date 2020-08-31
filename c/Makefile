PROGS = bench_ins bench_rand_ins

OBJS = geode.o geohash.o timer.o grid_queries.o bitmap_graphics.o

CFLAGS = -g -Wall -std=c11

%.o : %.s
	as -o $@ $<

%.o : %.c
	gcc -c ${CFLAGS} -o $@ $<

all : ${PROGS}

bench_ins : bench_ins.c ${OBJS}
	gcc ${CFLAGS} -o $@ $^ 

bench_rand_ins : bench_rand_ins.c ${OBJS}
	gcc ${CFLAGS} -o $@ $^

clean :
	rm -rf ${PROGS} ${OBJS} 
