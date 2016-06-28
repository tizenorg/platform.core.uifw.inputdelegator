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

#include <dlog.h>
#include <db-util.h>
#include <vconf.h>

#include "w-input-template.h"


#define INPUT_TEMPLATE_DB_VCONF "memory/wms/update_text_template_db"

#define INPUT_TEMPLATE_DB_PATH "/opt/usr/dbspace/.WearableManagerService.db"
#define INPUT_TEMPLATE_QUERY_LEN 1024

struct InputTemplate {
	std::vector<TemplateData> template_list;
	bool notify;
	bool user_template;
	input_template_changed callback;
	void *user_data;
};

static struct InputTemplate g_input_template;


static void _input_template_db_vconf_changed(keynode_t *key, void *data);

static bool input_template_init_app_control_list(app_control_h app_control);
static bool input_template_init_app_control(app_control_h app_control);
static bool input_template_init_db_util(void);
static bool input_template_init_default(void);


static void _input_template_db_vconf_changed(keynode_t *key, void *data)
{
	bool db_status = vconf_keynode_get_bool(key);

	PRINTFUNC(DLOG_DEBUG, "starts :: db_status = %d", db_status);

	if(db_status != 0)
		return;

	if (g_input_template.callback) {
		// Clear template list and reset to template_list from DB
		g_input_template.template_list.clear();
		input_template_init_db_util();

		g_input_template.callback(g_input_template.user_data);
	}
}

static bool input_template_init_app_control_list(app_control_h app_control)
{
	int ret;
	int len;

	char **list = NULL;

	ret = app_control_get_extra_data_array(app_control,
			"template_list", &list, &len);

	if (ret != APP_CONTROL_ERROR_NONE)
		return false;

	if (list == NULL)
		return false;

	for (int i = 0; i < len; i++) {
		struct TemplateData data;

		if (*(list + i)) {
			// if use_gettext is true,
			// it's default template string that requires gettext.
			data.use_gettext = false;
			data.text = *(list + i);

			g_input_template.template_list.push_back(data);

			free(*(list + i));
		}
	}

	free(list);

	g_input_template.user_template = true;

	return true;
}

static bool input_template_init_app_control(app_control_h app_control)
{
	bool ret;

	if (app_control == NULL)
		return false;

	ret = input_template_init_app_control_list(app_control);
	if (ret)
		return true;

	return false;
}

static bool input_template_init_db_util(void)
{
	sqlite3 *g_tt_db = NULL;
	sqlite3_stmt *stmt = NULL;

	int ret = -1;

	char query[INPUT_TEMPLATE_QUERY_LEN + 1] = {0, };

	ret = db_util_open(INPUT_TEMPLATE_DB_PATH, &g_tt_db, 0);

	if (ret != SQLITE_OK) {
		PRINTFUNC(DLOG_ERROR, "db_util open failed");
		return false;
	}

	snprintf(query, INPUT_TEMPLATE_QUERY_LEN,
			"select sortId, itemId, checked, message from %s",
			"tmpl_msg_table");

	sqlite3_prepare_v2(g_tt_db, query, strlen(query), &stmt, NULL);

	if (ret != SQLITE_OK) {
		PRINTFUNC(DLOG_ERROR, "Can not get query");
		return false;
	}

	ret = sqlite3_step(stmt);

	while(ret == SQLITE_ROW) {
		struct TemplateData data;
		int text_mode;

		// if checked is 0,
		// it's default template string that requires gettext.
		text_mode = sqlite3_column_int(stmt, 2);
		if (text_mode)
			data.use_gettext = false;
		else
			data.use_gettext = true;

		data.text = (char *)sqlite3_column_text(stmt, 3);
		PRINTFUNC(DLOG_DEBUG, "db text %s", data.text.c_str());
		g_input_template.template_list.push_back(data);

		ret = sqlite3_step(stmt);
	}

	ret = sqlite3_finalize(stmt);
	if (ret != SQLITE_OK)
		PRINTFUNC(DLOG_ERROR, "Can not finalize sqlite");

	ret = db_util_close(g_tt_db);
	if (ret != SQLITE_OK)
		PRINTFUNC(DLOG_ERROR, "Can not close db_util");

	/* set vconf callback  for DB update */
	ret = vconf_notify_key_changed(INPUT_TEMPLATE_DB_VCONF,
				_input_template_db_vconf_changed, NULL);

	if (ret < 0)
		PRINTFUNC(DLOG_ERROR, "Can not create vconf notify");

	g_input_template.user_template = false;
	return true;
}

static bool input_template_init_default(void)
{
	struct TemplateData data;

	data.text = "IDS_WMGR_MBODY_HOWS_IT_GOING_Q_M_TEXT_TEMPLATE";
	data.use_gettext = true;

	g_input_template.template_list.push_back(data);

	data.text = "IDS_WMGR_MBODY_WHATS_UP_Q_M_TEXT_TEMPLATE";
	data.use_gettext = true;

	g_input_template.template_list.push_back(data);

	data.text = "IDS_WMGR_MBODY_ILL_TALK_TO_YOU_SOON_M_TEXT_TEMPLATE";
	data.use_gettext = true;

	g_input_template.template_list.push_back(data);

	data.text = "IDS_WMGR_MBODY_ILL_CALL_YOU_LATER_M_TEXT_TEMPLATE";
	data.use_gettext = true;

	g_input_template.template_list.push_back(data);

	data.text = "IDS_MSG_BODY_WHERE_ARE_YOU_Q_M_TEXT_TEMPLATE";
	data.use_gettext = true;

	g_input_template.template_list.push_back(data);

	data.text = "IDS_MSG_BODY_WHEN_CAN_WE_MEET_Q_M_TEXT_TEMPLATE";
	data.use_gettext = true;

	g_input_template.template_list.push_back(data);

	data.text = "WDS_WMGR_MBODY_CALL_ME_LATER";
	data.use_gettext = true;

	g_input_template.template_list.push_back(data);
	g_input_template.user_template = false;

	return true;
}

static void input_template_deinit_db_util(void)
{
	vconf_ignore_key_changed(INPUT_TEMPLATE_DB_VCONF,
			_input_template_db_vconf_changed);
}

void input_template_init(app_control_h app_control)
{
	input_template_unset_notify();
	g_input_template.template_list.clear();
	g_input_template.user_template = false;

	bool ret;

	ret = input_template_init_app_control(app_control);
	if (ret)
		return;

	ret = input_template_init_db_util();
	if (ret)
		return;

	input_template_init_default();
}

void input_template_deinit(void)
{
	input_template_unset_notify();
	input_template_deinit_db_util();

	g_input_template.template_list.clear();
	g_input_template.user_template = false;
}

const std::vector<TemplateData> input_template_get_list(void)
{
	return g_input_template.template_list;
}

void input_template_set_notify(input_template_changed callback,
			void *user_data)
{
	if (callback == NULL) {
		PRINTFUNC(DLOG_ERROR, "empty callback function");
		return;
	}

	g_input_template.callback = callback;
	g_input_template.user_data = user_data;
}

void input_template_unset_notify(void)
{
	g_input_template.callback = NULL;
	g_input_template.user_data = NULL;
}

bool input_template_is_user_template(void)
{
	return (g_input_template.user_template);
}
