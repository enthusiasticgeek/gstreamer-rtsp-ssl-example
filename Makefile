all:
	gcc -g rtsp_client.c rtsp_client_tls_interaction.c -o rtsp_client -I/home/virgo/gstreamer `/usr/bin/pkg-config gstreamer-controller-1.0 gstreamer-plugins-base-1.0 gstreamer-base-1.0 gstreamer-1.0 gstreamer-rtsp-1.0 gstreamer-rtsp-server-1.0 --libs --cflags` 
	gcc -g rtsp_server.c -o rtsp_server `/usr/bin/pkg-config gstreamer-1.0 gstreamer-rtsp-1.0 gstreamer-rtsp-server-1.0 --libs --cflags` 
