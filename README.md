# gstreamer-rtsp-ssl-example
Gstreamer RTSP SSL example

## Purpose
I created this as I was unable to find a Gstreamer RTSP client-server example with SSL handshake. This example is a manifestation of my quest to understand the same.

Thanks to **Mandar Joshi (Github username: mndar https://github.com/mndar)** for immense help and his time in devising SSL portion of this code.

Original discussion post **"Usage examples tls-interaction property for rtspsrc and souphttpsrc"**, leading to this code, can be found here:

http://gstreamer-devel.966125.n4.nabble.com/Usage-examples-tls-interaction-property-for-rtspsrc-and-souphttpsrc-td4684845.html

SSL / TLS portion (rtsp_client_tls_interaction.c) of this code was adapted from the following URL with the Author's permission.

https://github.com/mndar/tlsinteraction

I have employed self signed certificates for this example.

## Setup And Usage

### Tested on Ubuntu 16.04 LTS (with Gstreamer 1.0 packages installed)

0. sudo apt-get install libgstreamer1.0-dev libgstrtspserver-1.0-dev gstreamer1.0-*

1. Adjust the path of the parameters in the following file to the desired directory in the file system.

   **rtsp_parameters.conf**
      
   RTSP_CA_CERT_PEM, RTSP_CERT_PEM, RTSP_CERT_KEY, RTSP_SERVER_PORT, RTSP_SERVER_MOUNT_POINT, RTSP_USERNAME, RTSP_PASSWORD

2. Run the following command
   **make**

3. **./rtsp_server**

4. Depending on the parameters

   run
   ./rtsp_client <URL>

   e.g. 
   **./rtsp_client rtsps://127.0.0.1:8554/test**

5. As long as the certs are correct and the Gstreamer packages are installed one should be able to view the video.

6. **Note:** Currently there is no peer verification on either client or server side. (it may be added in the future)
