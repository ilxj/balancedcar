
#include "balance.h"
#include "hal.h"
#include "constant.h"

#include "view.hpp"

#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#include <Box2D/Box2D.h>

#define MAX_SPEED	20.0
#define MAX_FORCE	5.0


b2Vec2	gravity(0,-9.8f);
b2World	world(gravity,false);
b2Body	*car;
static int volatile	PWM;

static void simulate_init()
{
	b2BodyDef	cardef,landdef;

	landdef.type = b2_staticBody;
	landdef.position = b2Vec2(0,-1);

	b2Body * land = world.CreateBody(&landdef);

	b2PolygonShape landshape;
	landshape.SetAsBox(100,1);

	GLfloat	* bgcolor = new GLfloat[3];

	bgcolor[0] = 0.0f;
	bgcolor[1] = 1.0f;
	bgcolor[2] = 0.0f;


	land->CreateFixture(&landshape,1)->SetUserData( bgcolor );

	cardef.type = b2_dynamicBody;
	cardef.position = b2Vec2(0,2.5);
	cardef.angle = -10*DEGTORAD;

	car = world.CreateBody(&cardef);


	b2PolygonShape carshape;
	b2FixtureDef   carfixturedef;

	carshape.SetAsBox(0.5,2);
	car->CreateFixture(&carshape,0);

	carfixturedef.shape = & carshape;
	carfixturedef.density = 0.1f;
	carfixturedef.friction = 1.0f;

	car->CreateFixture(&carfixturedef);

	b2CircleShape	carwheel;
	carwheel.m_p = b2Vec2(0.0,-2);
	carwheel.m_radius = 0.55;
	carfixturedef.friction = 0.05f;


	carfixturedef.shape = & carwheel;
	car->CreateFixture(&carfixturedef);
}

static float	simulate_car_wheel_speed()
{
	return - car->GetLinearVelocityFromLocalPoint(b2Vec2(0,-2)).x;
}

static b2Vec2 simulate_caculate_impulse(double deltime)
{
	float speed_precent;
	float force_precent;

	b2Vec2 impulse(0,0);

	/* Get Wheel Speed  */
	float wheel_speed = simulate_car_wheel_speed();

	/*the theoretical speed indicatied by pwm*/
	float pwm_speed	= MAX_SPEED* PWM/255.0;

	/* Caculate the Force at given speed  */
	if(PWM!=0)
	{
		speed_precent = (pwm_speed - wheel_speed)/pwm_speed;

		if(speed_precent >= 1)
			speed_precent = 1;
		else if(speed_precent <0)
			return impulse;

		force_precent = speed_precent;//*speed_precent;

		float Impulse = force_precent * MAX_FORCE * PWM / 255.0 * deltime;

#if 0
		impulse.Set(-Impulse * cos(car->GetAngle()),
				Impulse * sin(car->GetAngle()));
#else
		impulse.Set(-Impulse,0);
#endif
	}
	return impulse;
}

static void simulat_step(int unused)
{
	glutTimerFunc(1,simulat_step,0);

	static struct timespec pretime;
	static bool flag_pretime;
	struct timespec curtime;

	if(!flag_pretime){
		clock_gettime(CLOCK_MONOTONIC,&pretime);
		flag_pretime = true;
		return ;
	}

	clock_gettime(CLOCK_MONOTONIC,&curtime);

	long double deltime = (curtime.tv_sec- pretime.tv_sec) + (curtime.tv_nsec- pretime.tv_nsec)/1000000000.0;

	/*
	 * Simulate PWM driven Force
	 */

	car->ApplyLinearImpulse( simulate_caculate_impulse(deltime) , b2Vec2(0, 0));

//	car->SetFixedRotation(1);

	world.Step(deltime, 1000, 500);
	pretime = curtime;
}

static void balance_iter_lambda(int interval)
{
	glutTimerFunc(interval,balance_iter_lambda,interval);

	balance_iter(interval);
}

static void emu_do_view_draw(int interval)
{
	view_draw_frame(world);
	glutTimerFunc(interval,emu_do_view_draw,interval);
}

int main(int argc,char*argv[])
{
	glutInit(&argc,argv);

	view_init();
	balance_init();
	simulate_init();

	glutTimerFunc(2,balance_iter_lambda,2);
	glutTimerFunc(1,emu_do_view_draw,1000/30);
	glutTimerFunc(1,simulat_step,0);

	glutMainLoop();
	return 0;
}

EXTERN int hal_get_angle_speed(void)
{
	return car->GetAngularVelocity() * 1000;
}

EXTERN int hal_get_angle_accel()
{
	return  car->GetAngularDamping() * 1000;
}


EXTERN int hal_get_speed()
{
	return car->GetLinearVelocity().y*100;
}

EXTERN void hal_set_pwm(int pwm)
{
	PWM = pwm;
}

EXTERN void hal_delay(int ms)
{
	usleep(ms);
}
