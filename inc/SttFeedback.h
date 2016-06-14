/*
 * Copyright (c)  2010 Samsung Electronics, Inc.
 * All rights reserved.
 *
 * This software is a confidential and proprietary information
 * of Samsung Electronics, Inc. ("Confidential Information").  You
 * shall not disclose such Confidential Information and shall use
 * it only in accordance with the terms of the license agreement
 * you entered into with Samsung Electronics.
 */


#pragma once


#include "SttManager.h"

namespace is {
namespace stt {

class SttFeedback : public ISttFeedback
{
	public :
		SttFeedback();
		virtual ~SttFeedback();

		virtual void OnResult(
			std::string asrtype,
			stt_result_event_e event,
			std::vector<std::string> results,
			std::string msg
		);

		virtual void AutoStart(void);

		virtual void SttIdle(void);

		virtual void SttRecording(void);

		virtual void SttProcessing(void);

		virtual void OnError(stt_error_e reason);

		void SetVoiceData(void *data);

		void *owner;
};

}} /** end of is::stt*/
