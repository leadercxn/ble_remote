#ifndef TIMER_H__
#define TIMER_H__

#define TIMER_CREATE(p_id, mode, handle)                                    \
{                                                                           \
    uint32_t err_code;                                                      \
    err_code = app_timer_create(p_id, mode, handle);                        \
    APP_ERROR_CHECK(err_code);                                              \
}

#define TIMER_START(id, ms)                                                 \
{                                                                           \
    uint32_t err_code;                                                      \
    err_code = app_timer_start(id, APP_TIMER_TICKS(ms), NULL);              \
    APP_ERROR_CHECK(err_code);                                              \
}

#define TIMER_STOP(id)                                                      \
{                                                                           \
    uint32_t err_code;                                                      \
    err_code = app_timer_stop(id);                                          \
    APP_ERROR_CHECK(err_code);                                              \
}

#endif

