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
