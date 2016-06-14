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


#ifndef __W_INPUT_STT_TOS_H_
#define __W_INPUT_STT_TOS_H_

#include <Elementary.h>
#include <glib.h>
#include <glib-object.h>
#include <dlog.h>
#include <libintl.h>
#include <locale.h>
#include <vconf.h>
#include <vconf-keys.h>


void ise_show_tos_popup(void *data);
Eina_Bool is_sap_connection();
Eina_Bool is_tos_N66_agreed();
Eina_Bool set_tos_N66_agreed(Eina_Bool bSet);
Eina_Bool is_tos_Nuance_agreed();
Eina_Bool set_tos_Nuance_agreed(Eina_Bool bSet);
void launch_bt_connection_popup(App_Data* data);

//--------------------------n66--------------------------
void ise_show_tos_n66_popup(void *data);

#endif /* W_INPUT_STT_TOS_H_ */
