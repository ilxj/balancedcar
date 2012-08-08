#include "hal.h"

static int volatile	desired_angle;
static int volatile	desired_speed; /*(-255~255)*/
static int volatile	pwd; /*( -255~+255  )*/

/*
 * guess_gilt_angle
 * @angle_speed: current angle_rate
 * @angle_accel: angle_acceleration
 * @return: the guessed tilt angle, not precise, range -32767~32767 (-90)
 */
static int guess_tilt_angle(int angle_speed,int angle_accel)
{
	return angle_speed + angle_accel*2;
}

/*
 * angle PID
 */
static void balance_angle()
{
	int angle_speed	= hal_get_angle_speed();
	int angle_accel = hal_get_angle_accel();

	int current_speed = hal_get_speed();

	int	tilt_angle	= guess_tilt_angle(angle_speed,angle_accel);
	/*This fir P*/
	int angle_speed_P = (angle_speed - desired_angle);

	/*This fir I*/
	int angle_speed_I = (angle_speed - desired_angle);

	/*This fir D*/
	int angle_speed_D = (angle_speed - desired_angle);

	int PWM	= current_speed + angle_speed / 100 + tilt_angle ;

	hal_set_pwm(PWM);
}

/*
 * speed PID
 */
static void balance_speed()
{
	desired_angle = 0;
	return ;
	static int prespeed;
	int curspeed = hal_get_speed();

	int overspeed = curspeed - desired_speed;

	desired_angle = overspeed;

	desired_angle = -desired_angle;

	prespeed=curspeed;
}

EXTERN void balance_iter(int interval_ms)
{
	static int interval;

	balance_angle();

	if(interval == 9)
		balance_speed();

	interval ++;
	interval %= 10;
}

EXTERN void balance_init(void)
{

}

EXTERN void balance_run(void)
{
	for(;;hal_delay(2))
		balance_iter(2);
}

EXTERN void balance_set_speed(int speed)
{
	desired_speed = speed;
}
