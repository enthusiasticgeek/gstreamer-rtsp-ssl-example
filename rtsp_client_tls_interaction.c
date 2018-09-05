//Modified from https://github.com/mndar/tlsinteraction
//Reproduced with Author's permission
//Copyright (c) 2017 Mandar Joshi
//Copyright (c) 2017 Pratik M Tambe <enthusiasticgeek@gmail.com>

#include "rtsp_client_tls_interaction.h"

//This struct should contain instance variable for the subclass
struct _RtspClientTlsInteraction {
    GTlsInteraction parent_instance;
    GTlsCertificate *ca_cert, *cert_key;
    GTlsDatabase *database;
};

G_DEFINE_TYPE(RtspClientTlsInteraction, rtsp_client_tls_interaction, G_TYPE_TLS_INTERACTION)

//This function handles all initialisation
static void rtsp_client_tls_interaction_init (RtspClientTlsInteraction *tls_interaction) {
}

gboolean accept_tls_certificate (GTlsConnection *conn, GTlsCertificate *peer_cert,
                                 GTlsCertificateFlags errors, RtspClientTlsInteraction* user_data) {

    GError *error = NULL;
    gboolean accept = FALSE;
    GTlsCertificate *ca_tls_cert = (GTlsCertificate *) user_data->ca_cert;
    //GTlsDatabase* database = (GTlsDatabase *) user_data->database;
    //GTlsDatabase* database = g_tls_file_database_new ("/home/enthusiasticgeek/gstreamer/cert/toyCA.pem", &error);
    GTlsDatabase* database = g_tls_connection_get_database(G_TLS_CONNECTION(conn));
    if (database) {
        GSocketConnectable *peer_identity;
        GTlsCertificateFlags validation_flags;

        g_debug ("TLS peer certificate not accepted, checking user database...\n");

        //peer_identity =  g_tls_client_connection_get_server_identity(G_TLS_CLIENT_CONNECTION
        //            (conn));
        peer_identity = NULL;
        errors =
            g_tls_database_verify_chain (database, peer_cert,
                                         G_TLS_DATABASE_PURPOSE_AUTHENTICATE_SERVER, peer_identity,
                                         g_tls_connection_get_interaction (conn), G_TLS_DATABASE_VERIFY_NONE,
                                         NULL, &error);
        g_print("errors value %d\n",errors);

        //g_object_unref (database);
        if (error)
        {
            g_print ("failure verifying certificate chain: %s",
                     error->message);
            g_assert (errors != 0);
            g_clear_error (&error);
        }
    }

    if (error == 0) {
        return TRUE;
    }
    return FALSE;

    /*
    //Trying to verify peer
        if (error)
          goto verify_error;

        validation_flags = g_tls_client_connection_get_validation_flags(G_TLS_CLIENT_CONNECTION
            (conn));
        g_print("validation_flags value %d\n",validation_flags);

        accept = ((errors & validation_flags) == 0);
        if (accept)
          g_print ("Peer certificate accepted\n");
        else
          g_print ("Peer certificate not accepted (errors: 0x%08X)\n", errors);
      }

      return accept;

    verify_error:
      {
        g_print ("An error occurred while verifying the peer certificate: %s\n",
            error->message);
        g_clear_error (&error);
        return FALSE;
      }
    */
}

GTlsInteractionResult rtsp_client_request_certificate (GTlsInteraction *interaction,
        GTlsConnection *connection,
        GTlsCertificateRequestFlags flags,
        GCancellable *cancellable,
        GError **error) {
    RtspClientTlsInteraction *stls = (RtspClientTlsInteraction *) interaction;
    g_debug ("RtspClient Request Certificate");
    //g_print ("RtspClient Request Certificate\n");
    g_signal_connect (connection, "accept-certificate", G_CALLBACK (accept_tls_certificate), stls);
    g_tls_connection_set_certificate (connection, stls->cert_key);
    return G_TLS_INTERACTION_HANDLED;
}

//Virt funcs overrides, properties, signal defs here
static void rtsp_client_tls_interaction_class_init (RtspClientTlsInteractionClass *class)
{
    GTlsInteractionClass *object_class = G_TLS_INTERACTION_CLASS (class);
    object_class->request_certificate = rtsp_client_request_certificate;
}

RtspClientTlsInteraction * rtsp_client_tls_interaction_new (GTlsCertificate *cert_key, GTlsCertificate *ca_cert, GTlsDatabase* database) {
    RtspClientTlsInteraction *interaction = g_object_new (RTSP_CLIENT_TLS_INTERACTION_TYPE, NULL);
    interaction->cert_key = cert_key;
    interaction->ca_cert = ca_cert;
    interaction->database = database;
    return interaction;
}

