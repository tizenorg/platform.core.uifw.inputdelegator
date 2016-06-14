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

#ifndef __DEBUG_HEAD__
#define __DEBUG_HEAD__

#define LOG_TAG "INPUT_DELEGATOR"

#include <dlog.h>



/***************************************************************************************************
 * There are 9 log levels.
 *
 * DLOG_ERROR - LOGE
 * DLOG_WRAN  - LOGW
 * DLOG_INFO  - LOGI
 * DLOG_DEBUG - LOGD
 *
 * SECURE_ERROR - SECURE_LOGE
 * SECURE_WRAN  - SECURE_LOGW
 * SECURE_INFO  - SECURE_LOGI
 * SECURE_DEBUG - SECURE_LOGD
 *
 * NO_PRINT - If you want to see a log which has a NO_PRINT option, you can modify a log level.
 *
 *************************************************************************************************** */

#define PRINTFUNC_DLOG_ERROR(fmt, arg...) LOGE(fmt, ##arg)
#define PRINTFUNC_DLOG_WARN(fmt, arg...)  LOGW(fmt, ##arg)
#define PRINTFUNC_DLOG_INFO(fmt, arg...)  LOGI(fmt, ##arg)
#define PRINTFUNC_DLOG_DEBUG(fmt, arg...) LOGD(fmt, ##arg) //LOGD(fmt, ##arg)

#define PRINTFUNC_SECURE_ERROR(fmt, arg...) SECURE_LOGE(fmt, ##arg)
#define PRINTFUNC_SECURE_WARN(fmt, arg...)  SECURE_LOGW(fmt, ##arg)
#define PRINTFUNC_SECURE_INFO(fmt, arg...)  SECURE_LOGI(fmt, ##arg)
#define PRINTFUNC_SECURE_DEBUG(fmt, arg...) SECURE_LOGD(fmt, ##arg)

#define PRINTFUNC_NO_PRINT(fmt, arg...) LOGE(fmt, ##arg)

#define PRINTFUNC(level,fmt, arg...) PRINTFUNC_##level (fmt, ##arg)

/*****************************************************************************************************/


#endif
