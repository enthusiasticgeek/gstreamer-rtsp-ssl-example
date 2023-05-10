# gstreamer-rtsp-ssl-example
Gstreamer Video Streaming using RTSP SSL/TLS example

![alt text]( https://github.com/enthusiasticgeek/gstreamer-rtsp-ssl-example/blob/master/gst_test.png "example output")

**Important:**

~~This is SSL but still insecure. See last bullet point on this page.~~

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

0. sudo apt-get install libgstreamer1.0-dev libgstrtspserver-1.0-dev gstreamer1.0-libav gstreamer1.0-libav gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly libx264-dev && sudo apt install gstreamer1.0-*

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

**=== !!! Compiler warnings Glibc >= 2.62 !!! ===**

This warning indicates that you are using a deprecated data type, GTimeVal, in your code. The GTimeVal data type has been deprecated since GLib 2.62 and will be removed in a future version.
To avoid this warning, you should update your code to use the recommended replacement, GDateTime. GDateTime is a more modern data type that provides improved functionality for working with dates and times.
To update your code to use GDateTime:
Replace all occurrences of GTimeVal with GDateTime.
Update any function calls that accept or return GTimeVal to use the corresponding GDateTime functions instead.
Here is an example of how to update code that uses GTimeVal to use GDateTime:

            gcc -g rtsp_server.c -o rtsp_server `/usr/bin/pkg-config gstreamer-1.0 gstreamer-rtsp-1.0 gstreamer-rtsp-server-1.0 --libs --cflags` 
            In file included from /usr/include/gstreamer-1.0/gst/rtsp/gstrtsp.h:24,
                             from /usr/include/gstreamer-1.0/gst/rtsp/rtsp.h:27,
                             from /usr/include/gstreamer-1.0/gst/rtsp-server/rtsp-media.h:21,
                             from /usr/include/gstreamer-1.0/gst/rtsp-server/rtsp-session.h:58,
                             from /usr/include/gstreamer-1.0/gst/rtsp-server/rtsp-session-pool.h:33,
                             from /usr/include/gstreamer-1.0/gst/rtsp-server/rtsp-server-object.h:32,
                             from /usr/include/gstreamer-1.0/gst/rtsp-server/rtsp-server.h:28,
                             from rtsp_server.c:26:
            /usr/include/gstreamer-1.0/gst/rtsp/gstrtspconnection.h:79:1: warning: ‘GTimeVal’ is deprecated: Use 'GDateTime' instead [-Wdeprecated-declarations]
               79 | GstRTSPResult      gst_rtsp_connection_connect                (GstRTSPConnection * conn, GTimeVal * timeout);
                  | ^~~~~~~~~~~~~
            In file included from /usr/include/glib-2.0/glib/galloca.h:32,
                             from /usr/include/glib-2.0/glib.h:30,
                             from /usr/include/gstreamer-1.0/gst/gst.h:27,
                             from rtsp_server.c:25:
            /usr/include/glib-2.0/glib/gtypes.h:547:8: note: declared here
              547 | struct _GTimeVal
                  |        ^~~~~~~~~

Potential fix may be as follows:

            // Old code using GTimeVal
            GTimeVal current_time;
            g_get_current_time(&current_time);
            g_print("Current time: %ld\n", current_time.tv_sec);

            // Updated code using GDateTime
            GDateTime *current_time = g_date_time_new_now_local();
            g_print("Current time: %s\n", g_date_time_format(current_time, "%s"));
            g_date_time_unref(current_time);

In this example, the g_get_current_time() function has been replaced with g_date_time_new_now_local(), which returns a new GDateTime object representing the current local time. The tv_sec field of the GTimeVal structure has been replaced with the g_date_time_format() function, which formats the GDateTime object as a string. Finally, the current_time object must be explicitly freed using g_date_time_unref().
By updating your code to use GDateTime, you can avoid this warning and ensure that your code remains compatible with future versions of GLib.
