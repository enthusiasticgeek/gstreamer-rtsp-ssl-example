#ifndef _config_struct_h_
#define _config_struct_h_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "rtsp_parameters.conf"
#define MAXBUF 2048
#define DELIM "="

struct config
{
    char rtsp_ca_cert[MAXBUF];
    char rtsp_cert_pem[MAXBUF];
    char rtsp_cert_key[MAXBUF];
    char rtsp_server_port[MAXBUF];
    char rtsp_server_mount_point[MAXBUF];
    char rtsp_server_username[MAXBUF];
    char rtsp_server_password[MAXBUF];
};

struct config get_config(char *filename)
{
    struct config rtsp_config;
    FILE *file = fopen (filename, "r");

    if (file != NULL)
    {
        char line[MAXBUF];
        int i = 0;

        while(fgets(line, sizeof(line), file) != NULL)
        {
            //Remove the trailing new lines
            line[strcspn(line, "\r\n")] = 0;
            char *cfline;
            cfline = strstr((char *)line,DELIM);
            cfline = cfline + strlen(DELIM);

            if (i == 0) {
                memcpy(rtsp_config.rtsp_ca_cert,cfline,strlen(cfline));
            } else if (i == 1) {
                memcpy(rtsp_config.rtsp_cert_pem,cfline,strlen(cfline));
            } else if (i == 2) {
                memcpy(rtsp_config.rtsp_cert_key,cfline,strlen(cfline));
            } else if (i == 3) {
                memcpy(rtsp_config.rtsp_server_port,cfline,strlen(cfline));
            } else if (i == 4) {
                memcpy(rtsp_config.rtsp_server_mount_point,cfline,strlen(cfline));
            } else if (i == 5) {
                memcpy(rtsp_config.rtsp_server_username,cfline,strlen(cfline));
            } else if (i == 6) {
                memcpy(rtsp_config.rtsp_server_password,cfline,strlen(cfline));
            }

            i++;
        } // End while
        fclose(file);
    } // End if file
    return rtsp_config;
}
#endif
