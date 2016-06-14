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

#include <Elementary.h>
#include <efl_extension.h>

#define NUM_ITEMS 1

class MoreOption
{
	Evas_Object *nf;
	Evas_Object *more_option_layout;
	Eext_Object_Item *item;

	Eina_Bool option_opened;
	void *voicedata;

	public :
		MoreOption(Evas_Object *naviframe, void *voicedata);

		~MoreOption();

		void Create();

		/**
		 * Set option
		 *
		 */
		void SetContentLayout(Evas_Object *content);

		void Update();

		Evas_Object *getMoreOptionLayout(){return more_option_layout;};

	private :
		/**
		 * Main layout
		 *
		 */
		void AddLayout();

		/**
		 * Add layout for options
		 *
		 */
		void AddMorePage();

		/**
		 * Add icon style button ( used in more option )
		 *
		 * @param parent evas object parent
		 * @return launguage button object
		 *
		 */
		Evas_Object *AddLanguageIcon(Evas_Object *parent);

};

