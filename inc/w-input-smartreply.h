/*
 * w-input-smartreply
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


#ifndef _W_INPUT_SMARTREPLY_H_
#define _W_INPUT_SMARTREPLY_H_

#include <app_control.h>
#include <stdbool.h>

typedef void (*input_smartreply_changed)(void *user_data);


bool input_smartreply_init(app_control_h app_control);

void input_smartreply_deinit(void);

const char *input_smartreply_get_lang(void);

bool input_smartreply_get_reply(void);

bool input_smartreply_get_reply_async(void);

int input_smartreply_get_reply_num(void);

char *input_smartreply_get_nth_item(int index);

bool input_smartreply_send_feedback(const char *str);

void input_smartreply_set_notify(input_smartreply_changed callback,
				void *user_data);

void input_smartreply_unset_notify(void);

bool input_smartreply_is_enabled(void);

#endif
