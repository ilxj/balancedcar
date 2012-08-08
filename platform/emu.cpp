
#include "balance.h"
#include "hal.h"
#include "constant.h"

#include "view.hpp"

#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <GL/glut.h>
#include <Box2D/Box2D.h>

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
	land->CreateFixture(&landshape,1);

	cardef.type = b2_dynamicBody;
	cardef.position = b2Vec2(0,2.5);
	cardef.angle = -44*DEGTORAD;

	car = world.CreateBody(&cardef);


	b2PolygonShape carshape;
	b2FixtureDef   carfixturedef;

	carshape.SetAsBox(0.5,2);
	car->CreateFixture(&carshape,0);

	carfixturedef.shape = & carshape;
	carfixturedef.density = 1.0f;
	carfixturedef.friction = 0.4f;

	car->CreateFixture(&carfixturedef);

	carshape.SetAsBox(2,0.5,b2Vec2(-1,0),0);
	car->CreateFixture(&carfixturedef);

	b2CircleShape	carwheel;
	carwheel.m_p.Set(0.0,1);
	carwheel.m_radius = 2;

	carfixturedef.shape = & carwheel;
	car->CreateFixture(&carfixturedef);

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

	world.Step(deltime ,1000,500);

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
