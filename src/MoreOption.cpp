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

#include <assert.h>
#include <iostream>
#include <stdexcept>

#include <app.h>
#include <app_common.h>

#include "MoreOption.h"
#include "w-input-selector.h"
#include "w-input-stt-ise.h"
#include "w-input-stt-voice.h"

extern Evas_Object *g_setting_window;

MoreOption::MoreOption(Evas_Object *naviframe, void* voicedata)
	: nf(naviframe)
	, more_option_layout (NULL)
	, item (NULL)
	, option_opened (EINA_FALSE)
	, voicedata(voicedata) {

	/** todo. implement constructor */
}

MoreOption::~MoreOption() {

	/** todo. implement destructor */

//	if(more_option_layout)
//		evas_object_del(more_option_layout);

}

void MoreOption::Create() {

	try {
		AddLayout();
		AddMorePage();
	}
	catch(std::exception &e) {
		PRINTFUNC(DLOG_ERROR, "%s", e.what());
		assert(0);
	}
}

void MoreOption::AddLayout() {

	/** validation */
	if(!nf)
		PRINTFUNC(DLOG_ERROR, "Invalid naviframe.");

	/** add layout */
	more_option_layout = eext_more_option_add(nf);

	if(!more_option_layout)
		PRINTFUNC(DLOG_ERROR, "It's failed to create layout");

	evas_object_smart_callback_add(more_option_layout, "more,option,opened",
		[](void *data, Evas_Object *obj, void *event_info){

			PRINTFUNC(DLOG_DEBUG, "more option is opened!!! \n");

			if(!data) return;

			/**
			* if more option is completed, stt have to be stopped.
			*
			*/
			MoreOption *opt = (MoreOption *)data;
			VoiceData *vd = (VoiceData *) opt->voicedata;
			try {
				   if (vd->sttmanager->GetCurrent() == STT_STATE_RECORDING ||
						   vd->sttmanager->GetCurrent() == STT_STATE_PROCESSING) {
						   vd->sttmanager->Cancel();
				  }
			}
			catch(is::stt::SttException &e) {
				PRINTFUNC(DLOG_ERROR, "%s", e.what());
			}

			opt->option_opened = EINA_TRUE;

	}, this);

	evas_object_smart_callback_add(more_option_layout, "more,option,closed",
		[](void *data, Evas_Object *obj, void *event_info){

		PRINTFUNC(DLOG_DEBUG, "more option is closed!!! \n");

		MoreOption *opt = (MoreOption *)data;
		VoiceData *vd = (VoiceData *) opt->voicedata;

		activate_circle_scroller_for_stt_textbox(vd, EINA_TRUE);

		opt->option_opened = EINA_FALSE;


	}, this);

	evas_object_show(more_option_layout);


}

void MoreOption::SetContentLayout(Evas_Object *content) {

	/**
	 * Set content layout
	 *
	 */
	elm_object_part_content_set(more_option_layout, "elm.swallow.content", content);

	Elm_Object_Item *nit = NULL;
	nit = elm_naviframe_item_push(nf, NULL, NULL, NULL, more_option_layout, "empty");
	elm_naviframe_item_title_enabled_set(nit, EINA_FALSE, EINA_FALSE);

	elm_naviframe_item_pop_cb_set(nit,
		[](void *data, Elm_Object_Item *it)->Eina_Bool
		{
			MoreOption *opt = (MoreOption *)data;
			VoiceData *vd = (VoiceData *) opt->voicedata;

			if(vd->disclaimer  == 1){
				PRINTFUNC(DLOG_ERROR, "pop to top"); //inb case of (selector view -> disclaimer view-> stt view)

				if(opt->option_opened == EINA_FALSE){
					PRINTFUNC(DLOG_DEBUG, "pop_cb called in STT view\n");

					if(vd->sttmanager){
						if (vd->sttmanager->GetCurrent() == STT_STATE_CREATED ||
								   vd->sttmanager->GetCurrent() == STT_STATE_READY) {
							PRINTFUNC(DLOG_DEBUG, "STT_STATE_CREATED || STT_STATE_READY\n");

						} else if(vd->sttmanager->GetCurrent() == STT_STATE_RECORDING) {
							PRINTFUNC(DLOG_DEBUG, "STT_STATE_RECORDING\n");
							vd->effector->Stop();
							try{
								vd->sttmanager->Stop();
							}catch(is::stt::SttException &e){
								PRINTFUNC(DLOG_ERROR, "reason : %s", e.what());
							}

						} else if(vd->sttmanager->GetCurrent() == STT_STATE_PROCESSING) {
							PRINTFUNC(DLOG_DEBUG, "STT_STATE_RECORDING\n");
							try{
								vd->sttmanager->Cancel();
							}catch(is::stt::SttException &e){
								PRINTFUNC(DLOG_ERROR, "reason : %s", e.what());
							}
						}
					}
				}


				elm_naviframe_item_pop_to(elm_naviframe_bottom_item_get(vd->naviframe));
				vd->disclaimer	= 0;

				powerUnlock();
				_back_to_genlist_for_selector();

				return EINA_TRUE;
			}

			if(opt->option_opened == EINA_FALSE){
				PRINTFUNC(DLOG_DEBUG, "pop_cb called in STT view\n");

				if(vd->sttmanager){
					if (vd->sttmanager->GetCurrent() == STT_STATE_CREATED ||
							   vd->sttmanager->GetCurrent() == STT_STATE_READY) {
						PRINTFUNC(DLOG_DEBUG, "STT_STATE_CREATED || STT_STATE_READY\n");

					} else if(vd->sttmanager->GetCurrent() == STT_STATE_RECORDING) {
						PRINTFUNC(DLOG_DEBUG, "STT_STATE_RECORDING\n");
						vd->effector->Stop();
						try{
							vd->sttmanager->Stop();
						}catch(is::stt::SttException &e){
							PRINTFUNC(DLOG_ERROR, "reason : %s", e.what());
						}

					} else if(vd->sttmanager->GetCurrent() == STT_STATE_PROCESSING) {
						PRINTFUNC(DLOG_DEBUG, "STT_STATE_RECORDING\n");
						try{
							vd->sttmanager->Cancel();
						}catch(is::stt::SttException &e){
							PRINTFUNC(DLOG_ERROR, "reason : %s", e.what());
						}
					}

					_back_to_genlist_for_selector();

					destroy_voice();

					return EINA_TRUE;
				} else {
					PRINTFUNC(DLOG_DEBUG, "naviframe transition, not finished\n");
					return EINA_FALSE;
				}
			}
			return EINA_FALSE;
		}, this);
}

void MoreOption::Update()
{

	   char lang[6] = {0,};
	   strncpy(lang, ((VoiceData *)voicedata)->kbd_lang , 5);

       char* display_lang = get_lang_label(lang);
	   eext_more_option_item_part_text_set(item, "selector,sub_text", display_lang);

}


Evas_Object* MoreOption::AddLanguageIcon(Evas_Object *parent) {

	Evas_Object *icon = elm_image_add(parent);
	if (!icon) {
		PRINTFUNC(DLOG_ERROR, "It's failed to add image.");
	}

	std::string res_path;
	char *tmp_path = app_get_resource_path();
	if (tmp_path) {
		res_path = tmp_path;
		free(tmp_path);
	}

	std::string image_path = res_path + "images/prompt_ic_languages.png";

	elm_image_file_set(icon, image_path.c_str(), NULL);
	evas_object_size_hint_weight_set(icon, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(icon, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(icon);

	return icon;
}


void MoreOption::AddMorePage() {

	Evas_Object *img;

	item  = eext_more_option_item_append(more_option_layout);

	/* Slider content */
	eext_more_option_item_part_text_set(item, "selector,main_text", _("WDS_IME_HEADER_INPUT_LANGUAGES_ABB"));

	img = AddLanguageIcon(more_option_layout);

	eext_more_option_item_part_content_set(item, "item,icon", img);

	evas_object_smart_callback_add(more_option_layout, "item,clicked",
	[](void *data, Evas_Object *obj, void *event_info)
	{

		PRINTFUNC(DLOG_DEBUG, "item,clicked");
		if(g_setting_window == NULL)
			create_setting_window(obj);
	},NULL);

	evas_object_smart_callback_add(more_option_layout, "item,selected",
	[](void *data, Evas_Object *obj, void *event_info)
	{

		PRINTFUNC(DLOG_DEBUG, "item,selected");

		Eext_Object_Item *selected_item = (Eext_Object_Item *)event_info;

		//for custom accessibility
		if (elm_config_access_get())
		{
			Evas_Object *panel = elm_object_part_content_get(obj, "elm.swallow.right");
			if (!panel) return;

			Evas_Object *rotary_selector = elm_object_content_get(panel);
			if (!rotary_selector) return;

			Evas_Object *content = (Evas_Object *)edje_object_part_object_get(elm_layout_edje_get(rotary_selector), "content");
			if (!content) return;

			Evas_Object *content_access = elm_access_object_get(content);
			if (!content_access) return;

			if (eext_more_option_item_part_text_get(selected_item, "selector,main_text"))
			{
				std::string text;
				if(eext_more_option_item_part_text_get(selected_item, "selector,main_text"))
					text = text + std::string(eext_more_option_item_part_text_get(selected_item, "selector,main_text")) + " ";

				if(eext_more_option_item_part_text_get(selected_item, "selector,main_text"))
					text = text + std::string(eext_more_option_item_part_text_get(selected_item, "selector,sub_text"));

				elm_access_info_set(content_access, ELM_ACCESS_INFO, text.c_str());
			}
		}
	},NULL);


}

