CC      = gcc
CPP     = g++
AR      = ar
RM      = rm -f
CFLAGS  = -O3 -Wall -pedantic -Iinclude
OBJ     = compare.o compare_common.o compare_neon.o compare_posix.o compare_win.o convert.o convert_argb.o convert_from.o convert_from_argb.o cpu_id.o format_conversion.o mjpeg_decoder.o planar_functions.o rotate.o rotate_argb.o rotate_mips.o rotate_neon.o row_any.o row_common.o row_mips.o row_neon.o row_posix.o row_win.o scale.o scale_argb.o scale_argb_neon.o scale_mips.o scale_neon.o video_common.o
ASMOBJ = memcpy_mips.o row_x86.o x86inc.o
BIN     = libyuv.a

%.o: %.cc
	$(CPP) -c -o $@ $< $(CFLAGS)

$(BIN): $(OBJ)
	ar r $(BIN) $(OBJ)
	ranlib $(BIN)

clean:
	$(RM) *.o
