CC	= clang
CFLAGS	= -Wall -Wextra -O2 -I../include -I/usr/local/include -DLIBXML_STATIC -DLIBXSLT_STATIC -DXMLSEC \
		-DXMLSEC_STATIC -D_FILE_OFFSET_BITS=64 -DNDEBUG
LIBS	= -lz -lssl -lcrypto -lxml2 -lxmlsec1-openssl -lxmlsec1 -lxslt -lexpat -ltiff -lopenjpeg -lopendcp -lasdcp -lkumu 
LDFLAGS	= -L./../libopendcp -L/usr/local/lib -L/usr/lib -L/lib -Wl,-rpath,/usr/local/lib

opendcp_j2k:

	${CC} ${CFLAGS} ${LDFLAGS} -o opendcp_j2k opendcp_cli.c opendcp_j2k_cmd.c ${LIBS}

clean:
	rm -f *.o
	rm -f opendcp_j2k
