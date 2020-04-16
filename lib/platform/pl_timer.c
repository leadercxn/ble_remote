/* Copyright (c) 2016 SENSORO Co.,Ltd. All Rights Reserved.
 *
 */

#include <stdint.h>
#include "pl_timer.h"




#ifndef APP_TIMER_PRESCALER
#define  APP_TIMER_PRESCALER     0
#endif

#define  NS_PER_TICK             (1+APP_TIMER_PRESCALER)*30517
#define  RTC1_TICKS_MAX          16777216
#define  NS_PER_MS               1000000

typedef struct
{
    uint32_t utc_s;
    uint32_t sys_ms;
} run_time_utc_reference_t;

APP_TIMER_DEF(run_time_timer);

static uint32_t m_run_time_ms = 0;

static run_time_utc_reference_t m_ref;

static void run_time_handler(void * p_contex)
{
    static uint32_t previous_tick = 0;
    uint32_t        current_tick = 0;
    uint32_t        tick_diff = 0;
    static uint32_t ns_remainder = 0;

    current_tick = app_timer_cnt_get();
    if (current_tick > previous_tick)
    {
        //ticks since last update
        tick_diff = current_tick - previous_tick;
    }
    else if (current_tick < previous_tick)
    {
        //RTC counter has overflown
        tick_diff = current_tick + (RTC1_TICKS_MAX - previous_tick);
    }

    uint32_t ns = tick_diff * NS_PER_TICK;

    m_run_time_ms += ns/NS_PER_MS;
    ns_remainder += ns % NS_PER_MS;

    if (ns_remainder >= NS_PER_MS)
    {
        m_run_time_ms += ns_remainder/NS_PER_MS;
        ns_remainder = ns_remainder % NS_PER_MS;
    }
    previous_tick = current_tick;
}

uint32_t RunTimeGet(void)
{
    run_time_handler(NULL);
    return m_run_time_ms;
}

uint32_t RunTimeElapsedGet(uint32_t past)
{
    uint32_t now = RunTimeGet();

    return (now - past);
}

void RunTimeInit(void)
{
    run_time_handler(NULL);
    TIMER_CREATE(&run_time_timer, APP_TIMER_MODE_REPEATED, run_time_handler);
    TIMER_START(run_time_timer, 2000);
}

void RunTimeUtcSet(uint32_t timestamp)
{
    m_ref.utc_s = timestamp;
    m_ref.sys_ms = RunTimeGet();
}

void RunTimespecGet(RunTimespec_t *p_timespec)
{
    uint32_t ms = RunTimeGet();
    uint32_t last = ms - m_ref.sys_ms;

    p_timespec->tv_s = m_ref.utc_s + last/1000;
    p_timespec->tv_ms = last%1000;
}

void RunTimeTmGet(struct tm * p_tm, int8_t timezone)
{
    RunTimespec_t timespec;

    RunTimespecGet(&timespec);
    timespec.tv_s += timezone * 3600;

    localtime_r(&timespec.tv_s, p_tm);
}

void pl_time_init(void)
{
    ret_code_t ret;
    ret = app_timer_init();
    APP_ERROR_CHECK(ret);
}
