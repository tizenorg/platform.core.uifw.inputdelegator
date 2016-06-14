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
 * @file	 w-input-stt-engine.c
 * @brief
 * @author	 Samsung Electronics Co., Ltd.
 * @version  1.0
 */

#include <Elementary.h>
#include <gmodule.h>
#include <stdlib.h>
#include <string.h>
#include <vconf.h>
#include <feedback.h>
#include <wav_player.h>
#include <vconf-internal-keys.h>
#include <app.h>

#include "w-input-selector.h"
#include "w-input-stt-voice.h"
#include "w-input-stt-engine.h"
#include "Debug.h"

#define FREE(ptr)	\
		do { \
			if (ptr != NULL) { \
				free(ptr); \
				ptr = NULL; \
			} \
		} while (0);


static stt_state_e static_current_stt_state = STT_STATE_CREATED;
static stt_h g_stt;

static void _on_state_change_cb(stt_h stt, stt_state_e previous, stt_state_e current, void* voice_data);
static void _on_error_cb(stt_h stt, stt_error_e reason, void *voice_data);
static void _on_result_cb(stt_h stt, stt_result_event_e event, const char** data, int data_count, const char* msg, void *voice_data);

static bool is_auto_spacing(void *voice_data)
{
	VoiceData *voicedata = (VoiceData *)voice_data;
	int lang = 0, ret = 0;
#if 0
	ret = preference_get_int(PREFERENCE_ISE_STT_LANGUAGE, &lang);
	if(PREFERENCE_ERROR_NONE != ret){
		PRINTFUNC(DLOG_ERROR, "preference_get_int error!(%d)", ret);
	}
#else
	ret = vconf_get_int(VCONFKEY_ISE_STT_LANGUAGE, &lang);
	if (ret !=0) {
		PRINTFUNC(DLOG_ERROR, "Vconf_get_int error!(%d)", ret);
	}
#endif
	switch(lang) {
	case 0 :
		if((strcmp(voicedata->kbd_lang, "zh_CN") == 0) || (strcmp(voicedata->kbd_lang, "ja_JP") == 0))
			return false;
		else
			return true;
	case 7: // Japanese
	case 10: // Chinese
		return false;
	default :
		return true;
	}

	return true;
}

char * error_string(int ecode)
{
	char *str = NULL;
	switch (ecode) {
	case STT_ERROR_OUT_OF_MEMORY:
		str = "STT_ERROR_OUT_OF_MEMORY";
		break;
	case STT_ERROR_IO_ERROR:
		str = "STT_ERROR_IO_ERROR";
		break;
	case STT_ERROR_INVALID_PARAMETER:
		str = "STT_ERROR_INVALID_PARAMETER";
		break;
	case STT_ERROR_TIMED_OUT:
		str = "STT_ERROR_TIMED_OUT";
		break;
	case STT_ERROR_RECORDER_BUSY:
		str = "STT_ERROR_RECORDER_BUSY";
		break;
	case STT_ERROR_OUT_OF_NETWORK:
		str = "STT_ERROR_OUT_OF_NETWORK";
		break;
	case STT_ERROR_INVALID_STATE:
		str = " STT_ERROR_INVALID_STATE";
		break;
	case STT_ERROR_INVALID_LANGUAGE:
		str = "STT_ERROR_INVALID_LANGUAGE";
		break;
	case STT_ERROR_ENGINE_NOT_FOUND:
		str = "STT_ERROR_ENGINE_NOT_FOUND";
		break;
	case STT_ERROR_OPERATION_FAILED:
		str = "STT_ERROR_OPERATION_FAILED";
		break;
	case STT_ERROR_NOT_SUPPORTED_FEATURE:
		str = "STT_ERROR_NOT_SUPPORTED_FEATURE";
		break;
	}
	return str;
}

void voice_stt_set_silence_detection_func(bool bEnable)
{
	int ret = STT_ERROR_NONE;

	stt_option_silence_detection_e s_option;

	if(bEnable)
		s_option = STT_OPTION_SILENCE_DETECTION_TRUE;
	else
		s_option = STT_OPTION_SILENCE_DETECTION_FALSE;

	ret = stt_set_silence_detection(g_stt, s_option);
	if (STT_ERROR_NONE != ret) {
		PRINTFUNC(DLOG_ERROR,"stt_set_silence_detection Failed : error(%d) = %s", ret, error_string((stt_error_e)ret));
	} else {
		PRINTFUNC(NO_PRINT,"stt_set_silence_detection Successful");
	}

}


////////////////////////////////////////////////////////////////////////////////
// STT Callback functions
////////////////////////////////////////////////////////////////////////////////

void on_feedback(stt_h handle)
{
	int is_sound = 0;
	int is_sound_vibe = 0;

	if(vconf_get_bool(VCONFKEY_SETAPPL_SOUND_STATUS_BOOL, &is_sound)) {
		PRINTFUNC(DLOG_ERROR, "get sound status failed.");
	}

	if(vconf_get_bool(VCONFKEY_SETAPPL_VIBRATION_STATUS_BOOL, &is_sound_vibe)) {
		PRINTFUNC(DLOG_ERROR, "get vibe status failed.");
	}

	if(is_sound || is_sound_vibe) {
		stt_set_start_sound(handle, "/usr/share/ise-voice-input/audio/voice_start.wav");
		stt_set_stop_sound(handle, "/usr/share/ise-voice-input/audio/voice_stop.wav");
	}else{
		stt_unset_start_sound(handle);
		stt_unset_stop_sound(handle);
	}
}

////////////////////////////////////////////////////////////////////////////////
// STT APIs callers
////////////////////////////////////////////////////////////////////////////////

bool _app_stt_initialize(VoiceData *voice_data)
{
	PRINTFUNC(NO_PRINT,"_app_stt_initialize");
	VoiceData *vd = (VoiceData *)voice_data;

	try {
		if(vd->sttmanager) {
			vd->sttmanager->Cancel();
			delete vd->sttmanager;
			vd->sttmanager = NULL;
		}

		if(vd->sttfeedback) {
			delete vd->sttfeedback;
			vd->sttfeedback = NULL;
		}

		vd->sttfeedback = new is::stt::SttFeedback();
		vd->sttfeedback->SetVoiceData(vd);

		vd->sttmanager = new is::stt::SttManager(*(vd->sttfeedback));
		vd->sttmanager->Prepare();

	} catch(std::exception &e) {
		PRINTFUNC(DLOG_ERROR, "%s", e.what());
		return false;
	}

	return true;
}


