#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lz4-1.9.4/lz4.h"

#define FILE_MAX_SIZE 128*1024*1024
#define FILE_COUNT 17

void read_bmp(const char *);
void rgb_reformat(char[], size_t);

int main(int argc, char *argv[]) {

    char files[FILE_COUNT][64] = {
            "Baboon.bmp", "BaboonRGB.bmp", "Barbara.bmp", "Cameraman.bmp", "Goldhill.bmp", "Lena.bmp",
            "LenaRGB.bmp", "Peppers.bmp", "PeppersRGB.bmp",
            "test2.bmp", "test3.bmp", "test5.bmp", "test6.bmp", "test8.bmp", "test9.bmp", "test10.bmp", "test11.bmp"
    };
    char fileName[1024], *file;
    int i;

    for (i = 0; i < FILE_COUNT; i++) {
        file = files[i];
        fprintf(stdout, "%s ", file);
        sprintf(fileName, "../../img_compression/images/%s", file);
        read_bmp(fileName);
    }

    return 0;
}

void rgb_reformat(char buf[], size_t s)
{
    char *nb = NULL;
    int i, j, k, l;

    if (s % 3) {
        s += 3 - (s % 3);
    }
    nb = (char *) malloc(sizeof(char) * s);
    if (nb == NULL) {
        fprintf(stderr, "RGB reformat malloc failed.\n");
        return;
    }

    for (i = l = 0, j = (int)s/3, k = (int)s*2/3; k < s; i++, j++, k++, l += 3) {
        nb[i] = buf[l];
        nb[j] = buf[l+1];
        nb[k] = buf[l+2];
    }

    memcpy(buf, nb, s);
}

void read_bmp(const char name[])
{
    FILE *file = NULL;
    char *buf = NULL, *orig_buf = NULL, *lz4_buf = NULL;
    size_t read_size, orig_size;
    int lz4_buf_size, lz4_size;
    char bmp_fmt;


    buf = (char*) malloc(sizeof(char) * FILE_MAX_SIZE);
    if (buf == NULL) {
        fprintf(stderr, "Alloc buffer failed.\n");
        return;
    }

    file = fopen(name, "rb");
    if (file == NULL) {
        fprintf(stderr, "File open failed.\n");
        free(buf);
        return;
    }

    read_size = fread(buf, sizeof(char), FILE_MAX_SIZE, file);
    if (read_size == FILE_MAX_SIZE) {
        fprintf(stdout, "File maybe bigger than buffer size.\n");
    }
    fclose(file);
    if (read_size <= 54) {
        fprintf(stderr, "File size smaller than or equal with 54?\n");
        free(buf);
        return;
    }
    orig_size = read_size - 54;
    orig_buf = buf + 54;
    bmp_fmt = buf[28];

/*    if (bmp_fmt == 24) {
        rgb_reformat(orig_buf, orig_size);
    }*/

    lz4_buf_size = LZ4_compressBound((int)orig_size);
    lz4_buf = (char *) malloc(lz4_buf_size);
    if (lz4_buf == NULL) {
        fprintf(stderr, "Alloc lz4 buffer failed.\n");
        free(buf);
        return;
    }

    lz4_size = LZ4_compress_default(orig_buf, lz4_buf, (int)orig_size, lz4_buf_size);
    if (lz4_size <= 0) {
        fprintf(stderr, "LZ4_compress_default failed with code %d.\n", lz4_size);
    } else {
        fprintf(stdout, "LZ4 compress ratio %lf%% %d %d %d %d\n", (double)lz4_size*100.0/(double)orig_size, lz4_size, orig_size, lz4_buf_size, lz4_size);
    }

    free(lz4_buf);
    free(buf);

}

/*
Baboon.bmp LZ4 compress ratio 100.377703% 264164 263170 264218 264164
BaboonRGB.bmp LZ4 compress ratio 100.383490% 787906 784896 787990 787906
Barbara.bmp LZ4 compress ratio 100.392522% 264203 263170 264218 264203
Cameraman.bmp LZ4 compress ratio 94.478832% 62887 66562 66839 62887
Goldhill.bmp LZ4 compress ratio 100.303606% 263969 263170 264218 263969
Lena.bmp LZ4 compress ratio 100.159973% 263591 263170 264218 263591
LenaRGB.bmp LZ4 compress ratio 100.196589% 784900 783360 786448 784900
Peppers.bmp LZ4 compress ratio 100.376183% 264160 263170 264218 264160
PeppersRGB.bmp LZ4 compress ratio 99.838734% 780060 781320 784400 780060
test2.bmp LZ4 compress ratio 71.794103% 8619600 12006000 12053098 8619600
test3.bmp LZ4 compress ratio 60.198720% 3744842 6220800 6245211 3744842
test5.bmp LZ4 compress ratio 47.243490% 2938923 6220800 6245211 2938923
test6.bmp LZ4 compress ratio 46.286892% 2879415 6220800 6245211 2879415
test8.bmp LZ4 compress ratio 59.511880% 3702115 6220800 6245211 3702115
test9.bmp LZ4 compress ratio 88.218021% 205795 233280 234210 205795
test10.bmp LZ4 compress ratio 75.739165% 4711582 6220800 6245211 4711582
test11.bmp LZ4 compress ratio 73.710802% 5731752 7776000 7806510 5731752
 */

/*
rgb reformat

Baboon.bmp LZ4 compress ratio 100.377703% 264164 263170 264218 264164
BaboonRGB.bmp LZ4 compress ratio 100.388714% 787947 784896 787990 787947
Barbara.bmp LZ4 compress ratio 100.392522% 264203 263170 264218 264203
Cameraman.bmp LZ4 compress ratio 94.478832% 62887 66562 66839 62887
Goldhill.bmp LZ4 compress ratio 100.303606% 263969 263170 264218 263969
Lena.bmp LZ4 compress ratio 100.159973% 263591 263170 264218 263591
LenaRGB.bmp LZ4 compress ratio 99.911152% 782664 783360 786448 782664
Peppers.bmp LZ4 compress ratio 100.376183% 264160 263170 264218 264160
PeppersRGB.bmp LZ4 compress ratio 98.661496% 770862 781320 784400 770862
test2.bmp LZ4 compress ratio 76.560178% 9191815 12006000 12053098 9191815
test3.bmp LZ4 compress ratio 62.362252% 3879431 6220800 6245211 3879431
test5.bmp LZ4 compress ratio 46.413468% 2887289 6220800 6245211 2887289
test6.bmp LZ4 compress ratio 46.649032% 2901943 6220800 6245211 2901943
test8.bmp LZ4 compress ratio 58.851177% 3661014 6220800 6245211 3661014
test9.bmp LZ4 compress ratio 91.652949% 213808 233280 234210 213808
test10.bmp LZ4 compress ratio 78.995933% 4914179 6220800 6245211 4914179
test11.bmp LZ4 compress ratio 85.963477% 6684520 7776000 7806510 6684520
 */
