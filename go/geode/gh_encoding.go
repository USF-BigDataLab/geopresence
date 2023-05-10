package geode

import "encoding/base32"

const padding = "0000000"

var ghEncoding base32.Encoding

func init() {
	ghAlphabet := "0123456789bcdefghjkmnpqrstuvwxyz"
	enc := *base32.NewEncoding(ghAlphabet)
	ghEncoding = *enc.WithPadding(base32.NoPadding)
}

func GhDecodePadRight(geohash string) ([]byte, error) {
	// We still have to pad, the decoder will drop unfinished segments
	padAmt := len(geohash) % 8
	return ghEncoding.DecodeString(geohash + padding[:padAmt])
}
