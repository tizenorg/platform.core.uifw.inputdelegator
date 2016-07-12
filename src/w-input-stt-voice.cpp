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

#include <stdio.h>
#include <Elementary.h>
#include <Ecore_IMF.h>
#include <Ecore.h>
#include <device/power.h>
#include <efl_assist.h>
#include <app_common.h>
#include <app.h>
#include <string>
#include <efl_extension.h>
#include <feedback.h>

#include "Debug.h"
#include "w-input-selector.h"
#include "w-input-smartreply.h"
#include "w-input-stt-voice.h"
#include "w-input-stt-engine.h"
#include "w-input-stt-ise.h"

using namespace std;

#define _EDJ(x)  elm_layout_edje_get(x)


#define GRP_PORTRAIT "mic_control"

#define item_append(obj, style, index, cb, udata) \
		elm_genlist_item_append(obj, &(style), (void *)index, NULL, ELM_GENLIST_ITEM_NONE, cb, udata)

#define VIRTUAL_TEXT_AREA_FONT_STYLE \
		"DEFAULT='font=Tizen:style=Regular  font_size=32 color=#FFFFFF color_class=AT013 text_class=tizen wrap=mixed align=center' \
		newline='br' b='+ font=Tizen:style=Bold'link='+ underline=on underline_color=#FFFFFF'"

extern App_Data* app_data;
extern VoiceData *my_voicedata;

static bool power_state = false;

static bool bottom_button_access_state = false;

static Evas_Object *radio_gp = NULL;
Evas_Object *g_setting_window = NULL;
static Evas_Object *g_setting_naviframe = NULL;
static Evas_Object *g_more_option_layout = NULL;

static Ecore_Event_Handler *g_evt_key_down = NULL;

static bool g_is_n66 = true;

static Elm_Genlist_Item_Class itc_title;
static Elm_Genlist_Item_Class itc_1text;
static Elm_Genlist_Item_Class itc_2text;

static bool g_send_button_clicked = false;
//accessbility
static bool g_mic_clicked = false;

static void set_guide_text(VoiceData *vd, const char* text, bool translatable = false);

char *supported_language[13] = {
		"auto",
		"de_DE",
		"en_GB",
		"en_US",
		"es_ES",
		"es_US",
		"fr_FR",
		"it_IT",
		"pt_BR",
		"ru_RU",
		"zh_CN",
		"ja_JP",
		"ko_KR",
};

char *supported_language_n66[7] = {
                "auto",
                "en_US",
                "es_US",
                "fr_FR",
                "zh_CN",
                "ja_JP",
                "ko_KR",
};


char *disp_lang_array[13] = {
		"",
		"Deutsch",
		"English (United Kingdom)",
		"English (United States)",
		"Español (España)",
		"Español (América Latina)",
		"Français (France)",
		"Italiano",
		"Português (Brasil)",
		"Pусский",
		"简体中文",
		"日本語",
		"한국어",
};

char *disp_lang_array_n66[7] = {
                "",
                "English (United States)",
                "Español (América Latina)",
                "Français (France)",
                "简体中文",
                "日本語",
                "한국어",
};


typedef enum {
	STT_VOICE_AUTO,
	STT_VOICE_DE_DE,
	STT_VOICE_EN_GB,
	STT_VOICE_EN_US,
	STT_VOICE_ES_ES,
	STT_VOICE_ES_US,
	STT_VOICE_FR_FR,
	STT_VOICE_IT_IT,
	STT_VOICE_PT_BR,
	STT_VOICE_RU_RU,
	STT_VOICE_ZH_CN,
	STT_VOICE_JA_JP,
	STT_VOICE_KO_KR
}STT_VOICE_LANGUAGE_I;

typedef enum {
        STT_VOICE_N66_AUTO,
        STT_VOICE_N66_EN_US,
        STT_VOICE_N66_ES_US,
        STT_VOICE_N66_FR_FR,
        STT_VOICE_N66_ZH_CN,
        STT_VOICE_N66_JA_JP,
        STT_VOICE_N66_KO_KR
}STT_VOICE_LANGUAGE_N66_I;

typedef enum {
	BEEP_SOUND_START,
	BEEP_SOUND_STOP,
	VIBRATION_START,
	VIBRATION_STOP,
	BEEP_SOUND_TOUCH,
	WAKEUP_COMMAND,
}FeedbackType;

static void get_stt_default_language(VoiceData *my_voicedata);


char* get_lang_label(char lang[])
{
	char *str = NULL;

	if (strcmp(lang, "en_US") == 0)
		str = disp_lang_array[3];

	else if (strcmp(lang, "ko_KR") == 0)
		str = disp_lang_array[12];

	else if (strcmp(lang, "de_DE") == 0)
		str = disp_lang_array[1];

	else if (strcmp(lang, "fr_FR") == 0)
		str = disp_lang_array[6];

	else if (strcmp(lang, "it_IT") == 0)
		str = disp_lang_array[7];

	else if (strcmp(lang, "en_GB") == 0)
		str = disp_lang_array[2];

	else if (strcmp(lang, "ja_JP") == 0)
		str = disp_lang_array[11];

	else if (strcmp(lang, "zh_CN") == 0)
		str = disp_lang_array[10];

	else if (strcmp(lang, "ru_RU") == 0)
		str = disp_lang_array[9];

	else if (strcmp(lang, "pt_BR") == 0)
		str = disp_lang_array[8];

	else if (strcmp(lang, "es_ES") == 0)
		str = disp_lang_array[4];

	else if (strcmp(lang, "es_US") == 0)
		str = disp_lang_array[5];

	else
		str = disp_lang_array[3];

	return str;
}

char* get_lang_label_n66(char lang[])
{
	char *str = NULL;

	if (strcmp (lang, "en_US") == 0)
		str = disp_lang_array_n66[1];

	else if (strcmp (lang, "ko_KR") == 0)
		str = disp_lang_array_n66[6];

	else if (strcmp (lang, "fr_FR") == 0)
		str = disp_lang_array_n66[3];

	else if (strcmp (lang, "ja_JP") == 0)
		str = disp_lang_array_n66[5];

	else if (strcmp (lang, "zh_CN") == 0)
		str = disp_lang_array_n66[4];

	else if (strcmp (lang, "es_US") == 0)
		str = disp_lang_array_n66[2];

	else
		str = disp_lang_array_n66[1];

	return str;
}


static Eina_Bool
_bring_in_cb(void *data)
{
	if(!data)
		return ECORE_CALLBACK_CANCEL;

	Evas_Coord x, y, w, h;

	Evas_Object *scroller = (Evas_Object *)data;
	Evas_Object *inner_layout = NULL;

	inner_layout = (Evas_Object *) evas_object_data_get(scroller, "inner_layout");

	evas_object_geometry_get(inner_layout, &x, &y, &w, &h);
	elm_scroller_region_bring_in(scroller, x, h, w, h);

	PRINTFUNC(NO_PRINT, "scroller %d %d %d %d", x, y, w, h);

	return ECORE_CALLBACK_CANCEL;
}

static inline Evas_Coord get_text_block_size(Evas_Object *obj, std::string text)
{
	int calculated_height = 0;

	int max_height = 1280;
	int unit_width = 282;

	Evas_Coord width, height;

	char *strbuf = NULL;
	Evas_Object *tb = NULL;
	Evas_Textblock_Style *st = NULL;
	Evas_Textblock_Cursor *cur = NULL;

	tb = evas_object_textblock_add(evas_object_evas_get(obj));
	evas_object_textblock_legacy_newline_set(tb, EINA_FALSE);

	st = evas_textblock_style_new();
	evas_textblock_style_set(st, VIRTUAL_TEXT_AREA_FONT_STYLE);
	evas_object_textblock_style_set(tb, st);

	cur = evas_object_textblock_cursor_new(tb);
	strbuf = elm_entry_utf8_to_markup(text.c_str());
   	evas_object_resize(tb, unit_width, max_height);

	evas_object_textblock_text_markup_set(tb, strbuf);
	evas_textblock_cursor_format_prepend(cur, "+ wrap=mixed");
    evas_object_textblock_size_formatted_get(tb, &width, &height);
   	evas_object_resize(tb, unit_width, height);

	if(strbuf) free(strbuf);
	if(tb) evas_object_del(tb);
	if(st) evas_textblock_style_free(st);
	if(cur) evas_textblock_cursor_free(cur);

	return height;
}

static Eina_Bool _update_textblock_timer_cb(void *data)
{
	if(!data)
		return ECORE_CALLBACK_CANCEL;

	VoiceData* voicedata = (VoiceData*)data;

	voicedata->textblock_timer = NULL;

	std::string result_text;
	for(int i = 0; i < voicedata->stt_results.size(); i++){
		if(i == voicedata->stt_results.size()-1){
			result_text += voicedata->stt_results.at(i);
		} else {
			result_text += voicedata->stt_results.at(i);
			result_text += " ";
		}
	}

	Evas_Object *rect = NULL;
	Evas_Object *entry = NULL;
	Evas_Object *scroller = NULL;
	Evas_Object *box = NULL;
	Evas_Object *inner_layout = NULL;
	char *strbuf = NULL;

	scroller = elm_layout_content_get((Evas_Object *)voicedata->layout_main, "text_area");
	if(!scroller)
	{
		PRINTFUNC(DLOG_ERROR, "failed to get scroller");
		return ECORE_CALLBACK_CANCEL;
	}

	box = elm_object_content_get(scroller);
	if(!box)
	{
		PRINTFUNC(DLOG_ERROR, "failed to get box");
		return ECORE_CALLBACK_CANCEL;
	}

	inner_layout = (Evas_Object *) evas_object_data_get(scroller, "inner_layout");
	if(!inner_layout)
	{
		PRINTFUNC(DLOG_ERROR, "failed to get inner_layout");
		return ECORE_CALLBACK_CANCEL;
	}

	entry = elm_layout_content_get(inner_layout, "elm.swallow.content");
	if(!entry)
	{
		PRINTFUNC(DLOG_ERROR, "failed to get entry");
		return ECORE_CALLBACK_CANCEL;
	}

	Evas_Object *top = (Evas_Object *) evas_object_data_get(box, "top_padding");
	Evas_Object *bottom = (Evas_Object *) evas_object_data_get(box, "bottom_padding");

	if(top) {
		elm_box_unpack(box, top);
		evas_object_del(top);
		top = NULL;
	}
	if(bottom) {
		elm_box_unpack(box, bottom);
		evas_object_del(bottom);
		bottom = NULL;
	}

	Evas_Coord height = get_text_block_size(box, result_text);

//	if(height < 173) {
	{
		int text_area_height = 360-102; // screen H - bottom button H
		int top_height = ((text_area_height - height) / 2);
		if(top_height < 120) top_height = 120; // Top fade area H

		int bottom_height = (text_area_height - top_height - height);
		if(bottom_height < 1) bottom_height = 0;

		PRINTFUNC(SECURE_DEBUG, "str : %s", result_text.c_str());
		PRINTFUNC(DLOG_DEBUG, "height : %d", height);
		PRINTFUNC(DLOG_DEBUG, "top_height : %d", top_height);
		PRINTFUNC(DLOG_DEBUG, "bottom_height : %d", bottom_height);

		rect = evas_object_rectangle_add(evas_object_evas_get(box));
		evas_object_color_set(rect, 0, 0, 0, 100);
		evas_object_size_hint_weight_set(rect, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_min_set(rect, 282, top_height);
		evas_object_show(rect);

		elm_box_pack_before(box, rect, inner_layout);
		evas_object_data_set(box, "top_padding", (void *) rect);

		rect = evas_object_rectangle_add(evas_object_evas_get(box));
		evas_object_color_set(rect, 0, 0, 0, 100);
		evas_object_size_hint_weight_set(rect, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_min_set(rect, 282, bottom_height);
		evas_object_show(rect);

		elm_box_pack_end(box, rect);
		evas_object_data_set(box, "bottom_padding", (void *) rect);
	}

	elm_entry_entry_set(entry, result_text.c_str());

	Evas_Object *panel_layout = elm_layout_content_get((Evas_Object *)voicedata->layout_main, "left_panel_area");
	Evas_Object *panel_left = elm_layout_content_get((Evas_Object *)panel_layout, "elm.swallow.right");

	if (result_text.size() > 0) {
		elm_object_signal_emit(panel_left, "elm,state,enabled", "elm");
	} else {
		elm_object_signal_emit(panel_left, "elm,state,disabled", "elm");
	}

	elm_object_signal_emit((Evas_Object *)voicedata->layout_main, "idle,state,guide_text,bottom", "elm");

	if(voicedata->state == STT_STATE_VAL_LISTENING){
		if(voicedata->guide_text_timer != NULL){
			PRINTFUNC(DLOG_DEBUG, "Skip hide_guide_text");
			ecore_timer_del(voicedata->guide_text_timer);
			voicedata->guide_text_timer = NULL;
		}
		set_guide_text(voicedata, SK_TAP_TO_PAUSE, true);
	}

	elm_object_signal_emit(inner_layout, "scrolling", "entry");

	return ECORE_CALLBACK_CANCEL;
}

void _update_textblock(void *data)
{
	if(!data)
		return;

	VoiceData *voicedata = (VoiceData *) data;

	if(voicedata->textblock_timer == NULL){
		voicedata->textblock_timer = ecore_timer_add(1.0, _update_textblock_timer_cb, voicedata);
	} else {
		PRINTFUNC(DLOG_DEBUG, "skip : the timer is not expired.");
	}

	return;
}

void voice_get_string(const char *keyValue, _VoiceData *voicedata)
{
	if(!voicedata)
		return;

	char* strbuf = NULL;

	strbuf = elm_entry_utf8_to_markup(keyValue);

	PRINTFUNC(DLOG_DEBUG, "text : %s, voicedata->partial_result=%s", strbuf, voicedata->partial_result);

	if(strbuf){
		if(voicedata->partial_result){ // partial_result is not Null so replace
			if(strcmp(voicedata->partial_result, strbuf)){
				// different replace
				PRINTFUNC(DLOG_DEBUG, "different replace");

				voicedata->stt_results.pop_back();
				voicedata->stt_results.push_back(strbuf);

				free(voicedata->partial_result);
				voicedata->partial_result = strdup(strbuf);
			}
		} else { // partial_result is Null so first case
			PRINTFUNC(DLOG_DEBUG, "first push");
			voicedata->stt_results.push_back(strbuf);
			voicedata->partial_result = strdup(strbuf);
		}
	}

	_update_textblock(voicedata);

	if(strbuf)
		free(strbuf);

	PRINTFUNC(DLOG_DEBUG, "ends");
}

static Eina_Bool _recognition_failure_cb(void *data)
{
	if(data) {
		VoiceData *voicedata = (VoiceData *) data;

		voicedata->state = STT_STATE_VAL_INIT;
		set_animation_state(voicedata);
		voicedata->refresh_timer = NULL;
	}
	return ECORE_CALLBACK_CANCEL;
}

void start_by_press(VoiceData *voicedata)
{
	PRINTFUNC(DLOG_DEBUG, "");

	edje_object_signal_emit(_EDJ(voicedata->layout_main), "mouse,clicked,1", "background");

	return;
}

static void on_mic_button_press_cb(void *data, Evas_Object *obj, void *event_info)
{
	PRINTFUNC(NO_PRINT, "");

	VoiceData *voicedata = (VoiceData *)data;

	if (elm_config_access_get()){
		if(bottom_button_access_state == true){
			PRINTFUNC(DLOG_DEBUG, "skip until bottom buttom is being read by accessibility");
			return;
		}
	}

	edje_object_signal_emit(_EDJ(voicedata->layout_main), "mouse,clicked,1", "background");

	return;
}

static char *_left_cue_access_info_cb(void *data, Evas_Object *obj)
{
   char text[512];

   snprintf(text, sizeof(text), "%s, %s", gettext(TTS_SEND), gettext(TTS_BUTTON));

   return strdup(text);
}

static void _left_cue_access_activate_cb(void * data, Evas_Object *part_obj, Elm_Object_Item *item)
{
    Evas_Object *obj = (Evas_Object *)data;
    elm_layout_signal_emit(obj, "cue,clicked", "elm");

    elm_panel_toggle(obj);
}


static void _send_button_highlighted_cb(void *data, Evas_Object * obj, void *event_info)
{
	PRINTFUNC(DLOG_DEBUG, "");
	VoiceData* voicedata = (VoiceData*)data;

	if (voicedata->sttmanager->GetCurrent() == STT_STATE_RECORDING ||
	 voicedata->sttmanager->GetCurrent() == STT_STATE_PROCESSING)
		return;

	std::string text = std::string(gettext(TTS_SEND));
	text = text +" "+ gettext(TTS_BUTTON);
	if (elm_object_disabled_get(obj))
		text = text + " " + gettext(TTS_DISABLED);

	elm_access_say(text.c_str());
}

static Eina_Bool _mic_button_enable_cb(void *data)
{
	PRINTFUNC(DLOG_DEBUG, "");

	if(!data)
		return ECORE_CALLBACK_CANCEL;

	VoiceData* voicedata = (VoiceData*)data;

	voicedata->btn_disabling_timer = NULL;

	Evas_Object *button = (Evas_Object *)voicedata->mic_button;

	elm_object_disabled_set(button, EINA_FALSE);

	if (elm_config_access_get()){
		bottom_button_access_state = false;
	}

	return ECORE_CALLBACK_CANCEL;
}

static void _mic_highlighted_cb(void *data, Evas_Object * obj, void *event_info)
{
	PRINTFUNC(DLOG_DEBUG, "");
	VoiceData* voicedata = (VoiceData*)data;

	if (voicedata->sttmanager->GetCurrent() == STT_STATE_RECORDING ||
	voicedata->sttmanager->GetCurrent() == STT_STATE_PROCESSING)
	   return;


	if (elm_config_access_get()){
		bottom_button_access_state = true;
	} else {
		elm_object_disabled_set(voicedata->mic_button, EINA_TRUE);
	}


	if(voicedata->btn_disabling_timer == NULL){
		ecore_timer_del(voicedata->btn_disabling_timer);
	}
	voicedata->btn_disabling_timer = ecore_timer_add(3.0, _mic_button_enable_cb, voicedata);

	elm_access_say(gettext(SK_DOUBLE_TAP_TO_SPEAK));
}

static char *_mic_access_info_cb(void *data, Evas_Object *obj)
{
	if (data) return strdup((const char*)data);
	return NULL;
}


static void on_confirm_button_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	if(g_send_button_clicked == true){
		PRINTFUNC(DLOG_DEBUG, "skipped seding STT result");
		return;
	}

	g_send_button_clicked = true;

	if(!data)
		return;

	VoiceData* voicedata = (VoiceData*)data;

	std::string result_text;
	for(int i = 0; i < voicedata->stt_results.size(); i++){
		result_text += voicedata->stt_results.at(i);
		if(i != voicedata->stt_results.size()-1)
			result_text += " ";
	}

	PRINTFUNC(DLOG_DEBUG, "result_text = %s", result_text.c_str());
	input_smartreply_send_feedback(result_text.c_str());

	if(app_data->reply_type == REPLY_APP_CONTROL){
		reply_to_sender_by_appcontrol((void*)app_data, result_text.c_str(), "voice");
	} else {
		reply_to_sender_by_callback(result_text.c_str(), "voice");
		destroy_voice();
		powerUnlock();
		elm_exit();
	}

	return;
}

static void
_panel_cue_clicked_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
	PRINTFUNC(DLOG_DEBUG, "left cue clicked!!");

	on_confirm_button_clicked_cb(data, NULL, NULL);
}



void stt_feedback_initialize()
{
	int ret = 0;

	ret = feedback_initialize();
	if(ret != 0)
	{
		PRINTFUNC(DLOG_ERROR, "feedback_initialize failed!");
		return;
	}
	return;
}

void stt_feedback(FeedbackType type)
{
	int ret;

	ret = feedback_initialize();
	if(ret != 0)
	{
		PRINTFUNC(DLOG_ERROR, "feedback_initialize failed!");
		return;
	}

	if (type == VIBRATION_START){
//		feedback_play(FEEDBACK_PATTERN_VOICE_START);
	} else if (type == VIBRATION_STOP){
//		feedback_play(FEEDBACK_PATTERN_VOICE_STOP);
	}

	ret = feedback_deinitialize();
	if(ret != 0)
	{
		PRINTFUNC(DLOG_ERROR, "feedback_initialize failed!");
		return;
	}
}

void stt_feedback_deinitialize()
{
	int ret = 0;

	ret = feedback_deinitialize();
	if(ret != 0)
	{
		PRINTFUNC(DLOG_ERROR, "feedback_initialize failed!");
		return;
	}
	return;
}

static Eina_Bool _idler_cb(void *data)
{
	if(!data) return ECORE_CALLBACK_CANCEL;

	VoiceData *voicedata = (VoiceData *)data;

	PRINTFUNC(DLOG_DEBUG, "");


	if(true == _app_stt_initialize(voicedata)) {
		PRINTFUNC(NO_PRINT, "_app_stt_initialize None Error");
		voicedata->voicefw_state = 1;
		voicedata->state = STT_STATE_VAL_INIT;
	} else {
		voicedata->voicefw_state = 0;
		PRINTFUNC(DLOG_ERROR, "Initialization Fail!<br>Check STT-daemon is running");
	}

	Evas_Object *canvas = elm_object_part_content_get(voicedata->layout_main, "EFFECT_BG");


	is::ui::WInputSttMicEffect *ieffect = new is::ui::WInputSttMicEffect();
	ieffect->SetSttHandle(voicedata->sttmanager->GetSttHandle());

	is::ui::MicEffector *effector = new is::ui::MicEffector(canvas, voicedata->layout_main, *ieffect);
	voicedata->ieffect = ieffect;
	voicedata->effector = effector;

	ieffect->SetProgressBar(voicedata->progressbar);
	ieffect->SetSttHandle(voicedata->sttmanager->GetSttHandle());

	voicedata->mo->Update();

	elm_access_highlight_set(voicedata->mic_button);


	return ECORE_CALLBACK_CANCEL;
}


static Eina_Bool _power_delayed_unlock(void *data){
	PRINTFUNC(DLOG_DEBUG, "POWER Unlock");

	VoiceData *voicedata = (VoiceData *) data;
	device_power_release_lock(POWER_LOCK_DISPLAY);
	voicedata->power_unlock_timer = NULL;

	return ECORE_CALLBACK_CANCEL;
}

void powerUnlock()
{
	PRINTFUNC(DLOG_DEBUG, "POWER Unlock directly.");
	device_power_release_lock(POWER_LOCK_DISPLAY);
}

void powerLock(void *data, bool enable)
{
	int ret = DEVICE_ERROR_NONE;

	VoiceData *voicedata = (VoiceData *) data;

	if(enable) {
		ret = device_power_wakeup(false);
		PRINTFUNC(DLOG_DEBUG, "LCD Wakeup");

		if(ret != DEVICE_ERROR_NONE)
			PRINTFUNC(DLOG_ERROR, "LCD Wakeup ERROR = %d", ret);

		ret = device_power_request_lock(POWER_LOCK_DISPLAY, 0);
		PRINTFUNC(DLOG_DEBUG, "POWER LOCK");
		power_state = enable;

		if(ret != DEVICE_ERROR_NONE)
			PRINTFUNC(DLOG_ERROR, "ERROR = %d", ret);

		if(voicedata->power_unlock_timer != NULL){
			ecore_timer_del(voicedata->power_unlock_timer);
			voicedata->power_unlock_timer = NULL;
		}
	} else {
		PRINTFUNC(DLOG_DEBUG, "POWER Unlock Delayed(5 sec)");

		if(voicedata->power_unlock_timer != NULL){
			ecore_timer_del(voicedata->power_unlock_timer);
			voicedata->power_unlock_timer = NULL;
		}
		voicedata->power_unlock_timer = ecore_timer_add(5.0, _power_delayed_unlock, voicedata);
	}
}


static Eina_Bool hide_guide_text(void *data){
	if(data) {
		VoiceData *voicedata = (VoiceData *) data;

		voicedata->guide_text_timer = NULL;


		PRINTFUNC(DLOG_ERROR, "");
		elm_object_part_text_set(voicedata->layout_main, "elm.text", "");
	}
	return ECORE_CALLBACK_CANCEL;
}

static void set_guide_text(VoiceData *vd, const char* text, bool translatable)
{
	//elm_object_signal_emit(vd->layout_main, "idle,state,text,visible", "elm");

	if(translatable)
		elm_object_domain_translatable_part_text_set(vd->layout_main, "elm.text", PACKAGE, text);
	else
		elm_object_part_text_set(vd->layout_main, "elm.text", text);

	if(!strcmp(text, SK_SPEAK_NOW)){
		if(vd->guide_text_timer == NULL)
			vd->guide_text_timer = ecore_timer_add(2.0, hide_guide_text, vd);
	}
}

/**
 * @brief - function to send the signal to edc
 * 		 to change the animation as per stt state
 * @param - Ug data
 */
void set_animation_state(VoiceData *voicedata)
{
	PRINTFUNC(DLOG_DEBUG, "");

	PRINTFUNC(DLOG_ERROR, "voicedata->state == %s",
		voicedata->state == STT_STATE_VAL_INIT ?
			"STT_STATE_VAL_INIT" :
		voicedata->state == STT_STATE_VAL_LISTENING ?
			"STT_STATE_VAL_LISTENING" :
		voicedata->state == STT_STATE_VAL_PREPARE_LISTENING ?
			"STT_STATE_VAL_PREPARE_LISTENING" :
		voicedata->state == STT_STATE_VAL_PROCESSING ?
			"STT_STATE_VAL_PROCESSING" :
		voicedata->state == STT_STATE_VAL_PREPARE_PROCESSING ?
			"STT_STATE_VAL_PREPARE_PROCESSING" :
		voicedata->state == STT_STATE_VAL_TERMINATING ?
			"STT_STATE_VAL_TERMINATING" : "STT_STATE_VAL_NOT_RECOGNISED");

	if (voicedata->state == STT_STATE_VAL_INIT) {
#if 0
		if (voicedata->sttmanager->GetCurrent() == STT_STATE_READY) {
			set_guide_text(voicedata, "", false);
			//_elm_access_say(voicedata->layout_main, _(SK_INIT));
		} else {
			PRINTFUNC(DLOG_DEBUG, "SK_NETWORK_ERROR [%d]", voicedata->sttmanager->GetCurrent());
			set_guide_text(voicedata, _(SK_NETWORK_ERROR));
			//_elm_access_say(voicedata->layout_main, _(SK_NETWORK_CONNECTION_ERROR));
		}
#endif

		if(voicedata->effector)
			voicedata->effector->Stop(true);

		set_guide_text(voicedata, "");

		PRINTFUNC(DLOG_DEBUG, "%d", voicedata->stt_results.size());
		powerLock((void*)voicedata, false);
	} else if (voicedata->state == STT_STATE_VAL_LISTENING) {
		set_guide_text(voicedata, SK_SPEAK_NOW, true);

		if(voicedata->effector)
			voicedata->effector->Start();

		powerLock((void*)voicedata, true);
	} else if (voicedata->state == STT_STATE_VAL_PROCESSING) {
		set_guide_text(voicedata, "");
		if(voicedata->effector)
			voicedata->effector->Stop();

		stt_feedback(VIBRATION_STOP);
	} else {
		PRINTFUNC(DLOG_DEBUG, "SK_NETWORK_ERROR");
		set_guide_text(voicedata, _(SK_RECOGNITION_FAILED));
		//_elm_access_say(voicedata->layout_main, _(SK_RECOGNITION_FAILED));

		voicedata->state = STT_STATE_VAL_NOT_RECOGNISED;

		if(voicedata->refresh_timer) {
			ecore_timer_del(voicedata->refresh_timer);
		}

		if(voicedata->effector)
			voicedata->effector->Stop(true);

		voicedata->refresh_timer = ecore_timer_add(2.0, _recognition_failure_cb, voicedata);

		//powerLock((void*)voicedata, false);
	}
}

void show_error_message(VoiceData *vd, stt_error_e reason)
{
	if(reason == STT_ERROR_OUT_OF_NETWORK)
	{
		PRINTFUNC(DLOG_DEBUG, "SK_NETWORK_ERROR");

		int ancs_connected = 0;
		int ret = 0;
		ret = vconf_get_int("file/private/weconn/ancs_connected", &ancs_connected);

		if (ret == 0)
			PRINTFUNC(DLOG_ERROR, "ancs connected status : %d", ancs_connected);
		else
			PRINTFUNC(DLOG_ERROR, "vconf for ancs connection ERROR - %d", ret);

		if (ancs_connected) {
			char text[512];
			snprintf(text, sizeof(text), _(SK_NETWORK_ERROR_FOR_IOS), _(SK_SAMSUNG_GEAR));

			show_popup_toast((const char*)text, false);
		} else {
			set_guide_text(vd, _(SK_NETWORK_ERROR));
		}

		vd->state = STT_STATE_VAL_INIT;

	} else if (reason == STT_ERROR_RECORDER_BUSY) {
		PRINTFUNC(DLOG_WARN, "STT is used by another application");
		show_popup_toast(_(SK_STT_BUSY), false);
		vd->state = STT_STATE_VAL_INIT;
	} else {
		PRINTFUNC(DLOG_WARN, "Check error code");
		show_popup_toast(_(SK_STT_BUSY), false);
		vd->state = STT_STATE_VAL_INIT;
	}
}

static Eina_Bool _start_timer_cb(void* data)
{
	if(data) {
		VoiceData *voicedata = (VoiceData *) data;

		try {
			voicedata->state = STT_STATE_VAL_PREPARE_LISTENING;
			voicedata->sttmanager->Start();
		}
		catch (is::stt::SttException &e) {
			PRINTFUNC(DLOG_ERROR, "%s", e.what());

			if (e.GetEcode() == STT_ERROR_OUT_OF_NETWORK)
			{
				PRINTFUNC(DLOG_DEBUG, "SK_NETWORK_ERROR");
				set_guide_text(voicedata, _(SK_NETWORK_ERROR));
				voicedata->state = STT_STATE_VAL_INIT;
			} else if (e.GetEcode() == STT_ERROR_RECORDER_BUSY) {
				PRINTFUNC(DLOG_WARN, "STT is used by another application");
				show_popup_toast(_(SK_STT_BUSY), false);
				voicedata->state = STT_STATE_VAL_INIT;
			} else {
				PRINTFUNC(DLOG_WARN, "Check error code");
				show_popup_toast(_(SK_STT_BUSY), false);
				voicedata->state = STT_STATE_VAL_INIT;
			}
		}
		voicedata->start_timer = NULL;
	}
	return ECORE_CALLBACK_CANCEL;
}

/**
 * @brief - cancel button press callback for cross button
 * @param -
 */

void on_initial_anim_press_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
	PRINTFUNC(NO_PRINT, "");

	VoiceData *vd = (VoiceData *)data;

	int tempVal = vd->sttmanager->GetCurrent();
	if(tempVal == STT_STATE_CREATED) {
		PRINTFUNC(DLOG_WARN, "IGNORE TOUCH event before STT READY. STT is preparing", vd->state);
		return;
	}

	PRINTFUNC(DLOG_DEBUG, "SttManager State : %d", vd->sttmanager->GetCurrent());
	PRINTFUNC(DLOG_DEBUG, "Ui Voice State : %d", vd->state);

	switch(vd->state) {
		case STT_STATE_VAL_INIT:

			PRINTFUNC(DLOG_DEBUG, "%s", "STT_STATE_VAL_INIT");
			get_stt_default_language(vd);
			vd->sttmanager->SetLanguage(std::string(vd->kbd_lang));

			stt_feedback(VIBRATION_START);

			if(vd->start_timer) {
				ecore_timer_del(vd->start_timer);
				vd->start_timer = NULL;
			}

			vd->start_timer = ecore_timer_add(0.0, _start_timer_cb, vd);
			break;

		case STT_STATE_VAL_LISTENING :
			PRINTFUNC(DLOG_DEBUG, "%s", "STT_STATE_VAL_LISTENING");
			try {
				vd->state = STT_STATE_VAL_PREPARE_PROCESSING;
				vd->sttmanager->Stop();

				/**
				 * Cuased touch reponse time, it can be called to stop animator.
				 *
				 */
				set_guide_text(vd, "");

				if(vd->effector)
					vd->effector->Stop();
			}
			catch (is::stt::SttException &e) {
				PRINTFUNC(DLOG_ERROR, "%s", e.what());

				if(e.GetEcode() != STT_ERROR_INVALID_STATE) {
					PRINTFUNC(DLOG_DEBUG, "SK_NETWORK_ERROR");
					set_guide_text(vd, _(SK_RECOGNITION_FAILED));
					vd->state = STT_STATE_VAL_INIT;

					if(vd->effector)
						vd->effector->Stop(true);
				}
			}
			break;

		case STT_STATE_VAL_PROCESSING:
			PRINTFUNC(DLOG_DEBUG, "%s", "STT_STATE_VAL_PROCESSING");
			try {
				//vd->state = STT_STATE_VAL_PREPARE_CANCEL;
				vd->sttmanager->Cancel();
			}
			catch (is::stt::SttException &e) {
				PRINTFUNC(DLOG_ERROR, "%s", e.what());
			}

			break;

		case STT_STATE_VAL_NOT_RECOGNISED:
			PRINTFUNC(DLOG_DEBUG, "%s", "STT_STATE_VAL_NOT_RECOGNISED");
			vd->state = STT_STATE_VAL_INIT ;
			break;
		default:
			PRINTFUNC(DLOG_DEBUG, "default [%d]", vd->state);
			break;
	}
}

static inline void ea_naviframe_back(void *data, Evas_Object *obj, void *event_info)
{
	if(!data) return;

	evas_object_del((Evas_Object *) data);

	g_setting_window = NULL;
	g_setting_naviframe = NULL;

	if(g_evt_key_down)
		ecore_event_handler_del(g_evt_key_down);

	g_evt_key_down = NULL;

	//Hide more option
	if(g_more_option_layout){
		if(eext_more_option_opened_get(g_more_option_layout) == EINA_TRUE) {
			eext_more_option_opened_set(g_more_option_layout, EINA_FALSE);
		}
	}
}

static char *__get_genlist_title_label(void *data, Evas_Object *obj, const char *part)
{
	return strdup(_("WDS_VOICE_OPT_LANGUAGE_ABB"));
}

char *__get_genlist_item_label(void *data, Evas_Object *obj, const char *part)
{
	char text[128] = {0, };

	if(!strcmp(part, "elm.text"))
	{
		if((int)data == 0) {
			return strdup(_("IDS_VC_BODY_AUTOMATIC"));
		} else {
			char *s = NULL;
			if(g_is_n66) {
				s = (char *)disp_lang_array_n66[(int)data];
			} else {
				s = (char *)disp_lang_array[(int)data];
			}

			if(s) {
				char *p = strchr(s, '(');
				if(p) {
					strncpy(text, s, p-s);
				} else {
					strncpy(text, s, strlen(s));
				}
			} else {
				strncpy(text, "", strlen(""));
			}
		}
		return strdup(text);
	} else if (!strcmp(part, "elm.text.1")) {
		if ((int)data == 0) {
			char* value = NULL;
			value = vconf_get_str(VCONFKEY_LANGSET);
			if (NULL == value) {
				PRINTFUNC(DLOG_ERROR, "Fail to get display language");
				return NULL;
			}
			PRINTFUNC(DLOG_DEBUG, "system language (%s)", value);

			char system_lang[6] = {0, };
			strncpy(system_lang, value , 5);
			free(value);

			// confirm whether the system language is supported by stt engine or not.
			// if supported, set the language
			// otherwise, set language to en_US
			if(g_is_n66) {
				return strdup(get_lang_label_n66(system_lang));
			} else {
				return strdup(get_lang_label(system_lang));
			}
		} else {
			char *s = NULL;
			if(g_is_n66) {
				s = (char *)disp_lang_array_n66[(int)data];
			} else {
				s = (char *)disp_lang_array[(int)data];
			}

			if(s) {
				char *p = strchr(s, '(');
				if(p) {
					strncpy(text, p+1, strlen(s)-(p-s)-2);
				} else {
					strncpy(text, s, strlen(s));
				}
			} else {
				strncpy(text, "", strlen(""));
			}
			return strdup(text);
		}
	}
	return NULL;
}

static Evas_Object *__get_genlist_item_content(void *data, Evas_Object *obj, const char *part)
{
	int index = (int)data;
	Evas_Object * content = NULL;

	if (!strcmp(part, "elm.icon")) {
		content = elm_radio_add(obj);
		elm_radio_state_value_set(content, index);
		elm_radio_group_add(content, radio_gp);
		evas_object_size_hint_weight_set(content, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_propagate_events_set(content, EINA_TRUE);
		evas_object_size_hint_align_set(content, EVAS_HINT_FILL, EVAS_HINT_FILL);
		elm_object_style_set(content, "list");

		if (elm_config_access_get())
			elm_access_object_unregister(content);
	}
	return content;
}

static int get_language_value()
{
#if 1
	int lang = 0, ret = 0;

	ret = preference_get_int(PREFERENCE_ISE_STT_LANGUAGE, &lang);
	if(PREFERENCE_ERROR_NONE != ret){
		PRINTFUNC(DLOG_ERROR, "preference_get_int error!(%d)", ret);
		preference_set_int(PREFERENCE_ISE_STT_LANGUAGE, (int)2); //auto
		lang = 2;
	}
#else
	int lang = 0, ret = 0;

	ret = vconf_get_int(VCONFKEY_ISE_STT_LANGUAGE, &lang);
	if (ret !=0) {
		PRINTFUNC(DLOG_ERROR, "Vconf_get_int error!(%d)", ret);
	}
#endif
	if(g_is_n66) {
		if(lang < 0 || lang > 6) {
			PRINTFUNC(DLOG_WARN, "vconf lang orig(%d) to be 0", lang);
			lang = 0;
		}
		PRINTFUNC(DLOG_DEBUG, "n66 current language value for stt (%s).", disp_lang_array_n66[lang]);
	} else {
		if(lang < 0 || lang > 12) {
			PRINTFUNC(DLOG_WARN, "vconf lang orig(%d) to be 0", lang);
			lang = 0;
		}
		PRINTFUNC(DLOG_DEBUG, "current language value for stt (%s).", disp_lang_array[lang]);
	}

	return lang;
}

static void set_language_value(int type)
{
	// Add implementation to store language type.
	int ret = 0;

#if 1
	ret = preference_set_int(PREFERENCE_ISE_STT_LANGUAGE, (int)type);
	if(PREFERENCE_ERROR_NONE != ret){
		PRINTFUNC(DLOG_ERROR, "preference_set_int error!(%d)", ret);
	}

#else
	ret = vconf_set_int(VCONFKEY_ISE_STT_LANGUAGE, (int) type);
	if (ret != 0) {
		PRINTFUNC(DLOG_ERROR, "Vconf_set_int error!(%d)", ret);
		return;
	}
#endif
	PRINTFUNC(DLOG_DEBUG, "language type (%d)", type);

	// Update string
}

static void get_stt_default_language(VoiceData *my_voicedata)
{
	if(!my_voicedata) {
		PRINTFUNC(DLOG_ERROR, "my_voicedata NULL");
		return;
	}

	if(my_voicedata->kbd_lang) {
		free(my_voicedata->kbd_lang);
		my_voicedata->kbd_lang = NULL;
	}


	if(g_is_n66) {
		STT_VOICE_LANGUAGE_N66_I stt_lang;
		stt_lang = (STT_VOICE_LANGUAGE_N66_I)get_language_value();

		PRINTFUNC(DLOG_DEBUG, "language type (%d)", stt_lang);

		switch(stt_lang) {
		case STT_VOICE_N66_AUTO :
		{
			stt_get_default_language(my_voicedata->voicefw_handle, &my_voicedata->kbd_lang);

			// get system display language
			char* value = NULL;
			value = vconf_get_str(VCONFKEY_LANGSET);
			if (NULL == value) {
				PRINTFUNC(DLOG_ERROR, "Fail to get display language");
				return;
			}
			PRINTFUNC(DLOG_DEBUG, "system language (%s)", value);

			char system_lang[6] = {0, };
			strncpy(system_lang, value , 5);
			free(value);

			// confirm whether the system language is supported by stt engine or not.
			// if supported, set the language
			// otherwise, set language to en_US
			if(is_lang_supported_by_stt(system_lang) == TRUE) {
				my_voicedata->kbd_lang = strdup(system_lang);
				PRINTFUNC(DLOG_DEBUG, "Set auto language (%s)", system_lang);
			} else {
				my_voicedata->kbd_lang = strdup("en_US");
				PRINTFUNC(DLOG_DEBUG, "System language is not supported by STT (%s), en_US will be set", system_lang);
			}
		}
		break;
		case STT_VOICE_N66_EN_US :
		case STT_VOICE_N66_ES_US :
		case STT_VOICE_N66_FR_FR :
		case STT_VOICE_N66_JA_JP :
		case STT_VOICE_N66_KO_KR :
		case STT_VOICE_N66_ZH_CN :
		{
			my_voicedata->kbd_lang = strdup(supported_language_n66[stt_lang]);
		}
		break;
		default :
			my_voicedata->kbd_lang = strdup("en_US");
			break;
		}
	} else {
		STT_VOICE_LANGUAGE_I stt_lang;
		stt_lang = (STT_VOICE_LANGUAGE_I)get_language_value();

		PRINTFUNC(DLOG_DEBUG, "language type (%d)", stt_lang);

		switch(stt_lang) {
		case STT_VOICE_AUTO :
		{
			stt_get_default_language(my_voicedata->voicefw_handle, &my_voicedata->kbd_lang);

			// get system display language
			char* value = NULL;
			value = vconf_get_str(VCONFKEY_LANGSET);
			if (NULL == value) {
				PRINTFUNC(DLOG_ERROR, "Fail to get display language");
				return;
			}
			PRINTFUNC(DLOG_DEBUG, "system language (%s)", value);

			char system_lang[6] = {0, };
			strncpy(system_lang, value , 5);
			free(value);

			// confirm whether the system language is supported by stt engine or not.
			// if supported, set the language
			// otherwise, set language to en_US
			if(is_lang_supported_by_stt(system_lang) == TRUE) {
				my_voicedata->kbd_lang = strdup(system_lang);
				PRINTFUNC(DLOG_DEBUG, "Set auto language (%s)", system_lang);
			} else {
				my_voicedata->kbd_lang = strdup("en_US");
				PRINTFUNC(DLOG_DEBUG, "System language is not supported by STT (%s), en_US will be set", system_lang);
			}
		}
		break;
		case STT_VOICE_DE_DE :
		case STT_VOICE_EN_GB :
		case STT_VOICE_EN_US :
		case STT_VOICE_ES_ES :
		case STT_VOICE_ES_US :
		case STT_VOICE_FR_FR :
		case STT_VOICE_IT_IT :
		case STT_VOICE_JA_JP :
		case STT_VOICE_KO_KR :
		case STT_VOICE_RU_RU :
		case STT_VOICE_ZH_CN :
		case STT_VOICE_PT_BR :
		{
			my_voicedata->kbd_lang = strdup(supported_language[stt_lang]);
		}
		break;
		default :
			my_voicedata->kbd_lang = strdup("en_US");
			break;
		}
	}

	PRINTFUNC(DLOG_DEBUG, "stt language (%s)", my_voicedata->kbd_lang);
}

static Eina_Bool close_setting_window_idler_cb(void *data)
{
	if(g_setting_window && g_setting_naviframe) {
		ea_naviframe_back(g_setting_window, g_setting_naviframe, NULL);
	}

	return ECORE_CALLBACK_CANCEL;
}

static void language_set_genlist_radio_cb(void *data, Evas_Object *obj, void *event_info)
{
	PRINTFUNC(DLOG_DEBUG, "");
	if(!data) return;

	int index = 0;
	Evas_Object *genlist = (Evas_Object *) data;

	Elm_Object_Item * item = (Elm_Object_Item *) event_info;
	if (item) {
		elm_genlist_item_selected_set(item, 0);
		index = (int)elm_object_item_data_get(item);
		elm_genlist_item_update(item);
	}
	set_language_value(index);

	elm_radio_value_set(radio_gp, index);
	ecore_timer_add(0.3, close_setting_window_idler_cb, NULL);
}

static void language_changed_cb2(void *data, Evas_Object *obj, const char *emission, const char *source)
{
	if(!obj) return;
	elm_genlist_realized_items_update(obj);
}

Eina_Bool _ise_keydown_cb(void *data, int type, void *event)
{
	PRINTFUNC(DLOG_DEBUG, "");
	if(type == ECORE_EVENT_KEY_DOWN && g_setting_window && g_setting_naviframe) {
		PRINTFUNC(DLOG_DEBUG, "window will be deleted.");
		ea_naviframe_back(g_setting_window, g_setting_naviframe, NULL);
	}

	return ECORE_CALLBACK_DONE;
}

char *_language_list_access_info_cb(void *data, Evas_Object *obj)
{
	PRINTFUNC(DLOG_DEBUG, "%s", __func__);

	int value;
	int index;
	std::string text = std::string(gettext(TTS_RADIO_BUTTON));
	const Elm_Object_Item *item = (Elm_Object_Item *)data;

	Evas_Object* radio = elm_object_item_part_content_get(item, "elm.icon");

	index = elm_genlist_item_index_get(item);

	if(index == 1) // title
		return NULL;


	value = elm_radio_value_get(radio);
	//PRINTFUNC(DLOG_DEBUG,"index=%d value = %d", index, value);
	if(index == value + 2 ){
		text = text + std::string(" ") + std::string(gettext(TTS_SELECTED));
	} else {
		text = text + std::string(" ") + std::string(gettext(TTS_NOT_SELECTED));
	}

	return strdup(text.c_str());
}

static void _language_list_item_realized(void *data, Evas_Object *obj, void *event_info) //called when list scrolled
{
	PRINTFUNC(DLOG_DEBUG, "%s", __func__);
	if (elm_config_access_get()) {
		Elm_Object_Item *item = (Elm_Object_Item *)event_info;
		Evas_Object *item_access = elm_object_item_access_object_get(item);

		elm_access_info_cb_set(item_access, ELM_ACCESS_CONTEXT_INFO, _language_list_access_info_cb, (void*)item);
	}
}

static Evas_Object *create_language_list(Evas_Object *parent)
{
	if(!parent) return NULL;

	Evas_Object *genlist = elm_genlist_add(parent);
	if (!genlist) return NULL;

	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	elm_genlist_homogeneous_set(genlist, EINA_TRUE);
//	uxt_genlist_set_bottom_margin_enabled(genlist, EINA_TRUE);

	Evas_Object *circle_language_genlist = eext_circle_object_genlist_add(genlist, NULL);
	eext_circle_object_genlist_scroller_policy_set(circle_language_genlist, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
	evas_object_resize(circle_language_genlist, 360, 360);
	evas_object_show(circle_language_genlist);
	eext_rotary_object_event_activated_set(circle_language_genlist, EINA_TRUE);

	int i = 0;
	int lang_val = 0;
	Elm_Object_Item * item = NULL;
	Elm_Object_Item * item_title = NULL;

	evas_object_size_hint_weight_set(genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(genlist, EVAS_HINT_FILL, EVAS_HINT_FILL);

	radio_gp = elm_radio_add(genlist);
	elm_radio_state_value_set(radio_gp, -1);

	lang_val = get_language_value();

	//Title
	itc_title.item_style = "title";
	itc_title.func.text_get = __get_genlist_title_label;
	itc_title.func.content_get = NULL;

	item_title = elm_genlist_item_append(genlist, &itc_title, (void *)-1, NULL, ELM_GENLIST_ITEM_GROUP, NULL, genlist);

	// 2 line text
	itc_2text.item_style = "2text.1icon.1/sub1.multiline";
	itc_2text.func.text_get = __get_genlist_item_label;
	itc_2text.func.content_get = __get_genlist_item_content;


	item = item_append(genlist, itc_2text, (void *)0, language_set_genlist_radio_cb, genlist); // AUTO

	if(lang_val == i) {
		PRINTFUNC(DLOG_DEBUG, "%d item is choiced.", i);
		elm_genlist_item_show(item, ELM_GENLIST_ITEM_SCROLLTO_MIDDLE);
	}

	if ( item == NULL ) {
		PRINTFUNC(DLOG_DEBUG, "elm_genlist_item_append was failed");
	}

	// 1 line text
	itc_1text.item_style = "1text.1icon.1";
	itc_1text.func.text_get = __get_genlist_item_label;
	itc_1text.func.content_get = __get_genlist_item_content;

	if(g_is_n66) {
		for (i = 1; i < 7; i++)
		{
			char *s = (char *)disp_lang_array_n66[i];

			if(strchr(s, '(')){
				item = item_append(genlist, itc_2text, i, language_set_genlist_radio_cb, genlist);
			} else {
				item = item_append(genlist, itc_1text, i, language_set_genlist_radio_cb, genlist);
			}

			if(lang_val == i) {
				PRINTFUNC(DLOG_DEBUG, "%d item is choiced.", i);
				elm_genlist_item_show(item, ELM_GENLIST_ITEM_SCROLLTO_MIDDLE);
			}

			if ( item == NULL ) {
				PRINTFUNC(DLOG_DEBUG, "elm_genlist_item_append was failed");
				break;
			}
		}
	} else {
		for (i = 1; i < 13; i++)
		{
			char *s = (char *)disp_lang_array[i];

			if(strchr(s, '(')){
				item = item_append(genlist, itc_2text, i, language_set_genlist_radio_cb, genlist);
			} else {
				item = item_append(genlist, itc_1text, i, language_set_genlist_radio_cb, genlist);
			}

			if(lang_val == i) {
				PRINTFUNC(DLOG_DEBUG, "%d item is choiced.", i);
				elm_genlist_item_show(item, ELM_GENLIST_ITEM_SCROLLTO_MIDDLE);
			}

			if ( item == NULL ) {
				PRINTFUNC(DLOG_DEBUG, "elm_genlist_item_append was failed");
				break;
			}
		}
	}

	elm_radio_value_set(radio_gp, lang_val);

	elm_object_signal_callback_add(genlist, "elm,system,language,change", "elm", language_changed_cb2, NULL);
//	eext_rotary_event_callback_set(genlist, _language_list_rotary_cb, NULL);
	evas_object_smart_callback_add(genlist, "realized", _language_list_item_realized, NULL);


	g_evt_key_down = ecore_event_handler_add(ECORE_EVENT_KEY_DOWN, _ise_keydown_cb, NULL);
	return genlist;
}

void create_setting_window(Evas_Object *more_option_layout)
{
	Evas_Coord w, h;

	Evas_Object *window = NULL;
	Evas_Object *layout = NULL;
	Evas_Object *genlist = NULL;
	Evas_Object *naviframe = NULL;

	Elm_Object_Item *item = NULL;

	const char *str = "mobile";

	/**
	 * Create full size window
	 *
	 */
	window = elm_win_add(NULL, "voice-input-setting", ELM_WIN_BASIC);
	if(!window) {
		PRINTFUNC(DLOG_DEBUG, "it's fail to create window.");
		return;
	}

	elm_win_title_set(window, "voice-input-setting");
//	ecore_x_window_size_get(ecore_x_window_root_first_get(), &w, &h);
	elm_win_borderless_set(window, EINA_TRUE);
	evas_object_resize(window, w, h);

	elm_win_indicator_mode_set(window, ELM_WIN_INDICATOR_HIDE);
	elm_win_profiles_set(window, &str, 1);

	/**
	 * Set window theme
	 *
	 */
//	ea_theme_style_set(EA_THEME_STYLE_DEFAULT);
//	ea_theme_changeable_ui_enabled_set(EINA_TRUE);

	/**
	 * Create layout for language list
	 *
	 */
	layout = elm_layout_add(window);
	if (layout) {
		elm_layout_theme_set(layout, "layout", "application", "default");
		evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_show(layout);
		elm_win_resize_object_add(window, layout);
	}

	/**
	 * Set Window rotation
	 *
	 */
	if (elm_win_wm_rotation_supported_get(window)) {
		const int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(window, rots, 4);
	}

	/**
	 * Activate and show window
	 *
	 */
	elm_win_activate(window);
	evas_object_show(window);

	/**
	 * Naviframe
	 *
	 */
	naviframe = elm_naviframe_add(layout);
	if (naviframe == NULL){
		PRINTFUNC(DLOG_DEBUG, "create navi_frame failed");
	}

//	ea_object_event_callback_add(naviframe, EA_CALLBACK_BACK, ea_naviframe_back, window);
	elm_object_part_content_set(layout, "elm.swallow.content", naviframe);

	/**
	 * Push language list
	 *
	 */
	genlist = create_language_list(naviframe);

	//item = elm_naviframe_item_push(naviframe, "IDS_VC_HEADER_VOICE_INPUT_LANGUAGE", NULL, NULL, genlist, NULL);
	item = elm_naviframe_item_push(naviframe, NULL, NULL, NULL, genlist, "empty");
	//elm_object_item_domain_text_translatable_set(item, PACKAGE, EINA_TRUE);

	g_setting_window = window;
	g_setting_naviframe = naviframe;
	g_more_option_layout = more_option_layout;
}

static void language_changed_cb(void *data, Evas_Object * obj, void *event_info)
{
	PRINTFUNC(DLOG_DEBUG, "");

	if(!obj) return;
	if(!data) return;
}

static void get_text_part_width(Evas_Object *window, const char *text, Evas_Coord *calculated_width)
{
	if(!window) return;
	if(!text) return;
	if(!calculated_width) return;

	Evas_Coord width, height;

	char *strbuf = NULL;
	Evas_Object *tb = NULL;
	Evas_Textblock_Style *st = NULL;
	Evas_Textblock_Cursor *cur = NULL;

	tb = evas_object_textblock_add(evas_object_evas_get(window));
	evas_object_textblock_legacy_newline_set(tb, EINA_FALSE);

	st = evas_textblock_style_new();
	evas_textblock_style_set(st, VIRTUAL_TEXT_AREA_FONT_STYLE);
	evas_object_textblock_style_set(tb, st);

	cur = evas_object_textblock_cursor_new(tb);
	strbuf = elm_entry_utf8_to_markup(text);
	evas_object_resize(tb, 360, 47);

	evas_object_textblock_text_markup_set(tb, strbuf);
	evas_textblock_cursor_format_prepend(cur, "+ wrap=mixed");
	evas_object_textblock_size_formatted_get(tb, &width, &height);
	evas_object_resize(tb, width, 47);

	*calculated_width = width;

	if(strbuf) free(strbuf);
	if(tb) evas_object_del(tb);
	if(st) evas_textblock_style_free(st);
	if(cur) evas_textblock_cursor_free(cur);
}

void _stt_lang_changed_cb(keynode_t *key, void* data)
{
	PRINTFUNC(DLOG_DEBUG, "");
	if(!data) return;

	VoiceData *vd = (VoiceData *) data;
	get_stt_default_language(vd);
	vd->sttmanager->SetLanguage(std::string(vd->kbd_lang));
	vd->mo->Update();

	return;
}

static void __done_key_cb(void *data, Evas_Object *obj, void *event_info )
{
	PRINTFUNC(DLOG_DEBUG, "");

	if(!obj) return;
	if(!data) return;

	VoiceData *voicedata = (VoiceData *)data;

	elm_naviframe_item_pop(voicedata->naviframe);
}

static void __stt_detailed_entry_input_panel_event_cb(void *data, Ecore_IMF_Context *imf_context,  int value)
{
	if(!data) return;

	VoiceData *voicedata = (VoiceData *)data;

	switch (value) {
	case ECORE_IMF_INPUT_PANEL_STATE_HIDE: // 1
		elm_naviframe_item_pop(voicedata->naviframe);
		break;
	case ECORE_IMF_INPUT_PANEL_STATE_WILL_SHOW: // 2
		PRINTFUNC(DLOG_DEBUG, "keypad state will show.");
		break;
	case ECORE_IMF_INPUT_PANEL_STATE_SHOW: // 0
		PRINTFUNC(DLOG_DEBUG, "keypad state show.");
		break;
	}
}

static void __stt_detailed_entry_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	if (obj) {
		Ecore_IMF_Context *imf_context = (Ecore_IMF_Context *)elm_entry_imf_context_get(obj);
		ecore_imf_context_input_panel_event_callback_del(imf_context, ECORE_IMF_INPUT_PANEL_STATE_EVENT, __stt_detailed_entry_input_panel_event_cb);
		PRINTFUNC(DLOG_DEBUG, "input_panel_event_cb is deleted.");
	}
}

static Evas_Object *create_text_detiled_view(Evas_Object *parent)
{
	string edj_path = get_resource_path();
	edj_path = edj_path + STT_EDJ_FILE;

	//layout
	Evas_Object *layout = elm_layout_add(parent);
	elm_layout_file_set(layout, edj_path.c_str(), "entry_focused_layout");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(layout);

	//entry
	Evas_Object *entry = elm_entry_add(parent);
	evas_object_size_hint_weight_set(entry, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(entry, EVAS_HINT_FILL, EVAS_HINT_FILL);
//	ea_entry_selection_back_event_allow_set(entry, EINA_TRUE);
	elm_entry_context_menu_disabled_set(entry, EINA_TRUE);
	elm_entry_select_allow_set(entry, EINA_FALSE);
//	elm_entry_cursor_handler_disabled_set(entry, EINA_TRUE);
	elm_entry_single_line_set(entry, EINA_TRUE);
	elm_entry_scrollable_set(entry, EINA_TRUE);
	elm_entry_input_panel_return_key_type_set(entry, ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE);

	evas_object_show(entry);

	elm_object_part_content_set(layout, "contents", entry);
	return layout;
}

static Eina_Bool _idler_cb2(void *data)
{
	VoiceData *voicedata = (VoiceData *)data;

//	uxt_scroller_set_auto_scroll_enabled(voicedata->scroller, EINA_FALSE);

	return ECORE_CALLBACK_CANCEL;
}

static Eina_Bool __stt_entry_detailed_view_pop_cb(void *data, Elm_Object_Item *it)
{
	PRINTFUNC(DLOG_DEBUG, "");

	if(!data) return EINA_FALSE;

	VoiceData *voicedata = (VoiceData *)data;

	Evas_Object *main_entry = (Evas_Object *)voicedata->main_entry;
	Evas_Object *detailed_layout = elm_object_item_part_content_get(it, "elm.swallow.content");
	Evas_Object *detailed_entry = elm_object_part_content_get(detailed_layout, "contents");

	const char *str = NULL;

	int pos = 0;
	pos = elm_entry_cursor_pos_get(detailed_entry);
	PRINTFUNC(DLOG_DEBUG, "pos = %d", pos);

	str =  elm_entry_entry_get(detailed_entry);

	voicedata->stt_results.clear();
	voicedata->stt_results.push_back(str);

//	uxt_scroller_set_auto_scroll_enabled(voicedata->scroller, EINA_TRUE);
	elm_entry_entry_set(main_entry, str);
	elm_entry_cursor_pos_set(main_entry, pos);

	int x, y, w, h;
	elm_entry_cursor_geometry_get(main_entry, &x, &y, &w, &h);
	PRINTFUNC(DLOG_DEBUG, "%d %d %d %d", x, y, w, h);

	elm_scroller_region_bring_in(voicedata->scroller, 0, y+h, 300, 168);

	ecore_idler_add(_idler_cb2, voicedata);

	return EINA_TRUE;
}

static void _stt_entry_clicked_cb(void *data, Evas_Object * obj, void *event_info)
{
	PRINTFUNC(DLOG_DEBUG, "");

	if(!obj) return;
	if(!data) return;

	VoiceData *voicedata = (VoiceData *)data;

	char *str = NULL;

	int pos = 0;
	pos = elm_entry_cursor_pos_get(obj);
	PRINTFUNC(DLOG_DEBUG, "pos = %d", pos);

	str =  elm_entry_markup_to_utf8(elm_entry_entry_get(obj));

	if (str && strlen(str) > 0) {
		try {
			if (voicedata->sttmanager->GetCurrent() == STT_STATE_RECORDING ||
				voicedata->sttmanager->GetCurrent() == STT_STATE_PROCESSING) {
				voicedata->sttmanager->Cancel();
				PRINTFUNC(DLOG_DEBUG, "stt entry clicked callback during STT recording and processing");
				//return;
			}
		}
		catch(is::stt::SttException &e) {
			PRINTFUNC(DLOG_ERROR, "%s", e.what());
		}


		Evas_Object *ly_detailed_view = create_text_detiled_view(voicedata->naviframe);
		// Set text
		Evas_Object *detailed_entry = elm_object_part_content_get(ly_detailed_view, "contents");
		elm_entry_entry_set(detailed_entry, str);
		elm_entry_cursor_pos_set(detailed_entry, pos);
		evas_object_smart_callback_add(detailed_entry, "activated", __done_key_cb, voicedata); // Register callback for Done key

		Ecore_IMF_Context *imf_context = (Ecore_IMF_Context *)elm_entry_imf_context_get(detailed_entry);
		ecore_imf_context_input_panel_event_callback_add(imf_context, ECORE_IMF_INPUT_PANEL_STATE_EVENT, __stt_detailed_entry_input_panel_event_cb, voicedata);
		evas_object_event_callback_add(detailed_entry, EVAS_CALLBACK_DEL, __stt_detailed_entry_del_cb, NULL);

		// Navi Push
		Elm_Object_Item *navi_it = elm_naviframe_item_push(voicedata->naviframe, NULL, NULL, NULL, ly_detailed_view, NULL);
		elm_naviframe_item_pop_cb_set(navi_it, __stt_entry_detailed_view_pop_cb, (void *)voicedata);
		elm_naviframe_item_title_enabled_set(navi_it, EINA_FALSE, EINA_FALSE);
		free(str);
	}

	return;
}


void activate_circle_scroller_for_stt_textbox(void* data, Eina_Bool bActivate)
{
	PRINTFUNC(DLOG_DEBUG, "");

	if(!data) return;
	VoiceData *voicedata = (VoiceData *)data;

	Evas_Object *scroller = NULL;
	Evas_Object *circle_scroller = NULL;

	scroller = elm_layout_content_get((Evas_Object *)voicedata->layout_main, "text_area");
	circle_scroller = (Evas_Object *) evas_object_data_get(scroller, "circle");
	eext_rotary_object_event_activated_set(circle_scroller, bActivate);
}

static Evas_Object *create_textblock(void* data)
{
	if(!data) return NULL;

	VoiceData *voicedata = (VoiceData *)data;

	Evas_Object *scroller = NULL;
	Evas_Object *box = NULL;
	Evas_Object *inner_layout = NULL;
	Evas_Object *entry = NULL;
	Evas_Object *tb = NULL;
	Evas_Object *circle_scroller = NULL;

	string edj_path = get_resource_path();
	edj_path = edj_path + STT_EDJ_FILE;

	scroller = elm_scroller_add(voicedata->layout_main);

	elm_scroller_loop_set(scroller, EINA_FALSE, EINA_FALSE);

	evas_object_size_hint_align_set(scroller, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(scroller, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

//	elm_scroller_page_size_set(scroller, 0, 50);
//	elm_scroller_page_scroll_limit_set(scroller, 0, 1);

	elm_object_scroll_lock_x_set(scroller, EINA_TRUE);

//	uxt_scroller_set_auto_scroll_enabled(scroller, EINA_FALSE);

	circle_scroller = eext_circle_object_scroller_add(scroller, app_data->circle_surface);
	eext_circle_object_scroller_policy_set(circle_scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
	eext_rotary_object_event_activated_set(circle_scroller, EINA_TRUE);

	box = elm_box_add(scroller);

	inner_layout = elm_layout_add(scroller);
	elm_layout_file_set(inner_layout, edj_path.c_str(), "layout_textblock");
	evas_object_size_hint_weight_set(inner_layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	elm_object_signal_callback_add(inner_layout, "do_scroll", "entry",
		[](void *data, Evas_Object *obj, const char* signal, const char* e)
		{
			_bring_in_cb((void *) data);
		}, (void *) scroller);

	entry = elm_entry_add(inner_layout);
	voicedata->main_entry = entry;
	elm_entry_editable_set(entry, EINA_FALSE);

#define FORMAT_TEXT_AREA_FONT_STYLE \
	"DEFAULT='font=Tizen:style=Regular font_size=32 color=#%02x%02x%02x%02x text_class=tizen wrap=mixed align=center' newline='br' b='+ font=Tizen:style=Bold'link='+ underline=on underline_color=#%02x%02x%02x%02x'"
	int a, r, g, b;
	char customStyle[512];
//	ea_theme_color_get("AT02112", &r, &g, &b, &a, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	snprintf(customStyle, 512, FORMAT_TEXT_AREA_FONT_STYLE, r, g, b, a, r, g, b, a);

	elm_entry_text_style_user_push(entry, customStyle);

	evas_object_smart_callback_add(entry, "clicked", _stt_entry_clicked_cb, voicedata);

	elm_object_part_content_set(inner_layout, "elm.swallow.content", entry);

	elm_box_pack_end(box, inner_layout);

	elm_object_content_set(scroller, box);
	evas_object_data_set(scroller, "inner_layout", (void *) inner_layout);
	evas_object_data_set(scroller, "circle", (void *) circle_scroller);
	evas_object_show(scroller);
	evas_object_show(inner_layout);

	return scroller;
}

static Evas_Object *create_progressbar(Evas_Object *parent)
{
	Evas_Object *progressbar = NULL;

	progressbar = elm_progressbar_add(parent);
	elm_object_style_set(progressbar, "voice_input/process/small");
	//elm_progressbar_pulse(progressbar, EINA_TRUE);

	evas_object_size_hint_align_set(progressbar, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(progressbar, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

//	ea_theme_object_color_replace(progressbar, "B065L6", "AO012");

	evas_object_show(progressbar);

	return progressbar;
}

static Evas_Object *create_fullview(Evas_Object *parent, VoiceData *r_voicedata)
{
	PRINTFUNC(NO_PRINT, "");

	Evas_Object *more_option_layout = NULL;
	Evas_Object *layout_main = NULL;
	Evas_Object *lang_btn_box = NULL;
	Evas_Object *lang_btn = NULL;
	Evas_Object *to = NULL;
	Evas_Object *ao = NULL;

	char *lang_type_str = NULL;

	int lang_val = 0;
	int is_screen_reader_on = FALSE;

	VoiceData *voicedata = r_voicedata;

	/**
	 * add app_default_layout
	 *
	 */
	MoreOption* mo = new MoreOption(parent, voicedata);
	mo->Create();

	voicedata->mo = mo;


	layout_main = elm_layout_add(mo->getMoreOptionLayout());

	if (!layout_main)
		return NULL;

	voicedata->layout_main = layout_main;

	string edj_path = get_resource_path();
	edj_path = edj_path + STT_EDJ_FILE;

	if (vconf_get_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_TTS, &is_screen_reader_on) == -1) {
		PRINTFUNC(DLOG_ERROR, "Cannot read value of screen reader from vconf");
	}

	evas_object_size_hint_weight_set(layout_main, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(layout_main, EVAS_HINT_FILL, EVAS_HINT_FILL);

	elm_layout_file_set(layout_main, edj_path.c_str(), "mic_control");

	//elm_object_content_set(parent, layout_main);

	//Canvas for Cairo effect
	Evas_Object *canvas = evas_object_image_filled_add(evas_object_evas_get(layout_main));
//	evas_object_image_alpha_set(canvas, EINA_TRUE);
	evas_object_size_hint_align_set(canvas, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(canvas, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_move(canvas, 0, 0);

	Evas_Coord CanvasW, CanvasH;
	CanvasW = 360;
	CanvasH = 74;
	evas_object_image_size_set(canvas, CanvasW, CanvasH);
	evas_object_resize(canvas, CanvasW, CanvasH);
	evas_object_size_hint_min_set(canvas, CanvasW, CanvasH);
	evas_object_show(canvas);

	elm_object_part_content_set(layout_main, "EFFECT_BG", canvas);


	// MIC Button
	Evas_Object *m_mic_button = elm_button_add(layout_main);
	elm_object_style_set(m_mic_button, "vic/micbutton");
	elm_object_part_content_set(layout_main, "MIC", m_mic_button);
	if (elm_config_access_get()){
		elm_access_info_cb_set(m_mic_button, ELM_ACCESS_INFO, _mic_access_info_cb, NULL);
		elm_access_info_cb_set(m_mic_button, ELM_ACCESS_TYPE, _mic_access_info_cb, NULL);
		elm_access_info_cb_set(m_mic_button, ELM_ACCESS_STATE, _mic_access_info_cb, NULL);
		evas_object_smart_callback_add(m_mic_button, "access,highlighted", _mic_highlighted_cb, voicedata);
	}

	elm_object_disabled_set(m_mic_button, EINA_TRUE);
	evas_object_smart_callback_add(m_mic_button, "clicked", on_mic_button_press_cb, (void *) voicedata);

	voicedata->mic_button = m_mic_button;

	if(voicedata->btn_disabling_timer == NULL){
		voicedata->btn_disabling_timer = ecore_timer_add(2.0, _mic_button_enable_cb, voicedata);
	}

//	ecore_idler_add(_idler_cb, voicedata);
	ecore_timer_add(0.6, _idler_cb, voicedata);

	// Confirm panel layout
	Evas_Object *panel_layout;
	panel_layout = elm_layout_add(layout_main);
	elm_layout_theme_set(panel_layout, "layout", "drawer", "panel");
	evas_object_show(panel_layout);

	// Confirm panel
	Evas_Object *panel;
	panel = elm_panel_add(panel_layout);
	elm_panel_orient_set(panel, ELM_PANEL_ORIENT_LEFT);
	elm_layout_theme_set(panel, "panel", "left_confirm", "default");
	if(app_data->source_app_control){
		int ret;
		char *app_id = NULL;
		ret = app_control_get_extra_data(app_data->source_app_control, "selector_keyboard_app_id", &app_id);
		if (ret == APP_CONTROL_ERROR_NONE) {
			PRINTFUNC(DLOG_DEBUG, "app_id = %s", app_id);
		}

		if(app_id){
			if(!strcmp(app_id, "com.samsung.message.appcontrol.compose")
				|| !strcmp(app_id, "com.samsung.wemail-send")){
				elm_layout_theme_set(panel, "panel", "left_sending", "default");
			}
			free(app_id);
		}
	}

	elm_layout_signal_callback_add(panel, "cue,clicked", "elm", _panel_cue_clicked_cb, (void *) voicedata);
	evas_object_show(panel);

	elm_object_part_content_set(panel_layout, "elm.swallow.right", panel);
	elm_object_part_content_set(layout_main, "left_panel_area", panel_layout);

	elm_object_signal_emit(panel, "elm,state,disabled", "elm");
	elm_panel_toggle(panel);


	// Textblock
	Evas_Object *scroller = create_textblock(voicedata);
	elm_object_part_content_set(layout_main, "text_area", scroller);
	voicedata->scroller = scroller;

	// Progress Bar
	Evas_Object *progress_bar = create_progressbar(layout_main);
	elm_object_part_content_set(layout_main, "PROGRESS_BAR", progress_bar);
	voicedata->progressbar = progress_bar;

	// add callback
	elm_object_signal_callback_add(layout_main, "idle,state,pulse,visible", "", on_initial_anim_press_cb, voicedata);

	mo->SetContentLayout(layout_main);

	//accesbility chaining
	if (elm_config_access_get()) {
		//right cue
		Evas_Object *cue_access_right = NULL;

		Evas_Object *panel_right = elm_object_part_content_get(mo->getMoreOptionLayout(), "elm.swallow.right");
		if (!panel_right) {
		    PRINTFUNC(DLOG_DEBUG, "panel_right == NULL");
		}
		Evas_Object *cue_right = (Evas_Object *)edje_object_part_object_get(elm_layout_edje_get(panel_right), "cue.event");
		if (!cue_right) {
		    PRINTFUNC(DLOG_DEBUG, "cue_right == NULL");
		}
		Evas_Object *_access_right = elm_access_object_get(cue_right);
		if (!_access_right){
		    PRINTFUNC(DLOG_DEBUG, "_access_right == NULL");
		}
		cue_access_right = _access_right;

		//left cue
		Evas_Object *cue_access_left = NULL;
		Evas_Object *panel_layout = elm_layout_content_get(layout_main, "left_panel_area");
		Evas_Object *panel_left = elm_layout_content_get(panel_layout, "elm.swallow.right");
		if (!panel_left) {
		    PRINTFUNC(DLOG_DEBUG, "panel_left == NULL");
		}
		Evas_Object *cue_left = (Evas_Object *)edje_object_part_object_get(elm_layout_edje_get(panel_left), "cue.event");
		if (!cue_left) {
		    PRINTFUNC(DLOG_DEBUG, "cue_left == NULL");
		}
		Evas_Object *_access_left = elm_access_object_register(cue_left, panel_left);
		if (!_access_left){
		    PRINTFUNC(DLOG_DEBUG, "_access_left == NULL");
		}
		elm_access_info_cb_set(_access_left, ELM_ACCESS_INFO, _left_cue_access_info_cb, panel_left);
		elm_access_activate_cb_set(_access_left, _left_cue_access_activate_cb, panel_left);

		cue_access_left = _access_left;

		elm_access_highlight_next_set(m_mic_button, ELM_HIGHLIGHT_DIR_NEXT, cue_access_right);
		elm_access_highlight_next_set(cue_access_right, ELM_HIGHLIGHT_DIR_PREVIOUS, m_mic_button);

		elm_access_highlight_next_set(cue_access_right, ELM_HIGHLIGHT_DIR_NEXT, cue_access_left);
		elm_access_highlight_next_set(cue_access_left, ELM_HIGHLIGHT_DIR_PREVIOUS, cue_access_right);

		elm_access_highlight_next_set(cue_access_left, ELM_HIGHLIGHT_DIR_NEXT, m_mic_button);
		elm_access_highlight_next_set(m_mic_button, ELM_HIGHLIGHT_DIR_PREVIOUS, cue_access_left);
	}

	return layout_main;
}

int is_lang_supported_by_stt(char lang[])
{
	/* to check if the language is supported by stt */
	int index;
	for (index = (sizeof(supported_language) / sizeof(supported_language[0]))-1; index != 0; index--)
	{
		if (!strcmp(lang, supported_language[index])) {
			PRINTFUNC(DLOG_DEBUG, "current lang supported (%s)", supported_language[index]);
			return TRUE;
		}
	}

	PRINTFUNC(DLOG_DEBUG, "current lang not supported (%s)", lang);

	return FALSE;
}

int init_voice(Evas_Object *parent, const char *lang, VoiceData *r_voicedata)
{
	PRINTFUNC(DLOG_DEBUG, "[init_voice]");

//	stt_is_samsung_asr(&g_is_n66);

	VoiceData *voicedata = (VoiceData *)r_voicedata;
	if (!voicedata) {
		return FALSE;
	}

	if (!parent) {
		PRINTFUNC(DLOG_ERROR, "Parent is NULL\n");
		return FALSE;
	} else {
		PRINTFUNC(NO_PRINT, "Parent is there");
	}

	voicedata->voicefw_state = 0;

	/* Set Voice Language */
	if(voicedata->kbd_lang){
		free(voicedata->kbd_lang);
		voicedata->kbd_lang = NULL;
	}

	//stt_get_default_language(my_voicedata->voicefw_handle, &my_voicedata->kbd_lang);
	get_stt_default_language(voicedata);
	if(NULL == voicedata->kbd_lang || FALSE == is_lang_supported_by_stt(voicedata->kbd_lang)) {
		voicedata->kbd_lang = strdup("en_US");
	}

	PRINTFUNC(DLOG_DEBUG, "Voice input active language is : %s", voicedata->kbd_lang);

	voicedata->naviframe = parent;

	if (NULL == voicedata->naviframe) {
		return FALSE;
	}

	return TRUE;
}

static Eina_Bool init_view(void *data)
{
	VoiceData *voicedata = (VoiceData *)data;

	if(voicedata == NULL)
		return ECORE_CALLBACK_CANCEL;

	voicedata->layout_main = create_fullview(voicedata->naviframe, voicedata);

	if (voicedata->layout_main) {
		evas_object_show(voicedata->layout_main);
	}

	return ECORE_CALLBACK_CANCEL;
}


Evas_Object *show_voice_window(Evas_Object *parent, VoiceData *r_voicedata)
{
	PRINTFUNC(NO_PRINT, "[show_voice_window]");

	VoiceData *voicedata = (VoiceData *)r_voicedata;
	if (!voicedata) {
		return NULL;
	}

	if (!parent) {
		PRINTFUNC(NO_PRINT, "Parent is NULL\n");
		return NULL;
	} else {
		PRINTFUNC(NO_PRINT, "Parent is there");
	}

	if (NULL != voicedata->layout_main) {
		PRINTFUNC(DLOG_DEBUG, "delete previous layout");
		evas_object_del((voicedata)->layout_main);
		(voicedata)->layout_main = NULL;
	}

	if (NULL != voicedata->effector) {
		PRINTFUNC(DLOG_DEBUG, "delete previous effect");
		delete (voicedata->effector);
		(voicedata)->effector = NULL;
	}

	if (NULL != voicedata->ieffect) {
		PRINTFUNC(DLOG_DEBUG, "delete previous ieffect");
		delete (voicedata->ieffect);
		voicedata->ieffect = NULL;
	}

	if(voicedata->mo) {
		delete voicedata->mo;
		voicedata->mo = NULL;
	}

	init_view((void *)voicedata);
	return voicedata->layout_main;
}


void on_stt_pause(VoiceData *r_voicedata){
	PRINTFUNC(DLOG_DEBUG, "");

	VoiceData *voicedata = (VoiceData *)r_voicedata;

	if(voicedata){
		try{
			//voicedata->state = STT_STATE_VAL_PREPARE_CANCEL;
			voicedata->sttmanager->Cancel();
		}catch(is::stt::SttException &e){
			PRINTFUNC(DLOG_ERROR, "reason : %s", e.what());
		}

		//Hide more option and language settings by interrupt scenario.
		close_setting_window_idler_cb(NULL);
		Evas_Object *mo_layout = voicedata->mo->getMoreOptionLayout();
		if(mo_layout){
			if(eext_more_option_opened_get(mo_layout) == EINA_TRUE) {
				eext_more_option_opened_set(mo_layout, EINA_FALSE);
			}
		}
	}
}

void on_destroy(VoiceData *r_voicedata)
{
	PRINTFUNC(NO_PRINT, "");

	VoiceData *voicedata = (VoiceData *)r_voicedata;

	int result = STT_ERROR_NONE;

	powerUnlock();

	if (NULL != voicedata) {
		if (NULL != voicedata->layout_main) {
			voicedata->layout_main = NULL;
		}

		if (NULL != voicedata->naviframe) {
			voicedata->naviframe = NULL;
		}

		if (NULL != voicedata->start_timer) {
			ecore_timer_del(voicedata->start_timer);
			voicedata->start_timer = NULL;
		}

		if (NULL != voicedata->refresh_timer) {
			ecore_timer_del(voicedata->refresh_timer);
			voicedata->refresh_timer = NULL;
		}

		if (NULL != voicedata->progressbar_timer) {
			ecore_timer_del(voicedata->progressbar_timer);
			voicedata->progressbar_timer = NULL;
			elm_progressbar_pulse(voicedata->progressbar, EINA_FALSE);
		}

		if (NULL != voicedata->textblock_timer) {
			ecore_timer_del(voicedata->textblock_timer);
			voicedata->textblock_timer = NULL;
		}

		if (NULL != voicedata->guide_text_timer) {
			ecore_timer_del(voicedata->guide_text_timer);
			voicedata->guide_text_timer = NULL;
		}

		if (NULL != voicedata->btn_disabling_timer) {
			ecore_timer_del(voicedata->btn_disabling_timer);
			voicedata->btn_disabling_timer = NULL;
		}

		if (NULL != voicedata->power_unlock_timer) {
			ecore_timer_del(voicedata->power_unlock_timer);
			voicedata->power_unlock_timer = NULL;
		}

		if(voicedata->kbd_lang) {
			free(voicedata->kbd_lang);
			voicedata->kbd_lang = NULL;
		}

		if(voicedata->ieffect) {
			delete voicedata->ieffect;
			voicedata->ieffect = NULL;
		}

		if(voicedata->effector) {
			delete voicedata->effector;
			voicedata->effector = NULL;
		}

		if(voicedata->mo) {
			delete voicedata->mo;
			voicedata->mo = NULL;
		}

		if(voicedata->sttmanager) {
			delete voicedata->sttmanager;
			voicedata->sttmanager = NULL;
		}

		if(voicedata->sttfeedback) {
			delete voicedata->sttfeedback;
			voicedata->sttfeedback = NULL;
		}

		free(voicedata);
	}
}



