CC	= clang
CFLAGS	= -fPIC -c -Wall -Wextra -O2 -I../include -I/usr/local/include/libxml2 -I/usr/local/include/xmlsec1 -I/usr/local/include -DLIBXML_STATIC -DLIBXSLT_STATIC -DXMLSEC -DXMLSEC_STATIC -D_FILE_OFFSET_BITS=64 -DNDEBUG

libopendcp:

	${CC} ${CFLAGS}  \
		opendcp_j2k.c \
		opendcp_xml.c \
		opendcp_checksum.c \
		opendcp_common.c \
		opendcp_error.c \
		opendcp_log.c \
		asdcp_intf.cpp \
		opendcp_image.c \
		codecs/opendcp_decoder.c \
		codecs/opendcp_decoder_tif.c \
		codecs/opendcp_decoder_bmp.c \
		codecs/opendcp_decoder_dpx.c \
		codecs/opendcp_decoder_openjpeg.c \
		codecs/opendcp_encoder.c \
		codecs/opendcp_encoder_kakadu.c \
		codecs/opendcp_encoder_openjpeg.c \
		codecs/opendcp_encoder_tif.c \
		codecs/opendcp_encoder_ragnarok.c \
		codecs/opendcp_encoder_remote.c \
		opendcp_xml_sign.c 

	${CC} -shared \
		-o libopendcp.so \
				asdcp_intf.o \
				opendcp_checksum.o \
				opendcp_common.o \
				opendcp_decoder.o \
				opendcp_decoder_bmp.o \
				opendcp_decoder_dpx.o \
				opendcp_decoder_openjpeg.o \
				opendcp_decoder_tif.o \
				opendcp_encoder.o \
				opendcp_encoder_kakadu.o \
				opendcp_encoder_openjpeg.o \
				opendcp_encoder_ragnarok.o \
				opendcp_encoder_remote.o \
				opendcp_encoder_tif.o \
				opendcp_error.o \
				opendcp_image.o \
				opendcp_j2k.o \
				opendcp_log.o \
				opendcp_xml.o \
				opendcp_xml_sign.o \
		-lc

clean:
	rm -f *.o
	rm -f *.so
