package main

import (
	"bytes"
	"compress/lzw"
	"errors"
	"fmt"
	"os"
)

// test image from
// http://www.eecs.qmul.ac.uk/~phao/IP/Images/

func lzmTest(b []byte) (int, error) {
	// encode data
	result := bytes.NewBuffer(nil)
	lw := lzw.NewWriter(result, lzw.LSB, 8)
	n, err := lw.Write(b)
	lw.Close()
	if err != nil {
		fmt.Println(err)
		os.Exit(-1)
	}

	comTot := result.Len()

	orig := make([]byte, n*2)
	lr := lzw.NewReader(result, lzw.LSB, 8)
	nn, err := lr.Read(orig)
	if err != nil {
		return 0, err
	}

	// decode data
	if n != nn {
		return 0, errors.New(fmt.Sprint("Not match ", n, nn))
	}

	return comTot, nil
}

func main() {

	files := []string{"Baboon.bmp", "BaboonRGB.bmp", "Barbara.bmp", "Cameraman.bmp", "Goldhill.bmp", "Lena.bmp", "LenaRGB.bmp", "Peppers.bmp", "PeppersRGB.bmp",
		"test2.bmp", "test3.bmp", "test5.bmp", "test6.bmp", "test8.bmp", "test9.bmp", "test10.bmp", "test11.bmp"}

	for _, f := range files {
		readBmp("images/" + f)
	}

}

func readBmp(file string) {
	f, err := os.ReadFile(file)
	if err != nil {
		fmt.Println(err)
		os.Exit(-1)
	}

	comTot := 0
	for i := 0; i < len(f); i += 3456 * 3 {

		e := i + 3456*3
		if e > len(f) {
			e = len(f)
		}

		var r, g, b []byte
		for j := i; j < e; j += 3 {
			r = append(r, f[j])
			if j+1 < e {
				g = append(g, f[j+1])
			}
			if j+2 < e {
				b = append(b, f[j+2])
			}
		}
		n, err := lzmTest(r)

		if err != nil {
			fmt.Println(err)
			os.Exit(-1)
		}

		comTot += n
		n, err = lzmTest(g)

		if err != nil {
			fmt.Println(err)
			os.Exit(-1)
		}

		comTot += n
		n, err = lzmTest(b)

		if err != nil {
			fmt.Println(err)
			os.Exit(-1)
		}

		comTot += n
	}

	fmt.Println("Rate ", float64(comTot)*100/float64(len(f)), "%")

}

/*
Rate  123.04919004346108 %
Rate  120.06171740700431 %
Rate  115.16844968543901 %
Rate  93.55560225771586 %
Rate  106.77179892411026 %
Rate  110.88958453636447 %
Rate  103.55809624558798 %
Rate  109.25029632556301 %
Rate  103.22026528058915 %
Rate  79.77709412268177 %
Rate  64.41147790962462 %
Rate  49.52331303708462 %
Rate  49.76464646172374 %
Rate  69.90813479949858 %
Rate  95.69714801170133 %
Rate  74.67809082161388 %
Rate  72.74413217809445 %
*/
