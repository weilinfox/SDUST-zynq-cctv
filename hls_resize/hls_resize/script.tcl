############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2017 Xilinx, Inc. All Rights Reserved.
############################################################
open_project hls_resize
set_top hls_resize
add_files hls_resize/hls_resize.h
add_files hls_resize/hls_resize.cpp
add_files -tb hls_resize/test.png
add_files -tb hls_resize/main.cpp
open_solution "hls_resize"
set_part {xc7z020clg400-2} -tool vivado
create_clock -period 10 -name default
#source "./hls_resize/hls_resize/directives.tcl"
csim_design
csynth_design
cosim_design -tool xsim
export_design -rtl verilog -format ip_catalog -description "Resize 1920x1080 to 800x480" -vendor "weilinfox" -version "1.3" -display_name "hls_resize"
