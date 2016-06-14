/**
 * Copyright (c) 2000 - 2016 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * PROPRIETARY/CONFIDENTIAL
 * This software is the confidential and proprietary information of SAMSUNG
 * ELECTRONICS ("Confidential Information"). You shall not disclose such
 * Confidential Information and shall use it only in accordance with the terms
 * of the license agreement you entered into with SAMSUNG ELECTRONICS.
 * SAMSUNG make no representations or warranties about the suitability of the
 * software, either express or implied, including but not limited to the
 * implied warranties of merchantability, fitness for a particular purpose, or
 * non-infringement. SAMSUNG shall not be liable for any damages suffered by
 * licensee as a result of using, modifying or distributing this software or
 * its derivatives.
 *
 */

#pragma once

#include <vector>

extern "C" {
	#include <stt.h>
}

#include <Evas.h>
#include <Ecore.h>

#include "MicEffector.h"

namespace is {
namespace ui {

class WInputSttMicEffect : public IMicEffector
{
	private :
		unsigned long long square_sum;
		unsigned int count;


	public :
		WInputSttMicEffect();

		virtual ~WInputSttMicEffect();

		std::vector<int> GetVolume();

		void ProcessingAnimationStart();

		void ProcessingAnimationStop();

		float GetDecibel() const;

		int ConvertLevel();

		void SetSttHandle(stt_h handle);

		void SetProgressBar(Evas_Object *progress);

		stt_h handle;

		Ecore_Timer *processing_effect_timer;
		Evas_Object *progressbar;
};

}} /** end of is::ui */
