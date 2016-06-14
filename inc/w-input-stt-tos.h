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
void launch_bt_connection_popup(App_Data* data);

//--------------------------n66--------------------------
void ise_show_tos_n66_popup(void *data);

#endif /* W_INPUT_STT_TOS_H_ */
