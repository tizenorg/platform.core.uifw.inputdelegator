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

#include <app.h>
#include <Elementary.h>

#include <stdlib.h>

#include "w-input-keyboard.h"
#include "w-input-selector.h"
#include "w-input-smartreply.h"

extern InputKeyboardData g_input_keyboard_data;
extern App_Data* app_data;

Evas_Object *entry;

static void input_keyboard_app_control_send_reply_cb(app_control_h request, app_control_h reply,
						app_control_result_e result, void *user_data);

static void input_keyboard_app_control_send_reply_cb(app_control_h request, app_control_h reply,
						app_control_result_e result, void *user_data)
{
	char *feedback = NULL;
	if (result == APP_CONTROL_RESULT_SUCCEEDED) {
		if (reply) {
			int ret;
			ret = app_control_get_extra_data(reply,
					"template_feedback", &feedback);

			if (ret == APP_CONTROL_ERROR_NONE) {
				if (feedback) {
					input_smartreply_send_feedback(feedback);
				}
			}
		}
		reply_to_sender_by_callback(feedback, "keyboard");
		free(feedback);
		elm_exit();
	}
}


bool input_keyboard_init(app_control_h app_control)
{
	int ret = -1;
	char *default_text = NULL;
	char *guide_text = NULL;

	input_keyboard_deinit();

	ret = app_control_get_extra_data(app_control, APP_CONTROL_DATA_INPUT_DEFAULT_TEXT, &default_text);
	if (ret == APP_CONTROL_ERROR_NONE) {
		g_input_keyboard_data.default_text = default_text;
	}
	ret = app_control_get_extra_data(app_control, APP_CONTROL_DATA_INPUT_GUIDE_TEXT, &guide_text);
	if (ret == APP_CONTROL_ERROR_NONE) {
		g_input_keyboard_data.guide_text = guide_text;
	}

	return true;
}

void input_keyboard_deinit(void)
{
	if (g_input_keyboard_data.guide_text)
		free(g_input_keyboard_data.guide_text);

	if (g_input_keyboard_data.default_text)
		free(g_input_keyboard_data.default_text);

	g_input_keyboard_data.default_text = NULL;
	g_input_keyboard_data.guide_text = NULL;

	return;
}

void btn_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	app_control_h app_control;
	int ret = app_control_create(&app_control);
	if (ret != APP_CONTROL_ERROR_NONE) {
		PRINTFUNC(DLOG_ERROR, "Can not create app_control : %d", ret);
		return;
	}

	const char *getText = elm_entry_entry_get(entry);
	LOGD("button key clicked!! : getText = %s", getText);

	if (getText)
		input_smartreply_send_feedback(getText);

	char *app_id = NULL;
	app_control_get_caller(app_data->source_app_control, &app_id);
	if (app_id != NULL)
		app_control_set_app_id(app_control, app_id);
	app_control_set_operation(app_control, APP_CONTROL_OPERATION_DEFAULT);
	set_source_caller_app_id(app_control);
	free(app_id);
	reply_to_sender_by_callback(getText, "keyboard");
	elm_exit();
}

static Eina_Bool custom_back_cb(void *data, Elm_Object_Item *it)
{
	_back_to_genlist_for_selector();
	return EINA_TRUE;
}

void create_fullscreen_editor(void *data)
{
	App_Data *ad = (App_Data *)data;

	Evas_Object *box = elm_box_add(ad->naviframe);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(box);
	elm_win_resize_object_add(ad->naviframe, box);

	entry = elm_entry_add(box);
	elm_entry_single_line_set(entry, EINA_TRUE);
	elm_entry_scrollable_set(entry, EINA_TRUE);
	elm_scroller_policy_set(entry, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
	elm_object_part_text_set(entry, "elm.guide", g_input_keyboard_data.guide_text);
	elm_entry_entry_set(entry, g_input_keyboard_data.default_text);
	elm_entry_cursor_end_set(entry);
	evas_object_size_hint_weight_set(entry, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(entry, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(entry);
	elm_box_pack_end(box, entry);

	Evas_Object *btn = elm_button_add(box);
	elm_object_text_set(btn, "SEND");
	evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, 0.5);
	evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_smart_callback_add(btn, "clicked", btn_clicked_cb, NULL);
	evas_object_show(btn);
	elm_box_pack_end(box, btn);
	evas_object_resize(ad->naviframe, 360, 360);
	evas_object_show(ad->naviframe);

	Elm_Object_Item *nf_item = elm_naviframe_item_push(ad->naviframe, NULL, NULL, NULL, box, "empty");
	elm_naviframe_item_pop_cb_set(nf_item, custom_back_cb, NULL);
}

bool input_keyboard_launch(Evas_Object *window, void *data) {
	if (window == NULL) {
		PRINTFUNC(DLOG_ERROR, "Can not get window");
		return false;
	}
	create_fullscreen_editor(data);
	return true;
}

