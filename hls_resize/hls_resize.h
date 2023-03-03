#ifndef __HLS_RESIZE_H__
#define __HLS_RESIZE_H__

#include "hls_video.h"

#define SRC_HEIGHT 1080
#define SRC_WIDTH  1920

#define DST_HEIGHT 480
#define DST_WIDTH 800

typedef hls::stream<ap_axiu<24,1,1,1> >					AXI_STREAM;

typedef hls::Mat<SRC_HEIGHT, SRC_WIDTH, HLS_8UC3>		SRC_IMAGE;
typedef hls::Mat<DST_HEIGHT, DST_WIDTH, HLS_8UC3>		DST_IMAGE;

void hls_resize(AXI_STREAM&, AXI_STREAM&, int, int, int, int);

#endif
