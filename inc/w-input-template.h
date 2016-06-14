/*
 * w-input-template
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
