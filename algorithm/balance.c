#include "hal.h"

static int volatile	curspeed; // current speed, averaged
static int volatile	desired_angle;
static int volatile	desired_angle_correction;
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
	int angle_speed_P = (angle_speed - (desired_angle+desired_angle_correction));

	/*This fir I*/
	int angle_speed_I = (angle_speed - (desired_angle+desired_angle_correction));

	/*This fir D*/
	int angle_speed_D = (angle_speed - (desired_angle+desired_angle_correction));

	int PWM	= current_speed*12+ angle_speed *10 + tilt_angle *5 ;

//	PWM /=1;

	hal_set_pwm(PWM);

	/*record the speed*/
	current_speed += hal_get_speed();
	current_speed /= 2;
}

/*
 * speed PID
 */
static void balance_speed()
{
	desired_angle = 0;
	return ;
	static int prespeed;

	int overspeed = curspeed - desired_speed;

	desired_angle = overspeed *20 +(curspeed - prespeed)*10 - ((curspeed - prespeed)*20+curspeed)/10;

	desired_angle = -desired_angle / 15;

	prespeed=curspeed;

	if(desired_angle > 100)
		desired_angle = 100;
	else if (desired_angle < -100)
		desired_angle = -100;
//	desired_angle +=100;
}

/*
 * 2nd speed PID, run every 2s, this can balance the slow shifting.
 */
static void balance_speed_slow()
{

}

EXTERN void balance_iter(int interval_ms)
{
	static int interval=1;
	static int long_term;
	balance_angle();

	if(interval == 0)
		balance_speed();

	if(long_term ==9)
		balance_speed_slow();

	interval ++;
	interval %= 18;
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
