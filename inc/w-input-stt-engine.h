/*
* Copyright (c)  2016 Samsung Electronics, Inc.
* All rights reserved.
*
* This software is a confidential and proprietary information
* of Samsung Electronics, Inc. ("Confidential Information").  You
* shall not disclose such Confidential Information and shall use
* it only in accordance with the terms of the license agreement
* you entered into with Samsung Electronics.
*/


#ifndef W_INPUT_STT_ENGINE_H_
#define W_INPUT_STT_ENGINE_H_

#include <gmodule.h>
#include <vector>

#include "SttFeedback.h"
#include "SttManager.h"
#include "MoreOption.h"
#include "MicEffector.h"
#include "WInputSttMicEffect.h"

#ifdef __cplusplus
extern "C"{
#endif

#include <stt.h>


typedef enum _SttStateVal {
	STT_STATE_VAL_INIT = 0,
	STT_STATE_VAL_PREPARE_LISTENING,
	STT_STATE_VAL_LISTENING,
	STT_STATE_VAL_PREPARE_PROCESSING,
	STT_STATE_VAL_PROCESSING,
	STT_STATE_VAL_PREPARE_CANCEL,
	STT_STATE_VAL_NOT_RECOGNISED,
	STT_STATE_VAL_TERMINATING,
	STT_STATE_VAL_MAX
} SttStateVal;

typedef struct _VoiceDimension VoiceDimension;
typedef struct _VoiceData VoiceData;

struct _VoiceDimension {
	int x; // X position of voice area
	int y; // Y position of voice area
	int width; // Width of voice area
	int height; // Height of voice area
};

struct _VoiceData
{
	int voicefw_state; //0 means init failed else success
	stt_h voicefw_handle; //Wonyoung Lee added
	Evas_Object *naviframe; //main window
	Evas_Object *layout_main; //layout
	Evas_Object *progressbar; //progressbar
	Evas_Object *scroller; //scroller
	Evas_Object *main_entry; //entry
	Evas_Object *mic_button; //MIC button
	SttStateVal state;
	char *kbd_lang;
	Ecore_Timer *start_timer;
	Ecore_Timer *refresh_timer;
	Ecore_Timer *progressbar_timer;
	Ecore_Timer *textblock_timer;
	Ecore_Timer *guide_text_timer;
	Ecore_Timer *btn_disabling_timer;
	Ecore_Timer *power_unlock_timer;

	std::vector<std::string> stt_results;
	char *partial_result;
	int result_type;

	int disclaimer;

	MoreOption* mo;

	is::stt::SttFeedback *sttfeedback;
	is::stt::SttManager *sttmanager;

	is::ui::WInputSttMicEffect *ieffect;
	is::ui::MicEffector *effector;
};

typedef enum _Feedback_Type{
	FEEDBACK_RECORDING = 0,
	FEEDBACK_PROCESSING,
}Feedback_Type;


void set_animation_state(VoiceData *ud);
char * error_string(int ecode);
// STT functions
bool _app_stt_initialize(VoiceData *user_data);
void on_feedback(stt_h handle);


#if 0
void (*send_processed_string)(char *, int);
#endif

#ifdef __cplusplus
	}
#endif

#endif /* W_INPUT_STT_ENGINE_H_ */
