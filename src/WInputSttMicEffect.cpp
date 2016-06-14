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

#include <cmath>
#include <algorithm>
#include <numeric>

#include <efl_extension.h>
#include "WInputSttMicEffect.h"

namespace {
template<class T>
static unsigned long long SumSquare(unsigned long long const& a, T const& b) {
	return a + b*b;
}

const double MAX_AMPLITUDE_MEAN_16 = 23170.115738161934;
const double MAX_AMPLITUDE_MEAN_08 = 89.803909382810;

unsigned int sample_count = SAMPLE_COUNT;

}

using namespace is::ui;



WInputSttMicEffect::WInputSttMicEffect()
	: processing_effect_timer(NULL)
	, progressbar(NULL)
	, count (5)
	, square_sum (0)
	, handle (NULL) {

}



WInputSttMicEffect::~WInputSttMicEffect() {

	ProcessingAnimationStop();
}



std::vector<int> WInputSttMicEffect::GetVolume() {

	std::vector<int> result;

	short pcm[512] = {0};
	int size = 0;
	int ret = 0;

//	ret = stt_get_spectrum(handle, (void *) pcm, &size);


	count = 5;

	if(STT_ERROR_NONE != ret) {
		PRINTFUNC(DLOG_ERROR, "stt_audio_snapshot invalid (%d)", ret);
	}
	else {
		unsigned int level = 0;
		unsigned int step = (unsigned int) (size/2/sample_count);

		for (unsigned int k = 0; k < sample_count; k++ ){
			square_sum = std::accumulate(pcm + k*step, pcm + k*step + 5, 0ull, SumSquare<short>);
			level = ConvertLevel();
			result.push_back(level);
		}
	}

	return result;
}



float WInputSttMicEffect::GetDecibel() const
{
	float rms = std::sqrt( square_sum/count );
	return 20.0*log10(rms);
}



int WInputSttMicEffect::ConvertLevel()
{
	float db = GetDecibel();

	if ( db <= 30.0 ){
		return 0;
	}
	else if ( db <= 33.3 ){
		return 1;
	}
	else if ( db <= 36.6 ){
		return 2;
	}
	else if ( db <= 40 ){
		return 3;
	}
	else if ( db <= 43.3 ){
		return 4;
	}
	else if ( db <= 46.6 ){
		return 5;
	}
	else if ( db <= 50 ){
		return 6;
	}
	else if ( db <= 53.3 ){
		return 7;
	}
	else if ( db <= 56.6 ){
		return 8;
	}
	else if ( db <= 60 ){
		return 9;
	}
	else{
		return 10;
	}
}



void WInputSttMicEffect::ProcessingAnimationStart() {

	elm_progressbar_pulse(progressbar, EINA_TRUE);

	processing_effect_timer = ecore_timer_add ( 0.1,
		[](void *data)->Eina_Bool
		{
			if(!data) return ECORE_CALLBACK_CANCEL;
/*
			WInputSttMicEffect *effect = (WInputSttMicEffect *) data;
			Evas_Object *progressbar = effect->progressbar;

		   	double progress = eext_circle_value_get(progressbar);

			if (progress < 100)
				progress += 5.0;
			else
				progress = 0.0;

			eext_circle_value_set(progressbar, progress);
*/
			return ECORE_CALLBACK_RENEW;
		}, this);
}



void WInputSttMicEffect::ProcessingAnimationStop() {

	if(processing_effect_timer)
	{
		ecore_timer_del(processing_effect_timer);
		processing_effect_timer = NULL;
	}
	elm_progressbar_pulse(progressbar, EINA_FALSE);
}



void WInputSttMicEffect::SetSttHandle(stt_h handle) {

	this->handle = handle;
}



void WInputSttMicEffect::SetProgressBar(Evas_Object *progress) {

	this->progressbar = progress;

}
