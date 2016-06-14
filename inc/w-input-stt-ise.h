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
