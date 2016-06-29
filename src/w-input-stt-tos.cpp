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

#include <stdio.h>
#include <Elementary.h>
#include <Ecore.h>
#include <efl_assist.h>
#include <app_common.h>
#include <app.h>
#include <string>
#include <efl_extension.h>
//#include <ui_extension.h>


#include <stt.h>

#include "Debug.h"
#include "w-input-selector.h"
#include "w-input-stt-ise.h"
#include "w-input-stt-voice.h"
#include "w-input-stt-tos.h"
#include "w-input-stt-tos-text.h"

using namespace std;

#define _EDJ(x)  elm_layout_edje_get(x)

static Evas_Object *g_circle_object_first = NULL;

#define ISE_PREFERENCE_AGREEMENT "wearable_input_agreement"
#define VCONFKEY_USER_AGREEMENT "db/wms/user_agreement/svoice"
#define VCONFKEY_USER_AGREED_TOS            "db/private/ise/stt/tos"

#define N66_URL   "https://account.samsung.com/membership/pp"

char *nuance_support_language[12][2] = {
        {"en_GB", "en"}, // fallback
        {"de_DE", "de"},
        {"en_US", "en"},
        {"es_ES", "es"},
        {"es_US", "es"},
        {"fr_FR", "fr"},
        {"it_IT", "it"},
        {"pt_BR", "pt"},
        {"ru_RU", "ru"},
        {"zh_CN", "zh"},
        {"ja_JP", "ja"},
        {"ko_KR", "ko"},
};


std::string replaceAll(std::string str, const std::string& from, const std::string& to)
{
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }

    return str;
}

static void __bt_connection_result_cb(app_control_h request, app_control_h reply, app_control_result_e result, void *user_data)
{
    App_Data* ad = (App_Data*)user_data;

    char *val = NULL;

    if (reply == NULL) {
        PRINTFUNC(DLOG_ERROR, "service_h is NULL");
        return;
    }

    app_control_get_extra_data(reply, "__BT_CONNECTION__", &val);
    if (val) {
        if (strcmp(val, "Connected") == 0) {
            PRINTFUNC(DLOG_ERROR, "BT Connected");
        } else {
            PRINTFUNC(DLOG_ERROR, "BT Not Connected");
        }
        free(val);
    }

    if (ad && ad->app_type == APP_TYPE_STT) {
        PRINTFUNC(DLOG_DEBUG, "APP_TYPE_STT so exit here.");
        powerUnlock();
        elm_exit();
    }
}

void launch_bt_connection_popup(App_Data* data){
    App_Data* ad = (App_Data*)data;

    PRINTFUNC(DLOG_DEBUG, "Need to launch BT connection popup");
    app_control_h app_control;
    app_control_create(&app_control);
    app_control_set_app_id(app_control, "com.samsung.bt-connection-popup");
    app_control_add_extra_data(app_control, "msg", "user_defined");
    app_control_add_extra_data(app_control, "text", gettext("WDS_ST_TPOP_CONNECT_VIA_BLUETOOTH_TO_REVIEW_AND_ACCEPT_THE_LEGAL_NOTICE_ABB"));
    app_control_add_extra_data(app_control, "title", PACKAGE);
    app_control_send_launch_request(app_control, __bt_connection_result_cb, NULL);
    app_control_destroy(app_control);
}


Eina_Bool set_tos_N66_agreed(Eina_Bool bSet)
{
    int ret = vconf_set_bool(VCONFKEY_USER_AGREEMENT, bSet);
    if ( ret != VCONF_OK ) {
        PRINTFUNC(DLOG_ERROR, "vconf_set_bool failed ... [%d]!!!", ret);
        return EINA_FALSE;
    }

    return EINA_TRUE;
}

Eina_Bool is_tos_N66_agreed()
{
    if (1) return EINA_TRUE;

    int bVal = 0;
    int ret = 0;

    ret = vconf_get_bool(VCONFKEY_USER_AGREEMENT, &bVal);
    if (ret != VCONF_OK) {
        PRINTFUNC(DLOG_ERROR, "VCONFKEY_USER_AGREEMENT vconf_get_bool fail: %d", ret);
    }

    return bVal? EINA_TRUE : EINA_FALSE;
}

Eina_Bool is_sap_connection()
{
    int wms_connected = 0;
    Eina_Bool ret = false;

    vconf_get_int(VCONFKEY_WMS_WMANAGER_CONNECTED, &wms_connected);

    if (wms_connected) {
        int conn_type = -1;
        vconf_get_int("memory/private/sap/conn_type", &conn_type);
        if (conn_type == 0) {
            char *vendor_type = vconf_get_str("db/wms/host_status/vendor");
            if (vendor_type && !strcmp("LO", vendor_type)) {
                ret = true;
            } else {
                ret = false;
            }
            if (vendor_type)
                free(vendor_type);
        } else if (conn_type == 1) {
            ret = true;
        } else {
            ret = false;
        }
    } else {
        PRINTFUNC(DLOG_DEBUG, "standalone mode");
        ret = false;
    }
    return ret;
}


static void
_ise_tos_launch_web_link(std::string url)
{
/*
    wnoti_error_e    eError    = WNOTI_ERROR_NONE;

    eError    = wnoti_service_connect();
    if( eError != WNOTI_ERROR_NONE )
    {
        PRINTFUNC(DLOG_ERROR, "wnoti_service_connect failed : [%d]!!!", eError);
        return;
    }

    eError    = wnoti_request_to_browse((char*)(char *) "Samsung Keyboard", url.c_str());
    if( eError != WNOTI_ERROR_NONE )
    {
        PRINTFUNC(DLOG_ERROR, "wnoti_request_to_browse failed!!!", eError);
    }

    eError    = wnoti_service_disconnect();
    if( eError != WNOTI_ERROR_NONE )
    {
        PRINTFUNC(DLOG_ERROR, "wnoti_service_disconnect failed : [%d]!!!", eError);
    }
*/
}

static void show_terms(void *data, Evas_Object *obj, void *event_info)
{
    if (!obj) return;
    if (!data) return;
    if (!event_info) return;

    Evas_Object *parent_win = (Evas_Object *)data;

    int wms_connected = 0;
    vconf_get_int(VCONFKEY_WMS_WMANAGER_CONNECTED, &wms_connected);

    if (wms_connected == 1) {
         // companion mode
        Evas_Smart_Cb_Description * desc = (Evas_Smart_Cb_Description*)event_info;
        std::string tempurl = desc->name;

        std::string::size_type n = tempurl.find_first_not_of(" \"");
        if (n != std::string::npos) {
            tempurl = tempurl.substr(n, tempurl.length());
        }

        n = tempurl.find_last_not_of(" \"");
        if (n != std::string::npos) {
            tempurl = tempurl.substr(0, n + 1);
        }

        //----------------------url construction-----------------

            int find_it = 0;
            char *locale = vconf_get_str(VCONFKEY_LANGSET);
            if (locale) {
                PRINTFUNC(DLOG_DEBUG, "locale = %s", locale);

                int i = 0;
                while (nuance_support_language[i][0] != NULL) {
                    if (strstr(locale, nuance_support_language[i][0])) {
                        find_it = i;
                        break;
                    }
                    i++;
                }
                free(locale);
            }

        _ise_tos_launch_web_link(tempurl+ "/" + nuance_support_language[find_it][1]);

        show_popup_toast(gettext("WDS_MYMAG_TPOP_SHOWING_DETAILS_ON_YOUR_PHONE_ING"), true);
    } else {
         // stand alone mode
    }
}

static void _response_cb2(void *data, Evas_Object *obj, void *event_info)
{
    App_Data* ad = (App_Data*)data;

    if (!ad)
        return;

    if (obj)
        elm_object_disabled_set(obj, EINA_TRUE);

    elm_naviframe_item_pop(ad->naviframe);
}

static Eina_Bool
_naviframe_pop_cb2(void *data , Elm_Object_Item *it)
{
    PRINTFUNC(DLOG_DEBUG, "");

    if (g_circle_object_first)
        eext_rotary_object_event_activated_set(g_circle_object_first, EINA_TRUE);

    return EINA_TRUE;
}

static void _response_cb(void *data, Evas_Object *obj, void *event_info)
{
    PRINTFUNC(DLOG_DEBUG, "");

    App_Data* ad = (App_Data*)data;

    if (!ad) return;

    set_tos_N66_agreed(EINA_TRUE);

    ise_show_stt_popup(ad);

    set_disclaimer_flag();
}

static Eina_Bool
_naviframe_pop_cb(void *data , Elm_Object_Item *it)
{
    PRINTFUNC(DLOG_DEBUG, "");

    _back_to_genlist_for_selector();

    return EINA_TRUE;
}


std::string getColorStyle(const char *code)
{
    std::string strRet;
    int r, g, b, a;
    char strColor[256];
//    ea_theme_color_get(code, &r, &g, &b, &a, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    snprintf(strColor, 256, "<color=#%02x%02x%02x%02x>", r, g, b, a);

    strRet.append(strColor);
    return strRet;
}

std::string getFontStyle(const char *code)
{
    std::string strRet;
    int r, g, b, a;
    char *fontStyle;
    int  fontSize;
    char strColor[256];
    char strFont[256];
//    ea_theme_color_get(code, &r, &g, &b, &a, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    snprintf(strColor, 256, "<color=#%02x%02x%02x%02x>", r, g, b, a);

//    ea_theme_font_get(code,&fontStyle,&fontSize);
/*
    if (fontStyle) {
        snprintf(strFont, 256, "<font=Tizen:style=%s><font_size=%d>", fontStyle, fontSize);
    } else {
        snprintf(strFont, 256, "<font=Tizen:style=Regular><font_size=%d>", fontSize);
    }
    strRet.append(strFont);
    strRet.append(strColor);
    PRINTFUNC(DLOG_DEBUG, "====strRect==%s", strRet.c_str());
    //after uifw finish patch, this free must open, otherwise it will have memory leak
    if (fontStyle) {
        free(fontStyle);
        fontStyle = NULL;
    }
*/
    return strRet;
}

static std::string tagging(const std::string& str)
{
    std::string link_start = "<a href=";
    std::string link_end = "</a>";
    std::string label = str;
    std::string tag_str = "";
    std::string link_color = "";

    tag_str.append(getFontStyle("T142"));
    link_color.append(getColorStyle("T1611"));

    std::string::size_type found = label.find(link_start);

    while (found != std::string::npos) {
        tag_str.append(label, 0, found);
        tag_str.append(link_color);

        std::string::size_type end_pos = label.find(link_end);
        tag_str.append(label, found, end_pos-found);
        tag_str.append("</a>");
        tag_str.append("</color>");
        label = label.substr(end_pos+4);
        found = label.find(link_start);
    }

    tag_str.append(label);
    tag_str.append("</font_size></color></font>");

    return tag_str;
}

static void _language_changed_cb(void *_data, Evas_Object *_obj, void *_event_info)
{
    if (!_obj) return ;
    if (!_data) return ;

    int index = (int)_data;
    std::string terms = "";
    Elm_Object_Item *navi_it = NULL;

    char body_str[40000];

    switch (index) {
        case 1:
            snprintf(body_str, sizeof(body_str), gettext("LDS_IME_BODY_INFORMATION_PROVISION_AGREEMENT_P1_LEGALPHRASE_WC1"), " <a href=\"file://local-nuance-tos\">","</a>","<a href=\"http://www.vlingo.com/wap/samsung-asr-privacy-addendum\">","</a>");
            terms = tagging(std::string(body_str));
            terms = replaceAll(terms , std::string("\n"), std::string("<br>"));
            elm_entry_entry_set(elm_layout_content_get(_obj, "elm.swallow.content"), terms.c_str());
        break;
        case 2:
            terms = tagging(std::string(gettext(TOS_BODY2))+ std::string("<br>") + std::string(gettext(TOS_BODY3)));
            terms = replaceAll(terms , std::string("\n"), std::string("<br>"));
            elm_entry_entry_set(elm_layout_content_get(_obj, "elm.swallow.content"), terms.c_str());
        break;
        default:
        break;
    }
}

Evas_Object *create_tos_second_page(void* data){
    App_Data* ad = (App_Data*)data;

    if (!ad)
        return NULL;

    PRINTFUNC(DLOG_DEBUG, "");
    Evas_Object *btn;
    Evas_Object *outer_layout;
    Evas_Object *inner_layout;
    Evas_Object *scroller;
    Evas_Object *circle_scroller;

    std::string terms = "";

    string edj_path = get_resource_path();
    edj_path = edj_path + STT_EDJ_FILE;

//------------outer layout----------------------
    outer_layout = elm_layout_add(ad->naviframe);
    elm_layout_theme_set(outer_layout, "layout", "bottom_button", "default");
    evas_object_show(outer_layout);

//------------scroller----------------------
    scroller = elm_scroller_add(outer_layout);
    elm_scroller_bounce_set(scroller, EINA_TRUE, EINA_TRUE);
    evas_object_size_hint_weight_set(scroller, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(scroller, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
    elm_object_scroll_lock_x_set(scroller, EINA_TRUE);
    evas_object_show(scroller);
    circle_scroller = eext_circle_object_scroller_add(scroller, ad->circle_surface);
    eext_circle_object_scroller_policy_set(circle_scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
    eext_rotary_object_event_activated_set(circle_scroller, EINA_TRUE);
    elm_object_part_content_set(outer_layout, "elm.swallow.content", scroller);

//------------inner layout----------------------
    inner_layout = elm_layout_add(scroller);
    elm_layout_file_set(inner_layout, edj_path.c_str(), "elm_naviframe_item_basic_customized");
    elm_object_domain_translatable_part_text_set(inner_layout, "elm.text.title", PACKAGE, TOS_TITLE);
    evas_object_size_hint_weight_set(inner_layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_smart_callback_add(inner_layout, "language,changed", _language_changed_cb, (void *)2);

    elm_object_content_set(scroller, inner_layout);

//------------entry----------------------
    Evas_Object *entry    = elm_entry_add(inner_layout);
    elm_entry_editable_set(entry, EINA_FALSE);
    evas_object_size_hint_weight_set(entry, EVAS_HINT_EXPAND, 0.0);
    evas_object_size_hint_align_set(entry, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_show(entry);
    elm_entry_text_style_user_push(entry, "DEFAULT='align=center'");

    terms = tagging(std::string(gettext(TOS_BODY2))+ std::string("<br>")+ std::string(gettext(TOS_BODY3)));
    terms = replaceAll(terms , std::string("\n"), std::string("<br>"));

    elm_entry_entry_set(entry, terms.c_str());

    elm_object_part_content_set(inner_layout, "elm.swallow.content", entry);

//------------Bottom Button----------------------
    btn = elm_button_add(outer_layout);
    elm_object_style_set(btn, "bottom");
    elm_access_info_set(btn, ELM_ACCESS_INFO, gettext(POPUP_OK_BTN));
    //elm_object_domain_translatable_text_set(btn, PACKAGE, POPUP_OK_BTN);
    evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_object_part_content_set(outer_layout, "elm.swallow.button", btn);

    Evas_Object* ic = elm_image_add(btn);
    elm_image_resizable_set(ic, EINA_TRUE, EINA_TRUE);
    string path = get_resource_path();
    string path_ic = path + "/images/w_sip_number_btn_ic.png";
    elm_image_file_set(ic, path_ic.c_str(), NULL);
    elm_object_content_set(btn, ic);

    evas_object_smart_callback_add(btn, "clicked", _response_cb2, ad);

    evas_object_show(outer_layout);

    elm_object_signal_emit(inner_layout, "elm,state,visible", "elm");

    return outer_layout;
}

static void _ise_voice_tos_anchor_clicked(void *data, Evas_Object *obj, void *event_info)
{
    App_Data* ad = (App_Data*)data;

    if (!ad)
        return;

    Elm_Entry_Anchor_Info *ainfo = (Elm_Entry_Anchor_Info*)event_info;

    PRINTFUNC(DLOG_DEBUG, "ainfo->name = %s", ainfo->name);

    if (!strncmp(ainfo->name, "\"http://", 8)) {
        show_terms(data, obj, ainfo);

    } else if (!strncmp(ainfo->name, "\"file://", 8)) {
/*
        Evas_Object *senconds_layout = create_tos_second_page(ad);

        Elm_Object_Item *nit = NULL;
        nit = elm_naviframe_item_push(ad->naviframe, NULL, NULL, NULL, senconds_layout, "empty");
        elm_naviframe_item_pop_cb_set(nit, _naviframe_pop_cb2, NULL);
        elm_naviframe_item_title_enabled_set(nit, EINA_FALSE, EINA_FALSE);
*/
    }
}

Evas_Object *create_tos_popup(void *data)
{
    App_Data* ad = (App_Data*)data;

    if (!ad)
        return NULL;

    PRINTFUNC(DLOG_DEBUG, "");
    Evas_Object *btn;
    Evas_Object *outer_layout;
    Evas_Object *inner_layout;
    Evas_Object *scroller;
    Evas_Object *circle_scroller;

    std::string terms = "";

    string edj_path = get_resource_path();
    edj_path = edj_path + STT_EDJ_FILE;

//------------outer layout----------------------
    outer_layout = elm_layout_add(ad->naviframe);
    elm_layout_theme_set(outer_layout, "layout", "bottom_button", "default");
    evas_object_show(outer_layout);

//------------scroller----------------------
    scroller = elm_scroller_add(outer_layout);
    elm_scroller_bounce_set(scroller, EINA_TRUE, EINA_TRUE);
    evas_object_size_hint_weight_set(scroller, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(scroller, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
    elm_object_scroll_lock_x_set(scroller, EINA_TRUE);
    evas_object_show(scroller);
    circle_scroller = eext_circle_object_scroller_add(scroller, ad->circle_surface);
    eext_circle_object_scroller_policy_set(circle_scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
    eext_rotary_object_event_activated_set(circle_scroller, EINA_TRUE);
    elm_object_part_content_set(outer_layout, "elm.swallow.content", scroller);
    g_circle_object_first = circle_scroller;

//------------inner layout----------------------
    inner_layout = elm_layout_add(scroller);
    elm_layout_file_set(inner_layout, edj_path.c_str(), "elm_naviframe_item_basic_customized");
    elm_object_domain_translatable_part_text_set(inner_layout, "elm.text.title", PACKAGE, TOS_TITLE);
    evas_object_size_hint_weight_set(inner_layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_smart_callback_add(inner_layout, "language,changed", _language_changed_cb, (void *)1);

    elm_object_content_set(scroller, inner_layout);

//------------entry----------------------
    Evas_Object *entry    = elm_entry_add(inner_layout);
    elm_entry_editable_set(entry, EINA_FALSE);
    evas_object_size_hint_weight_set(entry, EVAS_HINT_EXPAND, 0.0);
    evas_object_size_hint_align_set(entry, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_show(entry);
    evas_object_smart_callback_add(entry, "anchor,clicked", _ise_voice_tos_anchor_clicked, ad);
    elm_entry_text_style_user_push(entry, "DEFAULT='align=center'");

    terms = tagging(std::string(gettext(TOS_NUANCE_BODY)));
    terms = replaceAll(terms , std::string("%s"), std::string(gettext(SK_GEAR_INPUT)));
    terms = replaceAll(terms , std::string("\n"), std::string("<br>"));

    elm_entry_entry_set(entry, terms.c_str());

    elm_object_part_content_set(inner_layout, "elm.swallow.content", entry);

//------------Bottom Button----------------------
    btn = elm_button_add(outer_layout);
    elm_object_style_set(btn, "bottom");
    elm_access_info_set(btn, ELM_ACCESS_INFO, gettext(POPUP_OK_AGREE));
    elm_object_domain_translatable_text_set(btn, PACKAGE, POPUP_OK_AGREE);
    evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_object_part_content_set(outer_layout, "elm.swallow.button", btn);

    evas_object_smart_callback_add(btn, "clicked", _response_cb, ad);

    evas_object_show(outer_layout);

    elm_object_signal_emit(inner_layout, "elm,state,visible", "elm");

    return outer_layout;
}


void ise_show_tos_popup(void *data)
{
    App_Data* ad = (App_Data*)data;

    if (!ad)
     return;

    Evas_Object *main_layout = create_tos_popup(ad);

    Elm_Object_Item *nit = NULL;
    nit = elm_naviframe_item_push(ad->naviframe, NULL, NULL, NULL, main_layout, "empty");
    elm_naviframe_item_pop_cb_set(nit, _naviframe_pop_cb, NULL);
    elm_naviframe_item_title_enabled_set(nit, EINA_FALSE, EINA_FALSE);
}

//--------------------------------n66--------------------------------------------------

static void _ise_voice_tos_n66_anchor_clicked(void *data, Evas_Object *obj, void *event_info)
{
    App_Data* ad = (App_Data*)data;

    if (!ad)
        return;

    _ise_tos_launch_web_link(N66_URL);
    show_popup_toast(gettext(SK_SHOWING_DETAIL), true);
}

static void _response_n66_cb(void *data, Evas_Object *obj, void *event_info)
{
    App_Data* ad = (App_Data*)data;

    if (!ad) return;

#if 0
    stt_set_user_agreement(true);
#else
    set_tos_N66_agreed(EINA_TRUE);
#endif

    ise_show_stt_popup(ad);

    set_disclaimer_flag();
}

static void _n66_language_changed_cb(void *_data, Evas_Object *_obj, void *_event_info)
{
    std::string terms = "";

    Evas_Object* entry = elm_layout_content_get(_obj, "elm.swallow.content");

    // 1. Get Country Code
    char *country_code = NULL;
//    country_code = vconf_get_str(VCONFKEY_CSC_COUNTRY_ISO);

    // 2. Set String ID depend on Count Code
    char* popup_title_id = NULL;
    char* popup_body_id = NULL;
//    if (country_code) {
//        if (!strcmp(country_code, "CN")) {
//            popup_title_id = TOS_TITLE_CHN;
//            popup_body_id = TOS_N66_BODY_CHN;
//        } else {
//            popup_title_id = TOS_TITLE;
//            popup_body_id = TOS_N66_BODY;
//        }
//    } else {
        PRINTFUNC(DLOG_ERROR, "Getting Count Code is Error!!! Set default TOS Text!");
        popup_title_id = TOS_TITLE;
        popup_body_id = TOS_N66_BODY;
//    }

//    ea_cutlink_h cutlink;
//    ea_cutlink_create(&cutlink, entry, EA_CUTLINK_USER);

    /* add a new scheme */
//    ea_cutlink_scheme_add(cutlink, "hello:", N66_URL);

    /* set prefix and postfix for a new scheme */
//    ea_cutlink_prefix_set(cutlink, "hello:",
//                  "<color=#5BB8FFEF underline=on "
//                  "underline_color=#5BB8FFEF>");
//    ea_cutlink_postfix_set(cutlink, "hello:", "</color>");

    char *markup = NULL;
    char *txt = NULL;
    /* set text for parsing */
    markup = elm_entry_utf8_to_markup(gettext(popup_body_id));
//    ea_cutlink_markup_set(cutlink, markup);

//    ea_cutlink_markup_apply(cutlink, gettext(popup_body_id), &txt);
    PRINTFUNC(DLOG_DEBUG, "%s", txt);

    terms = tagging(std::string(gettext(txt)));
    elm_entry_entry_set(entry, terms.c_str());

//    if (txt)
//        free(txt);

    if (markup)
        free(markup);
    /* destroy cutlink object */
//    ea_cutlink_destroy(cutlink);
}

Evas_Object *create_tos_n66_popup(void *data)
{
    App_Data* ad = (App_Data*)data;

    if (!ad)
        return NULL;

    PRINTFUNC(DLOG_DEBUG, "");
    Evas_Object *btn;
    Evas_Object *outer_layout;
    Evas_Object *inner_layout;
    Evas_Object *scroller;
    Evas_Object *circle_scroller;

    std::string terms = "";

    string edj_path = get_resource_path();
    edj_path = edj_path + STT_EDJ_FILE;

    // 1. Get Country Code
    char *country_code = NULL;
//    country_code = vconf_get_str(VCONFKEY_CSC_COUNTRY_ISO);

    // 2. Set String ID depend on Count Code
    char* popup_title_id = NULL;
    char* popup_body_id = NULL;
//    if (country_code) {
//        if (!strcmp(country_code, "CN")) {
//            popup_title_id = TOS_TITLE_CHN;
//            popup_body_id = TOS_N66_BODY_CHN;
//        } else {
//            popup_title_id = TOS_TITLE;
//            popup_body_id = TOS_N66_BODY;
//        }
//    } else {
        PRINTFUNC(DLOG_ERROR, "Getting Count Code is Error!!! Set default TOS Text!");
        popup_title_id = TOS_TITLE;
        popup_body_id = TOS_N66_BODY;
//    }


//------------outer layout----------------------
    outer_layout = elm_layout_add(ad->naviframe);
    elm_layout_theme_set(outer_layout, "layout", "bottom_button", "default");
    evas_object_show(outer_layout);

//------------scroller----------------------
    scroller = elm_scroller_add(outer_layout);
    elm_scroller_bounce_set(scroller, EINA_TRUE, EINA_TRUE);
    evas_object_size_hint_weight_set(scroller, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(scroller, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
    elm_object_scroll_lock_x_set(scroller, EINA_TRUE);
    evas_object_show(scroller);
    circle_scroller = eext_circle_object_scroller_add(scroller, ad->circle_surface);
    eext_circle_object_scroller_policy_set(circle_scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
    eext_rotary_object_event_activated_set(circle_scroller, EINA_TRUE);
    elm_object_part_content_set(outer_layout, "elm.swallow.content", scroller);
    g_circle_object_first = circle_scroller;

//------------inner layout----------------------
    inner_layout = elm_layout_add(scroller);
    elm_layout_file_set(inner_layout, edj_path.c_str(), "elm_naviframe_item_basic_customized");
    elm_object_domain_translatable_part_text_set(inner_layout, "elm.text.title", PACKAGE, popup_title_id);
    evas_object_size_hint_weight_set(inner_layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_smart_callback_add(inner_layout, "language,changed", _n66_language_changed_cb, NULL);

    elm_object_content_set(scroller, inner_layout);

//------------Entry----------------------
    Evas_Object *entry    = elm_entry_add(inner_layout);
    elm_entry_editable_set(entry, EINA_FALSE);
    evas_object_size_hint_weight_set(entry, EVAS_HINT_EXPAND, 0.0);
    evas_object_size_hint_align_set(entry, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_show(entry);
    evas_object_smart_callback_add(entry, "anchor,clicked", _ise_voice_tos_n66_anchor_clicked, ad);
    elm_entry_text_style_user_push(entry, "DEFAULT='align=center'");

//    ea_cutlink_h cutlink;
//    ea_cutlink_create(&cutlink, entry, EA_CUTLINK_USER);

    /* add a new scheme */
//    ea_cutlink_scheme_add(cutlink, "hello:", N66_URL);

    /* set prefix and postfix for a new scheme */
//    ea_cutlink_prefix_set(cutlink, "hello:",
//                  "<color=#5BB8FFEF underline=on "
//                  "underline_color=#5BB8FFEF>");
//    ea_cutlink_postfix_set(cutlink, "hello:", "</color>");

    char *markup = NULL;
    char *txt = NULL;
    /* set text for parsing */
    markup = elm_entry_utf8_to_markup(gettext(popup_body_id));
//    ea_cutlink_markup_set(cutlink, markup);

//    ea_cutlink_markup_apply(cutlink, gettext(popup_body_id), &txt);
    PRINTFUNC(DLOG_DEBUG, "%s", txt);

    terms = tagging(std::string(gettext(txt)));
    elm_entry_entry_set(entry, terms.c_str());

//    if (txt)
//        free(txt);

    if (markup)
        free(markup);
    /* destroy cutlink object */
//    ea_cutlink_destroy(cutlink);

    elm_object_part_content_set(inner_layout, "elm.swallow.content", entry);

//------------Bottom Button----------------------
    btn = elm_button_add(outer_layout);
    elm_object_style_set(btn, "bottom");
    elm_access_info_set(btn, ELM_ACCESS_INFO, gettext(POPUP_OK_AGREE));
    elm_object_domain_translatable_text_set(btn, PACKAGE, POPUP_OK_AGREE);
    evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_object_part_content_set(outer_layout, "elm.swallow.button", btn);

    evas_object_smart_callback_add(btn, "clicked", _response_n66_cb, ad);

    evas_object_show(outer_layout);

    elm_object_signal_emit(inner_layout, "elm,state,visible", "elm");

    return outer_layout;
}

void ise_show_tos_n66_popup(void *data)
{
    App_Data* ad = (App_Data*)data;

    if (!ad)
     return;

    Evas_Object *main_layout = create_tos_n66_popup(ad);

    Elm_Object_Item *nit = NULL;
    nit = elm_naviframe_item_push(ad->naviframe, NULL, NULL, NULL, main_layout, "empty");
    elm_naviframe_item_pop_cb_set(nit, _naviframe_pop_cb, NULL);
    elm_naviframe_item_title_enabled_set(nit, EINA_FALSE, EINA_FALSE);
}




