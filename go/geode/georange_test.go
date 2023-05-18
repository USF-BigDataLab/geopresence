package geode

import (
	"testing"
)

func TestBytesComparePadded_Equal(t *testing.T) {
	a := []byte{0, 1, 2}
	b := []byte{0, 1, 2}
	expected := 0
	actual := bytesComparePadded(a, b)
	if actual != expected {
		t.Errorf("a: %v, b: %v, actual: %v", a, b, actual)
	}
}

func TestBytesComparePadded_EqualPaddedA(t *testing.T) {
	a := []byte{0, 1, 2, 0, 0}
	b := []byte{0, 1, 2}
	expected := 0
	actual := bytesComparePadded(a, b)
	if actual != expected {
		t.Errorf("a: %v, b: %v, actual: %v", a, b, actual)
	}
}

func TestBytesComparePadded_EqualPaddedB(t *testing.T) {
	a := []byte{0, 1, 2}
	b := []byte{0, 1, 2, 0, 0, 0}
	expected := 0
	actual := bytesComparePadded(a, b)
	if actual != expected {
		t.Errorf("a: %v, b: %v, actual: %v", a, b, actual)
	}
}

func TestBytesComparePadded_UnequalDiffLength_LongA(t *testing.T) {
	a := []byte{0, 1, 2, 0, 0}
	b := []byte{0, 1, 3}
	expected := -1
	actual := bytesComparePadded(a, b)
	if actual != expected {
		t.Errorf("a: %v, b: %v, actual: %v", a, b, actual)
	}
}

func TestBytesComparePadded_UnequalDiffLength_LongB(t *testing.T) {
	a := []byte{0, 1, 2}
	b := []byte{0, 1, 3, 0, 0}
	expected := -1
	actual := bytesComparePadded(a, b)
	if actual != expected {
		t.Errorf("a: %v, b: %v, actual: %v", a, b, actual)
	}
}

func TestBytesComparePadded_LongA_NonzeroRemainder(t *testing.T) {
	a := []byte{0, 1, 2, 0, 1}
	b := []byte{0, 1, 2}
	expected := 1
	actual := bytesComparePadded(a, b)
	if actual != expected {
		t.Errorf("a: %v, b: %v, actual: %v", a, b, actual)
	}
}

func TestBytesComparePadded_LongB_NonzeroRemainder(t *testing.T) {
	a := []byte{0, 1, 2}
	b := []byte{0, 1, 2, 0, 1}
	expected := -1
	actual := bytesComparePadded(a, b)
	if actual != expected {
		t.Errorf("a: %v, b: %v, actual: %v", a, b, actual)
	}
}

func TestCheckInRange_LongGeohash(t *testing.T) {
	geohash := "01234567"
	upper := "02"
	lower := "01"
	testCheckInRange(geohash, upper, lower, t)
}

func TestCheckInRange_ShortGeohash(t *testing.T) {
	geohash := "12345"
	upper := "22222222"
	lower := "11111111"
	testCheckInRange(geohash, upper, lower, t)
}

func TestCheckInRange_WrapsZero(t *testing.T) {
	geohash := "zz"
	upper := "11111111"
	lower := "yyyyyyyy"
	testCheckInRange(geohash, upper, lower, t)
}

func testCheckInRange(geohash, upper, lower string, t *testing.T) {
	geoBytes, err := GhDecodePadRight(geohash)
	if err != nil {
		t.Errorf(err.Error())
	}
	gr, err := NewGeoRange(nil, upper, lower)
	if err != nil {
		t.Errorf(err.Error())
	}
	expected := true
	actual := gr.CheckGeoBytesInRange(geoBytes)
	if actual != expected {
		t.Errorf("geohash: %v, upper: %v, lower: %v, geobytes: %v, upperBytes: %v, lowerBytes: %v", geohash, upper, lower, geoBytes, gr.upperBound, gr.lowerBound)
	}
}
