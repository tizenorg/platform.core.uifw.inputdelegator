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

#include <app.h>
#include <Elementary.h>
#include <string>
#include <efl_assist.h>

#include "Debug.h"
#include "w-input-selector.h"

#define EMOTICON_CNT 27

static int previous_icon = -1;

using namespace std;

typedef struct {
    int code;
    char* name;
}Emoticon;

Emoticon emoticon_info[EMOTICON_CNT] = {
    {0x1f44c, "IDS_IME_BODY_OK_HAND_SIGN_M_EMOTICON_NAME_TTS"},
    {0x1f44d, "IDS_IME_BODY_THUMBS_UP_SIGN_M_EMOTICON_NAME"},
    {0x1f44e, "IDS_IME_BODY_THUMBS_DOWN_SIGN_M_EMOTICON_NAME"},
    {0x1f604, "IDS_IME_BODY_SMILING_FACE_WITH_OPEN_MOUTH_AND_SMILING_EYES_M_EMOTICON_NAME"},
    {0x1f606, "IDS_IME_BODY_SMILING_FACE_WITH_OPEN_MOUTH_AND_TIGHTLY_CLOSED_EYES_M_EMOTICON_NAME"},
    {0x1f60a, "IDS_IME_BODY_SMILING_FACE_WITH_SMILING_EYES_M_EMOTICON_NAME"},
    {0x1f60d, "IDS_IME_BODY_SMILING_FACE_WITH_HEART_SHAPED_EYES_M_EMOTICON_NAME"},
    {0x1f61a, "IDS_IME_BODY_KISSING_FACE_WITH_CLOSED_EYES_M_EMOTICON_NAME"},
    {0x1f61c, "IDS_IME_BODY_FACE_WITH_STUCK_OUT_TONGUE_AND_WINKING_EYE_M_EMOTICON_NAME"},
    {0x1f620, "IDS_IME_BODY_ANGRY_FACE_M_EMOTICON_NAME"},
    {0x1f621, "IDS_IME_BODY_POUTING_FACE_M_EMOTICON_NAME"},
    {0x1f622, "IDS_IME_BODY_CRYING_FACE_M_EMOTICON_NAME"},
    {0x1f624, "IDS_IME_BODY_FACE_WITH_LOOK_OF_TRIUMPH_M_EMOTICON_NAME"},
    {0x1f625, "IDS_IME_BODY_DISAPPOINTED_BUT_RELIEVED_FACE_M_EMOTICON_NAME"},
    {0x1f62a, "IDS_IME_BODY_SLEEPY_FACE_M_EMOTICON_NAME"},
    {0x1f62b, "IDS_IME_BODY_TIRED_FACE_M_EMOTICON_NAME"},
    {0x1f631, "IDS_IME_BODY_FACE_SCREAMING_IN_FEAR_M_EMOTICON_NAME"},
    {0x1f632, "IDS_IME_BODY_ASTONISHED_FACE_M_EMOTICON_NAME"},
    {0x1f637, "IDS_IME_BODY_FACE_WITH_MEDICAL_MASK_M_EMOTICON_NAME"},
    {0x1f495, "IDS_IME_BODY_TWO_HEARTS_M_EMOTICON_NAME"},
    {0x1f43d, "IDS_IME_BODY_PIG_NOSE_M_EMOTICON_NAME"},
    {0x1f415, "IDS_IME_BODY_DOG_M_EMOTICON_NAME"},
    {0x1f408, "IDS_IME_BODY_CAT_M_EMOTICON_NAME"},
    {0x1f414, "IDS_IME_BODY_CHICKEN_M_EMOTICON_NAME"},
    {0x1f433, "IDS_IME_BODY_SPOUTING_WHALE_M_EMOTICON_NAME"},
    {0x1f43c, "IDS_IME_BODY_PANDA_FACE_M_EMOTICON_NAME"},
    {0x1f42f, "IDS_IME_BODY_TIGER_FACE_M_EMOTICON_NAME"},
};

const char * get_emoticon_file_name(int index)
{
    static string path = get_resource_path() + string("images/u00000.png");

    int ipos = path.size()-9;
    char str_emoticon_code[10] = {0};
    snprintf(str_emoticon_code, sizeof(str_emoticon_code), "%x", emoticon_info[index].code);
    path.erase(ipos, 5);
    path.insert(ipos, str_emoticon_code);
    return path.c_str();
}

static Eina_Bool _custom_back_cb(void *data, Elm_Object_Item *it)
{
    _back_to_genlist_for_selector();
    return EINA_TRUE;
}

static Eina_Bool
_rotary_selector_rotary_cb(void *data, Evas_Object *obj, Eext_Rotary_Event_Info *info)
{
    PRINTFUNC(DLOG_DEBUG, "%s", __func__);

   if (info->direction == EEXT_ROTARY_DIRECTION_CLOCKWISE){
		evas_object_smart_callback_call(obj, "item,selected", (void*)data);
		eext_rotary_object_event_callback_del(obj, _rotary_selector_rotary_cb);
   }

	return ECORE_CALLBACK_PASS_ON;
}

static char *_access_info_prepend_cb(void *data, Evas_Object *obj)
{
   PRINTFUNC(DLOG_DEBUG, "%s", __func__);

   std::string text;

   if (previous_icon == 10 || previous_icon == 11){
		text = std::string("Page") + " " +std::string(dgettext(PACKAGE, emoticon_info[previous_icon].name));
		text = text + " " + gettext("WDS_TTS_TBBODY_DOUBLE_TAP_TO_SEND");
   } else {
	   text = std::string("Page");
   }
   return strdup(text.c_str());
}

static void _rotary_selector_item_clicked(void *data, Evas_Object *obj, void *event_info)
{
    PRINTFUNC(DLOG_DEBUG, "%s", __func__);
    App_Data* ad = (App_Data*) data;
    if (!ad)
        return;

    Eext_Object_Item *selected_item = (Eext_Object_Item *)event_info;
    Eina_List *rotary_selector_list = (Eina_List *)eext_rotary_selector_items_get(obj);

    int i = 0;
    Eina_List *l = rotary_selector_list;
    Eext_Object_Item *item = (Eext_Object_Item *)eina_list_data_get(l);

    for (i = 0; l != NULL; i++) {
        if (selected_item == item)
            break;

        l = eina_list_next(l);
        item = (Eext_Object_Item *)eina_list_data_get(l);
    }

    int length;
    const Eina_Unicode unicode_event[2] = { emoticon_info[i].code, 0 };
    char* utf_8 = eina_unicode_unicode_to_utf8(unicode_event, &length);

    reply_to_sender_by_callback((const char*)utf_8, "emoticon");

    PRINTFUNC(SECURE_DEBUG, "[%d]%s", i, utf_8);
    if (utf_8)
        free(utf_8);

	if(ad->reply_type == REPLY_APP_NORMAL)
	    elm_exit();
}

static void _rotary_selector_item_selected(void *data, Evas_Object *obj, void *event_info)
{
    PRINTFUNC(DLOG_DEBUG, "%s", __func__);
}

void ise_show_emoticon_popup_rotary(void *data)
{
    PRINTFUNC(DLOG_DEBUG, "%s", __func__);
    App_Data* ad = (App_Data*) data;
    if (!ad)
        return;

    Eext_Object_Item *first_it;

    Evas_Object *rotary_selector = eext_rotary_selector_add(ad->naviframe);
//    uxt_theme_object_replace_color(rotary_selector, "B01153", "AO0117");
    PRINTFUNC(DLOG_DEBUG, "replace color");
    for (int i = 0; i < EMOTICON_CNT; ++i)
    {
        Evas_Object *img = NULL;
        Eext_Object_Item *item = eext_rotary_selector_item_append(rotary_selector);

        if (i == 0) first_it = item;

        img = elm_image_add(rotary_selector);
        elm_image_file_set(img, get_emoticon_file_name(i), NULL);
        eext_rotary_selector_item_part_content_set(item, "item,bg_image", EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL, img);

        img = elm_image_add(rotary_selector);
        elm_image_file_set(img, get_emoticon_file_name(i), NULL);
        eext_rotary_selector_item_part_content_set(item, "selector,icon", EEXT_ROTARY_SELECTOR_ITEM_STATE_NORMAL, img);
    }

    evas_object_smart_callback_add(rotary_selector, "item,selected", _rotary_selector_item_selected, rotary_selector);
    evas_object_smart_callback_add(rotary_selector, "item,clicked", _rotary_selector_item_clicked, (void*)ad);

    Elm_Object_Item *nf_item = elm_naviframe_item_push(ad->naviframe, NULL, NULL, NULL, rotary_selector, "empty");
    elm_naviframe_item_pop_cb_set(nf_item, _custom_back_cb, NULL);
    eext_rotary_object_event_activated_set(rotary_selector, EINA_TRUE);

    PRINTFUNC(DLOG_DEBUG, "%s", __func__);
}
