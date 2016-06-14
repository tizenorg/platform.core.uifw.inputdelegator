/*
 * w-input-keyboard
 *
 * Copyright (c) 2000 - 2016 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS ("Confidential Information"). You agree and
 * acknowledge that this software is owned by Samsung and you shall
 * not disclose such Confidential Information and shall use it only
 * in accordance with the terms of the license agreement you entered
 * into with SAMSUNG ELECTRONICS. SAMSUNG make no representations or
 * warranties about the suitability of the software, either express
 * or implied, including but not limited to the implied warranties
 * of merchantability, fitness for a particular purpose, or
 * non-infringement. SAMSUNG shall not be liable for any damages
 * suffered by licensee arising out of or related to this software.
 *
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
