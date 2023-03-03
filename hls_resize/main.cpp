#include "hls_resize.h"

#include "hls_opencv.h"

#include "iostream"

#define INPUT_IMAGE "test.png"

int main (int argc, char** argv)
{
	IplImage* src = cvLoadImage(INPUT_IMAGE);//,CV_LOAD_IMAGE_ANYCOLOR);
	IplImage* dst = cvCreateImage(cvSize(DST_WIDTH,DST_HEIGHT),src->depth,src->nChannels);

	AXI_STREAM src_axi, dst_axi;

	IplImage2AXIvideo(src, src_axi);

	hls_resize(src_axi,dst_axi, SRC_HEIGHT, SRC_WIDTH, DST_HEIGHT, DST_WIDTH);

	AXIvideo2IplImage(dst_axi, dst);

	cvShowImage("src", src);
	cvShowImage("dst", dst);

	cvWaitKey(0);

	return 0;
}
