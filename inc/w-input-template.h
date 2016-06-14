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

#ifndef __W_INPUT_TEMPLATE_H_
#define __W_INPUT_TEMPLATE_H_

#include <vector>
#include <string>

#include <app_control.h>

struct TemplateData {
	std::string text;
	bool use_gettext;
};

typedef void (*input_template_changed) (void *user_data);


void input_template_init(app_control_h app_control);

void input_template_deinit(void);

const std::vector<TemplateData> input_template_get_list(void);

void input_template_set_notify(input_template_changed callback,
			void *user_data);

void input_template_unset_notify(void);

bool input_template_is_user_template(void);

#endif
