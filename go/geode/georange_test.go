package geode

import (
	"testing"
)

func TestCheckInRange_LongGeohash(t *testing.T) {
	geohash := "01234567"
	upper := "02"
	lower := "01"
	gr := new(GeoRange)
	gr.SetBounds(upper, lower)
	expected := true
	actual, err := gr.TryAddGeohash(geohash)
	if err != nil || actual != expected {
		t.Errorf("geohash: %v, upper: %v, lower: %v", geohash, upper, lower)
	}
}

func TestCheckInRange_ShortGeohash(t *testing.T) {
	geohash := "12345"
	upper := "11111111"
	lower := "22222222"
	gr := new(GeoRange)
	gr.SetBounds(upper, lower)
	expected := true
	actual, err := gr.TryAddGeohash(geohash)
	if err != nil || actual != expected {
		t.Errorf("geohash: %v, upper: %v, lower: %v", geohash, upper, lower)
	}
}

func TestCheckInRange_WrapsZero(t *testing.T) {
	geohash := "zz"
	upper := "yyyyyyyy"
	lower := "11111111"
	gr := new(GeoRange)
	gr.SetBounds(upper, lower)
	expected := true
	actual, err := gr.TryAddGeohash(geohash)
	if err != nil || actual != expected {
		t.Errorf("geohash: %v, upper: %v, lower: %v", geohash, upper, lower)
	}
}
