
XNVIEW_PLUGIN = xnview/Xpjg.usr
PACKJPG_LIB = packJPG/bitops.o packJPG/aricoder.o packJPG/packjpg.o
#PACKJPG_LIB = packJPG/packJPGlib.a #dunno why it doesn't work
LIBJPEG_LIB = libjpeg-turbo/.libs/libjpeg.a

DEPS = $(PACKJPG_LIB) xnview/xpjg.o $(LIBJPEG_LIB)
CFLAGS = -Wall -Wl,--kill-at -s -O2 -shared -static-libgcc -static-libstdc++
CPPFLAGS = $(CFLAGS) -o $(XNVIEW_PLUGIN) $(DEPS)

$(XNVIEW_PLUGIN): $(DEPS)
	g++ $(CFLAGS) -o $@ $^

xnview/xpjg.o: xnview/xpjg.c
	cd xnview
	gcc -Wall -march=i686 -O3 -o $@ -c $^

fprofile-use:
	cd packJPG && make --makefile=Makefile_dll_fprofile_use
	cd xnview && gcc -Wall -march=i686 -O3 -o xpjg.o -c xpjg.c
	g++ -fprofile-use $(CPPFLAGS)

fprofile-generate:
	cd packJPG && make --makefile=Makefile_dll_fprofile_generate
	cd xnview && gcc -Wall -march=i686 -O3 -o xpjg.o -c xpjg.c
	g++ -fprofile-generate $(CPPFLAGS)

$(PACKJPG_LIB): 
	cd packJPG && make --makefile=Makefile_lib

$(LIBJPEG_LIB):
	echo "Please build libjpeg"
	# built with:
	# export CFLAGS="-march=i686" && ./configure --with-jpeg8 && make
