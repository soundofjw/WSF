bin_PROGRAMS = wsftool meltwsf


# set the include path found by configure
INCLUDES = $(all_includes)

# the library search path.
meltwsf_SOURCES = MeltWSF.cpp WGMPack.cpp WSFComp.h WSFGlob.h WSFLoaders.cpp \
	WSFLoadersV1.cpp WSFMod.cpp WSFMod.h WSFModV1.cpp WSFModV1.h WSFPack.cpp WSFPack.h \
	WSFPackV1.cpp WSFPackV1.h WSFComp.c WSFOgg.cpp
AM_CFLAGS = -DNOOGG
AM_CXXFLAGS = -DNOOGG
wsftool_SOURCES = lzoconf.h WGMPack.cpp WGMPack.h WSFComp.c WSFComp.h WSFDoc.h \
	WSFGlob.h WSFLoaders.cpp WSFLoadersV1.cpp WSFMod.cpp WSFMod.h WSFModV1.cpp \
	WSFModV1.h WSFOgg.cpp WSFPack.cpp WSFPack.h WSFPackV1.cpp WSFPackV1.h WSFTool.cpp \
	WSFTool.h WSFToolMisc.cpp WSFSpecialTags.h

