CC	= clang
CFLAGS	= -Wall -Wextra -O2 -I../include -I/usr/local/include -DLIBXML_STATIC -DLIBXSLT_STATIC -DXMLSEC \
		-DXMLSEC_STATIC -D_FILE_OFFSET_BITS=64 -DNDEBUG
LIBS	= -pthread -lz -lssl -lcrypto -lxml2 -lxmlsec1-openssl -lxmlsec1 -lxslt -lexpat -ltiff -lopenjpeg -lopendcp -lasdcp -lkumu 
LDFLAGS	= -L./../libopendcp -L/usr/local/lib -L/usr/lib -L/lib -Wl,-rpath,/usr/local/lib

all: opendcp_j2k opendcp_mxf opendcp_xml opendcp_xml_verify

opendcp_j2k:

	${CC} ${CFLAGS} ${LDFLAGS} -o opendcp_j2k opendcp_cli.c opendcp_j2k_cmd.c ${LIBS}

opendcp_mxf:
	
	${CC} ${CFLAGS} ${LDFLAGS} -o opendcp_mxf opendcp_cli.c opendcp_mxf_cmd.c ${LIBS}

opendcp_xml:

	${CC} ${CFLAGS} ${LDFLAGS} -o opendcp_xml opendcp_cli.c opendcp_xml_cmd.c ${LIBS}

opendcp_xml_verify:

	${CC} ${CFLAGS} ${LDFLAGS} -o opendcp_xml_verify opendcp_cli.c opendcp_xml_verify_cmd.c ${LIBS}

clean:
	rm -f *.o
	rm -f opendcp_j2k
	rm -f opendcp_mxf
	rm -f opendcp_xml
	rm -f opendcp_xml_verify

