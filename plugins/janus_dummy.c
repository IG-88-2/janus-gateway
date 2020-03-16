#include "plugin.h"
#include <jansson.h>
#include <opus/opus.h>
#include <netdb.h>
#include <sys/time.h>

#include "../debug.h"
#include "../apierror.h"
#include "../config.h"
#include "../mutex.h"
#include "../rtp.h"
#include "../rtpsrtp.h"
#include "../rtcp.h"
#include "../record.h"
#include "../sdp-utils.h"
#include "../utils.h"
#include "../ip-utils.h"



#define JANUS_DUMMY_VERSION			10
#define JANUS_DUMMY_VERSION_STRING	"0.0.10"
#define JANUS_DUMMY_DESCRIPTION		"This is a plugin which does nothing."
#define JANUS_DUMMY_NAME			"JANUS dummy plugin"
#define JANUS_DUMMY_AUTHOR			"Your Mom"
#define JANUS_DUMMY_PACKAGE			"janus.plugin.dummy"



static janus_mutex sessions_mutex = JANUS_MUTEX_INITIALIZER; 



typedef struct janus_dummy_session {
	janus_plugin_session *handle;
	gint64 sdp_sessid;
	gint64 sdp_version;
	volatile gint started;
	volatile gint destroying;
	volatile gint destroyed;
	janus_refcount ref;
} janus_dummy_session;



int janus_dummy_init(janus_callbacks *callback, const char *config_path);

void janus_dummy_destroy(void);

int janus_dummy_get_api_compatibility(void);

int janus_dummy_get_version(void);

const char *janus_dummy_get_version_string(void);

const char *janus_dummy_get_description(void);

const char *janus_dummy_get_name(void);

const char *janus_dummy_get_author(void);

const char *janus_dummy_get_package(void);

void janus_dummy_create_session(janus_plugin_session *handle, int *error);

struct janus_plugin_result *janus_dummy_handle_message(janus_plugin_session *handle, char *transaction, json_t *message, json_t *jsep);

json_t *janus_dummy_handle_admin_message(json_t *message);

void janus_dummy_setup_media(janus_plugin_session *handle);

void janus_dummy_incoming_rtp(janus_plugin_session *handle, janus_plugin_rtp *packet);

void janus_dummy_incoming_rtcp(janus_plugin_session *handle, janus_plugin_rtcp *packet);

void janus_dummy_incoming_data(janus_plugin_session *handle, janus_plugin_data *packet);

void janus_dummy_slow_link(janus_plugin_session *handle, gboolean uplink, gboolean video);

void janus_dummy_hangup_media(janus_plugin_session *handle);

void janus_dummy_destroy_session(janus_plugin_session *handle, int *error);

json_t *janus_dummy_query_session(janus_plugin_session *handle);



static janus_plugin janus_dummy_plugin = {
	.init = janus_dummy_init,
	.destroy = janus_dummy_destroy,
	.get_api_compatibility = janus_dummy_get_api_compatibility,
	.get_version = janus_dummy_get_version,
	.get_version_string = janus_dummy_get_version_string,
	.get_description = janus_dummy_get_description,
	.get_name = janus_dummy_get_name,
	.get_author = janus_dummy_get_author,
	.get_package = janus_dummy_get_package,
	.create_session = janus_dummy_create_session,
	.handle_message = janus_dummy_handle_message,
	.handle_admin_message = janus_dummy_handle_admin_message,
	.setup_media = janus_dummy_setup_media,
	.incoming_rtp = janus_dummy_incoming_rtp,
	.incoming_rtcp = janus_dummy_incoming_rtcp,
	.incoming_data = janus_dummy_incoming_data,
	.slow_link = janus_dummy_slow_link,
	.hangup_media = janus_dummy_hangup_media,
	.destroy_session = janus_dummy_destroy_session, 
	.query_session = janus_dummy_query_session
};



/* Plugin creator */
janus_plugin *create(void) {
	return &janus_dummy_plugin;
}



static volatile gint initialized; 



static volatile gint stopping;



static janus_callbacks *gateway = NULL;



int janus_dummy_init(janus_callbacks *callback, const char *config_path) {

	if (g_atomic_int_get(&stopping)) {
		return -1;
	}

	if (callback == NULL || config_path == NULL) {
		/* Invalid arguments */
		return -1;
	}

	printf("start dummy...");

	gateway = callback;

	//read & parse configs
	//create hash table for sessions ---> g_hash_table_new_full ?
	//create async queue for messages ---> g_async_queue_new_full ?
	//create hash table for rooms ---> g_hash_table_new_full ?
	//iterate over rooms in config (linked list), add rooms one by one to hash table
	/*
	janus_mutex_lock
	g_hash_table_lookup
	janus_mutex_unlock
	janus_refcount_increase ?
	g_thread_try_new
	*/

	return 0;

}



void janus_dummy_destroy(void) {

	if (!g_atomic_int_get(&initialized)) {
		return;
	}

	g_atomic_int_set(&stopping, 1);

	//g_async_queue_push  exit_message
	//g_thread_join  handler_thread
	//g_hash_table_destroy  sessions
	//g_async_queue_unref  mesages
	//janus_config_destroy
	//janus_mutex_lock   
	//janus_mutex_unlock
	//g_free
	//g_atomic_int_set(&initialized, 0);
	//g_atomic_int_set(&stopping, 0);

}



int janus_dummy_get_api_compatibility(void) {
	/* Important! This is what your plugin MUST always return: don't lie here or bad things will happen */
	return JANUS_PLUGIN_API_VERSION;
}



int janus_dummy_get_version(void) {
	return JANUS_DUMMY_VERSION;
}



const char* janus_dummy_get_version_string(void) {
	return JANUS_DUMMY_VERSION_STRING;
}



const char* janus_dummy_get_description() {
	return JANUS_DUMMY_DESCRIPTION;
}



const char *janus_dummy_get_name() {
	return JANUS_DUMMY_NAME;
}



const char *janus_dummy_get_author() {
	return JANUS_DUMMY_AUTHOR;
}



const char *janus_dummy_get_package() {
	return JANUS_DUMMY_PACKAGE;
}



static void janus_dummy_session_free(const janus_refcount *session_ref) {

	//janus_refcount_containerof(session_ref, janus_dummy_session, ref);
	//TODO will it be really session ?
	janus_dummy_session *session = (janus_dummy_session*)((char*)(session_ref) - offsetof(janus_dummy_session, ref));
	
	janus_refcount_decrease(&session->handle->ref);
	
	g_free(session);

}



void janus_dummy_create_session(janus_plugin_session *handle, int *error) {

	if (g_atomic_int_get(&stopping) || !g_atomic_int_get(&initialized)) {
		*error = -1;
		return;
	}

	janus_dummy_session *session = g_malloc0(sizeof(janus_dummy_session));

	session->handle = handle;

	g_atomic_int_set(&session->started, 0);
	g_atomic_int_set(&session->destroying, 0);
	g_atomic_int_set(&session->destroyed, 0);

	handle->plugin_handle = session;
	
	janus_refcount_init(&session->ref, janus_dummy_session_free);

	janus_mutex_lock(&sessions_mutex);
	
	//g_hash_table_insert sessions, handle, session

	janus_mutex_unlock(&sessions_mutex);

	return;

}



struct janus_plugin_result *janus_dummy_handle_message(janus_plugin_session *handle, char *transaction, json_t *message, json_t *jsep) {

	if (g_atomic_int_get(&stopping) || !g_atomic_int_get(&initialized)) {
		return janus_plugin_result_new(
			JANUS_PLUGIN_ERROR, 
			g_atomic_int_get(&stopping) ? "Shutting down" : "Plugin not initialized", 
			NULL
		);
	}
	
	json_t *root = message;
	json_t *response = NULL;
	json_t *request = json_object_get(root, "request");
	
	const char *request_text = json_string_value(request);
	
	printf("new dummy request %s \n", request_text);
	
	response = json_object();

	json_object_set_new(response, "hey dummy", json_string("ok!"));
	
	return janus_plugin_result_new(
		JANUS_PLUGIN_OK, 
		"hello from dummy...", 
		response //?
	);

}



json_t *janus_dummy_handle_admin_message(json_t *message) {
	
	janus_plugin_result *r = janus_dummy_handle_message(NULL, NULL, message, NULL);

	return r->content;

}



void janus_dummy_setup_media(janus_plugin_session *handle) {
	
	if (g_atomic_int_get(&stopping) || !g_atomic_int_get(&initialized)) {
		return;
	}

	//get session 
	//get participant 
	//get participant room
	
	//gateway->push_event(p->session->handle, &janus_dummy_plugin, NULL, pub, NULL);
	//send message to each participant in the room
	
}



void janus_dummy_incoming_rtp(janus_plugin_session *handle, janus_plugin_rtp *packet) {

	if (handle == NULL || g_atomic_int_get(&handle->stopped) || g_atomic_int_get(&stopping) || !g_atomic_int_get(&initialized)) {
		return;
	}

	//get session
	//get participant
	
	char *buf = packet->buffer;
	uint16_t len = packet->length;

	//check participant reset decoder
	//opus_decoder_destroy
	//packets order ???
	//check audiolevels
	//seq_number===expected_seq
	//opus_decoder_ctl
	//opus_decode
	//g_list_insert_sorted
	//g_list_length(participant->inbuf)


	janus_rtp_header *rtp = (janus_rtp_header *) buf;
	int plen = 0;

	const unsigned char *payload = (const unsigned char *)janus_rtp_payload(buf, len, &plen);

	if (!payload) {
		return;
	}
	
}



void janus_dummy_incoming_rtcp(janus_plugin_session *handle, janus_plugin_rtcp *packet) {

	

}



void janus_dummy_incoming_data(janus_plugin_session *handle, janus_plugin_data *packet) {



}



void janus_dummy_slow_link(janus_plugin_session *handle, gboolean uplink, gboolean video) {



}



void janus_dummy_hangup_media(janus_plugin_session *handle) {
	
	if (g_atomic_int_get(&stopping) || !g_atomic_int_get(&initialized)) {
		return;
	}

	//say No WebRTC media anymore
	//get session using handle
	//session set started to false
	//set session hangingup to one using g_atomic_int_compare_and_exchange
	//get session participant
	//lock room mutex
	//get participant room
	//send leaving event to all participants inside room
	//int ret = gateway->push_event(p->session->handle, &janus_dummy_plugin, NULL, event, NULL);
	//opus decoder destroy
	//get rid of queued packets
	//janus_refcount_decrease
	
}



void janus_dummy_destroy_session(janus_plugin_session *handle, int *error) {

	if (g_atomic_int_get(&stopping) || !g_atomic_int_get(&initialized)) {
		*error = -1;
		return;
	}

	//get session 
	//remove from hash table
	//destroy session
	
	return;

}



json_t *janus_dummy_query_session(janus_plugin_session *handle) {

}
