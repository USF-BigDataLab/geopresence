package geopresence

import "hash"

// Implements the hash.Hash interface https://pkg.go.dev/hash#Hash
type TruncatedHash struct {
	data  []byte
	index int
}

func NewTruncatedHash(size int) hash.Hash {
	return &TruncatedHash{
		data:  make([]byte, size, size),
		index: 0,
	}
}

// Write (via the embedded io.Writer interface) adds more data to the running hash.
// It never returns an error.
func (t *TruncatedHash) Write(src []byte) (int, error) {
	newInd := t.index + len(src)
	if newInd > cap(t.data) {
		newInd = cap(t.data)
	}
	copy(t.data[t.index:newInd], src)
	t.index = newInd
	return len(src), nil
}

// Sum appends the current hash to b and returns the resulting slice.
// It does not change the underlying hash state.
func (t *TruncatedHash) Sum(b []byte) []byte {
	return append(b, t.data...)
}

// Reset resets the hash to its initial state.
func (t *TruncatedHash) Reset() {
	t.data = t.data[:0]
}

// Size returns the size of the hash in bytes.
func (t *TruncatedHash) Size() int {
	return cap(t.data)
}

// BlockSize returns the block size of the hash in bytes.
func (t *TruncatedHash) BlockSize() int {
	return cap(t.data)
}
