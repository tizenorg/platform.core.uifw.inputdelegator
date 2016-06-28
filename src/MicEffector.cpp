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

#include <string.h>
#include <efl_assist.h>
#include "MicEffector.h"

using namespace is::ui;

/**
 * animation configuration values
 *
 */
static size_t start_stop_anim_count = 28;
static size_t spectrum_count = SAMPLE_COUNT;
static float spectrum_posx = 40.0f;

static float timeout_s = 1.0f / 60.0f;

#define MATH_PI (3.141592)

double cubic_easy_in_out(double index, double start, double end, double duration)
{
	index /= duration/2;
	if (index < 1)
		return end/2*index*index*index + start;

	index -= 2;
	return end/2*(index*index*index + 2) + start;
}

double cubic_easy_in(double index, double start, double end, double duration)
{
	index /= duration;
	return end*index*index*index*index*index + start;
}

double cubic_easy_out(double index, double start, double end, double duration)
{
	index /= duration;
	index--;
	return end*(index*index*index + 1) + start;
}


/**
 * Constructor
 *
 * #1. Create ea_vector handle for drawing effect.
 *     ( 1 canvas, 1 paint and 45 paths )
 * #2. Drawing empty frame to avoid broken screen.
 *
 */

MicEffector::MicEffector(Evas_Object *canvas, Evas_Object *layout, IMicEffector& effect)
	: drawcount(0)
	, forcestop(false)
	, started(false)
	, timer(NULL)
	, layout(layout)
	, ieffect(effect)
	, fake(false)
{
//	path = ea_vector_path_create();
//	paint = ea_vector_paint_create();
//	ea_vector_paint_set_style(paint, EA_VECTOR_PAINT_STYLE_STROKE);
//	ea_vector_paint_set_line_cap(paint, EA_VECTOR_PAINT_LINE_CAP_ROUND);
//	ea_vector_paint_set_line_join(paint, EA_VECTOR_PAINT_LINE_JOIN_ROUND);
//	ea_vector_paint_set_line_width(paint, 3.0);
//	ea_vector_paint_set_color(paint, 1.0f, 1.0f, 1.0f, 1.0f);

//	this->canvas = ea_vector_canvas_create(canvas);

	DrawDummyFrame();
};



/**
 * Destructor
 *
 * #1. Destroy ea_vector handle
 *
 */
MicEffector::~MicEffector()
{
	if(timer)
	{
		ecore_timer_del(timer);
		timer = NULL;
	}

//	ea_vector_path_destroy(path);
//	ea_vector_paint_destroy(paint);
//	ea_vector_canvas_destroy(canvas);
}



/**
 * Draw empty frame to remove or initiate screen
 *
 */
void MicEffector::DrawDummyFrame()
{
//	ea_vector_path_reset(path);
//	ea_vector_path_move_to(path, 0, 0);
//	ea_vector_path_line_to(path, 0, 0);
//	ea_vector_canvas_draw(canvas, path, paint);
}



/**
 * Draw Que animation
 *
 * In case of start, it shows 2 more dots per one time.
 * In case of stop, it remove 2 dots per one time.
 *
 * it works during 22 times.
 *
 */
void MicEffector::DrawQue(int idx, bool is_start)
{
	float margin = spectrum_posx;
	float posx = 0.0;

	double speed = cubic_easy_out(idx + 1.0, 0.0, 23.0, 23);

	unsigned int start = start_stop_anim_count - (int) speed;
	unsigned int end = start_stop_anim_count + (int) speed;

	double opacity;

	if (is_start) {
		opacity = cubic_easy_out(idx, 0.0, 1.0, 26.0);
	} else {
		opacity = cubic_easy_out(idx, 0, 1.0, 26.0);
	}

//	ea_vector_path_reset(path);

	for(unsigned int i = start; i < end; i++)
	{
		posx = margin + (i * 5);

//		ea_vector_path_move_to(path, posx, 37.0f);
//		ea_vector_path_line_to(path, posx, 38.0f);

//		ea_vector_paint_set_color(paint, 1.0f, 1.0f, 1.0f, opacity);
	}

//	ea_vector_canvas_draw(canvas, path, paint);
}



float MicEffector::GetAmplifyValue(unsigned int idx)
{
	float amplify = 0.0;

	int max[SAMPLE_COUNT] = {
		/**
		 * dot "A" (9)
		 *
		 */
		1, 1, 1, 1,


		1, 1, 1, 1, 1, 1, 1, 1, 1,

		/**
		 * dot "B" (9)
		 *
		 */
		10, 8, 2, 3, 10, 11, 6, 12, 4,

		/**
		 * dot "C" (15)
		 *
		 */
		 3, 5, 9, 12, 11, 8, 14, 15, 13, 11, 12, 6, 8, 3, 2,

		/**
		 * reverse dot "B" (9)
		 *
		 */
		 4, 12, 6, 11, 10, 3, 2, 8, 10,

		/**
		 * dot "A" (9)
		 *
		 */
		1, 1, 1, 1, 1, 1, 1, 1, 1,

		1, 1, 1, 1
	};

	amplify = (float) max[idx] / 10.0f * 1.9f;

	return amplify;
}

/**
 * Draw effect animation
 *
 * It draws volume effect. during 5 times.
 * Center of effect area, it applies amplified value.
 *
 */
void MicEffector::DrawWave(unsigned int idx, int amount, int prev_amount, double opacity, bool is_lastcmd)
{
	float ratio = GetAmplifyValue(idx);

	float am = ((float) amount) * ratio;
	float pam = ((float) prev_amount) * ratio;
	float cnt = (float) drawcount;

	float posx = spectrum_posx;

	float height = pam > am?
		pam - cubic_easy_in_out(cnt + 1.0, am, pam, 7):
		cubic_easy_in_out(cnt + 1.0, pam, am, 7);

	posx += idx * 5;

//	ea_vector_path_move_to(path, posx, (37.0f - (height / 2.0)));
//	ea_vector_path_line_to(path, posx, (38.0f + (height / 2.0)));

	if (is_lastcmd) {
//		ea_vector_paint_set_color(paint, 0.1451f, 0.204f, 0.255f, opacity);//RGB = 37:52:65
	} else {
//		ea_vector_paint_set_color(paint, 1.0f, 1.0f, 1.0f, opacity);//RGB: 255 255 255
	}
}



/**
 * Effect Start
 *
 */
void MicEffector::Start()
{
	if(timer) {
		ecore_timer_del(timer);
		timer = NULL;
	}

	drawcount = 0;

	prev.clear();
	current.clear();

	for(size_t i = 0; i < spectrum_count; i++)
	{
		prev.push_back(0);
		current.push_back(0);
	}

	Listening();

	/**
	 * Que animation
	 *
	 */
	timer = ecore_timer_add(timeout_s,
				[](void *data)->Eina_Bool
				{
					MicEffector *effector = static_cast<MicEffector*>(data);

					effector->DrawQue(effector->drawcount);

					if(effector->drawcount < (int) start_stop_anim_count) {
						effector->drawcount += 2;
						return ECORE_CALLBACK_RENEW;
					} else {
						for(unsigned int i = 0; i < spectrum_count; i++)
							effector->DrawWave(i, 0, 0);

//						ea_vector_canvas_draw(effector->canvas, effector->path, effector->paint);

						effector->drawcount = 0;
						effector->timer = NULL;
						effector->VolumeCheck();
						effector->Effect();
						return ECORE_CALLBACK_CANCEL;
					}
				},
				this);
}

/**
 * Volume effect
 *
 */
void MicEffector::Effect(bool fake)
{
	/**
	 * Volume effect animation
	 *
	 */
	if ( timer ) {
			ecore_timer_del(timer);
			timer = NULL;
	}

	timer = ecore_timer_add(timeout_s,
				[](void *data)->Eina_Bool
				{
					MicEffector *effector = static_cast<MicEffector *>(data);

					bool is_empty_prev = effector->prev.empty();


//					ea_vector_path_reset(effector->path);

					for(unsigned int i = 0; i < effector->current.size(); i++)
					{
						if (is_empty_prev)
							effector->DrawWave(i, effector->current.at(i), 0);
						else
							effector->DrawWave(i, effector->current.at(i), effector->prev.at(i));
					}
//					ea_vector_canvas_draw(effector->canvas, effector->path, effector->paint);

					if (effector->drawcount < 7)	{
						effector->drawcount++;
					} else {
						effector->drawcount = 0;
						effector->VolumeCheck(effector->fake);
					}

					return ECORE_CALLBACK_RENEW;
				}, this);
}



/**
 * Stop volume animation effect
 *
 */
void MicEffector::Stop(bool forced)
{
	if(timer)
	{
		ecore_timer_del(timer);
		timer = NULL;
	}

	if(!started)
	{
		Idle();
		return;
	}

	forcestop = forced;

	timer = ecore_timer_add(timeout_s,
		[](void *data)->Eina_Bool
		{
			MicEffector *effector = static_cast<MicEffector*>(data);

			effector->DrawQue(start_stop_anim_count - effector->drawcount, false);

			if(effector->drawcount < (int) start_stop_anim_count) {
				effector->drawcount += 2;
				return ECORE_CALLBACK_RENEW;
			} else {
				if(!effector->forcestop) {
					effector->Processing();
				} else {
					effector->Idle();
				}

				effector->forcestop = false;
				effector->drawcount = 0;
				effector->timer = NULL;
				return ECORE_CALLBACK_CANCEL;
			}
		}, this);
}

/**
 * Signal. Refresh volume effect
 *
 */
void MicEffector::VolumeCheck(bool fake)
{
	std::vector<int> volumes;

	this->fake = fake;

	if(!fake) {
		volumes = ieffect.GetVolume();
	} else {
		for(unsigned int i = 0; i < spectrum_count; i++) {
			volumes.push_back(rand() % 2);
		}
	}

	prev.clear();
	prev.assign(current.begin(), current.end());

	current.clear();
	current.assign(volumes.begin(), volumes.end());
}



/**
 * Signal. Listening effect
 *
 */
void MicEffector::Listening()
{
	started = true;

	elm_object_signal_emit(layout, "elm,state,eq,show", "eq");
	elm_object_signal_emit(layout, "elm,state,listening", "elm");
}



/**
 * Signal. Processing effect
 *
 */
void MicEffector::Processing()
{
	started = false;

	elm_object_signal_emit(layout, "elm,state,eq,hide", "eq");
	elm_object_signal_emit(layout, "elm,state,processing", "elm");

	ieffect.ProcessingAnimationStart();
}



/**
 * Signal. Idle effect
 *
 */
void MicEffector::Idle()
{
	const char *text;
	const char *state;
	double val;

	started = false;

	elm_object_signal_emit(layout, "elm,state,eq,hide", "eq");

	text = elm_object_part_text_get(layout, "elm.text");
	state = edje_object_part_state_get(elm_layout_edje_get(layout), "guide_text_block", &val);

	if ((text && strlen(text) > 0) && (state && !strcmp(state, "bottom")))
		elm_object_signal_emit(layout, "elm,state,init_message", "elm");
	else
		elm_object_signal_emit(layout, "elm,state,init", "elm");

	ieffect.ProcessingAnimationStop();
}

