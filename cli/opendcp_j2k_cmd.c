/*
    OpenDCP: Builds Digital Cinema Packages
    Copyright (c) 2010-2013 Terrence Meiczinger, All Rights Reserved

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <malloc_np.h>
#include <opendcp.h>
#include <opendcp_encoder.h>
#include <opendcp_decoder.h>
#include "opendcp_cli.h"



/* prototypes */
char *basename_noext(const char *str);
int   is_dir(char *path);
void  build_j2k_filename(const char *in, char *path, char *out);
void  version();
void  dcp_usage();

void version() {
    FILE *fp;

    fp = stdout;
    fprintf(fp, "\n%s version %s %s\n\n", OPENDCP_NAME, OPENDCP_VERSION, OPENDCP_COPYRIGHT);

    exit(0);
}

void dcp_usage() {
    FILE *fp;
    fp = stdout;

    fprintf(fp, "\n%s version %s %s\n\n", OPENDCP_NAME, OPENDCP_VERSION, OPENDCP_COPYRIGHT);
    fprintf(fp, "Usage:\n");
    fprintf(fp, "       opendcp_j2k -i <file> -o <file> [options ...]\n\n");
    fprintf(fp, "Required:\n");
    fprintf(fp, "       -i | --input <file>            - input file\n");
    fprintf(fp, "       -o | --output <file>           - output directory\n");
    fprintf(fp, "\n");
    fprintf(fp, "Options:\n");
    fprintf(fp, "       -r | --rate <rate>                 - frame rate (default 24)\n");
    fprintf(fp, "       -p | --profile <profile>           - profile cinema2k | cinema4k (default cinema2k)\n");
    fprintf(fp, "       -b | --bw                          - max Mbps bandwitdh (default: 250)\n");
    fprintf(fp, "       -3 | --3d                          - adjust frame rate for 3D\n");
    fprintf(fp, "       -e | --encoder <openjpeg | kakadu> - jpeg2000 encoder (default openjpeg)\n");
    fprintf(fp, "       -x | --no_xyz                      - do not perform rgb->xyz color conversion\n");
    fprintf(fp, "       -c | --colorspace <color>          - select source colorpsace: (srgb, rec709, p3, srgb_complex, rec709_complex)\n");
    fprintf(fp, "       -f | --calculate                   - Calculate RGB->XYZ values instead of using LUT\n");
    fprintf(fp, "       -g | --dpx <linear | film | video> - process dpx image as linear, log film, or log video (default linear)\n");
    fprintf(fp, "       -z | --resize                      - resize image to DCI compliant resolution\n");
    fprintf(fp, "       -s | --start                       - start frame\n");
    fprintf(fp, "       -d | --end                         - end frame\n");
    fprintf(fp, "       -t | --threads <threads>           - set number of threads (default 4)\n");
    fprintf(fp, "       -m | --tmp_dir                     - sets temporary directory (usually tmpfs one) to save there temporary tiffs for Kakadu\n");
    fprintf(fp, "       -n | --no_overwrite                - do not overwrite existing jpeg2000 files\n");
    fprintf(fp, "       -l | --log_level <level>           - sets the log level 0:Quiet, 1:Error, 2:Warn (default),  3:Info, 4:Debug\n");
    fprintf(fp, "       -h | --help                        - show help\n");
    fprintf(fp, "       -v | --version                     - show version\n");
    fprintf(fp, "\n\n");
    fprintf(fp, "^ Kakadu requires you to download and have the kdu_compress utility in your path.\n");
    fprintf(fp, "  You must agree to the Kakadu non-commerical licensing terms or have a commerical license and assume all respsonsibility of its use.\n");
    fprintf(fp, "\n\n");

    fclose(fp);
    exit(0);
}

char *substring(const char *str, size_t begin, size_t len) {
    char   *result;

    if (str == 0 || strlen(str) == 0 || strlen(str) < begin || strlen(str) < (begin + len)) {
        return NULL;
    }

    result = (char *)malloc(len);

    if (!result) {
        return NULL;
    }

    result[0] = '\0';
    strncat(result, str + begin, len);

    return result;
}

char *basename_noext(const char *str) {
    if (str == 0 || strlen(str) == 0) {
        return NULL;
    }

    char *base = strrchr(str, '/') + 1;
    char *ext  = strrchr(str, '.');

    return strndup(base, ext - base);
}

void build_j2k_filename(const char *in, char *path, char *out) {
    OPENDCP_LOG(LOG_DEBUG, "Building filename from %s", in);

    if (!is_dir(path)) {
        snprintf(out, MAX_FILENAME_LENGTH, "%s", path);
    }
    else {
        char *base = basename_noext(in);
        snprintf(out, MAX_FILENAME_LENGTH, "%s/%s.j2c", path, base);

        if (base) {
            free(base);
        }
    }
}

int is_dir(char *path) {
    struct stat st_in;

    if (stat(path, &st_in) != 0 ) {
        return 0;
    }

    if (S_ISDIR(st_in.st_mode)) {
        return 1;
    }

    return 0;
}

int main (int argc, char **argv) {
    int c, result;
    opendcp_t *opendcp;
    char *in_path  = NULL;
    char *out_path = NULL;

    if ( argc <= 1 ) {
        dcp_usage();
    }

    opendcp = opendcp_create();

    /* set initial values */
    opendcp->log_level       = LOG_WARN;
    opendcp->cinema_profile  = DCP_CINEMA2K;
    opendcp->frame_rate      = 24;
    opendcp->j2k.xyz         = 1;
    opendcp->j2k.encoder     = OPENDCP_ENCODER_OPENJPEG;
    opendcp->j2k.start_frame = 1;
    opendcp->j2k.bw          = 250;
    opendcp->tmp_path        = NULL;

    /* parse options */
    while (1)
    {
        static struct option long_options[] =
        {
            {"bw",             required_argument, 0, 'b'},
            {"colorspace",     required_argument, 0, 'c'},
            {"end",            required_argument, 0, 'd'},
            {"encoder",        required_argument, 0, 'e'},
            {"calculate",      required_argument, 0, 'f'},
            {"dpx ",           required_argument, 0, 'g'},
            {"help",           required_argument, 0, 'h'},
            {"input",          required_argument, 0, 'i'},
            {"log_level",      required_argument, 0, 'l'},
            {"tmp_dir",        required_argument, 0, 'm'},
            {"output",         required_argument, 0, 'o'},
            {"profile",        required_argument, 0, 'p'},
            {"rate",           required_argument, 0, 'r'},
            {"start",          required_argument, 0, 's'},
            {"threads",        required_argument, 0, 't'},
            {"3d",             no_argument,       0, '3'},
            {"no_overwrite",   no_argument,       0, 'n'},
            {"version",        no_argument,       0, 'v'},
            {"no_xyz",         no_argument,       0, 'x'},
            {"resize",         no_argument,       0, 'z'},
            {0, 0, 0, 0}
        };

        /* getopt_long stores the option index here. */
        int option_index = 0;

        c = getopt_long (argc, argv, "b:c:d:e:g:i:l:m:o:p:r:s:t:w:3fhnvxz",
                         long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
        {
            break;
        }

        switch (c)
        {
            case 0:

                /* If this option set a flag, do nothing else now. */
                if (long_options[option_index].flag != 0) {
                    break;
                }

                break;

            case '3':
                opendcp->stereoscopic = 1;
                break;

            case 'b':
                opendcp->j2k.bw = atoi(optarg);
                break;

            case 'c':
                if (!strcmp(optarg, "srgb")) {
                    opendcp->j2k.lut = CP_SRGB;
                }
                else if (!strcmp(optarg, "rec709")) {
                    opendcp->j2k.lut = CP_REC709;
                }
                else if (!strcmp(optarg, "p3")) {
                    opendcp->j2k.lut = CP_P3;
                }
                else if (!strcmp(optarg, "srgb_complex")) {
                    opendcp->j2k.lut = CP_SRGB_COMPLEX;
                }
                else if (!strcmp(optarg, "rec709_complex")) {
                    opendcp->j2k.lut = CP_REC709_COMPLEX;
                }
                else {
                    fprintf(stderr, "Invalid colorspace argument\n");
                    exit(1);
                }

                break;

            case 'd':
                opendcp->j2k.end_frame = strtol(optarg, NULL, 10);
                break;

            case 'f':
                opendcp->j2k.xyz_method = 1;
                break;

            case 'e':
                if (!strcmp(optarg, "openjpeg")) {
                    opendcp->j2k.encoder = OPENDCP_ENCODER_OPENJPEG;
                }
                else if (!strcmp(optarg, "kakadu")) {
                    opendcp->j2k.encoder = OPENDCP_ENCODER_KAKADU;
                }
                else if (!strcmp(optarg, "ragnarok")) {
                    opendcp->j2k.encoder = OPENDCP_ENCODER_RAGNAROK;
                }
                else if (!strcmp(optarg, "remote")) {
                    opendcp->j2k.encoder = OPENDCP_ENCODER_REMOTE;
                }
                else {
                    fprintf(stderr, "Invalid encoder argument\n");
                    exit(1);
                }

                break;

            case 'g':
                if (!strcmp(optarg, "linear")) {
                    opendcp->j2k.dpx = DPX_LINEAR;
                }
                else if (!strcmp(optarg, "film")) {
                    opendcp->j2k.dpx = DPX_FILM;
                }
                else if (!strcmp(optarg, "video")) {
                    opendcp->j2k.dpx = DPX_VIDEO;
                }
                else {
                    fprintf(stderr, "Invalid dpx argument\n");
                    exit(1);
                }

                break;

            case 'h':
                dcp_usage();
                break;

            case 'i':
                in_path = optarg;
                break;

            case 'l':
                opendcp->log_level = atoi(optarg);
                break;

            case 'm':
                opendcp->tmp_path = optarg;
                break;

            case 'o':
                out_path = optarg;
                break;

            case 'n':
                opendcp->j2k.no_overwrite = 1;
                break;

            case 'p':
                if (!strcmp(optarg, "cinema2k")) {
                    opendcp->cinema_profile = DCP_CINEMA2K;
                }
                else if (!strcmp(optarg, "cinema4k")) {
                    opendcp->cinema_profile = DCP_CINEMA4K;
                }
                else {
                    fprintf(stderr, "Invalid cinema profile argument\n");
                    exit(1);
                }

                break;

            case 'r':
                opendcp->frame_rate = atoi(optarg);
                break;

            case 's':
                opendcp->j2k.start_frame = atoi(optarg);
                break;

            case 't':
                opendcp->threads = atoi(optarg);
                break;

            case 'x':
                opendcp->j2k.xyz = 0;
                break;

            case 'v':
                version();
                break;

            case 'z':
                opendcp->j2k.resize = 1;
                break;
        }
    }

    /* set log level */
    opendcp_log_init(opendcp->log_level);

    if (opendcp_encoder_enable("j2c", NULL, opendcp->j2k.encoder)) {
        dcp_fatal(opendcp, "Could not enable encoder");
    }

    if (opendcp->log_level > 0) {
        printf("\nOpenDCP J2K %s %s\n", OPENDCP_VERSION, OPENDCP_COPYRIGHT);

        if (opendcp->j2k.encoder == OPENDCP_ENCODER_KAKADU) {
            printf("  Encoder: Kakadu\n");
        }
        else if (opendcp->j2k.encoder == OPENDCP_ENCODER_REMOTE)  {
            printf("  Encoder: Remote\n");
        }
        else if (opendcp->j2k.encoder == OPENDCP_ENCODER_RAGNAROK)  {
            printf("  Encoder: Ragnarok\n");
        }
        else {
            printf("  Encoder: OpenJPEG\n");
        }
    }

    /* cinema profile check */
    if (opendcp->cinema_profile != DCP_CINEMA4K && opendcp->cinema_profile != DCP_CINEMA2K) {
        dcp_fatal(opendcp, "Invalid profile argument, must be cinema2k or cinema4k");
    }

    /* end frame check */
    if (opendcp->j2k.end_frame < 0) {
        dcp_fatal(opendcp, "End frame  must be greater than 0");
    }

    /* start frame check */
    if (opendcp->j2k.start_frame < 1) {
        dcp_fatal(opendcp, "Start frame must be greater than 0");
    }

    /* frame rate check */
    if (opendcp->frame_rate > 60 || opendcp->frame_rate < 1 ) {
        dcp_fatal(opendcp, "Invalid frame rate. Must be between 1 and 60.");
    }

    /* encoder check */
    if (opendcp->j2k.encoder == OPENDCP_ENCODER_KAKADU) {
        result = system("kdu_compress -u >/dev/null 2>&1");

        if (result >> 8 != 0) {
            dcp_fatal(opendcp, "kdu_compress was not found. Either add to path or remove -e 1 flag");
        }
    }

    /* bandwidth check */
    if (opendcp->j2k.bw < 10 || opendcp->j2k.bw > 250) {
        dcp_fatal(opendcp, "Bandwidth must be between 10 and 250, but %d was specified", opendcp->j2k.bw);
    }
    else {
        opendcp->j2k.bw *= 1000000;
    }

    /* input path check */
    if (in_path == NULL) {
        dcp_fatal(opendcp, "Missing input file");
    }

    /* output path check */
    if (out_path == NULL) {
        dcp_fatal(opendcp, "Missing output path");
    }

    opendcp->j2k.end_frame = 1;
    opendcp->j2k.start_frame = 1;
    char out[MAX_FILENAME_LENGTH];
    build_j2k_filename(in_path, out_path, out);

    /* check if file already exists */
	struct stat st;
	if (stat(out,&st)!=0) {
    if(access(out, F_OK) != 0 || opendcp->j2k.no_overwrite == 0) {
        result = convert_to_j2k(opendcp, in_path, out);
    } else {
        result = OPENDCP_NO_ERROR;
	dcp_fatal(opendcp, "Exiting...");
    }
	}
    opendcp_delete(opendcp);

    exit(0);
}
