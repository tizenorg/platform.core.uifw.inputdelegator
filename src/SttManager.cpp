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

#include <assert.h>
#include <vconf.h>
#include <vconf-keys.h>

#include "Debug.h"
#include "SttManager.h"

using namespace is::stt;

enum {
	READY = 0x0001,
	RECORDING = 0x0010,
	PROCESSING = 0x0100,
	CREATE = 0x1000
};

static inline const char *stt_state_str(stt_state_e cur) {
	if (cur == STT_STATE_CREATED)
		return (const char *) "STT_STATE_CREATED";

	else if (cur == STT_STATE_READY)
		return (const char *) "STT_STATE_READY";

	else if (cur == STT_STATE_RECORDING)
		return (const char *) "STT_STATE_RECORDING";

	else if (cur == STT_STATE_PROCESSING)
		return (const char *) "STT_STATE_PROCESSING";

	else
		return (const char *) "ABNORMAL CASE";
}

SttManager::SttManager(ISttFeedback& feedback)
: ifeedback(feedback),
  iscancelled(false)
{
	try {
		/**
		* Create stt handle.
		*
		*/
		int ret = stt_create(&handle);

		if(ret != STT_ERROR_NONE)
			throw SttException(ret, ErrorString((stt_error_e)ret));

		/**
		* Set default properties
		*
		*/
		EnableFeedback();
	}
	catch(SttException &e) {
		PRINTFUNC(DLOG_ERROR, "reason : %s", e.what());
		assert(0);
	}
}

SttManager::~SttManager() {
	try {
		EnableFeedback(false);

		Cancel();
		UnPrepare();
	}
	catch(SttException &e) {
		PRINTFUNC(DLOG_ERROR, "reason : %s", e.what());
		stt_destroy(handle);
	}
}

void SttManager::Prepare() {
	/**
	* Prepare stt service.
	*
	*/
	int ret = stt_prepare(handle);

	if(ret != STT_ERROR_NONE)
		throw SttException(ret, ErrorString((stt_error_e)ret));
}

void SttManager::UnPrepare() {
   /**
    * UnPrepare stt service.
    *
    */
   int ret = stt_unprepare(handle);

   if (ret != STT_ERROR_NONE)
	   throw SttException(ret, ErrorString((stt_error_e)ret));
}

void SttManager::Start() {
	if(!Validate((int) READY)) {
	    throw SttException((int) STT_ERROR_INVALID_STATE, "INVALID STATE - !STT_STATE_READY");
	}

	PRINTFUNC(DLOG_DEBUG, "HERE");

	iscancelled = false;

	/**
	* Start stt service.
	*
	*/
	asrtype = STT_RECOGNITION_TYPE_FREE_PARTIAL;
	int ret;

	bool bval = false;

//	stt_is_samsung_asr(&bval);

	if( bval == true && !language.compare("en_GB")) {
		PRINTFUNC(DLOG_DEBUG, "en_GB requested, change to en_US");
        ret = stt_start(handle, "en_US", asrtype.c_str());
	} else {
        ret = stt_start(handle, language.c_str(), asrtype.c_str());
	}

	if(ret != STT_ERROR_NONE)
		throw SttException(ret, ErrorString((stt_error_e)ret));
}

void SttManager::Stop() {
	if(!Validate((int) RECORDING)) {
		throw SttException((int) STT_ERROR_INVALID_STATE, "INVALID STATE - !STT_STATE_RECORDING");
	}

	/**
	* Stop stt service.
	*
	*/
	int ret = stt_stop(handle);

	if(ret != STT_ERROR_NONE)
		throw SttException(ret, ErrorString((stt_error_e)ret));
}

void SttManager::Cancel() {
	if(iscancelled) {
		PRINTFUNC(DLOG_WARN, "iscancelled (%d)", iscancelled);
		return;
	}

	if(!Validate((int) (RECORDING|PROCESSING))) {
		throw SttException((int) STT_ERROR_INVALID_STATE, "INVALID STATE - !(STT_STATE_RECORDING or STT_STATE_PROCESSING)");
	}

	/**
	* Cancel stt service (recording, processing)
	*
	*/
	int ret = stt_cancel(handle);

	if(ret != STT_ERROR_NONE)
		throw SttException(ret, ErrorString((stt_error_e)ret));

	iscancelled = true;
	PRINTFUNC(DLOG_INFO, "iscancelled (%d)", iscancelled);

	ifeedback.SttIdle();
}


bool SttManager::Validate(int state) {
	stt_state_e cur;

	int ret = stt_get_state(handle, &cur);
	if (ret != STT_ERROR_NONE) {
		return false;
	}

	PRINTFUNC(DLOG_DEBUG, "validate state - %d", state);
	PRINTFUNC(DLOG_DEBUG, "stt deamon state - %s",
		cur == STT_STATE_CREATED ? "STT_STATE_CREATED" :
		cur == STT_STATE_READY ? "STT_STATE_READY" :
		cur == STT_STATE_RECORDING ? "STT_STATE_RECORDING" :
		cur == STT_STATE_PROCESSING ? "STT_STATE_PROCESSING" : "ABNORMAL");

	switch(cur) {
		case STT_STATE_CREATED :
			if (state & CREATE) return true;
			break;
		case STT_STATE_READY :
			if (state & READY) return true;
			break;
		case STT_STATE_RECORDING :
			if (state & RECORDING) return true;
			break;
		case STT_STATE_PROCESSING :
			if (state & PROCESSING) return true;
			break;
		default :
			break;
	}

	return false;
}

void SttManager::Initialize() {
   /** Todo. add routine to intialize */
}

void SttManager::PrintResultState(stt_result_event_e result_type)
{
	std::string result;

	switch (result_type) {
		case STT_RESULT_EVENT_FINAL_RESULT :
			result = "STT_RESULT_EVENT_FINAL_RESULT";
			break;
		case STT_RESULT_EVENT_PARTIAL_RESULT :
			result = "STT_RESULT_EVENT_PARTIAL_RESULT";
			break;
		case STT_RESULT_EVENT_ERROR :
			result = "STT_RESULT_EVENT_ERROR";
			break;
		default :
			result = "UNKNOWN";
			break;
	}
	PRINTFUNC(DLOG_INFO, "result type : %s", result.c_str());
}

void SttManager::on_result(
   stt_h handle,
   stt_result_event_e event,
   const char** data,
   int size,
   const char* msg,
   void *user_data) {
   PrintResultState(event);

   if (!user_data) {
		PRINTFUNC(DLOG_ERROR, "user_data null");
		throw SttException((int)STT_ERROR_INVALID_PARAMETER, "invalid self reference");
   }

   SttManager& manager = *((SttManager *) user_data);

   std::vector<std::string> results;

   PRINTFUNC(DLOG_INFO, "result size : %d, msg : %s", size, msg);

   for (size_t i = 0; i < (size_t) size; i++) {
	if (data[i]) {
		results.push_back(std::string(data[i]));
	}

	if (msg)
	   manager.ifeedback.OnResult(manager.asrtype, event, results, std::string(msg));
	else
	   manager.ifeedback.OnResult(manager.asrtype, event, results, std::string(""));
   }
}

void SttManager::PrintState(stt_state_e previous, stt_state_e current)
{
	std::string prev;
	std::string curr;

	switch (previous) {
		case STT_STATE_READY :
			prev = "STT_STATE_READY";
			break;
		case STT_STATE_CREATED :
			prev = "STT_STATE_CREATED";
			break;
		case STT_STATE_RECORDING :
			prev = "STT_STATE_RECORDING";
			break;
		case STT_STATE_PROCESSING :
			prev = "STT_STATE_PROCESSING";
			break;
		default :
			prev = "UNKNOWN";
			break;
	}

	switch (current) {
		case STT_STATE_READY :
			curr = "STT_STATE_READY";
			break;
		case STT_STATE_CREATED :
			curr = "STT_STATE_CREATED";
			break;
		case STT_STATE_RECORDING :
			curr = "STT_STATE_RECORDING";
			break;
		case STT_STATE_PROCESSING :
			curr = "STT_STATE_PROCESSING";
			break;
		default :
			curr = "UNKNOWN";
			break;
	}
	PRINTFUNC(DLOG_INFO, "previous: %s(%d), current: %s(%d)", prev.c_str(), previous, curr.c_str(), current);
}

void SttManager::on_state_changed(
	stt_h handle,
	stt_state_e previous,
	stt_state_e current,
	void *user_data) {
	PrintState(previous, current);

	if (!user_data)
		throw SttException((int)STT_ERROR_INVALID_PARAMETER, "invalid self reference");

	SttManager& manager = *((SttManager *) user_data);

	if (current== STT_STATE_READY) {
		if (previous == STT_STATE_CREATED) {
			manager.EnableSilenceDetection();
			manager.ifeedback.AutoStart();
		} else if (previous == STT_STATE_RECORDING) {
			std::string msg;
			std::vector<std::string> results;
			manager.ifeedback.OnResult(manager.asrtype, STT_RESULT_EVENT_ERROR, results, msg);
		} else {
			manager.ifeedback.SttIdle();
		}
	} else if (current == STT_STATE_RECORDING) {
			manager.ifeedback.SttRecording();
	} else if (current == STT_STATE_PROCESSING) {
		if (!manager.iscancelled) {
			PRINTFUNC(DLOG_INFO, "iscancelled (%d)", manager.iscancelled);
			manager.ifeedback.SttProcessing();
		} else {
			manager.iscancelled = false;
			PRINTFUNC(DLOG_INFO, "iscancelled (%d)", manager.iscancelled);
		}
	}
}

void SttManager::PrintErrorState(stt_error_e reason)
{
	std::string res;

	switch (reason) {
		case STT_ERROR_OUT_OF_MEMORY :
			res = "STT_ERROR_OUT_OF_MEMORY";
			break;
		case STT_ERROR_IO_ERROR :
			res = "STT_ERROR_IO_ERROR";
			break;
		case STT_ERROR_INVALID_PARAMETER :
			res = "STT_ERROR_INVALID_PARAMETER";
			break;
		case STT_ERROR_TIMED_OUT :
			res = "STT_ERROR_TIMED_OUT";
			break;
		case STT_ERROR_RECORDER_BUSY :
			res = "STT_ERROR_RECORDER_BUSY";
			break;
		case STT_ERROR_OUT_OF_NETWORK :
			res = "STT_ERROR_OUT_OF_NETWORK";
			break;
		case STT_ERROR_PERMISSION_DENIED :
			res = "STT_ERROR_PERMISSION_DENIED";
			break;
		case STT_ERROR_NOT_SUPPORTED :
			res = "STT_ERROR_NOT_SUPPORTED";
			break;
		case STT_ERROR_INVALID_STATE :
			res = "STT_ERROR_INVALID_STATE";
			break;
		case STT_ERROR_INVALID_LANGUAGE :
			res = "STT_ERROR_INVALID_LANGUAGE";
			break;
		case STT_ERROR_ENGINE_NOT_FOUND :
			res = "STT_ERROR_ENGINE_NOT_FOUND";
			break;
		case STT_ERROR_OPERATION_FAILED :
			res = "STT_ERROR_OPERATION_FAILED";
			break;
		case STT_ERROR_NOT_SUPPORTED_FEATURE :
			res = "STT_ERROR_NOT_SUPPORTED_FEATURE";
			break;
		default :
			res = "UNKNOWN ERROR REASON";
			break;
	}
	PRINTFUNC(DLOG_INFO, "Error reason %s(%d)", res.c_str(), reason);
}

void SttManager::on_error(
   stt_h handle,
   stt_error_e reason,
   void *user_data) {
   PRINTFUNC(DLOG_INFO, "stt-daemon error (%d)", reason);

   if (!user_data)
		throw SttException((int)STT_ERROR_INVALID_PARAMETER, "invalid self reference");

   SttManager& manager = *((SttManager *) user_data);
   manager.ifeedback.OnError(reason);
}

void SttManager::SetLanguage(std::string language) {
   this->language = language;
}

void SttManager::EnableFeedback(bool enabled) {
   int ret = STT_ERROR_NONE;

   void *udata = static_cast<void *>(this);

   if (enabled) {
      ret = stt_set_recognition_result_cb(handle, on_result, udata);
      if (STT_ERROR_NONE != ret)
		throw SttException(ret, ErrorString((stt_error_e)ret));

      ret = stt_set_error_cb(handle, on_error, udata);
      if (STT_ERROR_NONE != ret)
		throw SttException(ret, ErrorString((stt_error_e)ret));

      ret = stt_set_state_changed_cb(handle, on_state_changed, udata);
      if (STT_ERROR_NONE != ret)
		throw SttException(ret, ErrorString((stt_error_e)ret));
   } else {
      ret = stt_unset_error_cb(handle);
      if (STT_ERROR_NONE != ret)
		throw SttException(ret, ErrorString((stt_error_e)ret));

      ret = stt_unset_state_changed_cb(handle);
      if (STT_ERROR_NONE != ret)
		throw SttException(ret, ErrorString((stt_error_e)ret));

      ret = stt_unset_recognition_result_cb(handle);
      if (STT_ERROR_NONE != ret)
		throw SttException(ret, ErrorString((stt_error_e)ret));
   }
}

const char* SttManager::ErrorString(int ecode) {
   const char *str = NULL;

   switch (ecode) {
      case STT_ERROR_OUT_OF_MEMORY:
         str = (const char *) "STT_ERROR_OUT_OF_MEMORY";
         break;
      case STT_ERROR_IO_ERROR:
         str = (const char *) "STT_ERROR_IO_ERROR";
         break;
      case STT_ERROR_INVALID_PARAMETER:
         str = (const char *) "STT_ERROR_INVALID_PARAMETER";
         break;
      case STT_ERROR_TIMED_OUT:
         str = (const char *) "STT_ERROR_TIMED_OUT";
         break;
      case STT_ERROR_RECORDER_BUSY:
         str = (const char *) "STT_ERROR_RECORDER_BUSY";
         break;
      case STT_ERROR_OUT_OF_NETWORK:
         str = (const char *) "STT_ERROR_OUT_OF_NETWORK";
         break;
      case STT_ERROR_INVALID_STATE:
         str = (const char *) " STT_ERROR_INVALID_STATE";
         break;
      case STT_ERROR_INVALID_LANGUAGE:
         str = (const char *) "STT_ERROR_INVALID_LANGUAGE";
         break;
      case STT_ERROR_ENGINE_NOT_FOUND:
         str = (const char *) "STT_ERROR_ENGINE_NOT_FOUND";
         break;
      case STT_ERROR_OPERATION_FAILED:
         str = (const char *) "STT_ERROR_OPERATION_FAILED";
         break;
      case STT_ERROR_NOT_SUPPORTED_FEATURE:
         str = (const char *) "STT_ERROR_NOT_SUPPORTED_FEATURE";
         break;
   }
   return str;
}

void SttManager::SoundFeedback() {
   int is_sound = 0;
   int is_sound_vibe = 0;

   if (vconf_get_bool(VCONFKEY_SETAPPL_SOUND_STATUS_BOOL, &is_sound)) {
      PRINTFUNC(DLOG_ERROR, "get sound status failed.");
   }

   if (vconf_get_bool(VCONFKEY_SETAPPL_VIBRATION_STATUS_BOOL, &is_sound_vibe)) {
      PRINTFUNC(DLOG_ERROR, "get vibe status failed.");
   }

   if (is_sound || is_sound_vibe) {
      stt_set_start_sound(handle, "/usr/share/ise-voice-input/audio/voice_start.wav");
      stt_set_stop_sound(handle, "/usr/share/ise-voice-input/audio/voice_stop.wav");
   } else {
      stt_unset_start_sound(handle);
      stt_unset_stop_sound(handle);
   }
}



void SttManager::EnableSilenceDetection(bool enabled) {
	stt_option_silence_detection_e s_option;

	if (enabled)
		s_option = STT_OPTION_SILENCE_DETECTION_TRUE;
	else
		s_option = STT_OPTION_SILENCE_DETECTION_FALSE;

	int ret = stt_set_silence_detection(handle, s_option);
	if (STT_ERROR_NONE != ret) {
		PRINTFUNC(
			DLOG_ERROR,
			"error(%d) = %s",
			ret,
			ErrorString((stt_error_e) ret));
	} else {
		PRINTFUNC(NO_PRINT, "stt_set_silence_detection Successful");
	}
}

