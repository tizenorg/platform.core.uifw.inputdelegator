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
#include "SttFeedback.h"

#include "w-input-stt-engine.h"
#include "w-input-stt-voice.h"

using namespace is::stt;

SttFeedback::SttFeedback()
{
	owner = NULL;
}



SttFeedback::~SttFeedback()
{

}



void SttFeedback::OnResult (
	std::string asrtype,
	stt_result_event_e event,
	std::vector<std::string> results,
	std::string msg) {

	if(!owner) return;

	VoiceData& vd = *((VoiceData *) owner);

	if(vd.state == STT_STATE_VAL_TERMINATING) {
		PRINTFUNC(DLOG_WARN, "STT_STATE_VAL_TERMINATING");
		return;
	}

	// asrtype
	// Partial result case : STT_RECOGNITION_TYPE_FREE_PARTIAL
	// Normal result case : STT_RECOGNITION_TYPE_FREE

	if(results.size() < 0) {
		PRINTFUNC(DLOG_ERROR, "Result size : %d", results.size());
		vd.state = STT_STATE_VAL_NOT_RECOGNISED;
		set_animation_state(&vd);
	}
	else {
		PRINTFUNC(DLOG_INFO, "Meaningful result : size (%d)", results.size());

		for(auto result : results) {
			PRINTFUNC(DLOG_INFO, "Results");
			if(!result.empty() && result.length() > 0) {
				PRINTFUNC(DLOG_INFO,"%s\n", result.c_str());
				switch(event){
					case STT_RESULT_EVENT_FINAL_RESULT:
						PRINTFUNC(DLOG_DEBUG,"STT_RESULT_EVENT_FINAL_RESULT");
						vd.state = STT_STATE_VAL_INIT;
						set_animation_state(&vd);
						vd.result_type = STT_RESULT_EVENT_FINAL_RESULT;
						voice_get_string(result.c_str(), &vd);
						break;
					case STT_RESULT_EVENT_PARTIAL_RESULT:
						PRINTFUNC(DLOG_DEBUG,"STT_RESULT_EVENT_PARTIAL_RESULT");
						vd.result_type = STT_RESULT_EVENT_PARTIAL_RESULT;
						voice_get_string(result.c_str(), &vd);
						break;
					default:
						PRINTFUNC(DLOG_INFO, "");
						break;
				}
				return;
			}
			PRINTFUNC(DLOG_INFO, "Empty result");
		}

		PRINTFUNC(DLOG_INFO, "Need to specify this case");

		/**
		 * Note.
		 *
		 * if recognized result doesn't have any data,
		 * it will process as recognition fail.
		 *
		 */
		//vd.state = STT_STATE_VAL_NOT_RECOGNISED;
		vd.state = STT_STATE_VAL_INIT;
		set_animation_state(&vd);
	}
}


void SttFeedback::AutoStart(void) {
	PRINTFUNC(DLOG_DEBUG, "start");

	if (elm_config_access_get()){
		PRINTFUNC(DLOG_DEBUG, "accessbility on: It will not start automatically.");
	} else {
		start_by_press((VoiceData *) owner);
	}
}


void SttFeedback::SttIdle(void)
{
	if(!owner) {
		PRINTFUNC(DLOG_WARN, "no owner");
		return;
	}

	VoiceData& vd = *((VoiceData *) owner);

	/**
	 * Note.
	 *
	 * When recognition is failed, do nothing.
	 * Because after result, it's called continuous. So it looks not natural.
	 *
	 * So in this case, we will not change as INIT state.
	 * using 2 sec timer, it will change as idle state.
	 *
	 */
	if(vd.state == STT_STATE_VAL_NOT_RECOGNISED) {
		PRINTFUNC(DLOG_INFO, "Ignore when state was STT_STATE_VAL_NOT_RECOGNISED");
		return;
	}

	PRINTFUNC(DLOG_DEBUG, "UI will go to idle state");

	vd.state = STT_STATE_VAL_INIT;
	set_animation_state(&vd);
}



void SttFeedback::SttRecording(void)
{
	if(!owner) {
		PRINTFUNC(DLOG_WARN, "no owner");
		return;
	}

	VoiceData& vd = *((VoiceData *) owner);

	if(vd.partial_result){
		free(vd.partial_result);
		vd.partial_result = NULL;
	}

	PRINTFUNC(DLOG_DEBUG, "UI will go to listening state");

	vd.state = STT_STATE_VAL_LISTENING;
	set_animation_state(&vd);
}



void SttFeedback::SttProcessing(void)
{
	if(!owner) {
		PRINTFUNC(DLOG_WARN, "no owner");
		return;
	}

	VoiceData& vd = *((VoiceData *) owner);

	PRINTFUNC(DLOG_DEBUG, "UI will go to processing state");

	vd.state = STT_STATE_VAL_PROCESSING;
	set_animation_state(&vd);
}



void SttFeedback::OnError(stt_error_e reason)
{
	if(!owner) {
		PRINTFUNC(DLOG_WARN, "no owner");
		return;
	}

	VoiceData& vd = *((VoiceData *) owner);

	PRINTFUNC(DLOG_ERROR,"error = %d\n", reason);
	vd.state = STT_STATE_VAL_INIT;
	set_animation_state(&vd);
	show_error_message(&vd, reason);
}


void SttFeedback::SetVoiceData(void *data) {

	if(!data) {
		PRINTFUNC(DLOG_WARN, "no data");
		return;
	}

	owner = data;
}
