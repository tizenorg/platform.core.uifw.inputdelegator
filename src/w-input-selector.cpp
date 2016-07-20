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
#include <dlog.h>
#include <Eina.h>
#include <string>
#include <efl_assist.h>
#include <vconf.h>
#include <vconf-keys.h>

//#include "ui_extension.h"
#include "w-input-selector.h"
#include "w-input-template.h"
#include "w-input-smartreply.h"
#include "w-input-keyboard.h"
#include "w-input-stt-ise.h"
#include "w-input-emoticon.h"
#include "w-input-stt-tos.h"

#ifdef SUPPORT_LOG_MANAGER
#include <samsung_log_manager.h>
#include <samsung_log_manager_uri.h>
#endif

#include <stt.h>


using namespace std;

App_Data* app_data = NULL;
#ifdef SUPPORT_LOG_MANAGER
static samsung_log_manager_h g_log_manager = NULL;
#endif

InputKeyboardData g_input_keyboard_data;

static Elm_Object_Item *it_empty;
static Elm_Object_Item *it_title;
static Elm_Object_Item *it_plus;
static int g_smartreply_current_number = 0;	/* currently loaded smartreply number */
static unsigned int g_template_current_number = 0;	/* currnetly loaded template number */

Evas_Coord last_step; // 0 ~ 9 for gesture, 10~11 for rotary

static Eina_Bool force_highlight_to_top = EINA_FALSE;
static Eina_Bool force_highlight_to_bottom = EINA_FALSE;
static Eina_Bool is_genlist_highlighed = EINA_FALSE;

void _init_app_data(App_Data* app_data);
static void _app_language_changed(app_event_info_h event_info, void *user_data);
static int _app_control_request_transient_app_cb(void *data);
#ifdef SUPPORT_LOG_MANAGER
static void log_manager_init(void);
static void log_manager_exit(void);
static void log_manager_set(const char *feature, const char *extra, const char *value);
#endif

static char *_genlist_text_set_theme_color(const char *str, const char *code);

Evas_Object* _create_genlist(Evas_Object* parent);
void _create_genlist_items(void* user_data, bool is_type_reply);
void _update_genlist_items(void *user_data);
void _update_smartreply_items(void *user_data);
void _update_template_items(void *user_data);
static void _popup_close_cb(void *data, Evas_Object *obj, void *event_info);
static void _popup_back_cb(void *data, Evas_Object *obj, void *event_info);
char * _toast_delete_popup_access_info_cb(void *data, Evas_Object *obj);



void _init_app_data(App_Data* app_data)
{
    app_data->win_main = NULL;
    app_data->layout_main = NULL;
    app_data->conform = NULL;
    app_data->naviframe = NULL;
    app_data->genlist = NULL;

    app_data->res_path = NULL;
}



Evas_Object* load_edj(Evas_Object* parent, const char* file, const char* group)
{
	Evas_Object* window;
	Evas_Object* content;
	int ret;

	window = elm_layout_add(parent);
	if (window) {
		ret = elm_layout_file_set(window, file, group);
		if (!ret) {
			evas_object_del(window);
			return NULL;
		}

		content = elm_layout_add(parent);
		elm_layout_theme_set(content, "layout", "application", "default");
		elm_object_part_content_set(window, "elm.swallow.content", content);

		evas_object_size_hint_weight_set(window, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	}

	return window;
}

void init_customizing_theme(void)
{
	string stt_edj_path = get_resource_path();
	string app_edj_path = get_resource_path();

	stt_edj_path = stt_edj_path + STT_EDJ_FILE;
	app_edj_path = app_edj_path + APP_EDJ_FILE;

	elm_theme_extension_add(NULL, stt_edj_path.c_str());
	elm_theme_extension_add(NULL, app_edj_path.c_str());
}


static Eina_Bool back_cb(void *data, Elm_Object_Item *it)
{
    reply_to_sender_by_callback_for_back();
    elm_exit();
    return EINA_FALSE;
}

static void _stt_clicked_cb(void *data, Evas_Object * obj, void *event_info)
{
	App_Data* ad = (App_Data*)data;

	PRINTFUNC(DLOG_DEBUG, "");

	if(!ad)
		return;

#if 0
	bool bAgreed = false;
	stt_get_user_agreement(&bAgreed);
	if(!bAgreed){
#endif
	if(is_tos_N66_agreed() == EINA_FALSE){
		if(is_sap_connection() == EINA_TRUE){
			ise_show_tos_n66_popup(ad);
		} else {
			launch_bt_connection_popup(ad);
		}
	} else {
		ise_show_stt_popup(ad);
	}


#ifdef SUPPORT_LOG_MANAGER
	log_manager_set("IM01", "STT", NULL);
#endif
}

static void _input_smartreply_notify_cb(void *user_data)
{
	ecore_main_loop_iterate();
	_update_genlist_items((void *)app_data);
}

static void _input_template_notify_cb(void *user_data)
{
	_update_genlist_items((void *)app_data);
}


static void _emoticon_clicked_cb(void *data, Evas_Object * obj, void *event_info)
{
	App_Data* ad = (App_Data*)data;

	PRINTFUNC(DLOG_DEBUG, "");

	if(!ad)
		return;

//	ise_show_emoticon_popup(ad);
	ise_show_emoticon_popup_rotary(ad);

#ifdef SUPPORT_LOG_MANAGER
        log_manager_set("IM01", "Emoticon", NULL);
#endif
}

static void _keyboard_clicked_cb(void *data, Evas_Object * obj, void *event_info)
{
	App_Data* ad = (App_Data *)data;

	PRINTFUNC(DLOG_DEBUG, "");

	if (!ad)
		return;

#ifdef SUPPORT_LOG_MANAGER
	log_manager_set("IM01", "Keypad", NULL);
#endif

    if (app_data->app_type == APP_TYPE_KEYBOARD_FROM_SMS)
        input_keyboard_launch(ad->win_main);
    else
        input_keyboard_launch_with_ui(ad->win_main, data);
}

static void __bt_connection_result_cb(app_control_h request, app_control_h reply, app_control_result_e result, void *user_data)
{
    char *val = NULL;

    if (reply == NULL) {
        PRINTFUNC(DLOG_ERROR, "service_h is NULL");
        return;
    }

    app_control_get_extra_data(reply, "__BT_CONNECTION__", &val);
    if (val) {
        if ( strcmp(val, "Connected") == 0 ) {
            PRINTFUNC(DLOG_ERROR, "BT Connected");
        } else {
            PRINTFUNC(DLOG_ERROR, "BT Not Connected");
        }

        free(val);
    }
}


static void __ise_smartreply_gl_sel(void *data, Evas_Object *obj, void *event_info)
{
	App_Data* app_data = (App_Data*) data;
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;

	if (item) {
		elm_genlist_item_selected_set(item, EINA_FALSE);

		int index = (unsigned int) elm_object_item_data_get(item);

#ifdef SUPPORT_LOG_MANAGER
		log_manager_set("IM01", "Preset", NULL);
		log_manager_set("IM02", "Smart Reply", NULL);
#endif
		char *reply = input_smartreply_get_nth_item(index);
		if (reply) {
			input_smartreply_send_feedback(reply);
			if(app_data->reply_type == REPLY_APP_CONTROL){
				reply_to_sender_by_appcontrol((void*)app_data, reply, "smartreply");
				free(reply);
			} else {
				reply_to_sender_by_callback(reply, "smartreply");
				free(reply);
				elm_exit();
			}
		}
	}
}

static void __ise_template_add_gl_sel(void *data, Evas_Object *obj, void *event_info)
{
	App_Data* app_data = (App_Data*) data;
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;

	if (item) {
		elm_genlist_item_selected_set(item, EINA_FALSE);

		if(is_sap_connection() == EINA_TRUE){
			// Show toast popup
			show_popup_toast(gettext("WDS_IME_TPOP_ADD_TEMPLATE_ON_YOUR_PHONE_ABB"), true);

			// send message to Host
			app_control_h appctrl;
			app_control_create(&appctrl);
			app_control_set_app_id(appctrl, "com.samsung.w-manager-service");
			app_control_add_extra_data(appctrl, "type", "gm_setting");
			app_control_add_extra_data(appctrl, "data/setting_menu", "texttemplate_setting");
			app_control_send_launch_request(appctrl, NULL, NULL);
			app_control_destroy(appctrl);
		} else {
			app_control_h app_control;
			app_control_create(&app_control);
			app_control_set_app_id(app_control, "com.samsung.bt-connection-popup");
			app_control_add_extra_data(app_control, "msg", "perform");
			app_control_send_launch_request(app_control, __bt_connection_result_cb, NULL);
			app_control_destroy(app_control);
		}
	}
}

static void __ise_template_gl_sel(void *data, Evas_Object *obj, void *event_info)
{
	App_Data* app_data = (App_Data*) data;
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	int index = 0;

	if (item) {
		elm_genlist_item_selected_set(item, EINA_FALSE);

		index = (unsigned int) elm_object_item_data_get(item);
		const std::vector<TemplateData>  template_list = input_template_get_list();

		//@ToDo : should call reply function when any template string is selected and confirmed.
		// Here reply string will be template_list[index].text.c_str();
		if( index < template_list.size()) {
#ifdef SUPPORT_LOG_MANAGER
			log_manager_set("IM01", "Preset", NULL);
			log_manager_set("IM02", "Preset except smart reply", NULL);
#endif
			if(app_data->reply_type == REPLY_APP_CONTROL){
				reply_to_sender_by_appcontrol((void*)app_data, gettext(template_list[index].text.c_str()), "template");
			} else {
				reply_to_sender_by_callback(gettext(template_list[index].text.c_str()), "template");
				elm_exit();
			}
		}
	}
}

static Evas_Object * __ise_gl_3button_content_get(void *data, Evas_Object *obj, const char *part)
{
    if (!strcmp(part, "elm.icon.1") ||  (!strcmp(part, "elm.icon.2")) ||  (!strcmp(part, "elm.icon.3"))) {
		Evas_Object* btn = elm_button_add(obj);
		evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
		Evas_Object* ic = elm_image_add(btn);
		elm_image_resizable_set(ic, EINA_TRUE, EINA_TRUE);
		string path = get_resource_path();
		if (!strcmp(part, "elm.icon.1")) {
  			elm_object_style_set(btn, "anchor");
			string path_ic = path + "images/w_mode_stt_ic.png";
			elm_image_file_set(ic, path_ic.c_str(), NULL);
			elm_object_content_set(btn, ic);
			evas_object_layer_set(btn, 32000);

			int powerSavingMode = -1;
			int ret;
			// Power Saving mode check
			ret = vconf_get_int(VCONFKEY_SETAPPL_PSMODE, &powerSavingMode);
			if ( ret != 0 )
			{
				PRINTFUNC(DLOG_ERROR, "VCONFKEY_SETAPPL_PSMODE FAILED");
			}

			if (powerSavingMode == SETTING_PSMODE_WEARABLE_ENHANCED) {
				PRINTFUNC(DLOG_ERROR, "Power Saving mode, disable stt !!!");
				elm_object_disabled_set(btn, EINA_TRUE);
			}
		} else if (!strcmp(part, "elm.icon.2")){
			elm_object_style_set(btn, "anchor");
			string path_ic = path + "images/Delta_w_mode_emoticon_ic.png";
			elm_image_file_set(ic, path_ic.c_str(), NULL);
			elm_object_content_set(btn, ic);
			evas_object_layer_set(btn, 32000);

		} else if (!strcmp(part, "elm.icon.3")){
			elm_object_style_set(btn, "anchor");
			string path_ic = path + "images/w_mode_keyboard_ic.png";
			elm_image_file_set(ic, path_ic.c_str(), NULL);
			elm_object_content_set(btn, ic);
			evas_object_layer_set(btn, 32000);
			evas_object_propagate_events_set(btn, EINA_FALSE);
		}

		return btn;
	} else if (!strcmp(part, "elm.icon.1.touch_area") ||  (!strcmp(part, "elm.icon.2.touch_area")) ||  (!strcmp(part, "elm.icon.3.touch_area"))) {
		Evas_Object* btn = elm_button_add(obj);
		elm_object_style_set(btn, "touch_area");
		evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
		string path = get_resource_path();
		if (!strcmp(part, "elm.icon.1.touch_area")) {
			int powerSavingMode = -1;
			int ret;
			// Power Saving mode check
			ret = vconf_get_int(VCONFKEY_SETAPPL_PSMODE, &powerSavingMode);
			if ( ret != 0 )
			{
				PRINTFUNC(DLOG_ERROR, "VCONFKEY_SETAPPL_PSMODE FAILED");
			}

			if (powerSavingMode == SETTING_PSMODE_WEARABLE_ENHANCED) {
				PRINTFUNC(DLOG_ERROR, "Power Saving mode, disable stt !!!");
				elm_object_disabled_set(btn, EINA_TRUE);
			}

			evas_object_layer_set(btn, 32000);
			evas_object_smart_callback_add(btn, "clicked", _stt_clicked_cb, app_data);

		} else if (!strcmp(part, "elm.icon.2.touch_area")){
			evas_object_layer_set(btn, 32000);
			evas_object_smart_callback_add(btn, "clicked", _emoticon_clicked_cb, app_data);
		} else if (!strcmp(part, "elm.icon.3.touch_area")) {
			evas_object_layer_set(btn, 32000);
			evas_object_propagate_events_set(btn, EINA_FALSE);
			evas_object_smart_callback_add(btn, "clicked", _keyboard_clicked_cb, app_data);
		}

		return btn;
	} else if (!strcmp(part, "base")) {
		Evas_Object* btn = elm_button_add(obj);
		elm_object_style_set(btn, "ime_transparent");
		return btn;
	}
	return NULL;
}

static char * __ise_smartreply_gl_text_get(void *data, Evas_Object *obj, const char *part)
{
	if(!strcmp(part, "elm.text")) {
		int index;
		char *reply = NULL;

		index = (int)data;
		if (index < 0)
			return NULL;

		reply = input_smartreply_get_nth_item(index);
		if (reply == NULL)
			return NULL;

		/* Set hightlight color on first 3 smartreply items */
		if (index < 3) {
			char *colored = _genlist_text_set_theme_color(reply, "AT0113");

			if (colored == NULL) {
				return reply;
			} else {
				free(reply);
				return colored;
			}
		}

		return reply;
	}
	return NULL;
}

static char * __ise_template_gl_text_get(void *data, Evas_Object *obj, const char *part)
{
	if(!strcmp(part, "elm.text")) {
		unsigned int index = (unsigned int)data;
		const std::vector<TemplateData>  template_list = input_template_get_list();

		if(index < template_list.size()) {
			if(template_list[index].use_gettext) {
				return strdup(gettext(template_list[index].text.c_str()));
			} else {
				Eina_Strbuf *buf = NULL;
				const char *str = NULL;
				char *markup = NULL;

				buf = eina_strbuf_new();
				if(buf) {
					eina_strbuf_append(buf, template_list[index].text.c_str());
					eina_strbuf_replace_all(buf, "\n", "");
					eina_strbuf_replace_all(buf, "\r", "");

					str = eina_strbuf_string_get(buf);

					if (str) {
						markup = elm_entry_utf8_to_markup(str);
					}
					eina_strbuf_free(buf);
				}

				return markup;
			}
		}
	}
	return NULL;
}

static char * __ise_addtemplate_gl_text_get(void *data, Evas_Object *obj, const char *part)
{
	if(!strcmp(part, "elm.text")) {
		return(strdup(gettext("WDS_IME_MBODY_ADD_TEMPLATE_ABB")));
	}
	return NULL;
}

static Evas_Object * __ise_gl_1button_content_get(void *data, Evas_Object *obj, const char *part)
{
	if (!strcmp(part, "elm.icon")) {
		Evas_Object* btn = elm_button_add(obj);
		evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
		Evas_Object* ic = elm_image_add(btn);
		elm_image_resizable_set(ic, EINA_TRUE, EINA_TRUE);
		string path = get_resource_path();
		elm_object_style_set(btn, "ime_button_template");
		string path_ic = path + "/images/w_list_add_ic.png";
		elm_image_file_set(ic, path_ic.c_str(), NULL);
		elm_object_content_set(btn, ic);
		evas_object_layer_set(btn, 32000);

		return btn;
	}
	return NULL;
}

static void __ise_gl_lang_changed(void *data, Evas_Object *obj, void *event_info)
{
	//Update genlist items. The Item texts will be translated in the _gl_text_get().
	elm_genlist_realized_items_update(obj);
}

static int _app_control_request_transient_app_cb(void *data)
{
	return 0;
}


static char *_genlist_text_set_theme_color(const char *str, const char *code)
{
	int r, g, b, a;
	Eina_Strbuf *buf = NULL;
	char *colored = NULL;

	if (code == NULL)
		return NULL;

	buf = eina_strbuf_new();
	if (buf == NULL) {
		PRINTFUNC(DLOG_ERROR, "Can not get create strbuf");
		return NULL;
	}

//	ea_theme_color_get(code,
//			&r, &g, &b, &a,
//			NULL, NULL, NULL, NULL,
//			NULL, NULL, NULL, NULL);

	eina_strbuf_append_printf(buf, "<color=#%02x%02x%02x%02x>%s</color>",
			r, g, b, a, str);

	colored = eina_strbuf_string_steal(buf);

	eina_strbuf_free(buf);

	return colored;
}


static void _app_control_send_reply_cb(app_control_h request, app_control_h reply,
						app_control_result_e result, void *user_data)
{
		PRINTFUNC(DLOG_DEBUG, "");
		reply_to_sender_by_callback(NULL, NULL);
		elm_exit();
}

void set_source_caller_app_id(app_control_h app_control)
{
	if(!app_control){
		PRINTFUNC(DLOG_ERROR, "can't get app_control");
		return;
	}

//	CscFeatureBool is_security_permission_manager = CSC_FEATURE_BOOL_FALSE;
//	is_security_permission_manager = csc_feature_get_bool(CSC_FEATURE_DEF_BOOL_SECURITY_PERMISSION_MANAGER);

//	if(is_security_permission_manager == CSC_FEATURE_BOOL_TRUE){

		char *caller = NULL;
		app_control_get_caller(app_data->source_app_control, &caller);

		if(caller){
			PRINTFUNC(DLOG_DEBUG, "caller = %s", caller);
			app_control_add_extra_data(app_control, "caller_appid", caller);
			free(caller);
		}
//	}
}

void reply_to_sender_by_appcontrol(void *data, const char *value, const char *type)
{
	PRINTFUNC(DLOG_DEBUG, "");

	int ret;
	char *app_id;

	app_control_h app_control;

	App_Data *appdata = (App_Data *)data;

	if(!appdata){
		PRINTFUNC(DLOG_ERROR, "Null pointer");
		return;
	}

	ret = app_control_create(&app_control);
	if (ret != APP_CONTROL_ERROR_NONE) {
		PRINTFUNC(DLOG_ERROR, "Can not create app_control : %d", ret);
		return;
	}

	app_id = g_input_keyboard_data.app_id;
	if (app_id == NULL) {
		PRINTFUNC(DLOG_INFO, "app id is undefined, use defualt");
		app_id = "com.samsung.message.appcontrol.compose";
	}

	if(!strcmp(app_id, "com.samsung.message.appcontrol.compose")){
		app_id = "com.samsung.msg-send";
	}

	app_control_set_app_id(app_control, app_id);
	app_control_set_operation(app_control, APP_CONTROL_OPERATION_DEFAULT);
/*
	ret =  app_control_set_window(app_control,
				elm_win_xwindow_get(appdata->win_main));
	if (ret != APP_CONTROL_ERROR_NONE) {
		PRINTFUNC(DLOG_ERROR, "Can not app control set window : %d", ret);
		app_control_destroy(app_control);
		return;
	}
*/
	if (g_input_keyboard_data.data_array && g_input_keyboard_data.data_array_len > 0) {
		 app_control_add_extra_data_array(app_control,
						 "selector_keyboard_data_array",
						 (const char**)(g_input_keyboard_data.data_array),
						 g_input_keyboard_data.data_array_len);
	}

	app_control_add_extra_data(app_control, "selector_send_data_array", "sending_popup");

	if (value)
		app_control_add_extra_data(app_control, "selected_context", value);

	if (type)
		app_control_add_extra_data(app_control, "reply_type", type);

	set_source_caller_app_id(app_control);

	ret = app_control_send_launch_request(app_control,
					_app_control_send_reply_cb,
					NULL);

	if (ret != APP_CONTROL_ERROR_NONE)
		PRINTFUNC(DLOG_ERROR, "Can not launch app_control: %d", ret);

	app_control_destroy(app_control);

	return;
}

void reply_to_sender_by_callback(const char *value, const char *type)
{
	PRINTFUNC(DLOG_DEBUG, "");

	app_control_h app_control;

	if(app_control_create(&app_control) == 0) {
		int ret;

		if (value)
			app_control_add_extra_data(app_control, "selected_context", value);

		if (type)
			app_control_add_extra_data(app_control, "reply_type", type);

		set_source_caller_app_id(app_control);

		ret = app_control_reply_to_launch_request(app_control, app_data->source_app_control, APP_CONTROL_RESULT_SUCCEEDED);
		if (ret != APP_CONTROL_ERROR_NONE)
			PRINTFUNC(DLOG_ERROR, "reply failed : %d", ret);

		app_control_destroy(app_control);
		app_control_destroy(app_data->source_app_control);
		app_data->source_app_control = NULL;
	}
}

void reply_to_sender_by_callback_for_back()
{
	PRINTFUNC(DLOG_DEBUG, "");

	app_control_h app_control;

	if(app_control_create(&app_control) == 0) {
		int ret;

		app_control_add_extra_data(app_control, "back_to_composer", "yes");

		ret = app_control_reply_to_launch_request(app_control, app_data->source_app_control, APP_CONTROL_RESULT_SUCCEEDED);
		if (ret != APP_CONTROL_ERROR_NONE)
			PRINTFUNC(DLOG_ERROR, "reply failed : %d", ret);

		app_control_destroy(app_control);
		app_control_destroy(app_data->source_app_control);
		app_data->source_app_control = NULL;
	}
}

char* get_resource_path()
{
	if(NULL == app_data->res_path) {
		app_data->res_path = app_get_resource_path();
		PRINTFUNC(DLOG_INFO, "set resource path = %s", app_data->res_path);
	}
	return (app_data->res_path);
}

char* get_shared_resource_path()
{
	if(NULL == app_data->shared_res_path) {
		app_data->shared_res_path = app_get_shared_resource_path();
		PRINTFUNC(DLOG_INFO, "set shared resource path = %s", app_data->shared_res_path);
	}
	return (app_data->shared_res_path);
}

void show_gl_focus(Eina_Bool bVisible){
	if(app_data->genlist == NULL)
		return;

	if(bVisible == EINA_TRUE){
		elm_object_signal_emit(app_data->genlist, "elm,state,focus_bg,enable", "elm");
		//elm_layout_theme_set(app_data->genlist, "genlist", "base", "focus_bg");
		//elm_object_signal_emit(app_data->genlist, "elm,state,focus_bg,show", "elm");
	} else {
		elm_object_signal_emit(app_data->genlist, "elm,state,focus_bg,disable", "elm");
		//elm_layout_theme_set(app_data->genlist, "genlist", "base", "default");
		//elm_object_signal_emit(app_data->genlist, "elm,state,focus_bg,hide", "elm");
	}
}

void show_popup_toast(const char *text, bool check_img)
{
	PRINTFUNC(DLOG_ERROR, "show_popup_toast");

	Evas_Object *popup;

	popup = elm_popup_add(app_data->win_main);
	elm_object_style_set(popup, "toast/circle");
	elm_popup_orient_set(popup, ELM_POPUP_ORIENT_BOTTOM);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	if (check_img) {
		string path = get_resource_path();
		string path_ic = path + "/images/toast_check_icon.png";
		Evas_Object * img = elm_image_add(popup);
		elm_image_file_set(img, path_ic.c_str(), NULL);
		elm_object_part_content_set(popup, "toast,icon", img);
	}
	if(text) {
		elm_object_part_text_set(popup, "elm.text", text);
	}

	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, _popup_back_cb, NULL);
	evas_object_smart_callback_add(popup, "dismissed", _popup_close_cb, NULL);
	evas_object_smart_callback_add(popup, "block,clicked", _popup_back_cb, NULL);

	elm_popup_timeout_set(popup, 2.0);
	evas_object_show(popup);
}

static void _popup_close_cb(void *data, Evas_Object *obj, void *event_info)
{
	if(obj){
		evas_object_hide(obj);
		evas_object_del(obj);
	}
}

static void _popup_back_cb(void *data, Evas_Object *obj, void *event_info)
{
	if(obj)
		elm_popup_dismiss(obj);
}

char * _toast_delete_popup_access_info_cb(void *data, Evas_Object *obj)
{
	PRINTFUNC(DLOG_ERROR, "%s", __func__);
	const char * info = elm_object_part_text_get((Evas_Object*)data, "elm.text");

	return strdup(info);
}


void _back_to_genlist_for_selector()
{
	PRINTFUNC(DLOG_DEBUG, "");

	if(!app_data) return;

	if(app_data->app_type == APP_TYPE_STT || app_data->app_type == APP_TYPE_EMOTICON){
		PRINTFUNC(DLOG_DEBUG, "launched as STT/EMOTICON mode, So exit here.");
		reply_to_sender_by_callback(NULL, NULL);
		elm_exit();
	}

	Evas_Object *circle_genlist = (Evas_Object *) evas_object_data_get(app_data->genlist, "circle");

	eext_rotary_object_event_activated_set(circle_genlist, EINA_TRUE);
}


char *_it_plus_access_info_cb(void *data, Evas_Object *obj)
{
	PRINTFUNC(DLOG_DEBUG, "%s", __func__);
	std::string text = 	std::string(gettext("IDS_ACCS_BODY_BUTTON_TTS"));

	return strdup(text.c_str());
}

char *_access_info_cb(void *data, Evas_Object *obj)
{
	PRINTFUNC(DLOG_DEBUG, "%s", __func__);
	return strdup(gettext("WDS_TTS_TBBODY_DOUBLE_TAP_TO_SEND"));
}

Eina_Bool _access_action_next_cb(void *data, Evas_Object *obj, Elm_Access_Action_Info *action_info)
{
	Evas_Object *genlist = (Evas_Object*)data;
	PRINTFUNC(DLOG_ERROR, "[TNT] _access_action_next_cb called, genlist type:%s", evas_object_type_get(genlist));
	Elm_Object_Item *it = elm_genlist_first_item_get(genlist);//emptry item?
	PRINTFUNC(DLOG_ERROR, "[TNT] get first item:%p", it);
	it = elm_genlist_item_next_get(it);//title item(3 button?)
	PRINTFUNC(DLOG_ERROR, "[TNT] get Second item:%p, title_item:%p", it, it_title);
	elm_genlist_item_show(it, ELM_GENLIST_ITEM_SCROLLTO_IN);
	force_highlight_to_top = EINA_TRUE;
	return EINA_TRUE;
}

Eina_Bool _access_action_prev_cb(void *data, Evas_Object *obj, Elm_Access_Action_Info *action_info)
{
	Evas_Object *genlist = (Evas_Object*)data;
	PRINTFUNC(DLOG_ERROR, "[TNT] _access_action_prev_cb called, genlist type:%s", evas_object_type_get(genlist));
	Elm_Object_Item *it = elm_genlist_last_item_get(genlist);//plus button item
	PRINTFUNC(DLOG_ERROR, "[TNT] get last item:%p", it);
	elm_genlist_item_show(it, ELM_GENLIST_ITEM_SCROLLTO_IN);
	force_highlight_to_bottom = EINA_TRUE;
	return EINA_TRUE;
}

static void _item_realized(void *data, Evas_Object *obj, void *event_info) //called when list scrolled
{
	PRINTFUNC(DLOG_DEBUG, "%s", __func__);
}

Evas_Object* _create_genlist(Evas_Object* navi)
{
	Evas_Object* genlist = elm_genlist_add(navi);
	if(NULL == genlist)
		return NULL;

	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	elm_genlist_scroller_policy_set(genlist, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);

	Evas_Object* circle_object_genlist = eext_circle_object_genlist_add(genlist, app_data->circle_surface);
	eext_circle_object_genlist_scroller_policy_set(circle_object_genlist, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
	evas_object_data_set(genlist, "circle", (void *) circle_object_genlist);
	eext_rotary_object_event_activated_set(circle_object_genlist, EINA_TRUE);

	evas_object_size_hint_weight_set(genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(genlist, EVAS_HINT_FILL, EVAS_HINT_FILL);

	evas_object_smart_callback_add(genlist, "language,changed", __ise_gl_lang_changed, genlist);
	evas_object_show(genlist);

//	uxt_genlist_set_bottom_margin_enabled(genlist, EINA_TRUE);

	show_gl_focus(EINA_FALSE);

	Elm_Object_Item *nf_main_item = elm_naviframe_item_push(navi,
            NULL,
            NULL,
            NULL,
            genlist,
            "empty");

	elm_naviframe_item_pop_cb_set(nf_main_item, back_cb, app_data);
	evas_object_smart_callback_add(genlist, "realized", _item_realized, NULL);

	return genlist;
}

static void _item_position_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
	if(!obj) return;
	if(!data) return;

	Evas_Object *genlist = (Evas_Object *)obj;
	Elm_Object_Item *gen_item = (Elm_Object_Item *)data;

	Evas_Coord y;
	elm_scroller_region_get(genlist, NULL, &y, NULL, NULL);

//	PRINTFUNC(DLOG_DEBUG,"y=%d",y);

	if (250 > y && y >= 100){
		if (last_step == 0) return;
		last_step = 0;
		elm_object_item_signal_emit(gen_item, "elm,action,ime,0.0", "elm");
		show_gl_focus(EINA_TRUE);
	} else if (100 > y && y >= 50){
		if (last_step == 1) return;
		last_step = 1;
		elm_object_item_signal_emit(gen_item, "elm,action,ime,0.1", "elm");
		show_gl_focus(EINA_TRUE);
	} else if (y < 50 && y >=0){
		if (last_step == 2) return;
		last_step = 2;
		elm_object_item_signal_emit(gen_item, "elm,action,ime,0.9", "elm");
		show_gl_focus(EINA_FALSE);
	}
}

static char *
_main_menu_title_text_get(void *data, Evas_Object *obj, const char *part)
{
	char buf[1024];

	snprintf(buf, 1023, "%s", "Select method");
	return strdup(buf);
}

void _create_genlist_items(void* user_data, bool is_type_reply)
{
	LOGD("[psw] _create_genlist_items");

	App_Data* app_data = (App_Data*) user_data;

	if(NULL == app_data->genlist) {
		app_data->genlist = _create_genlist(app_data->naviframe);
		if(NULL == app_data->genlist)
		return;
	}

	elm_genlist_clear(app_data->genlist);
	g_smartreply_current_number = 0;	/* reset previous smartreply size to 0 */

	Elm_Genlist_Item_Class * itc0 = elm_genlist_item_class_new();
	itc0->item_style = "title";
	itc0->func.text_get = _main_menu_title_text_get;
	itc0->func.content_get = NULL;
	itc0->func.state_get = NULL;
	itc0->func.del = NULL;

	Elm_Genlist_Item_Class * itc1 = elm_genlist_item_class_new();
	itc1->item_style = "3button";
	itc1->func.text_get = NULL;
	itc1->func.content_get = __ise_gl_3button_content_get;
	itc1->func.state_get = NULL;
	itc1->func.del = NULL;


	// dummy title for empty space
	it_empty = elm_genlist_item_append(app_data->genlist, itc0,
			NULL, NULL,
			ELM_GENLIST_ITEM_NONE,
			NULL, NULL);

	if (!is_type_reply) {
		// 3 Buttons
		it_title = elm_genlist_item_append(app_data->genlist, itc1,
				NULL, NULL,
				ELM_GENLIST_ITEM_NONE,
				NULL, NULL);

		elm_genlist_item_select_mode_set(it_title, ELM_OBJECT_SELECT_MODE_NONE);
	}
	if (input_smartreply_is_enabled() == false) {
		_update_template_items(app_data);
		if(g_template_current_number > 0) {
			Evas_Object *circle_genlist = (Evas_Object *) evas_object_data_get(app_data->genlist, "circle");
			eext_circle_object_genlist_scroller_policy_set(circle_genlist,
					ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
		}
	} else {
		// dummy title for bottom
		Elm_Object_Item *dummy;

		dummy = elm_genlist_item_append(app_data->genlist, itc0,
				NULL, NULL,
				ELM_GENLIST_ITEM_NONE,
				NULL, NULL);

		elm_genlist_item_select_mode_set(dummy, ELM_OBJECT_SELECT_MODE_NONE);
	}


	Elm_Object_Item *item = elm_genlist_item_next_get(it_title);
	elm_genlist_item_show(item, ELM_GENLIST_ITEM_SCROLLTO_MIDDLE);

	evas_object_smart_callback_add(app_data->genlist, "elm,item,position,changed", _item_position_changed_cb, it_title);

	elm_genlist_item_class_free(itc0);
	elm_genlist_item_class_free(itc1);
}

void _update_genlist_items(void *user_data)
{
	int len = 0;

	if (input_smartreply_is_enabled())
		len = input_smartreply_get_reply_num();

	if (g_smartreply_current_number != len)
		_update_smartreply_items(user_data);

	_update_template_items(user_data);

	g_smartreply_current_number = len;

	if (g_smartreply_current_number > 0 || g_template_current_number > 0) {
		Evas_Object *circle_genlist = (Evas_Object *) evas_object_data_get(app_data->genlist, "circle");
		eext_circle_object_genlist_scroller_policy_set(circle_genlist,
							ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
	}
}

void _update_smartreply_items(void *user_data)
{
	App_Data* app_data;

	Elm_Object_Item *first;
	Elm_Object_Item *menu;
	Elm_Object_Item *pos;

	int len = 0;
	int i = 0;

	app_data = (App_Data *)user_data;

	if (app_data == NULL) {
		PRINTFUNC(DLOG_ERROR, "Can not get app_data");
		return;
	}

	if (app_data->genlist == NULL) {
		/* smartreply will update when genlist is exist only */
		PRINTFUNC(DLOG_ERROR, "Can not get getlist");
		return;
	}

	if (input_smartreply_is_enabled())
		len = input_smartreply_get_reply_num();

	if (g_smartreply_current_number == len)
		return;

	first = elm_genlist_first_item_get(app_data->genlist);
	menu = elm_genlist_item_next_get(first);

	/* Remove current smartreply list */
	pos = elm_genlist_item_next_get(menu);
	for (i = 0; i < g_smartreply_current_number; i++) {
		Elm_Object_Item *next;

		if (pos == NULL)
			break;

		next = elm_genlist_item_next_get(pos);
		elm_object_item_del(pos);
		pos = next;
	}

	/* Append newly added smartreply list */
	if (len > 0) {
		Elm_Genlist_Item_Class *itc;

		itc = elm_genlist_item_class_new();

		itc->item_style = "1text";
		itc->func.text_get = __ise_smartreply_gl_text_get;
		itc->func.content_get = NULL;
		itc->func.state_get = NULL;
		itc->func.del = NULL;

		pos = menu;
		for (i = 0; i < len; i++) {
			pos = elm_genlist_item_insert_after(app_data->genlist,
							itc,
							(void *)i,
							NULL,
							pos,
							ELM_GENLIST_ITEM_NONE,
							__ise_smartreply_gl_sel,
							app_data);
		}

		if (menu)
			elm_genlist_item_show(menu, ELM_GENLIST_ITEM_SCROLLTO_TOP);

		elm_genlist_item_class_free(itc);
	}

	g_smartreply_current_number = len;
}


void _update_template_items(void *user_data)
{
	App_Data* app_data;

	Elm_Object_Item *first;
	Elm_Object_Item *menu;
	Elm_Object_Item *pos;
	Elm_Object_Item *template_pos;

	unsigned int template_len = 0;
	unsigned int i = 0;

	app_data = (App_Data *)user_data;

	if (app_data == NULL) {
		PRINTFUNC(DLOG_ERROR, "Can not get app_data");
		return;
	}

	if (app_data->genlist == NULL) {
		/* smartreply will update when genlist is exist only */
		PRINTFUNC(DLOG_ERROR, "Can not get getlist");
		return;
	}

	if (app_data->app_type == APP_TYPE_REPLY) {
		menu = elm_genlist_first_item_get(app_data->genlist);
	} else {
		first = elm_genlist_first_item_get(app_data->genlist);
		menu = elm_genlist_item_next_get(first);
	}

	pos = menu;
	/* move to smartreply next if it need */
	if (input_smartreply_is_enabled()) {
		int smartreply_len = 0;

		smartreply_len = input_smartreply_get_reply_num();
		if (smartreply_len > 0) {
			int j = 0;

			for (j = 0; j < smartreply_len; j++) {
				if (pos == NULL)
					break;

				pos = elm_genlist_item_next_get(pos);
			}
		}
	}

	/* Remove Current template list */
	template_pos = elm_genlist_item_next_get(pos);
	for (i = 0 ; i < g_template_current_number; i++) {
		Elm_Object_Item *next;

		if (template_pos == NULL) {
			PRINTFUNC(DLOG_ERROR, "Template mismatched, iter : %d, current num: %d",
					i, g_template_current_number);
			break;
		}

		next = elm_genlist_item_next_get(template_pos);
		elm_object_item_del(template_pos);
		template_pos = next;
	}

	/* Remove Add or dummpy button */
	if(template_pos) {
		Elm_Object_Item *next;

		next = elm_genlist_item_next_get(template_pos);
		elm_object_item_del(template_pos);
		template_pos = next;
	}

	/* Append New Template list */
	const std::vector<TemplateData> template_list = input_template_get_list();
	template_len = 0;

	if (template_list.size() > 0) {
		Elm_Genlist_Item_Class *itc;

		itc = elm_genlist_item_class_new();

		itc->item_style = "1text";
		itc->func.text_get = __ise_template_gl_text_get;
		itc->func.content_get = NULL;
		itc->func.state_get = NULL;
		itc->func.del = NULL;

		for (i = 0; i < template_list.size(); i++) {
			bool matched = false;

			if(!template_list[i].use_gettext) {
				int smartreply_len = 0;
				int j = 0;

				smartreply_len = input_smartreply_get_reply_num();
				for (j = 0; j < smartreply_len; j++) {
					char *reply = NULL;

					reply = input_smartreply_get_nth_item(j);
					if (reply) {
						if (!strcmp(reply, template_list[i].text.c_str())) {
							matched = true;
							free(reply);
							break;
						}
						free(reply);
					}
				}
			}

			if (matched == false) {
				pos = elm_genlist_item_insert_after(app_data->genlist,
						itc,
						(void *)i,
						NULL,
						pos,
						ELM_GENLIST_ITEM_NONE,
						__ise_template_gl_sel,
						app_data);
				template_len++;
			}
		}

		elm_genlist_item_class_free(itc);
	}


	// template add button (+)
/*	if (!input_template_is_user_template()) {
		Elm_Genlist_Item_Class *itc;

		itc = elm_genlist_item_class_new();

		itc->item_style = "1text.1icon";
		itc->func.text_get = __ise_addtemplate_gl_text_get;;
		itc->func.content_get = __ise_gl_1button_content_get;
		itc->func.state_get = NULL;
		itc->func.del = NULL;


		pos = elm_genlist_item_insert_after(app_data->genlist,
				itc,
				NULL,
				NULL,
				pos,
				ELM_GENLIST_ITEM_NONE,
				__ise_template_add_gl_sel,
				app_data);

		elm_genlist_item_class_free(itc);
	}
*/
	if (menu)
		elm_genlist_item_show(menu, ELM_GENLIST_ITEM_SCROLLTO_TOP);

	g_template_current_number = template_len;
}


bool _app_create(void* user_data)
{
	int width = 1000, height = 1000;
//	App_Data* app_data = NULL;
	Evas_Object* layout = NULL;
	Evas_Object* conform = NULL;
	Evas_Object* bg = NULL;
	Evas_Object* window = NULL;
	Eext_Circle_Surface *surface;

	if (!user_data) {
		return false;
	}

	_app_language_changed(NULL, NULL);
#ifdef SUPPORT_LOG_MANAGER
	log_manager_init();
#endif

	app_data = (App_Data*)user_data;

	elm_app_base_scale_set(1.3);

	window = elm_win_add(NULL, PACKAGE, ELM_WIN_BASIC);
	if (window) {
		init_customizing_theme();

		elm_win_title_set(window, PACKAGE);
		elm_win_borderless_set(window, EINA_TRUE);
//		ecore_x_window_size_get(ecore_x_window_root_first_get(), &width, &height);
		evas_object_resize(window, width, height);
		elm_win_indicator_mode_set(window, ELM_WIN_INDICATOR_SHOW);
	} else {
		LOGE("elm_win_add() is failed.");
		return false;
	}

	bg = elm_bg_add(window);
	evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(window, bg);
	evas_object_show(bg);

	layout = elm_layout_add(window);
	elm_layout_theme_set(layout, "layout", "application", "default");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	conform = elm_conformant_add(window);
	surface = eext_circle_surface_conformant_add(conform);
	evas_object_size_hint_weight_set(conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);


	elm_win_resize_object_add(window, conform);
	elm_object_content_set(conform, layout);

	evas_object_show(layout);
	evas_object_show(conform);
	evas_object_show(window);
	app_data->win_main = window;
	app_data->conform = conform;
	app_data->layout_main = layout;
	app_data->circle_surface = surface;
	app_data->app_type = APP_TYPE_SELECT_MODE;

	Evas_Object *naviframe = elm_naviframe_add(layout);
	elm_naviframe_prev_btn_auto_pushed_set(naviframe, EINA_FALSE);
	eext_object_event_callback_add(naviframe, EEXT_CALLBACK_BACK, eext_naviframe_back_cb, NULL);
	elm_object_part_content_set(layout, "elm.swallow.content", naviframe);
	evas_object_show(naviframe);

	app_data->naviframe = naviframe;

	return true;
}

void _app_service(app_control_h service, void* user_data)
{
	int ret;
	bool b_ret;
	char *context = NULL;
	char *app_id = NULL;

	app_control_clone(&(app_data->source_app_control), service);

	app_data->reply_type = REPLY_APP_NORMAL;
	ret = app_control_get_extra_data(service, "selector_keyboard_app_id", &app_id);
	if (ret == APP_CONTROL_ERROR_NONE) {
		PRINTFUNC(DLOG_DEBUG, "app_id = %s", app_id);
 	}

	if(app_id){
		if(!strcmp(app_id, "com.samsung.message.appcontrol.compose") || !strcmp(app_id, "com.samsung.wemail-send")) {
			app_data->reply_type = REPLY_APP_CONTROL;
		} else {
			app_data->reply_type = REPLY_APP_NORMAL;
		}
	}
 	if (app_id)
		free(app_id);

    bool is_type_reply = false;
	ret = app_control_get_extra_data(service, APP_CONTROL_DATA_INPUT_TYPE, &context);
	if (ret == APP_CONTROL_ERROR_NONE) {
		if (!strcmp(context, "input_voice")) {
			app_data->app_type = APP_TYPE_STT;
			_stt_clicked_cb((void *)app_data, NULL, NULL);
			goto ACTIVATE;
		} else if (!strcmp(context, "input_emoticon")) {
			app_data->app_type = APP_TYPE_EMOTICON;
			_emoticon_clicked_cb((void *)app_data, NULL, NULL);
			goto ACTIVATE;
        } else if (!strcmp(context, "input_keyboard")) {
			app_data->app_type = APP_TYPE_KEYBOARD;
			input_keyboard_init(service);
			_keyboard_clicked_cb((void *)app_data, NULL, NULL);
			goto ACTIVATE;
		} else if (!strcmp(context, "keyboard_input_from_sms")) {
			app_data->app_type = APP_TYPE_KEYBOARD_FROM_SMS;
			input_keyboard_init(service);
			_keyboard_clicked_cb((void *)app_data, NULL, NULL);
			goto ACTIVATE;
		} else if (!strcmp(context, "input_reply")) {
			app_data->app_type = APP_TYPE_REPLY;
            is_type_reply = true;
		}
	}

	if (app_data->genlist == NULL)
		app_data->genlist = _create_genlist(app_data->naviframe);

	input_keyboard_init(service);

//	input_smartreply_init(service);
//	input_smartreply_set_notify(_input_smartreply_notify_cb, NULL);

//	if (input_smartreply_is_enabled())
//		input_smartreply_get_reply_async();


	input_template_init(service);
	input_template_set_notify(_input_template_notify_cb,  NULL);

	_create_genlist_items(app_data, is_type_reply);

ACTIVATE :
	elm_win_activate(app_data->win_main);

//	app_control_request_transient_app(service, elm_win_xwindow_get(app_data->win_main), _app_control_request_transient_app_cb,  NULL);
	if(context)
		free(context);
}


void _app_pause(void* user_data)
{
	PRINTFUNC(DLOG_DEBUG, "");
	pause_voice();
}

void _app_resume(void* user_data)
{
	PRINTFUNC(DLOG_DEBUG, "");
}

void _app_terminate(void* user_data)
{
	App_Data* app_data = NULL;
	app_data = (App_Data*)user_data;

	if(app_data->genlist){
		evas_object_smart_callback_del(app_data->genlist, "elm,item,position,changed", _item_position_changed_cb);
	}

	if(app_data->res_path)
		free(app_data->res_path);

	if(app_data->shared_res_path)
		free(app_data->shared_res_path);

	input_keyboard_deinit();
	input_smartreply_deinit();

	input_template_unset_notify();
	input_template_deinit();

#ifdef SUPPORT_LOG_MANAGER
	log_manager_exit();
#endif
}

static int init_i18n(const char *domain, const char *dir, char *lang_str)
{
	if (setlocale(LC_ALL, "") == NULL) {
		PRINTFUNC(DLOG_INFO, "Some environment variable is invalid, setlocale(LC_ALL, \"\") has returns\ed a null pointer");
		if (setlocale(LC_ALL, lang_str) == NULL)
			return -1;
	}
	if (bindtextdomain(domain, dir) == NULL)
		return -1;
	if (textdomain(domain) == NULL)
		return -1;

	return 0;
}

/**
 * @brief Set language and locale.
 *
 * @return void
 */
static void _app_language_changed(app_event_info_h event_info, void *user_data)
{
	char* lang_str = vconf_get_str(VCONFKEY_LANGSET);
	if (lang_str) {
		setenv("LANG", lang_str, 1);
		setenv("LC_MESSAGES", lang_str, 1);
	} else {
		setenv("LANG", "en_GB.utf8", 1);
		setenv("LC_MESSAGES", "en_GB.utf8", 1);
		lang_str = strdup("en_GB.UTF-8");
	}

	init_i18n(PACKAGE, LOCALEDIR, lang_str);

	if (lang_str) {
		elm_language_set(lang_str);
		free(lang_str);
	}
}



int main(int argc, char* argv[])
{
	App_Data app_data = {0, };
	ui_app_lifecycle_callback_s event_callback = {0, };
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = _app_create;
	event_callback.terminate = _app_terminate;
	event_callback.pause = _app_pause;
	event_callback.resume = _app_resume;
	event_callback.app_control = _app_service;

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, NULL, &app_data);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, NULL, &app_data);
	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, NULL, &app_data);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, _app_language_changed, &app_data);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, NULL, &app_data);

	_init_app_data(&app_data);

	int ret = ui_app_main(argc, argv, &event_callback, &app_data);
	if (ret != APP_ERROR_NONE) {
		LOGD("ui_app_main() is failed. err = %d", ret);
	}

	return ret;
}

#ifdef SUPPORT_LOG_MANAGER
static void log_manager_init(void)
{
	if (g_log_manager)
		return;

	samsung_log_manager_create(&g_log_manager);
}

static void log_manager_exit(void)
{
	samsung_log_manager_destroy(g_log_manager);
	g_log_manager = NULL;
}

static void log_manager_set(const char *feature, const char *extra, const char *value)
{
	const char *uri = USE_APP_FEATURE_SURVEY_URI;

	unsigned int request_id;
	bundle *log_data;

	if (g_log_manager == NULL) {
		PRINTFUNC(DLOG_ERROR, "log manager doesn't initialized");
		return;
	}

	if (feature == NULL) {
		PRINTFUNC(DLOG_ERROR, "feature is empty");
		return;
	}

	log_data = bundle_create();

	bundle_add(log_data, "app_id", PACKAGE);
	bundle_add(log_data, "feature", feature);

	if (extra)
		bundle_add(log_data, "extra", extra);
	if (value)
		bundle_add(log_data, "value", value);

	samsung_log_manager_insert(g_log_manager, uri, log_data, NULL, NULL, &request_id);

	bundle_free(log_data);
}
#endif

