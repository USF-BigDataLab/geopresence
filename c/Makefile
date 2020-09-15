PROGS = bench_ins bench_rand_ins

OBJDIR = obj

OBJS = $(addprefix $(OBJDIR)/, geode.o geohash.o timer.o grid_queries.o bitmap_graphics.o)

CFLAGS = -g -Wall -std=c11

obj/%.o : src/%.c
	@mkdir -p $(@D)
	gcc -c ${CFLAGS} -o $@ $<

all : ${PROGS}

bench_ins : src/bench_ins.c ${OBJS}
	gcc ${CFLAGS} -o $@ $^ 

bench_rand_ins : src/bench_rand_ins.c ${OBJS}
	gcc ${CFLAGS} -o $@ $^

clean :
	rm -rf ${PROGS} ${OBJDIR} 
