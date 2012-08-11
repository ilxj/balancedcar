#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void balance_init(void);

void balance_iter(int interval_ms);

void balance_run(void);

/*
 * Called by Hardware Layer. if it will receive remote control :)
 */
void balance_set_speed(int speed);

#ifdef __cplusplus
}
#endif
