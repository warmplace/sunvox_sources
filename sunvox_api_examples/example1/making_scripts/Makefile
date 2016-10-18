SUNDOG = ../../../sundog_engine/

APPNAME = sunvox_test

include $(SUNDOG)various/sundog_makefile.inc

## ------------------------------------------ ##
## Main target options:                       ##
## ------------------------------------------ ##

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
    #LIBS = -L/usr/X11R6/lib -lpthread -lrt -lstdc++ -lX11 -lGL -lXext
    # X11 libs:
    #LIBS = -L/usr/X11R6/lib -lpthread -lrt -lstdc++ -lX11
    # SDL:
    LIBS = -L/usr/X11R6/lib -lpthread -lrt -lstdc++ -lSDL
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
LIB_SUNVOX = sunvox_engine.o
LIB_PSYNTH = \
    psynth_net.o \
    psynths_echo.o \
    psynths_filter.o \
    psynths_generator.o \
    psynths_kicker.o \
    psynths_sampler.o \
    psynths_distortion.o \
    psynths_flanger.o \
    psynths_spectravoice.o \
    psynths_loop.o \
    psynths_delay.o \
    psynths_fm.o \
    psynths_lfo.o \
    psynths_reverb.o \
    psynths_vocal_filter.o
ADD_LIBS = $(LIB_SUNVOX) $(LIB_PSYNTH)
OBJECTS += user_code.o $(ADD_LIBS)

## ------------------------------------------ ##
## User application rules:                    ##
## ------------------------------------------ ##

user_code.o: ../main/user_code.cpp $(STD_INCS)
	$(CC) $(DEFINES) -c ../main/user_code.cpp

sunvox_engine.o: ../main/sunvox_engine.cpp ../main/sunvox_engine.h $(STD_INCS)
	$(CC) $(DEFINES) -c ../main/sunvox_engine.cpp

sunvox_tables.o: ../main/sunvox_tables.cpp $(STD_INCS)
	$(CC) $(DEFINES) -c ../main/sunvox_tables.cpp

psynth_net.o: ../main/psynth/psynth_net.cpp ../main/psynth/psynth_net.h $(STD_INCS)
	$(CC) $(DEFINES) -c ../main/psynth/psynth_net.cpp

psynths_echo.o: ../main/psynth/psynths_echo.cpp ../main/psynth/psynth.h $(STD_INCS)
	$(CC) $(DEFINES) -c ../main/psynth/psynths_echo.cpp

psynths_flanger.o: ../main/psynth/psynths_flanger.cpp ../main/psynth/psynth.h $(STD_INCS)
	$(CC) $(DEFINES) -c ../main/psynth/psynths_flanger.cpp

psynths_filter.o: ../main/psynth/psynths_filter.cpp ../main/psynth/psynth.h $(STD_INCS)
	$(CC) $(DEFINES) -c ../main/psynth/psynths_filter.cpp

psynths_generator.o: ../main/psynth/psynths_generator.cpp ../main/psynth/psynth.h $(STD_INCS)
	$(CC) $(DEFINES) -c ../main/psynth/psynths_generator.cpp

psynths_spectravoice.o: ../main/psynth/psynths_spectravoice.cpp ../main/psynth/psynth.h $(STD_INCS)
	$(CC) $(DEFINES) -c ../main/psynth/psynths_spectravoice.cpp

psynths_kicker.o: ../main/psynth/psynths_kicker.cpp ../main/psynth/psynth.h $(STD_INCS)
	$(CC) $(DEFINES) -c ../main/psynth/psynths_kicker.cpp

psynths_sampler.o: ../main/psynth/psynths_sampler.cpp ../main/psynth/psynth.h $(STD_INCS)
	$(CC) $(DEFINES) -c ../main/psynth/psynths_sampler.cpp

psynths_distortion.o: ../main/psynth/psynths_distortion.cpp ../main/psynth/psynth.h $(STD_INCS)
	$(CC) $(DEFINES) -c ../main/psynth/psynths_distortion.cpp

psynths_loop.o: ../main/psynth/psynths_loop.cpp ../main/psynth/psynth.h $(STD_INCS)
	$(CC) $(DEFINES) -c ../main/psynth/psynths_loop.cpp

psynths_delay.o: ../main/psynth/psynths_delay.cpp ../main/psynth/psynth.h $(STD_INCS)
	$(CC) $(DEFINES) -c ../main/psynth/psynths_delay.cpp

psynths_fm.o: ../main/psynth/psynths_fm.cpp ../main/psynth/psynth.h $(STD_INCS)
	$(CC) $(DEFINES) -c ../main/psynth/psynths_fm.cpp

psynths_lfo.o: ../main/psynth/psynths_lfo.cpp ../main/psynth/psynth.h $(STD_INCS)
	$(CC) $(DEFINES) -c ../main/psynth/psynths_lfo.cpp

psynths_reverb.o: ../main/psynth/psynths_reverb.cpp ../main/psynth/psynth.h $(STD_INCS)
	$(CC) $(DEFINES) -c ../main/psynth/psynths_reverb.cpp

psynths_vocal_filter.o: ../main/psynth/psynths_vocal_filter.cpp ../main/psynth/psynth.h $(STD_INCS)
	$(CC) $(DEFINES) -c ../main/psynth/psynths_vocal_filter.cpp
