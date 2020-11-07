PROGS = bench_ins bench_rand_ins bench_query bench_rtree

OBJDIR = obj

OBJS = $(addprefix $(OBJDIR)/, geode.o geohash.o timer.o bitmap_graphics.o)
OBJS_Q = $(addprefix $(OBJDIR)/, geode.o geohash.o timer.o bitmap_graphics.o rtree.o)

CFLAGS = -g -Wall -std=c11 -O3

obj/%.o : src/%.c
	@mkdir -p $(@D)
	gcc -c ${CFLAGS} -o $@ $<

all : ${PROGS}

bench_ins : src/bench_ins.c ${OBJS}
	gcc ${CFLAGS} -o $@ $^ 

bench_rand_ins : src/bench_rand_ins.c ${OBJS}
	gcc ${CFLAGS} -o $@ $^

bench_query : src/bench_query.c ${OBJS_Q}
	gcc ${CFLAGS} -o $@ $^ 

bench_rtree : src/bench_rtree.c ${OBJS_Q}
	gcc ${CFLAGS} -o $@ $^ 

clean :
	rm -rf ${PROGS} ${OBJDIR} 
