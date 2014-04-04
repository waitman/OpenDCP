OpenDCP
-------
OpenDCP is a collection of tools to create digital cinema packages (DCP).

opendcp_j2k:		Convert TIFF images to DCI compliant jpeg2000 images 
opendcp_mxf:		Create MXF files from a sequence of jpeg2000 images, mpeg2 file, or pcm wav files
opendcp_xml:		Generate the XML files need for DCPs
opendcp_xml_verify:	Verify the digital signature of an XML file	
opendcp:            GUI version of the tool

Refer to COMPILE.txt for compiling. Help is available using the -h or --help arguments.

If you have any issues, please create a bug or enhancement at http://code.google.com/p/opendcp/issues/list.


**** NOTE: Modified Version based on upstream svn .30
by Waitman Gobble <ns@waitman.net>
This version is modified to build on FreeBSD 11.0-CURRENT

1) Replaced cmake build with Makefile
2) Removed libasdcplib (see ports section below)
3) libdcp is built as shared library
4) build isn't complete. switch to libopendcp/ and 'make', then switch to cli/ and 'make'. 
	Copy libopendcp/libopendcp.so to LD_PATH, ie /usr/local/lib
	there is no install at the moment. 'make clean' works.
5) removed OpenMP code. 
6) changed opendcp_j2k so it only operates on one file at a time.
7) added runj2k (see license in cli/LICENSE-BSD). This program will let you 
	specify number of threads to launch, or optionally you can use a switch
	to have the program calculate the optimal number of threads.


*** PORTS ****

security/xmlsec1
textproc/libxslt
textproc/expat2
graphics/tiff
graphics/openjpeg
multimedia/asdcplib

Note: The multimedia/asdcplib port does not install all the header files.
Workaround is to 'make install', cd to wrksrc and copy *.h to
/usr/local/include. Then 'make clean' to clean up wrksrc.
multimedia/asdcplib also installs libkumu.


