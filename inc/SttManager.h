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


#pragma once

#include <string>
#include <vector>
#include <stdexcept>


extern "C" {
	#include <stt.h>
}

namespace is {
namespace stt {

class SttException : public std::runtime_error {
	public :
		SttException(int code, const char *reason)
			: std::runtime_error(reason)
			, ecode(code) { };

		int GetEcode() {
			return ecode;
		};

		int ecode;
};

class ISttFeedback
{
	public :
		virtual ~ISttFeedback() = 0;
		/**
		 * Result event which can be overloaded
		 *
		 * @param event
		 * @param results
		 * @param msg
		 */
		virtual void OnResult(
			std::string asrtype,
			stt_result_event_e event,
			std::vector<std::string> results,
			std::string msg) = 0;

		virtual void AutoStart(void) = 0;

		virtual void SttIdle(void) = 0;

		virtual void SttRecording(void) = 0;

		virtual void SttProcessing(void) = 0;

		/**
		 * Error event which can be overloaded
		 *
		 * @param reason
		 */
		virtual void OnError(stt_error_e reason) = 0;
};
inline ISttFeedback::~ISttFeedback() { };

class SttManager
{
	private :
		stt_h handle;
		std::string language;

	public :
		ISttFeedback& ifeedback;
		bool iscancelled;
		std::string asrtype;

	public :

		SttManager(ISttFeedback& feedback);

		~SttManager();

		/**
		 * Prepare stt service.
		 *
		 */
		void Prepare();

		/**
		 * Unprepare stt service
		 *
		 */
		void UnPrepare();

		/**
		 * Start stt service
		 *
		 */
		void Start();

		/**
		 * Stop stt service
		 *
		 */
		void Stop();

		/**
		 * Cancle stt service
		 *
		 */
		void Cancel();

		/**
		 * Initialize
		 *
		 */
		void Initialize();

		bool Validate(int state);


		/**
		 * result cb for tizen stt api.
		 *
		 * @param handle
		 * @param event
		 * @param data
		 * @param data_count
		 * @param msg
		 * @param user_data
		 */
		static void on_result(
			stt_h handle,
			stt_result_event_e event,
			const char **data,
			int data_count,
			const char *msg,
			void *user_data);

		/**
		 * state changed cb for tizen stt api.
		 *
		 * @param handle
		 * @param previous
		 * @param current
		 * @param user_data
		 */
		static void on_state_changed(
			stt_h handle,
			stt_state_e previous,
			stt_state_e current,
			void *user_data);

		/**
		 * error cb for tizen stt api.
		 *
		 * @param handle
		 * @param reason
		 * @param user_data
		 */
		static void on_error(stt_h handle, stt_error_e reason, void *user_data);

		/**
		 * Setter language property
		 *
		 * @param language
		 */
		void SetLanguage(std::string language);

		/**
		 * Enable cb event or not
		 *
		 * @param enabled
		 */
		void EnableFeedback(bool enabled = true);

		/**
		 * Enable silence detect
		 *
		 * @param enabled
		 */
		void EnableSilenceDetection(bool enabled = false);

		/**
		 * Sound feedback
		 *
		 */
		void SoundFeedback();

		/**
		 * Convert erroro code as string
		 *
		 * @param errocode
		 * @return human readable string about error code.
		 */
		const char *ErrorString(int ecode);

		stt_state_e GetCurrent(void) {
			stt_state_e cur;
			stt_get_state(handle, &cur);

			return cur;
		};

		stt_h GetSttHandle() {	return handle;	}

	private :
		static void PrintErrorState(stt_error_e reason);
		static void PrintState(stt_state_e previous, stt_state_e current);
		static void PrintResultState(stt_result_event_e result_type);
};

}} /** end of is::stt */
