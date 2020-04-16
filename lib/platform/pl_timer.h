/* Copyright (c) 2016 SENSORO Co.,Ltd. All Rights Reserved.
 *
 */

#ifndef PL_TIME_H__
#define PL_TIME_H__

#include <time.h>
#include "app_timer.h"

typedef struct
{
    uint32_t tv_s;
    uint32_t tv_ms;
} RunTimespec_t;

#define pl_timer_create(p_id, mode, handler)                                        \
{                                                                                   \
    uint32_t err_code;                                                              \
    err_code = app_timer_create(p_id, mode, handler);                               \
    APP_ERROR_CHECK(err_code);                                                      \
}                                                                                   \

#define pl_timer_start(id, ms)                                                      \
{                                                                                   \
    uint32_t err_code;                                                              \
    err_code = app_timer_start(id, APP_TIMER_TICKS(ms), NULL);                      \
    APP_ERROR_CHECK(err_code);                                                      \
}                                                                                   \
 
#define pl_timer_start_with_param(id, ms, param)                               \
{                                                                           \
    uint32_t err_code;                                                      \
    err_code = app_timer_start(id, APP_TIMER_TICKS(ms), param);             \
    APP_ERROR_CHECK(err_code);                                              \
}
#define pl_timer_stop(id)                                                           \
{                                                                                   \
    uint32_t err_code;                                                              \
    err_code = app_timer_stop(id);                                                  \
    APP_ERROR_CHECK(err_code);                                                      \
}


#define TIMER_CREATE  pl_timer_create
#define TIMER_START   pl_timer_start
#define TIMER_START_WITH_PARAM  pl_timer_start_with_param
#define TIMER_STOP    pl_timer_stop
#define TIMER_DEF     APP_TIMER_DEF

/**
 * @brief Function for initializing run-time
 */
void RunTimeInit(void);

/**
 * @brief Function for getting time from system on in ms
 *
 * @retval  Timestamp in ms
 */
uint32_t RunTimeGet(void);

/**
 * @brief Function for getting elapsed time from one past timestamp to now
 *
 * @param[i] past  The past timestamp
 *
 * @retval  The elaped time in ms
 */
uint32_t RunTimeElapsedGet(uint32_t past);

/**
 * [RunTimeUtcSet description]
 *
 * @param    timestamp    [description]
 */
void RunTimeUtcSet(uint32_t timestamp);

/**
 * [RunTimespecGet description]
 *
 * @param    p_timespec    [description]
 */
void RunTimespecGet(RunTimespec_t *p_timespec);

/**
 * [RunTimeTmGet description]
 *
 * @param    p_tm    [description]
 */
void RunTimeTmGet(struct tm * p_tm, int8_t timezone);

void pl_time_init(void);

#endif
