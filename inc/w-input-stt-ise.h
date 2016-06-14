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


#ifndef W_INPUT_STT_ISE_H_
#define W_INPUT_STT_ISE_H_

/*
 * This file will be included from ISE
 */


#include <Elementary.h>

#ifdef EAPI
# undef EAPI
#endif


#ifdef __GNUC__
# if __GNUC__ >= 4
#  define EAPI __attribute__ ((visibility("default")))
# else
#  define EAPI
# endif
#else
# define EAPI
#endif

#ifdef __cplusplus
extern "C"
{
#endif

void show_voice_input(Evas_Object *parent, const char *lang, void (*get_string)(char *, int));
void pause_voice();
void destroy_voice();
int is_lang_supported_by_voice_input(const char *lang);
void ise_show_stt_popup(void *data);
void set_disclaimer_flag();


#ifdef __cplusplus
}
#endif

#endif /* W_INPUT_STT_ISE_H_ */
