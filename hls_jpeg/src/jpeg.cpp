#include "jpeg.hpp"
#include "constrains.h"


template <typename Number, typename Limit>
Number clamp(Number value, Limit minValue, Limit maxValue)
{
  if (value <= minValue) return minValue;
  if (value >= maxValue) return maxValue;
  return value;
}

int32_t scaledLuminance[8 * 8] = {0};
int32_t scaledChrominance[8 * 8] = {0};
uint8_t quantLuminance  [8*8] = {0};
uint8_t quantChrominance[8*8] = {0};

void write_first_Value(uint8_t valuewrite, AXI_STREAM &outputStream) {
	ap_axiu<8, 1, 1, 1> value;
	value.data = valuewrite;
	value.user = 1;
	value.last = 0;
	value.keep = -1;
	outputStream << value;
}
void write_last_Value(uint8_t valuewrite, AXI_STREAM &outputStream) {
	ap_axiu<8, 1, 1, 1> value;
	value.data = valuewrite;
	value.user = 0;
	value.last = 1;
	value.keep = -1;
	outputStream << value;
}
void write_Value(uint8_t valuewrite, AXI_STREAM &outputStream) {
	ap_axiu<8, 1, 1, 1> value;
	value.data = valuewrite;
	value.user = 0;
	value.last = 0;
	value.keep = -1;
	outputStream << value;

}
void write_Value_16bit(uint16_t valuewrite, AXI_STREAM &outputStream) {
	uint8_t msb = (uint8_t) (valuewrite >> 8);
	uint8_t lsb = (uint8_t) (valuewrite);

	ap_axiu<8, 1, 1, 1> value;
	value.data = msb;
	value.user = 0;
	value.last = 0;
	value.keep = -1;
	outputStream << value;

	ap_axiu<8, 1, 1, 1> value2;
	value2.data = lsb;
	value2.user = 0;
	value2.last = 0;
	value2.keep = -1;
	outputStream << value2;

}

void mergeStream(AXI_STREAM &s1, AXI_STREAM &s3) {
	bool sof1 = 0;
	ap_axiu<8, 1, 1, 1> axi1;
	bool firstPartDone = false;
	while (!sof1) {
		#pragma HLS pipeline II=1
		#pragma HLS loop_tripcount avg=0 max=0
		s1 >> axi1;
		sof1 = axi1.user.to_int();
	}

	//write_first_Value((uint8_t) (axi1.data), s3);
	bool first = 1;
	while (!axi1.last.to_bool()) {
		s1 >> axi1;
		if (first){
			write_first_Value((uint8_t) axi1.data, s3);
			first=false;
		}
		else{
			if (!axi1.last.to_bool()){
				write_Value((uint8_t) axi1.data, s3);
			}else{
				write_last_Value((uint8_t) axi1.data, s3);
			}
		}

	}
}

void DCT(int32_t block[8 * 8], uint8_t stride) {
	//const float16 SqrtHalfSqrt = 1.306562965; 	//     sqrt((2 + sqrt(2)) / 2) 	= cos(pi * 1 / 8) * sqrt(2)
	//const float16 InvSqrt = 0.707106781; 		// 1 / sqrt(2)                	= cos(pi * 2 / 8)
	//const float16 HalfSqrtSqrt = 0.382683432; 	//     sqrt(2 - sqrt(2)) / 2  	= cos(pi * 3 / 8)
	//const float16 InvSqrtSqrt = 0.541196100; 	// 1 / sqrt(2 - sqrt(2))      	= cos(pi * 3 / 8) * sqrt(2)

	// 32bit 浮点改为 int64 整数模拟 9bit 定点 *512
	const int32_t SqrtHalfSqrt = 669;// 9bit 669;//1338;//2676;//5352; //10703;//21407;// 42813; //85627;
	const int32_t InvSqrt = 362;// 9bit 362;//724;//1448;//2896;//5793;//11585;//23170;//46341;
	const int32_t HalfSqrtSqrt = 196;// 9bit 196;//392;//784;//1567;//3135;//6270;//12540;//25080;
	const int32_t InvSqrtSqrt = 277;// 9bit 277;//554;//1108;//2217;//4433;//8867;//17734;//35468;

	// modify in-place
	int32_t& block0 = block[0];
	int32_t& block1 = block[1 * stride];
	int32_t& block2 = block[2 * stride];
	int32_t& block3 = block[3 * stride];
	int32_t& block4 = block[4 * stride];
	int32_t& block5 = block[5 * stride];
	int32_t& block6 = block[6 * stride];
	int32_t& block7 = block[7 * stride];

	int32_t add07 = block0 + block7;
	int32_t sub07 = block0 - block7;
	int32_t add16 = block1 + block6;
	int32_t sub16 = block1 - block6;
	int32_t add25 = block2 + block5;
	int32_t sub25 = block2 - block5;
	int32_t add34 = block3 + block4;
	int32_t sub34 = block3 - block4;

	int32_t add0347 = add07 + add34;
	int32_t sub07_34 = add07 - add34;
	int32_t add1256 = add16 + add25;
	int32_t sub16_25 = add16 - add25;

	block0 = (add0347 + add1256) << 9;
	block4 = (add0347 - add1256) << 9;

	int32_t z1 = (sub16_25 + sub07_34) * InvSqrt;
	block2 = (sub07_34<<9) + (z1);
	block6 = (sub07_34<<9) - (z1);

	int32_t sub23_45 = sub25 + sub34;
	int32_t sub12_56 = sub16 + sub25;
	int32_t sub01_67 = sub16 + sub07;

	int32_t z5 = (sub23_45 - sub01_67) * HalfSqrtSqrt;
	int32_t z2 = sub23_45 * InvSqrtSqrt + z5;
	int32_t z3 = sub12_56 * InvSqrt;
	int32_t z4 = sub01_67 * SqrtHalfSqrt + z5;
	int32_t z6 = (sub07<<9) + z3;
	int32_t z7 = (sub07<<9) - z3;
	block1 = (z6 + z4);
	block7 = (z6 - z4);
	block5 = (z7 + z2);
	block3 = (z7 - z2);
}

void Quant(int32_t* block, const int32_t scaled[8 * 8], int16_t *quant,
		int &posNonZero, int &DC) {
#pragma HLS inline off

	//This Loop is theoretical part of the dct, but scaled[] is prepared and optimized for quantization
/*	int16_t block16[64];
	quantLoop: for (int i = 0; i < 8 * 8; i++) {
#pragma HLS PIPELINE II=1
#pragma HLS LOOP_FLATTEN off
		//fblock[i] = (float)block[i] * (float)scaled[i] / (float)4294967296.0;
		//block16[i] = (int16_t)(((block[i]>0 ? block[i]+33554432 : block[i]-33554432) * scaled[i]) / 281474976710656); // 13 22
		block16[i] = (int16_t)(((block[i]>0 ? block[i]+134217728 : block[i]-134217728) * scaled[i]) / 281474976710656); // 14 20
	}
*/

	QuantToIntLoop: for (int i = 0; i < 8 * 8; i++) {
#pragma HLS PIPELINE II=1
#pragma HLS LOOP_FLATTEN off
		/*float16 value = fblock[ZigZagInv[i]];
		if (value > 0)
			value += (float16) 0.5;
		else
			value -= (float16) 0.5;
		quant[i] = int(value);*/
		//quant[i] = block16[ZigZagInv[i]];

		uint8_t zigzag = ZigZagInv[i];
		quant[i] = (int16_t)((((block[zigzag]>0 ? block[zigzag]+131072 : block[zigzag]-131072)>>11) * scaled[zigzag]) / 4194304); // 7bit 15bit

		if (quant[i] != 0)
			posNonZero = i;
	}
	DC = quant[0];

}

void huffmann(int16_t *quant, int posNonZero, int DC, int lastDC,
		AXI_BIT_WRITER2& writer, const BitCode huffmanDC[256],
		const BitCode huffmanAC[256]) {
#pragma HLS inline off
	int diff = DC - lastDC;
	if (diff == 0)
		writer.write(huffmanDC[0x00]);
	else {
		BitCode bits = codewordsArray[diff + CodeWordLimit];
		writer.write(huffmanDC[bits.numBits]);
		writer.write(bits);
	}

	int offset = 0;
	for (int i = 1; i <= posNonZero; i++) {

		while (quant[i] == 0) {
			offset += 0x10;
			if (offset > 0xF0) {
				writer.write(huffmanAC[0xf0]);
				offset = 0;
			}
			i++;
		}

		BitCode encoded = codewordsArray[quant[i] + CodeWordLimit];
		writer.write(huffmanAC[offset + encoded.numBits]);
		writer.write(encoded);
		offset = 0;
	}

	if (posNonZero < 8 * 8 - 1) // = 63
		writer.write(huffmanAC[0x00]);
}

int16_t encodeBlock(AXI_BIT_WRITER2& w, int32_t block[8*8],
		const int32_t scaled[8 * 8], int16_t lastDC, const BitCode huffDC[256],
		const BitCode huffAC[256]) {

	//float16 *block64 = (float16*) block;

	DCT_row: for (int offset = 0; offset < 8; offset++)
		DCT(block + offset * 8, 1);
	DCT_col: for (int offset = 0; offset < 8; offset++)
		DCT(block + offset * 1, 8);

	int16_t quantized[8 * 8];
	int posNonZero = 0;
	int DC = 0;
	Quant(block, scaled, quantized, posNonZero, DC);

	huffmann(quantized, posNonZero, DC, lastDC, w, huffDC, huffAC);

	return DC;
}
/**
 * Prepare the header of a JPEG file with constants and
 * other definitions to
 */
void encode_jpeg(AXI_BIT_WRITER2 &writer, uint8_t quality_, IMAGE3s &matIn) {
//void writeHeader(AXI_BIT_WRITER2 &writer, uint8_t quality_) {

	writer.writeStartSymbol();

	// https://blog.csdn.net/yun_hen/article/details/78135122

	uint16_t height = MAX_HEIGHT;
	uint16_t width = MAX_WIDTH;

	// 写死固定输出
	const uint8_t HeaderJfif[2 + 2 + 16] = { 0xFF, 0xD8, 0xFF, 0xE0, 0, 16, 'J',
			'F', 'I', 'F', 0, 1, 1, 0, 0, 1, 0, 1, 0, 0 };
	writer.write(HeaderJfif);


	// 此处受 quality 影响
	uint8_t quality = clamp<uint8_t>(quality_, 1, 100);
	quality = quality < 50 ? 5000 / quality : 200 - quality * 2;
	for (int i = 0; i < 8 * 8; i++) {
		uint8_t lum = (DefaultQuantLuminance[ZigZagInv[i]] * quality + 50) / 100;
		uint8_t chr = (DefaultQuantChrominance[ZigZagInv[i]] * quality + 50) / 100;

		quantLuminance[i] = clamp(lum, 1, 255);
		quantChrominance[i] = clamp(chr, 1, 255);
	}

	// 客户端自行计算，可以不输出
	writer.addMarker(0xdb, 132);
	writer.write(0);
	writer.write(quantLuminance);
	writer.write(1);
	writer.write(quantChrominance);

	// 写死固定输出
	writer.addMarker(0xC0, 2 + 6 + 3 * 3);
	writer.write(0x08);
	writer.write(height >> 8);
	writer.write((uint8_t)height);
	writer.write(width >> 8);
	writer.write((uint8_t)width);
	writer.write(3);
	for (int id = 1; id <= 3; id++) {
		writer.write(id);
		writer.write(0x11);
		writer.write(id == 1 ? 0 : 1);
	}
	// 写死固定输出
	writer.addMarker(0xC4, (2 + 208 + 208));
	writer.write(0);
	writer.write(DcLuminanceCodesPerBitsize);
	writer.write(DcLuminanceValues);
	writer.write(0x10);
	writer.write(AcLuminanceCodesPerBitsize);
	for (int i =0; i<162;i++){
		writer.write(AcLuminanceValues[i]);
	}
	//writer.write(AcLuminanceValues);

	// 写死
	//generateHuffmanTable(DcLuminanceCodesPerBitsize, DcLuminanceValues,huffmanLuminanceDC);
	//generateHuffmanTable(AcLuminanceCodesPerBitsize, AcLuminanceValues,huffmanLuminanceAC);


	// 写死固定输出
	writer.write(1);
	writer.write(DcChrominanceCodesPerBitsize);
	writer.write(DcChrominanceValues);
	writer.write(0x11);
	writer.write(AcChrominanceCodesPerBitsize);
	for (int i =0; i<162;i++){
		writer.write(AcChrominanceValues[i]);
	}


	// 写死
	//generateHuffmanTable(DcChrominanceCodesPerBitsize, DcChrominanceValues,huffmanChrominanceDC);
	//generateHuffmanTable(AcChrominanceCodesPerBitsize, AcChrominanceValues,huffmanChrominanceAC);

	// 写死固定输出
	writer.addMarker(0xDA, 2 + 1 + 2 * 3 + 3);
	writer.write(3);

	for (int id = 1; id <= 3; id++) {
		writer.write(id);
		writer.write(id == 1 ? 0 : 0x11);
	}
	static const uint8_t Spectral[3] = { 0, 0x3f, 0 };
	writer.write(Spectral);


	// 受 quality 影响
	static const float AanScaleFactors[8] = { 1, 1.387039845f, 1.306562965f,
			1.175875602f, 1, 0.785694958f, 0.541196100f, 0.275899379f };
	prepQuant:
	for (int i = 0; i < 8 * 8; i++) {
		int row = ZigZagInv[i] / 8;
		int column = ZigZagInv[i] % 8;
		float factor = 1 / (AanScaleFactors[row] * AanScaleFactors[column] * 8);
		//scaledLuminance[ZigZagInv[i]] = (factor / quantLuminance[i])*4194304.0+0.5; // 22bit
		//scaledChrominance[ZigZagInv[i]] = (factor / quantChrominance[i])*4194304.0+0.5;
		//scaledLuminance[ZigZagInv[i]] = (int64_t)((factor / quantLuminance[i])*1048576.0+0.5); // 20bit
		//scaledChrominance[ZigZagInv[i]] = (int64_t)((factor / quantChrominance[i])*1048576.0+0.5);
		//scaledLuminance[ZigZagInv[i]] = (int64_t)((factor / quantLuminance[i])*65536.0+0.5); // 16bit
		//scaledChrominance[ZigZagInv[i]] = (int64_t)((factor / quantChrominance[i])*65536.0+0.5);
		scaledLuminance[ZigZagInv[i]] = (int32_t)((factor / quantLuminance[i])*32768.0+0.5); // 15bit
		scaledChrominance[ZigZagInv[i]] = (int32_t)((factor / quantChrominance[i])*32768.0+0.5);
	}


	/* 写死
	uint8_t numBits = 1;
	int32_t mask = 1;
	BitCode* codewords = &codewordsArray[CodeWordLimit];
	for (int16_t value = 1; value < CodeWordLimit; value++) {
		if (value > mask) {
			numBits++;
			mask = (mask << 1) | 1;
		}
		codewords[-value] = BitCode(mask - value, numBits);
		codewords[+value] = BitCode(value, numBits);
	}*/

	//writer.writeLastSymbol();
//}

//void Encode_color(IMAGE3s &matIn,AXI_BIT_WRITER2 &datawriter) {
	//writer.writeStartSymbol();
	int16_t linebuffer1[8][MAX_WIDTH] = {0};
	int16_t linebuffer2[8][MAX_WIDTH] = {0};
	int16_t linebuffer3[8][MAX_WIDTH] = {0};
	int32_t window1[8*8] = {0};
	int32_t window2[8*8] = {0};
	int32_t window3[8*8] = {0};
	int line = 0;
	int16_t lastYDC = 0, lastCbDC = 0, lastCrDC = 0;
	for (int y = 0; y < MAX_HEIGHT; y++) {
		for (int x = 0; x < MAX_WIDTH; x++) {
			#pragma HLS loop flatten off
			#pragma HLS pipeline II=2
			hls::Scalar<3, int16_t> pixel;
			matIn >> pixel;

			linebuffer1[line][x] = pixel.val[0];	//y
			linebuffer2[line][x] = pixel.val[1];	//cr
			linebuffer3[line][x] = pixel.val[2];	//cb
		}
		if (line == 7) {
			line = 0;
			for (int step = 0; step < MAX_WIDTH; step += 8) {
				for (int i = 0; i < 8; i++) {
					for (int j = 0; j < 8; j++) {
						window1[j*8+i] = linebuffer1[j][i + step];	//y
						window2[j*8+i] = linebuffer2[j][i + step];	//cr
						window3[j*8+i] = linebuffer3[j][i + step];	//cb
					}
				}
				lastYDC = encodeBlock(writer, window1, scaledLuminance,
						lastYDC, huffmanLuminanceDC, huffmanLuminanceAC);
				lastCbDC = encodeBlock(writer, window2, scaledChrominance,
						lastCbDC, huffmanChrominanceDC, huffmanChrominanceAC);
				lastCrDC = encodeBlock(writer, window3, scaledChrominance,
						lastCrDC, huffmanChrominanceDC, huffmanChrominanceAC);
			}
		} else {
			line++;
		}
	}

	// 写死固定输出
	writer.flush();
	writer.writeEndofJPEG();
}

void readMat(IMAGE3s &matIn,uint8_t quality, AXI_STREAM &outputStream){
#pragma HLS Dataflow
	AXI_BIT_WRITER2 writer; //headerwriter,datawriter;
	//writeHeaderGray(headerwriter,quality);
	//Encode_gray(matIn,datawriter);
	encode_jpeg(writer, quality, matIn);
	mergeStream(writer.stream,outputStream);
}

