
#include "hls_resize.h"

void hls_resize(AXI_STREAM &src_axi, AXI_STREAM &dst_axi, int &src_rows, int &src_cols, int &dst_rows, int &dst_cols)
{

#pragma HLS INTERFACE axis port = src_axi
#pragma HLS INTERFACE axis port = dst_axi
#pragma HLS INTERFACE s_axilite port = src_rows
#pragma HLS INTERFACE s_axilite port = src_cols
#pragma HLS INTERFACE s_axilite port = dst_rows
#pragma HLS INTERFACE s_axilite port = dst_cols
#pragma HLS INTERFACE s_axilite port = return

SRC_IMAGE imag_0(src_rows, src_cols);
DST_IMAGE imag_1(dst_rows, dst_cols);

#pragma HLS dataflow
hls::AXIvideo2Mat(src_axi, imag_0);
//hls::Resize_opr_linear(imag_0, imag_1);
hls::Resize(imag_0, imag_1, HLS_INTER_LINEAR );
hls::Mat2AXIvideo(imag_1, dst_axi);

}
