/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Debug.h"

#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include <dlog.h>
#include <vconf.h>
//#include <smartreply_service.h>

#include "w-input-smartreply.h"

#ifndef INPUT_SMARTREPLY_VCONF
#define INPUT_SMARTREPLY_VCONF "db/wms/smart_reply"
#endif


typedef struct _InputSmartreplyData InputSmartreplyData;

struct _InputSmartreplyData
{
	char *caller_id;
	char *sender;
	char *message;
	char *lang;

	int enabled;

	input_smartreply_changed callback;
	void *user_data;

//	smartreply_reply_h *candidate_list;
	int candidate_list_len;
};


static void _input_smartreply_get_reply_callback(int handle, int error,
						int length);
/* Disable smartreply on/off feature
static void _input_smartreply_vconf_changed(keynode_t *key, void *data);
*/


static InputSmartreplyData *g_input_smartreply_data = NULL;

static void _input_smartreply_get_reply_callback(int handle, int error,
						int length)
{

}



bool input_smartreply_init(app_control_h app_control)
{
	return true;
}

void input_smartreply_deinit(void)
{
	return;
}

bool input_smartreply_get_reply(void)
{
	return true;
}

bool input_smartreply_get_reply_async(void)
{
	return true;
}

int input_smartreply_get_reply_num(void)
{
	if (g_input_smartreply_data == NULL) {
		PRINTFUNC(DLOG_ERROR, "InputSmartreplyData uninitialized");
		return 0;
	}

	if (g_input_smartreply_data->enabled == 0) {
		PRINTFUNC(DLOG_WARN, "Smartreply is disabled");
		return 0;
	}

	return g_input_smartreply_data->candidate_list_len;
}

char *input_smartreply_get_nth_item(int index)
{
    char *message = NULL;

	return message;
}

bool input_smartreply_send_feedback(const char *str)
{
	return true;
}



void input_smartreply_set_notify(input_smartreply_changed callback,
			void *user_data)
{
	if (callback == NULL) {
		PRINTFUNC(DLOG_ERROR, "empty callback function");
		return;
	}

	if (g_input_smartreply_data == NULL) {
		PRINTFUNC(DLOG_WARN, "InputSmartreplyData uninitialized");
		return;
	}

	g_input_smartreply_data->callback = callback;
	g_input_smartreply_data->user_data = user_data;
}


void input_smartreply_unset_notify(void)
{
	if (g_input_smartreply_data == NULL) {
		PRINTFUNC(DLOG_WARN, "InputSmartreplyData uninitialized");
		return;
	}

	g_input_smartreply_data->callback = NULL;
	g_input_smartreply_data->user_data = NULL;
}

bool input_smartreply_is_enabled(void)
{
	if (g_input_smartreply_data == NULL) {
		PRINTFUNC(DLOG_WARN, "InputSmartreplyData uninitialized");
		return false;
	}

	if (g_input_smartreply_data->enabled)
		return true;

	return false;
}
