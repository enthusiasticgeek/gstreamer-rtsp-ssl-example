# gstreamer-rtsp-ssl-example
Gstreamer Video Streaming using RTSP SSL/TLS example

**Important:**

This is SSL but still insecure. See last bullet point on this page.

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

0. sudo apt-get install libgstreamer1.0-dev libgstrtspserver-1.0-dev gstreamer1.0-libav gstreamer1.0-libav gstreamer1.0-plugins-bad gstreamer1.0-*

1. Adjust the path of the parameters in the following file to the desired directory in the file system.

   **rtsp_parameters.conf**
      
   RTSP_CA_CERT_PEM, RTSP_CERT_PEM, RTSP_CERT_KEY, RTSP_SERVER_PORT, RTSP_SERVER_MOUNT_POINT, RTSP_USERNAME, RTSP_PASSWORD

2. Run the following command

   **make**

3. Open a terminal window and run the following command.

   **./rtsp_server**

4. Depending on the parameters run the following in another terminal window.
   
   **./rtsp_client** **RTSP URL**

   e.g. 
   **./rtsp_client rtsps://127.0.0.1:8554/test**

5. As long as the certs are correct and the Gstreamer packages are installed one should be able to view the video.

6. **Note:** Currently there is no root/intermediate CA peer verification on either client or server side. (it may be added in the future)

**For MAC OSX**

1. Install pkg-config using homebrew
    
    brew install pkg-config
    
2. Follow the installation of Gstreamer binaries on OSX https://gstreamer.freedesktop.org/documentation/installing/on-mac-osx.html

3. export PKG_CONFIG_PATH=/Library/Frameworks/GStreamer.framework/Versions/1.0/lib/pkgconfig/  
   #Adjust above path as appropriate for one's installation and follow same steps 1 through 6 as Ubuntu 
