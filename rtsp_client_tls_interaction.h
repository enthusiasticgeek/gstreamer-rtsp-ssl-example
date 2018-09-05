//Modified from https://github.com/mndar/tlsinteraction
//Reproduced with Author's permission
//Copyright (c) 2017 Mandar Joshi
//Copyright (c) 2017 Pratik M Tambe <enthusiasticgeek@gmail.com>
#ifndef _rtsp_client_tls_interaction_h_
#define _rtsp_client_tls_interaction_h_
#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>


G_BEGIN_DECLS

G_DECLARE_FINAL_TYPE(RtspClientTlsInteraction, rtsp_client_tls_interaction, RTSP_CLIENT_TLS, INTERACTION, GTlsInteraction)

#define RTSP_CLIENT_TLS_INTERACTION_TYPE (rtsp_client_tls_interaction_get_type ())

RtspClientTlsInteraction *rtsp_client_tls_interaction_new (GTlsCertificate*, GTlsCertificate*, GTlsDatabase*);

G_END_DECLS

#endif

