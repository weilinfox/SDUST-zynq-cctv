package main

import (
	"bytes"
	"fmt"
	"github.com/pierrec/lz4/v4"
	"io"
	"os"
)

func main() {

	files := []string{"Baboon.bmp", "BaboonRGB.bmp", "LenaRGB.bmp", "PeppersRGB.bmp",
		"test2.bmp", "test3.bmp", "test5.bmp", "test6.bmp", "test8.bmp", "test9.bmp", "test10.bmp", "test11.bmp"}

	for _, f := range files {
		n, err := readBmpLz4("images/" + f)
		if err != nil {
			fmt.Println("Error", err, n)
		}
	}
}

func readBmpLz4(file string) (int, error) {
	// fmt.Println(file)
	f, err := os.ReadFile(file)
	if err != nil {
		fmt.Println(err)
		os.Exit(-1)
	}

	b := bytes.NewBuffer(nil)
	zw := lz4.NewWriter(b)

	n, err := zw.Write(f)
	if err != nil {
		return n, err
	}
	zw.Close()

	fmt.Println("Lz4 data len", b.Len(), "rate", float64(b.Len())*100/float64(len(f)), "%")

	// The pipe will uncompress the data from the writer.
	ub := make([]byte, len(f)*2)
	zr := lz4.NewReader(b)

	n, err = zr.Read(ub)
	if err != io.EOF {
		return n, err
	}

	if n != len(f) {
		fmt.Println("Lz4 decode match ", n == len(f), n, len(f))
	}

	return 0, nil
}

/*
Lz4 data len 263243 rate 100.00721818679148 %
Lz4 data len 784969 rate 100.0024205363399 %
Lz4 data len 783433 rate 100.00242528216243 %
Lz4 data len 781393 rate 100.00243161405422 %
Lz4 data len 8260299 rate 68.80111483756446 %
Lz4 data len 3660177 rate 58.83721109674009 %
Lz4 data len 3754887 rate 60.35967087477057 %
Lz4 data len 2893058 rate 46.50580129352015 %
Lz4 data len 3862894 rate 62.095879440346934 %
Lz4 data len 208852 rate 89.50774426358782 %
Lz4 data len 4581586 rate 73.64882699385004 %
Lz4 data len 5304324 rate 68.21356950453276 %

对于一般的图像都有比较好的压缩效果
*/
