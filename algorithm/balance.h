#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void balance_init(void);

void balance_iter(int interval_ms);

void balance_run(void);

/*
 * Called by remote control system
 */
void balance_set_speed(int speed);

#ifdef __cplusplus
}
#endif
