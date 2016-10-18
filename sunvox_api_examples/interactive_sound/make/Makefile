SUNDOG = ../../../sundog_engine/
SUNVOX = ../../../sunvox_engine/

APPNAME = sunvox_test

include $(SUNDOG)various/sundog_makefile.inc
include $(SUNVOX)sunvox_makefile.inc

## ------------------------------------------ ##
## Main target options:                       ##
## ------------------------------------------ ##

CFLAGS = -O3 -s
#CFLAGS = -g

ifeq ($(MAKECMDGOALS),palm)
    SDK = /usr/local/share/palmdev/sdk-5r3/include/
    SUNDOG_DEFINES = \
	-DARCH_ARM \
	-DPALMOS \
	-DCOLOR8BITS \
	-DGRAYSCALE \
	-DDIRECTDRAW \
	-DNOSTORAGE \
	-DSTYPE_INT16
endif
ifeq ($(MAKECMDGOALS),linux)
    #(remove -lrt on FreeBSD and Cygwin)
    # X11 (OpenGL) libs:
    #LIBS = -L/usr/X11R6/lib -lpthread -lrt -lstdc++ -lX11 -lGL -lXext -lasound
    # X11 libs:
    #LIBS = -L/usr/X11R6/lib -lpthread -lrt -lstdc++ -lX11 -lasound
    # SDL:
    LIBS = -L/usr/X11R6/lib -lpthread -lrt -lstdc++ -lSDL -lasound
    SUNDOG_DEFINES = \
	-DARCH_X86 \
	-DLINUX \
	-DDIRECTDRAW \
	-DCOLOR32BITS \
	-DSTYPE_FLOAT
endif
ifeq ($(MAKECMDGOALS),wince)
    SUNDOG_DEFINES = \
	-DARCH_ARM \
	-DWINCE \
	-DCOLOR16BITS \
	-DDIRECTDRAW \
	-DRGB565 \
	-DSTYPE_INT16
endif

## ------------------------------------------ ##
## Object files and includes:                 ##
## ------------------------------------------ ##

# User application:
ADD_LIBS = \
    $(LIB_SUNVOX)
OBJECTS += user_code.o $(ADD_LIBS)

## ------------------------------------------ ##
## User application rules:                    ##
## ------------------------------------------ ##

user_code.o: ../main/user_code.cpp $(STD_INCS)
	$(CC) $(DEFINES) -c ../main/user_code.cpp
