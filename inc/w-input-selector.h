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

#ifndef __W_INPUT_SELECTOR_H_
#define __W_INPUT_SELECTOR_H_

#if !defined(PACKAGE)
#define PACKAGE "org.tizen.inputdelegator"
#endif

#ifdef _
#undef _
#endif

#define _(str) gettext(str)

#define APP_EDJ_FILE "edje/w-input-selector.edj"
#define STT_EDJ_FILE "edje/w-input-stt.edj"
#define LOCALEDIR	"/usr/apps/org.tizen.inputdelegator/res/locale"
#define RESOURCEDIR "/usr/apps/org.tizen.inputdelegator/res/"


#include <app_control.h>
#include <Elementary.h>
#include <Evas.h>
#include <efl_extension.h>

enum {
	APP_TYPE_SELECT_MODE = 1,
	APP_TYPE_STT,
	APP_TYPE_EMOTICON,
	APP_TYPE_REPLY,
	APP_TYPE_HANDWRITING,
	APP_TYPE_KEYBOARD,
	APP_TYPE_KEYBOARD_FROM_SMS,
};

enum {
	REPLY_APP_NORMAL = 0,
	REPLY_APP_CONTROL,
};


typedef struct appdata{
	Evas_Object* win_main;
	Evas_Object* layout_main;
	Evas_Object* conform;
	Evas_Object* naviframe;
	Evas_Object* genlist;
	Eext_Circle_Surface *circle_surface;

	app_control_h source_app_control;
	int app_type;
	int reply_type;
	char* res_path;
	char* shared_res_path;
} App_Data;


typedef enum
{
	GESTURE_MODE_DEFAULT,
	GESTURE_MODE_START,
	GESTURE_MODE_FREEZE,
	GESTURE_MODE_END,
} Gesture_Mode_Type;

struct _InputKeyboardData
{
	char *app_id;
	char **data_array;
	int data_array_len;
	char *guide_text;
	char *default_text;
};

typedef struct _InputKeyboardData InputKeyboardData;



void _app_terminate(void* user_data);
void reply_to_sender_by_callback_for_back();
void reply_to_sender_by_callback(const char *value, const char *type);
void reply_to_sender_by_appcontrol(void *data, const char *value, const char *type);
char* get_resource_path();
char* get_shared_resource_path();
void show_gl_focus(Eina_Bool bVisible);
void show_popup_toast(const char *text, bool check_img);
void _back_to_genlist_for_selector();
void set_source_caller_app_id(app_control_h app_control);

#endif
