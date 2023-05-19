package geode

import (
	"bytes"
	"fmt"
)

type GeoRange struct {
	geode       *Geode // Underlying container
	upperBound  []byte // Inclusive limit
	lowerBound  []byte // Exclusive limit for accepting geohash
	wrapsZero   bool   // When lower bound > upper bound
	isUnbounded bool   // If bounds equal, accepts all geohashes
}

func NewGeoRange(geode *Geode, upperGeohash string, lowerGeohash string) (*GeoRange, error) {
	ngr := &GeoRange{
		geode: geode,
	}
	err := ngr.SetBounds(upperGeohash, lowerGeohash)
	if err != nil {
		return nil, err
	}
	return ngr, nil
}

func (gr *GeoRange) SetBounds(ub string, lb string) error {
	ubBytes, err := GhDecodePadRight(ub)
	if err != nil {
		return fmt.Errorf("upper bound error: %w", err)
	}
	lbBytes, err := GhDecodePadRight(lb)
	if err != nil {
		return fmt.Errorf("lower bound error: %w", err)
	}
	if len(ubBytes) < 1 || len(lbBytes) < 1 {
		return fmt.Errorf("bounds must be nonzero")
	}
	gr.upperBound = ubBytes
	gr.lowerBound = lbBytes
	lowerUpperComp := bytesComparePadded(gr.lowerBound, gr.upperBound)
	gr.wrapsZero = false
	if lowerUpperComp > 0 {
		gr.wrapsZero = true
	}
	gr.isUnbounded = false
	if lowerUpperComp == 0 {
		gr.isUnbounded = bytes.Equal(ubBytes, lbBytes)
	}
	return nil
}

func (gr *GeoRange) GetGeode() *Geode {
	return gr.geode
}

// Calls close on geode to release resources
func (gr *GeoRange) Close() {
	gr.geode.Close()
}

// Attempts to decode geohash, then if it is in range, adds to this container
//
// Returns:
//
//	isAdded: True if geohash was added to the container
//	err: Non-nil when the geohash cannot decode
func (gr *GeoRange) TryAddGeohash(geohash string) (isAdded bool, decodes error) {
	geoBytes, err := GhDecodePadRight(geohash)
	if err != nil {
		return false, err
	}
	if !gr.CheckGeoBytesInRange(geoBytes) {
		return false, nil
	}
	gr.geode.AddGeohash(geohash)
	return true, nil
}

func (gr *GeoRange) CheckGeoBytesInRange(geohash []byte) bool {
	if len(geohash) == 0 {
		return false
	}
	if gr.isUnbounded {
		return true
	}
	isGtLower := bytesComparePadded(geohash, gr.lowerBound) > 0
	isLteUpper := bytesComparePadded(geohash, gr.upperBound) <= 0
	if gr.wrapsZero {
		// When 0 is wrapped, only need either condition satisfied
		return isGtLower || isLteUpper
	} else {
		return isGtLower && isLteUpper
	}
}

// Ignores right-padded 0s, returns 0 if a == b, -1 if a < b, 1 if a > b.
func bytesComparePadded(a, b []byte) int {
	if len(a) == len(b) {
		return bytes.Compare(a, b)
	}
	// Get the overlap and remainder
	var left, right, rem []byte
	var onDiff int
	if len(a) > len(b) {
		left = a[:len(b)]
		right = b
		rem = a[len(b):]
		onDiff = 1
	} else {
		left = a
		right = b[:len(a)]
		rem = b[len(a):]
		onDiff = -1
	}
	// If the overlap not equal, it's done
	cmp := bytes.Compare(left, right)
	if cmp != 0 {
		return cmp
	}
	// If padding all 0s, return equal
	for _, ele := range rem {
		if ele != 0 {
			return onDiff
		}
	}
	return 0
}
