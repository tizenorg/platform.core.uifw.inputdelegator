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

#include <vector>
#include <efl_assist.h>

#include "Debug.h"

#define SAMPLE_COUNT 59

namespace is {	/** name space input selector */
namespace ui {	/** name space ui */

/**
 * Interface for external controller
 *
 */
class IMicEffector
{
	public :
		virtual ~IMicEffector() = 0;
		virtual std::vector<int> GetVolume() = 0;
		virtual void ProcessingAnimationStart() = 0;
		virtual void ProcessingAnimationStop() = 0;
};
inline IMicEffector::~IMicEffector() { };

/**
 * Effect & Effect Controller
 *
 */
class MicEffector
{
	public :
		MicEffector(Evas_Object *canvas, Evas_Object *layout, IMicEffector& effect);
		~MicEffector();

		/**
		 * Start Effect.
		 *
		 * It support signaling to edje and start animation.
		 *
		 */
		void Start();

		/**
		 * Equalizer Effect animation.
		 *
		 * @param volumes sampled volume values.
		 * @param fake default false. if it set as true, it works using fixed volume values.
		 *
		 */
		void Effect(bool fake = false);

		/**
		 * Stop Effect.
		 *
		 * It support signaling to edje and stop animation.
		 *
		 * @param forced default false. if it set as true, it should go idle state without processing.
		 *
		 */
		void Stop(bool forced = false);

		/**
		 * Get volumes via ieffect and refresh equalizer effect.
		 *
		 * @param fake default false. if it set as true, it works using fixed volume values.
		 *
		 */
		void VolumeCheck(bool fake = false);

		/**
		 * Draw image to show que animation.
		 *
		 * @param idx frame index value
		 *
		 */
		void DrawQue(int idx, bool is_start=true);

		/**
		 * Draw image to show que animation.
		 *
		 * @param idx frame index value
		 * @param amount current volume amount
		 * @param prev_amount previous volume amount
		 * @param opacity opacity value for effect
		 * @param is_lastcmd
		 *
		 */
		void DrawWave(unsigned int idx, int amount, int prev_amount, double opacity = 1.0f, bool is_lastcmd=false);

		/**
		 * Get ratio for tuned volume value.
		 *
		 * @param idx volume stick index
		 * @return ratio for each volume stick
		 *
		 */
		float GetAmplifyValue(unsigned int idx);

	private :
		/**
		 * Support idle state actions.
		 *
		 */
		void Idle();

		/**
		 * Support listening state actions.
		 *
		 */
		void Listening();

		/**
		 * Support processing state actions.
		 *
		 */
		void Processing();

		/**
		 * Draw dummy frame to avoid broken frame showing.
		 * It have to be call one time internally.
		 *
		 */
		void DrawDummyFrame();

		/**
		 * Efl vector canvas handle
		 *
		 */
//		ea_vector_canvas_h *canvas;
//		ea_vector_path_h *path;
//		ea_vector_paint_h *paint;

		/**
		 * Volume values.
		 *
		 */
		std::vector<int> prev;
		std::vector<int> current;

	public :
		int startcount;
		int drawcount;
		bool forcestop;
		bool started;
		bool fake;

		/**
		 * timer handle
		 *
		 */
		Ecore_Timer *timer;

		/**
		 * mic widget layout
		 *
		 */
		Evas_Object *layout;

		/**
		 * interface with app
		 *
		 */
		IMicEffector& ieffect;

};

}} /** end of is::ui */

