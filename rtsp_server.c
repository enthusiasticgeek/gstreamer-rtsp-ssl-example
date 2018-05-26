/* GStreamer
 * Copyright (C) 2008 Wim Taymans <wim.taymans at gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

//#############################################################################################
//Modified from https://github.com/GStreamer/gst-rtsp-server/blob/master/examples/test-video.c
//Copyright (c) 2017 Pratik M Tambe <enthusiasticgeek@gmail.com>
//#############################################################################################

#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>
#include <gio/gio.h>
#include "config_struct.h"

/* define this if you want the resource to only be available when using
 * user/password as the password */
#undef WITH_AUTH
#define WITH_AUTH 1

/* define this if you want the server to use TLS (it will also need WITH_AUTH
 * to be defined) */
#undef WITH_TLS
#define WITH_TLS 1

/* this timeout is periodically run to clean up the expired sessions from the
 * pool. This needs to be run explicitly currently but might be done
 * automatically as part of the mainloop. */
static gboolean
timeout (GstRTSPServer * server)
{
    GstRTSPSessionPool *pool;

    pool = gst_rtsp_server_get_session_pool (server);
    gst_rtsp_session_pool_cleanup (pool);
    g_object_unref (pool);

    return TRUE;
}

gboolean
accept_certificate (GstRTSPAuth *auth,
                    GTlsConnection *conn,
                    GTlsCertificate *peer_cert,
                    GTlsCertificateFlags errors,
                    gpointer user_data) {

    GError *error = NULL;
    gboolean accept = FALSE;
    GTlsCertificate *ca_tls_cert = (GTlsCertificate *) user_data;
   
    /*config struct*/
    //struct config rtsp_config;
    //rtsp_config = get_config(FILENAME);
    //GTlsDatabase* database = g_tls_file_database_new (rtsp_config.rtsp_ca_cert, &error);
    //GTlsDatabase* database = g_tls_file_database_new ("/home/enthusiasticgeek/gstreamer/cert/toyCA.pem", &error);
    GTlsDatabase* database = g_tls_connection_get_database(G_TLS_CONNECTION(conn));
    if (database) {
        GSocketConnectable *peer_identity;
        GTlsCertificateFlags validation_flags;
        g_debug ("TLS peer certificate not accepted, checking user database...\n");
        peer_identity = NULL;
        errors =
            g_tls_database_verify_chain (database, peer_cert,
                                         G_TLS_DATABASE_PURPOSE_AUTHENTICATE_CLIENT, peer_identity,
                                         g_tls_connection_get_interaction (conn), G_TLS_DATABASE_VERIFY_NONE,
                                         NULL, &error);
        g_print("errors value %d\n",errors);

        //g_object_unref (database);
        if (error)
        {
            g_warning ("failure verifying certificate chain: %s",
                       error->message);
            g_assert (errors != 0);
            g_clear_error (&error);
        }
    }

    if (error == 0) {
        return TRUE;
    }
    return FALSE;
}

int
main (int argc, char *argv[])
{
    GMainLoop *loop;
    GstRTSPServer *server;
    GstRTSPMountPoints *mounts;
    GstRTSPMediaFactory *factory;
#if WITH_AUTH
    GstRTSPAuth *auth;
    GstRTSPToken *token;
    gchar *basic;
    GstRTSPPermissions *permissions;
#endif
#if WITH_TLS
    GTlsCertificate *cert;
    GTlsCertificate *ca_cert;
    GError *error = NULL;
#endif

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

    gst_init (&argc, &argv);

    loop = g_main_loop_new (NULL, FALSE);

    /* create a server instance */
    server = gst_rtsp_server_new ();

    gst_rtsp_server_set_service(server,rtsp_config.rtsp_server_port); //set the port #
#if WITH_AUTH
    /* make a new authentication manager. it can be added to control access to all
     * the factories on the server or on individual factories. */
    auth = gst_rtsp_auth_new ();
#if WITH_TLS
    g_print("Inside RTSP server's TLS portion\n");
    cert = g_tls_certificate_new_from_files(rtsp_config.rtsp_cert_pem,rtsp_config.rtsp_cert_key,&error);
    //cert = g_tls_certificate_new_from_files("/home/enthusiasticgeek/gstreamer/cert/toyIntermediate.pem","/home/enthusiasticgeek/gstreamer/cert/toyDecryptedIntermediate.key",&error);
    if (cert == NULL) {
        g_printerr ("failed to parse PEM: %s\n", error->message);
        return -1;
    }
    GTlsDatabase* database = g_tls_file_database_new (rtsp_config.rtsp_ca_cert, &error);
    //GTlsDatabase* database = g_tls_file_database_new ("/home/enthusiasticgeek/gstreamer/cert/toyCA.pem", &error);
    gst_rtsp_auth_set_tls_database (auth, database);
    ca_cert = g_tls_certificate_new_from_file(rtsp_config.rtsp_ca_cert,&error);
    //ca_cert = g_tls_certificate_new_from_file("/home/enthusiasticgeek/gstreamer/cert/toyCA.pem",&error);
    if (ca_cert == NULL) {
        g_printerr ("failed to parse CA PEM: %s\n", error->message);
        return -1;
    }
    gst_rtsp_auth_set_tls_authentication_mode(auth, G_TLS_AUTHENTICATION_REQUIRED);
    //gst_rtsp_auth_set_tls_authentication_mode(auth, G_TLS_AUTHENTICATION_REQUESTED);
    //GTlsCertificateFlags verification = g_tls_certificate_verify(cert, NULL, ca_cert);
    //g_print("verification code = %d\n", verification);
    gst_rtsp_auth_set_tls_certificate (auth, cert);
    g_signal_connect (auth, "accept-certificate", G_CALLBACK
                      (accept_certificate), ca_cert);
#endif

    /* make user token */
    token =
        gst_rtsp_token_new (GST_RTSP_TOKEN_MEDIA_FACTORY_ROLE, G_TYPE_STRING,
                            rtsp_config.rtsp_server_username, NULL);
    //"user", NULL);
    basic = gst_rtsp_auth_make_basic (rtsp_config.rtsp_server_username, rtsp_config.rtsp_server_password);
    //basic = gst_rtsp_auth_make_basic ("user", "password");

    gst_rtsp_auth_add_basic (auth, basic, token);
    g_free (basic);
    gst_rtsp_token_unref (token);

    /* configure in the server */
    gst_rtsp_server_set_auth (server, auth);
#endif

    /* get the mount points for this server, every server has a default object
     * that be used to map uri mount points to media factories */
    mounts = gst_rtsp_server_get_mount_points (server);

    /* make a media factory for a test stream. The default media factory can use
     * gst-launch syntax to create pipelines.
     * any launch line works as long as it contains elements named pay%d. Each
     * element with pay%d names will be a stream */
    factory = gst_rtsp_media_factory_new ();
    /*
    gst_rtsp_media_factory_set_launch (factory, "( "
                                       "videotestsrc ! video/x-raw,width=352,height=288,framerate=15/1 ! "
                                       "x264enc ! rtph264pay name=pay0 pt=96 "
                                       "audiotestsrc ! audio/x-raw,rate=8000 ! "
                                       "alawenc ! rtppcmapay name=pay1 pt=97 " ")");
   */

   //To find microphone connected to webcam use the command line
   //See http://oz9aec.net/software/gstreamer/pulseaudio-device-names
   //pactl list | grep -A2 'Source #' | grep 'Name: .*\.usb' | cut -d" " -f2

   /*
    gst_rtsp_media_factory_set_launch (factory, "( "
                                       "v4l2src device=/dev/video2 ! video/x-raw,width=640,height=480,framerate=30/1 ! "
                                       "x264enc pass=qual quantizer=20 tune=zerolatency ! rtph264pay name=pay0 pt=96 "
                                       "pulsesrc device=alsa_input.usb-046d_0825_1E32BDE0-02.analog-mono ! audio/x-raw ! "
                                       "alawenc ! rtppcmapay name=pay1 pt=97 " ")");
   */
    gst_rtsp_media_factory_set_launch (factory, "( "
                                       "v4l2src device=/dev/video2 ! video/x-raw,width=640,height=480,framerate=30/1 ! "
                                       "x264enc pass=qual quantizer=20 tune=zerolatency ! rtph264pay name=pay0 pt=96 "
                                       "pulsesrc device=alsa_input.pci-0000_00_1f.3.analog-stereo ! audio/x-raw ! "
                                       "alawenc ! rtppcmapay name=pay1 pt=97 " ")");


#if WITH_AUTH
    /* add permissions for the user media role */
    permissions = gst_rtsp_permissions_new ();
    gst_rtsp_permissions_add_role (permissions, rtsp_config.rtsp_server_username,
                                   //gst_rtsp_permissions_add_role (permissions, "user",
                                   GST_RTSP_PERM_MEDIA_FACTORY_ACCESS, G_TYPE_BOOLEAN, TRUE,
                                   GST_RTSP_PERM_MEDIA_FACTORY_CONSTRUCT, G_TYPE_BOOLEAN, TRUE, NULL);
    gst_rtsp_media_factory_set_permissions (factory, permissions);
    gst_rtsp_permissions_unref (permissions);
#if WITH_TLS
    gst_rtsp_media_factory_set_profiles (factory, GST_RTSP_PROFILE_SAVP);
#endif
#endif

    /* attach the test factory to the /test url */
    gst_rtsp_mount_points_add_factory (mounts, rtsp_config.rtsp_server_mount_point, factory);
    //gst_rtsp_mount_points_add_factory (mounts, "/test", factory);

    /* don't need the ref to the mapper anymore */
    g_object_unref (mounts);

    /* attach the server to the default maincontext */
    if (gst_rtsp_server_attach (server, NULL) == 0)
        goto failed;

    /* add a timeout for the session cleanup */
    g_timeout_add_seconds (2, (GSourceFunc) timeout, server);

    /* start serving, this never stops */
#if WITH_TLS
    g_print ("stream ready at rtsps://127.0.0.1:%s/%s\n", rtsp_config.rtsp_server_port, rtsp_config.rtsp_server_mount_point);
#else
    g_print ("stream ready at rtsp://127.0.0.1:%s/%s\n", rtsp_config.rtsp_server_port, rtsp_config.rtsp_server_mount_point);
#endif
    g_main_loop_run (loop);

    return 0;

    /* ERRORS */
failed:
    {
        g_print ("failed to attach the server\n");
        return -1;
    }
}

