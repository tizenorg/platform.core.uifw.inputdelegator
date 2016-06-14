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


#ifndef W_INPUT_STT_VOICE_H_
#define W_INPUT_STT_VOICE_H_

#include <Elementary.h>
#include <glib.h>
#include <glib-object.h>
#include <dlog.h>
#include <libintl.h>
#include <locale.h>
#include <vconf.h>
#include <vconf-keys.h>

#include "MoreOption.h"
#include "w-input-stt-engine.h"

#define SK_INIT 				"IDS_VOICE_BODY_TAP_MIC"
#define SK_SPEAK_NOW 			"WDS_VOICE_NPBODY_SPEAK_NOW_ABB"
#define SK_RECOGNITION_FAILED 	"WDS_VOICE_TPOP_RECOGNITION_FAILED_ABB"

#define SK_DOUBLE_TAP_TO_SPEAK 			"WDS_TTS_TBBODY_DOUBLE_TAP_TO_SPEAK"
#define SK_TAP_TO_PAUSE 			"WDS_VOICE_NPBODY_TAP_TO_PAUSE_ABB"

#define TTS_RADIO_BUTTON				"IDS_MSGS_BODY_RADIO_BUTTON"
#define TTS_NOT_SELECTED				"IDS_MSGS_BODY_NOT_SELECTED_T_TTS"
#define TTS_SELECTED				"IDS_MSG_BODY_SELECTED_TTS"

#define TTS_SEND				"IDS_AMEMO_BUTTON_SEND"
#define TTS_BUTTON				"IDS_ACCS_BODY_BUTTON_TTS"
#define TTS_DISABLED				"IDS_ACCS_BODY_DISABLED_TTS"

#define SK_NETWORK_ERROR				"WDS_VOICE_MBODY_NETWORK_ERROR_ABB"
#define SK_STT_BUSY						"IDS_VTR_BODY_RECOGNITION_SERVICE_BUSY"

#define POPUP_OK_AGREE                          "WDS_ST_ACBUTTON_AGREE_ABB"
#define POPUP_OK_BTN                            "IDS_ST_SK_OK"

#define TOS_TITLE						"IDS_VOICE_BODY_PRIVACY_NOTICE_LEGALPHRASE_WC1"
#define TOS_TITLE_CHN					"IDS_VOICE_BODY_PRIVACY_NOTICE_WC1_LEGALPHRASE_CHN_WC1"

#define TOS_NUANCE_BODY						"LDS_TTS_TO_PROVIDE_PS_SAMSUNG_USES_SPEECH_RECOGNITION_SERVICES_PROVIDED_BY_A_THIRD_PARTY_SERVICE_PROVIDER_MSG_LEGALPHRASE"
#define TOS_BODY2 							"LDS_IME_BODY_GEAR_INPUT_AND_USE_OF_A_SPEECH_RECOGNITION_SERVICE_P1_LEGALPHRASE_WC1"
#define TOS_BODY3							"LDS_IME_BODY_GEAR_INPUT_AND_USE_OF_A_SPEECH_RECOGNITION_SERVICE_P2_LEGALPHRASE_WC1"

#define TOS_N66_BODY_CHN					"IDS_VOICE_BODY_SAMSUNG_S_VOICE_ALLOWS_FOR_VOICE_CONTROL_FOR_CERTAIN_FEATURES_LEGALPHRASE_CHN_WC1"
#define TOS_N66_BODY						"IDS_VOICE_BODY_SAMSUNG_S_VOICE_ALLOWS_FOR_VOICE_CONTROL_FOR_CERTAIN_FEATURES_SUCH_AS_S_VOICE_GEAR_INPUT_VOICE_MEMO_AND_FIND_MY_CAR_MSG_LEGALPHRASE_WC1"

#define SK_SHOWING_DETAIL					"WDS_MYMAG_TPOP_SHOWING_DETAILS_ON_YOUR_PHONE_ING"
#define SK_GEAR_INPUT					"WDS_VOICE_BODY_GEAR_INPUT"

#define SK_NETWORK_ERROR_FOR_IOS		"WDS_WMGR_POP_MAKE_SURE_THE_PS_APP_IS_ACTIVE_ON_YOUR_PHONE"
#define SK_SAMSUNG_GEAR					"WDS_STU_BODY_SAMSUNG_GEAR"


#define SCREEN_WIDTH                    360
#define SCREEN_HEIGHT                   480

#define VCONFKEY_ISE_STT_LANGUAGE	"db/private/ise/stt/language"
#define PREFERENCE_ISE_STT_LANGUAGE	"ise/stt/language"

#define EVAS_CANDIDATE_LAYER 32000

int init_voice(Evas_Object *parent, const char *lang, VoiceData *r_voicedata);
int is_lang_supported_by_stt(char lang[]);
Evas_Object *show_voice_window(Evas_Object *parent, VoiceData *voicedata);
void on_stt_pause(VoiceData *voicedata);
void on_destroy(VoiceData *voicedata);
char* get_lang_label(char lang[]);
void _stt_lang_changed_cb(keynode_t *key, void* data);
void start_by_press(VoiceData *voicedata);
void voice_get_string(const char *keyValue, VoiceData *voicedata);
void create_setting_window(Evas_Object *more_option_layout);
void activate_circle_scroller_for_stt_textbox(void* data, Eina_Bool bActivate);
void stt_feedback_initialize();
void stt_feedback_deinitialize();
void show_error_message(VoiceData *voicedata, stt_error_e reason);
void powerUnlock(void);


#endif /* W_INPUT_STT_VOICE_H_ */
