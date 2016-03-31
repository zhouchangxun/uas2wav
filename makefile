#Target name
TGT = uas2wav 

#Target dir
TGTDIR = ./output/bin

#Object dir
OBJDIR = ./output

#Source files
SRC = u2w.cpp

PWLIB_INC = ../common/pwlib/include
PWLIB_LIB = ../common/pwlib/lib


EXT_INCS_DIR = -I$(PWLIB_INC) -I../../lib_base/ -I../../common
EXT_LIB_DIR = -L$(PWLIB_LIB) -L../../ump_out/lib -L../common/ipp/lib

STD_LIB = -lpthread -lrt  -lippscemerged -lippsemerged -lippscmerged -lippsmerged -lippcore
EXT_LIB_D = -laio_d -laio2_d -lSpeech_d  -lpt_linux_x86_d
EXT_LIB =  -laio_r -laio2_r -lSpeech_r -lspeexdsp_r -lpt_linux_x86_r


CXXFLAGS += -Wall -DPTRACING_zhoucx -D_REENTRANT -felide-constructors 

CCFLAGS += -Wall -g

#include ../../common/mak/makefile_pch
all:
	g++ $(CXXFLAGS) $(SRC) $(EXT_INCS_DIR) $(EXT_LIB_DIR)  $(EXT_LIB) $(STD_LIB) -o $(TGT)
