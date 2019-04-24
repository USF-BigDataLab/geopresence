CFLAGS += -Wall -std=c11

driver_src=driver.c bitmap.c common.c geogrid.c geohash.c roaring.c bitmaphm.c
driver_obj=$(driver_src:.c=.o)

insrt_bnch_src=insrt_bnch.c bitmap.c common.c geogrid.c geohash.c roaring.c bitmaphm.c
insrt_bnch_obj=$(insrt_bnch_src:.c=.o)

all: geo

insrt_bnch: insrt_bnch_tst

geo: $(driver_obj)
	$(CC) $(CFLAGS) $(driver_obj) -o $@

insrt_bnch_tst: $(insrt_bnch_obj)
	$(CC) $(CFLAGS) $(insrt_bnch_obj) -o $@

bitmap.o: bitmap.c bitmap.h
common.o: common.c common.h
geogrid.o: geogrid.c geogrid.h
geohash.o: geohash.c geohash.h
roaring.o: roaring.c roaring.h
driver.o: driver.c
insrt_bnch.o: insrt_bnch.c
bitmaphm.o: bitmaphm.c bitmaphm.h

clean:
	rm -f geo insrt_bnch_tst insrt_bnch.o $(driver_obj)
