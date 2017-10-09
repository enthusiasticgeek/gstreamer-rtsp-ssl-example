//Usage
//First run
// ./rtsp_server
//Then run
//./rtsp_client rtsps://127.0.0.1:8554/test

//Display RTSP streaming of video
//(c) 2017 Pratik M Tambe <enthusiasticgeek@gmail.com>

#include <string.h>
#include <math.h>
#include <gst/gst.h>
#include <glib.h>
#include <gio/gio.h>
#include "rtsp_client_tls_interaction.h"
#include "config_struct.h"

static gboolean bus_call (GstBus *bus,GstMessage *msg, gpointer data) {
    GMainLoop *loop = (GMainLoop *) data;
    switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_EOS:
        g_print ("Stream Ends\n");
        g_main_loop_quit (loop);
        break;
    case GST_MESSAGE_ERROR: {
        gchar  *debug;
        GError *error;
        gst_message_parse_error (msg, &error, &debug);
        g_free (debug);
        g_printerr ("Error: %s\n", error->message);
        g_error_free (error);
        g_main_loop_quit (loop);
        break;
    }
    default:
        break;
    }
    return TRUE;
}

static void on_pad_added (GstElement *element, GstPad *pad, gpointer data) {
    GstPad *sinkpad;
    GstElement *decoder = (GstElement *) data;
    /* We can now link this pad with the rtsp-decoder sink pad */
    g_print ("Dynamic pad created, linking source/demuxer\n");
    sinkpad = gst_element_get_static_pad (decoder, "sink");
    gst_pad_link (pad, sinkpad);
    gst_object_unref (sinkpad);
}

int main (int argc, char *argv[])
{
   
    if(argc!=2){
      g_print("Please pass the URL of the RTSP stream\ne.g. %s rtsps://127.0.0.1:8554/test\n",argv[0]);
      exit(1);
    }

    GMainLoop *loop;
    GstBus *bus;
    GstElement *source;
    GstElement *decoder;
    GstElement *sink;
    GstElement *pipeline;
    GstElement *demux;
    GstElement *parse;
    GstElement *videoconvert;

    /*config struct*/
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


    /* Initializing GStreamer */
    gst_init (&argc, &argv);
    loop = g_main_loop_new (NULL, FALSE);

    //gst-launch-0.10 rtspsrc location=rtsp://<ip> ! decodebin ! ffmpegvideoconvert ! autovideosink
    //gst-launch -v rtspsrc location="rtsp://<ip> ! rtpmp4vdepay ! mpeg4videoparse ! ffdec_mpeg4 ! ffmpegvideoconvert! autovideosink
    //gst-launch -v rtspsrc location="rtsp://<ip> ! rtpmp4vdepay ! ffdec_mpeg4 ! ffmpegvideoconvert! autovideosink
    /* Create Pipe's Elements */
    pipeline = gst_pipeline_new ("video player");
    g_assert (pipeline);
    source   = gst_element_factory_make ("rtspsrc", "Source");
    g_assert (source);
    demux = gst_element_factory_make ("rtph264depay", "Depay");
    g_assert (demux);
    parse = gst_element_factory_make ("h264parse", "Parse");
    g_assert (parse);
    decoder = gst_element_factory_make ("avdec_h264", "Decoder");
    g_assert (decoder);
    videoconvert     = gst_element_factory_make ("videoconvert", "VideoConvert");
    g_assert(videoconvert);
    sink     = gst_element_factory_make ("autovideosink", "Output");
    g_assert (sink);

    /*Make sure: Every elements was created ok*/
    if (!pipeline || !source || !demux || !parse || !decoder || !videoconvert || !sink) {
        g_printerr ("One of the elements wasn't create... Exiting\n");
        return -1;
    }

    g_print(" \nPipeline is Part(A) ->(dynamic/runtime link)  Part(B)[ Part(B-1) -> Part(B-2) -> Part(B-3) ]\n\n");
    g_print(" [source](dynamic)->(dynamic)[demux]->[parse]->[decoder]->[videoconvert]->[videosink] \n\n");

    /* Set video Source */
    g_object_set (G_OBJECT (source), "location", argv[1], NULL);
    g_object_set (G_OBJECT (source), "do-rtcp", TRUE, NULL);
    g_object_set (G_OBJECT (source), "latency", 0, NULL);
    g_object_set (G_OBJECT (source), "user-id", rtsp_config.rtsp_server_username, NULL);
    //g_object_set (G_OBJECT (source), "user-id", "user", NULL);
    g_object_set (G_OBJECT (source), "user-pw", rtsp_config.rtsp_server_password, NULL);
    //g_object_set (G_OBJECT (source), "user-pw", "password", NULL);

    /*
    typedef enum {
      G_TLS_CERTIFICATE_UNKNOWN_CA    = (1 << 0),
      G_TLS_CERTIFICATE_BAD_IDENTITY  = (1 << 1),
      G_TLS_CERTIFICATE_NOT_ACTIVATED = (1 << 2),
      G_TLS_CERTIFICATE_EXPIRED       = (1 << 3),
      G_TLS_CERTIFICATE_REVOKED       = (1 << 4),
      G_TLS_CERTIFICATE_INSECURE      = (1 << 5),
      G_TLS_CERTIFICATE_GENERIC_ERROR = (1 << 6),

      G_TLS_CERTIFICATE_VALIDATE_ALL  = 0x007f
    } GTlsCertificateFlags;
    G_TLS_CERTIFICATE_UNKNOWN_CA
    	The signing certificate authority is not known.

    G_TLS_CERTIFICATE_BAD_IDENTITY
    	The certificate does not match the expected identity of the site that it was retrieved from.

    G_TLS_CERTIFICATE_NOT_ACTIVATED
    	The certificate's activation time is still in the future

    G_TLS_CERTIFICATE_EXPIRED
    	The certificate has expired

    G_TLS_CERTIFICATE_REVOKED
    	The certificate has been revoked according to the GTlsConnection's certificate revocation list.

    G_TLS_CERTIFICATE_INSECURE
    	The certificate's algorithm is considered insecure.

    G_TLS_CERTIFICATE_GENERIC_ERROR
    	Some other error occurred validating the certificate

    G_TLS_CERTIFICATE_VALIDATE_ALL
    	the combination of all of the above flags
    */

    //validate all
    //g_object_set (G_OBJECT (source), "tls-validation-flags", G_TLS_CERTIFICATE_VALIDATE_ALL, NULL);
    g_object_set (G_OBJECT (source), "tls-validation-flags", G_TLS_CERTIFICATE_INSECURE, NULL);
    GTlsCertificate *cert;
    GError *error=NULL;
    cert = g_tls_certificate_new_from_files(rtsp_config.rtsp_cert_pem,rtsp_config.rtsp_cert_key,&error);
    //cert = g_tls_certificate_new_from_files("/home/enthusiasticgeek/gstreamer/cert/toyIntermediate.pem","/home/enthusiasticgeek/gstreamer/cert/toyDecryptedIntermediate.key",&error);
    if (cert == NULL) {
        g_printerr ("failed to parse PEM: %s\n", error->message);
        return -1;
    }
    GTlsDatabase* database = g_tls_file_database_new (rtsp_config.rtsp_ca_cert, &error);
    //GTlsDatabase* database = g_tls_file_database_new ("/home/enthusiasticgeek/gstreamer/cert/toyCA.pem", &error);

    GTlsCertificate *ca_cert;
    ca_cert = g_tls_certificate_new_from_file(rtsp_config.rtsp_ca_cert,&error);
    //ca_cert = g_tls_certificate_new_from_file("/home/enthusiasticgeek/gstreamer/cert/toyCA.pem",&error);
    if (ca_cert == NULL) {
        g_printerr ("failed to parse CA PEM: %s\n", error->message);
        return -1;
    }

    g_object_set (G_OBJECT (source), "tls-database", database, NULL);
    RtspClientTlsInteraction *interaction =
        rtsp_client_tls_interaction_new (cert, ca_cert, database);

    g_object_set (G_OBJECT (source), "tls-interaction", interaction, NULL);

    /* Putting a Message handler */
    bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
    gst_bus_add_watch (bus, bus_call, loop);
    gst_object_unref (bus);

    /* Add Elements to the Bin */
    gst_bin_add_many (GST_BIN (pipeline), source, demux, parse, decoder, videoconvert, sink, NULL);

    /* Link confirmation */
    if (!gst_element_link_many (demux, parse, decoder, videoconvert, sink, NULL)) {
        g_warning ("Linking part (B) Fail...");
    }

    g_print("\nNote that the source will be linked to the demuxer(depayload) dynamically.\n"
            "The reason is that rtspsrc may contain various elements (for example\n"
            "audio and video). The source pad(s) will be created at run time,\n"
            "by the rtspsrc when it detects the amount and nature of elements.\n"
            "Therefore we connect a callback function which will be executed\n"
            "when the \"pad-added\" is emitted.\n");

    /* Dynamic Pad Creation */
    if(! g_signal_connect (source, "pad-added", G_CALLBACK (on_pad_added),demux))
    {
        g_warning ("Linking part (A) with part (B) Fail...");
    }
    /* Run the pipeline */
    g_print ("Playing: %s\n", argv[1]);
    gst_element_set_state (pipeline, GST_STATE_PLAYING);
    g_main_loop_run (loop);

    /* Ending Playback */
    g_print ("End of the Streaming... ending the playback\n");
    gst_element_set_state (pipeline, GST_STATE_NULL);

    /* Eliminating Pipeline */
    g_print ("Eliminating Pipeline\n");
    gst_object_unref (GST_OBJECT (pipeline));

    return 0;
}
