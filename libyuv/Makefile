CC      = gcc
CPP     = g++
AR      = ar
RM      = rm -f
CFLAGS  = -msse2 -O3 -fopenmp -Wl,--strip-all -Iinclude #-Wall -pedantic
OBJ     = compare.o compare_common.o compare_neon.o compare_posix.o compare_win.o convert.o convert_argb.o convert_from.o convert_from_argb.o convert_jpeg.o convert_to_argb.o convert_to_i420.o cpu_id.o format_conversion.o mjpeg_decoder.o mjpeg_validate.o planar_functions.o rotate.o rotate_argb.o rotate_mips.o rotate_neon.o row_any.o row_common.o row_mips.o row_neon.o row_posix.o row_win.o scale.o scale_argb.o scale_argb_neon.o scale_common.o scale_mips.o scale_neon.o video_common.o
ASMOBJ = row_x86.o x86inc.o
BIN     = libyuv.a

%.o: %.cc
	$(CPP) -c -o $@ $< $(CFLAGS)

$(BIN): $(OBJ)
	ar r $(BIN) $(OBJ)
	ranlib $(BIN)

clean:
	$(RM) *.o
