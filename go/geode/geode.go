package geode

// #cgo CFLAGS: -g -Wall -std=c11 -O3 -I${SRCDIR}/../c/src
// #cgo LDFLAGS: -lm -lstdc++ -lcrypto -L${SRCDIR} -lgeode -lcount -Wl,-rpath=${SRCDIR}
// #include <stdlib.h>
// #include "../../c/src/geode.h"
import "C"

import (
	"unsafe"
)

type Geode C.struct_geode

type LatLon struct {
	lat float64
	lon float64
}

type queryResult C.struct_query_result

// Ensure memory is freed with *GeoRange.Free()
func NewGeode(baseHash string, precision uint, hashSize uint) *Geode {
	bh := C.CString(baseHash)
	defer C.free(unsafe.Pointer(bh))
	// allocates memory, must be freed manually
	cgp := C.geode_create(bh, C.uint(precision), C.uint(hashSize))
	return (*Geode)(cgp)
}

func (gp *Geode) Free() {
	C.geode_free((*C.struct_geode)(gp))
}

func (gp *Geode) AddGeohash(geohash string) {
	gh := C.CString(geohash)
	defer C.free(unsafe.Pointer(gh))
	C.geode_add_geohash((*C.struct_geode)(gp), gh)
}

func (gp *Geode) AddXy(x int, y int) {
	C.geode_add_xy((*C.struct_geode)(gp), C.int(x), C.int(y))
}

func (gp *Geode) PrintStdout() {
	C.print_geode((*C.struct_geode)(gp))
}

func (gp *Geode) PolygonQuery(points []LatLon) []uint {
	n := len(points)
	// Convert the points to spatial ranges
	spaRanges := make([]C.struct_spatial_range, n, n)
	for i, point := range points {
		// Query only uses lat and long from spatial range structs
		spaRanges[i] = C.struct_spatial_range{
			latitude:  C.double(point.lat),
			longitude: C.double(point.lon)}
	}
	queryRes := C.polygon_query_geode((*C.struct_geode)(gp), &spaRanges[0], C.int(n))
	// Do not free the locations since we are using those as the slice data
	defer C.free(unsafe.Pointer(queryRes))
	pLocs := unsafe.Pointer(queryRes.locations)
	// Go 1.17 required
	// Todo: Check if go garbage collection frees the underlying data
	return unsafe.Slice((*uint)(pLocs), queryRes.sz)
}
