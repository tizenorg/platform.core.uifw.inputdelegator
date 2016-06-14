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


#ifndef _W_INPUT_KEYBOARD_H_
#define _W_INPUT_KEYBOARD_H_

#include <app_control.h>
#include <Evas.h>

bool input_keyboard_init(app_control_h app_control);
void input_keyboard_deinit(void);
bool input_keyboard_launch(Evas_Object *window);
bool input_keyboard_launch_with_ui(Evas_Object *window, void * data);

#endif
