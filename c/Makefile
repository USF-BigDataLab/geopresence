CFLAGS += -Wall -std=c11

src=driver.c bitmap.c common.c geogrid.c geohash.c roaring.c
obj=$(src:.c=.o)

all: geo

geo: $(obj)
	$(CC) $(CFLAGS) $(obj) -o $@

bitmap.o: bitmap.c bitmap.h
common.o: common.c common.h
geogrid.o: geogrid.c geogrid.h
geohash.o: geohash.c geohash.h
roaring.o: roaring.c roaring.h
driver.o: driver.c

clean:
	rm -f geo $(obj)
