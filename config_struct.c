//Adapted from Ron Brash's blog https://www.pacificsimplicity.ca/blog/simple-read-configuration-file-struct-example
//Copyright (c) 2017 Pratik M Tambe <enthusiasticgeek@gmail.com>
#include "config_struct.h"
//Uncomment to use as a standalone program
//gcc -g config_struct.c -o config_struct

int main(int argc, char **argv)
{
    struct config rtsp_config;
    rtsp_config = get_config(FILENAME);

    // Struct members
    printf("%s",rtsp_config.rtsp_ca_cert);
    printf("%s",rtsp_config.rtsp_cert_pem);
    printf("%s",rtsp_config.rtsp_cert_key);
    printf("%s",rtsp_config.rtsp_server_port);
    printf("%s",rtsp_config.rtsp_server_mount_point);
    printf("%s",rtsp_config.rtsp_server_username);
    printf("%s",rtsp_config.rtsp_server_password);

    // Cast port as int
    int x;
    x = atoi(rtsp_config.rtsp_server_port);
    printf("%d\n",x);

    return 0;
}

