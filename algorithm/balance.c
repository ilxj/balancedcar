#include "hal.h"

static int volatile	desired_angle;
static int volatile	desired_angle_correction;
static int volatile	desired_speed; /*(-127~127)*/ // but, can't excude half of max speed, aka 127
static int volatile	pwd; /*( -255~+255  )*/

static int volatile	detected_curspeed_moment; // current speed, not averaged
static int volatile	detected_curspeed; // current speed, averaged
static int volatile	detected_angle_speed;

/*
 * guess_gilt_angle
 * @angle_speed: current angle_rate
 * @angle_accel: angle_acceleration
 * @return: the guessed tilt angle, not precise, range -32767~32767 (-90)
 */
static int guess_tilt_angle(int angle_speed,int angle_accel)
{
	return angle_speed + angle_accel*4;
}

/*
 * angle PID
 */
static void balance_angle()
{
	int angle_speed	= hal_get_angle_speed();
	int angle_accel = hal_get_angle_accel();

	int current_speed = detected_curspeed_moment;

	if (angle_speed == 0)
	{
		hal_set_pwm(0);
		return;
	}

	int	tilt_angle	= guess_tilt_angle(angle_speed,angle_accel) - desired_angle;
	// - desired_angle;
	/*This fir P*/
	int angle_speed_P = (angle_speed - (desired_angle+desired_angle_correction));

	/*This fir I*/
	int angle_speed_I = (angle_speed - (desired_angle+desired_angle_correction));

	/*This fir D*/
	int angle_speed_D = (angle_speed - (desired_angle+desired_angle_correction));

	int PWM	= current_speed*8 + (angle_speed *60 + tilt_angle *50) /22;// + desired_angle /10 ;

	hal_set_pwm(PWM);
}

/*
 * speed PID
 */
static void balance_speed()
{
	static int prespeed;

	int overspeed = detected_curspeed - 50 - desired_speed * 8 ;
	int speed_accel = (detected_curspeed_moment- prespeed)*20;

	desired_angle = overspeed *32 + speed_accel*32 + ((detected_curspeed_moment - prespeed)*8+detected_curspeed_moment);

	desired_angle = -desired_angle / 64;

	prespeed=detected_curspeed;

	/*
	 * Limit the angle, too big result un-recoverable
	 */
	if(desired_angle > 150)
		desired_angle = 150;
	else if (desired_angle < -150)
		desired_angle = -150;

//	hal_printf("curspeed=%d, overspeed = %d , desired_angle=%d\n",detected_curspeed_moment,overspeed,desired_angle);

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
	/*record the speed*/
	detected_curspeed_moment = hal_get_speed();
	detected_curspeed += detected_curspeed_moment;
	detected_curspeed /= 2;

	balance_angle();

	if(interval == 0)
		balance_speed();

	if(long_term ==9)
		balance_speed_slow();

	interval ++;
	interval %= 8;
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
