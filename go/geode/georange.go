package geode

import (
	"fmt"
)

type GeoRange struct {
	geode      *Geode
	upperBound []byte
	lowerBound []byte
	wrapsZero  bool
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
	gr.wrapsZero = checkLessThan(ubBytes, lbBytes)
	return nil
}

func (gr *GeoRange) Close() {
	gr.geode.Close()
}

func (gr *GeoRange) AddGeohash(geohash string) error {
	// Future: C library also decodes this, should add C function to take decoded geohash
	geoBytes, err := GhDecodePadRight(geohash)
	if err != nil {
		return err
	}
	// Future: Verify compiler inlines function
	if gr.CheckGeoBytesInRange(geoBytes) {
		gr.geode.AddGeohash(geohash)
	}
	return nil
}

func (gr *GeoRange) TryAddGeohash(geohash string) (bool, error) {
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

func checkLessThan(left, right []byte) bool {
	// Bytewise compare left and right
	isEqual := true
	for i := 0; i < len(left); i++ {
		if left[i] > right[i] {
			return false
		}
		if left[i] < right[i] {
			isEqual = false
		}
	}
	return isEqual
}

func (gr *GeoRange) CheckGeoBytesInRange(geohash []byte) bool {
	if len(geohash) == 0 {
		return false
	}
	// Todo: Compare vs lower and upper separately, stopping at first non-equal byte in each
	// for i := 0; i < len(geohash); i++ {
	// 	// Check if satisfied either upper or lower precision
	// 	if (i > len(gr.upperBoundupper)) || (i > len(gr.lowerBound)) {
	// 		// Error condition, means all are equal through their lengths
	// 		return true
	// 	}
	// 	if gr.wrapsZero {
	// 		if geohash[i] >= lower[i] || geohash[i] < upper[i] {
	// 			return true
	// 		}
	// 	}
	// }

	// Check each geohash byte
	// for i := 0; i < len(geohash); i++ {
	// 	// Check if satisfied either upper or lower precision
	// 	if (i > len(upper)) || (i > len(lower)) {
	// 		return true
	// 	}
	// 	if wrapsZero {
	// 		// Fails if >= upper AND < lower
	// 		if geohash[i] >= upper[i] && geohash[i] < lower[i] {
	// 			return false
	// 		}
	// 	} else {
	// 		// Not wrapping zero, fails if >= upper or < lower
	// 		if geohash[i] >= upper[i] || geohash[i] < lower[i] {
	// 			return false
	// 		}
	// 	}
	// }
	return true
}
