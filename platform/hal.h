#pragma once

#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN
#endif


#ifdef __cplusplus
extern "C" {
#endif

/*
 * hal_get_angle_speed
 * @return: the angle, scale form -2048~2048, already filtered by kalman filter
 *
 * if the underlying hardware implementation don't filter the nose,
 * the balance will not be stable.
 */
int		hal_get_angle_speed(void);

/*
 * hal_get_angle_accel
 * @return: the accel speed of the angle rate
 *
 * note: already filtered
 */
int		hal_get_angle_accel(void);

/*
 * hal_get_speed
 * @return:	the speed of the motor, scale form -255~255.
 *
 * note: the speed of motor is not the speed of the car
 */
int		hal_get_speed(void);

/*
 *
 */
void	hal_delay(int ms);

/*
 * hal_set_pwm
 *
 * set PWM signal of the underlying hardware
 */
void	hal_set_pwm(int pwm);

/*
 * hal_get_pwm
 *
 * get current pwm settings
 */
int hal_get_pwm();

int hal_printf(const char *__restrict __format, ...);

#ifdef __cplusplus
}
#endif
