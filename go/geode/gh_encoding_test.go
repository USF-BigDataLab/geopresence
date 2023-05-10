package geode

import (
	"bytes"
	"testing"
)

func TestGhDecodePadRight_OneDigit(t *testing.T) {
	toDecode := "z" // 0b11111
	actual, err := GhDecodePadRight(toDecode)
	expected := []byte{0b11111000}
	if err != nil {
		t.Errorf("error decoding \"%s\": %s", toDecode, err)
	}
	if !bytes.Equal(actual, expected) {
		t.Errorf("actual: %v, expected: %v", actual, expected)
	}
}

func TestGhDecodePadRight_SevenDigits(t *testing.T) {
	toDecode := "zzzzzzz" // 0b1 * 5 * 7 = 35 bits
	actual, err := GhDecodePadRight(toDecode)
	expected := []byte{0xff, 0xff, 0xff, 0xff, 0b11100000}
	if err != nil {
		t.Errorf("error decoding \"%s\": %s", toDecode, err)
	}
	if !bytes.Equal(actual, expected) {
		t.Errorf("actual: %v, expected: %v", actual, expected)
	}
}

func TestGhDecodePadRight_NoPaddingOnCompleteSegment(t *testing.T) {
	toDecode := "zzzzzzzz" // 10 bits, no padding
	actual, err := GhDecodePadRight(toDecode)
	expected := []byte{0xff, 0xff, 0xff, 0xff, 0xff}
	if err != nil {
		t.Errorf("error decoding \"%s\": %s", toDecode, err)
	}
	if !bytes.Equal(actual, expected) {
		t.Errorf("actual: %v, expected: %v", actual, expected)
	}
}

func TestGhDecodePadRight_DecodesAlphabet(t *testing.T) {
	toDecode := "0123456789bcdefghjkmnpqrstuvwxyz" // 5*32 = 160b = 20B
	actual, err := GhDecodePadRight(toDecode)
	expected := []byte{0, 68, 50, 20, 199, 66, 84, 182, 53, 207, 132, 101, 58, 86, 215, 198, 117, 190, 119, 223}
	if err != nil {
		t.Errorf("error decoding \"%s\": %s", toDecode, err)
	}
	if !bytes.Equal(actual, expected) {
		t.Errorf("actual: %v, expected: %v", actual, expected)
	}
}
