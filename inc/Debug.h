/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of SAMSUNG
 * ELECTRONICS ("Confidential Information"). You agree and acknowledge that
 * this software is owned by Samsung and you shall not disclose such
 * Confidential Information and shall use it only in accordance with the
 * terms of the license agreement you entered into with SAMSUNG ELECTRONICS.
 * SAMSUNG make no representations or warranties about the suitability of
 * the software, either express or implied, including but not limited to the
 * implied warranties of merchantability, fitness for a particular purpose,
 * or non-infringement. SAMSUNG shall not be liable for any damages suffered
 * by licensee arising out of or related to this software.
 */

#ifndef __DEBUG_HEAD__
#define __DEBUG_HEAD__

#define LOG_TAG "IME_DELEGATOR"

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
