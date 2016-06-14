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

