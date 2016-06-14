  /*
  * SLP
  * Copyright (c)  2010 Samsung Electronics, Inc.
  * All rights reserved.
  *
  * This software is a confidential and proprietary information
  * of Samsung Electronics, Inc. ("Confidential Information").	You
  * shall not disclose such Confidential Information and shall use
  * it only in accordance with the terms of the license agreement
  * you entered into with Samsung Electronics.
  */

 /**
 * @file	 w-input-stt-ise.cpp
 * @brief
 * @author	 Samsung Electronics Co., Ltd.
 * @version  1.0
 */

 /*
  * Function to be used to show the voice input
  */

#include <vconf.h>

#include "Debug.h"
#include "w-input-selector.h"
#include "w-input-stt-ise.h"
#include "w-input-stt-voice.h"
#include "w-input-stt-engine.h"


VoiceData *my_voicedata = NULL;

#define FREE(ptr)	\
	 do { \
		 if (ptr != NULL) { \
			 free(ptr); \
			 ptr = NULL; \
		 } \
	 } while (0);

void show_voice_input(Evas_Object *parent, const char *lang, void (*get_string)(char *, int))
{
	PRINTFUNC(DLOG_ERROR, "show voice");
	int init = 0;

	destroy_voice();

	//stt_feedback_initialize();

	if (!my_voicedata) {
		my_voicedata = (VoiceData*)malloc(sizeof(VoiceData));
		if (my_voicedata == NULL) {
			PRINTFUNC(DLOG_ERROR, "%d::::Heap Overflow, Voice Input cannot be shown!", __LINE__);
		return;
		}
		memset(my_voicedata, 0, sizeof(VoiceData));
	}

	init = init_voice(parent, lang, my_voicedata);
	if (init) {
		if (my_voicedata->naviframe) {
			show_voice_window(my_voicedata->naviframe, my_voicedata);
		}
	} else {
		destroy_voice();
		PRINTFUNC(DLOG_ERROR, "%d::::Fail to create Voice window!", __LINE__);
		return;
	}

	vconf_notify_key_changed(VCONFKEY_ISE_STT_LANGUAGE, _stt_lang_changed_cb, my_voicedata);

}


void  pause_voice(){

	PRINTFUNC(DLOG_DEBUG, "pause_voice");

	powerUnlock();
	if (my_voicedata) {
		on_stt_pause(my_voicedata);
	}


}

 void destroy_voice()
 {
	 PRINTFUNC(DLOG_DEBUG, "destroy voice");

	 //stt_feedback_deinitialize();  // It disable w-input-selector touch sound. So removed.

	 vconf_ignore_key_changed(VCONFKEY_ISE_STT_LANGUAGE, _stt_lang_changed_cb);

	 if (my_voicedata) {
		 on_destroy(my_voicedata);
		 my_voicedata = NULL;
	 }

 }

 int is_lang_supported_by_voice_input(const char *lang)
 {
	 PRINTFUNC(DLOG_DEBUG, "Is lang %s supported by voice input", lang);
	 char kbd_lang[6];

	 if (NULL == lang)
		 return FALSE;

	 strncpy(kbd_lang, lang, sizeof(kbd_lang));
	 kbd_lang[5] = '\0';

	 return (is_lang_supported_by_stt(kbd_lang));
 }

void ise_show_stt_popup(void *data)
 {
	 App_Data* ad = (App_Data*)data;

	 if(!ad)
		 return;

	 show_voice_input((Evas_Object*)ad->naviframe, NULL, NULL);
 }

 void set_disclaimer_flag()
 {
	 PRINTFUNC(DLOG_DEBUG, "set_disclaimer_flag");

	 if (my_voicedata) {
		 my_voicedata->disclaimer = 1;
	 }

 }


