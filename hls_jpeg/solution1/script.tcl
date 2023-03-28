############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2017 Xilinx, Inc. All Rights Reserved.
############################################################
open_project JPEG-Encoder-Image-Compression-main
set_top top_level
add_files JPEG-Encoder-Image-Compression-main/src/constrains.h
add_files JPEG-Encoder-Image-Compression-main/src/jpeg.cpp
add_files JPEG-Encoder-Image-Compression-main/src/jpeg.hpp
add_files JPEG-Encoder-Image-Compression-main/src/toplevel.cpp
add_files JPEG-Encoder-Image-Compression-main/src/toplevel.hpp
add_files -tb JPEG-Encoder-Image-Compression-main/src/files/LenaRGB.bmp
add_files -tb JPEG-Encoder-Image-Compression-main/src/jpeg.cpp
add_files -tb JPEG-Encoder-Image-Compression-main/src/jpeg.hpp
add_files -tb JPEG-Encoder-Image-Compression-main/src/tb.cpp
add_files -tb JPEG-Encoder-Image-Compression-main/src/files/test2.bmp
add_files -tb JPEG-Encoder-Image-Compression-main/src/toplevel.cpp
add_files -tb JPEG-Encoder-Image-Compression-main/src/toplevel.hpp
open_solution "solution1"
set_part {xc7z020clg400-2} -tool vivado
create_clock -period 10 -name default
#source "./JPEG-Encoder-Image-Compression-main/solution1/directives.tcl"
csim_design
csynth_design
cosim_design -O -trace_level all -tool xsim
export_design -flow impl -rtl verilog -format ip_catalog
