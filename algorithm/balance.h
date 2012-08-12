#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void balance_iter(int interval_ms);

/*
 * if Hardware Layer decide to pause the excution of the balance_iter
 * then call  balance_reset before resume.
 */
void balance_reset(void);

void balance_run(void);

/*
 * Called by Hardware Layer. if it will receive remote control :)
 */
void balance_set_speed(int speed);

/*
 * Called by Hardware Layer. if it will display on LED
 */
int balance_get_speed(void);

#ifdef __cplusplus
}
#endif
