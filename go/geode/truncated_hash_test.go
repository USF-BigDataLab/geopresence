package geode

import (
	"bytes"
	"testing"
)

func TestWriteTruncates(t *testing.T) {
	capacity := 5
	th := NewTruncatedHash(capacity)
	toWrite := []byte{0, 1, 2, 3, 4, 5, 6}
	th.Write(toWrite)
	actual := th.Sum(nil)
	expected := toWrite[:capacity]
	if !bytes.Equal(actual, expected) {
		t.Errorf("write did not truncate, actual: %v, expected: %v", actual, expected)
	}
}

func TestMultiWriteAppends(t *testing.T) {
	capacity := 5
	th := NewTruncatedHash(capacity)
	toWrite := []byte{0, 1, 2, 3, 4}
	th.Write(toWrite[0:2])
	th.Write(toWrite[2:5])
	actual := th.Sum(nil)
	if !bytes.Equal(actual, toWrite) {
		t.Errorf("write did not append, actual: %v, expected: %v", actual, toWrite)
	}
}

func TestPartialWriteReturnsFull(t *testing.T) {
	capacity := 5
	th := NewTruncatedHash(capacity)
	expected := []byte{0, 1, 2, 0, 0}
	toWrite := expected[0:3]
	th.Write(toWrite)
	actual := th.Sum(nil)
	if !bytes.Equal(actual, expected) {
		t.Errorf("partial write mismatch, actual: %v, expected: %v", actual, expected)
	}
}
