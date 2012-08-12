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

#include "simulator.hpp"

#define MAX_SPEED	20.0
#define MAX_FORCE	18.0

simulator simul;

void simulator::car_move_extra_load(b2Vec2 centor)
{
	static b2Fixture *extra_load;

	if (extra_load)
		car->DestroyFixture(extra_load);

	b2PolygonShape carshape;
	b2FixtureDef carfixturedef;

	carfixturedef.shape = &carshape;
	carfixturedef.density = 0.15f;
	carfixturedef.friction = 2.0f;

	carshape.SetAsBox(5, 0.5, centor, 0);

	extra_load = car->CreateFixture(&carfixturedef);
}

void simulator::init()
{
	b2BodyDef cardef, landdef;

	landdef.type = b2_staticBody;
	landdef.position = b2Vec2(0, -1);

	b2Body * land = CreateBody(&landdef);

	b2PolygonShape landshape;

	GLfloat * bgcolor = new GLfloat[3];

	bgcolor[0] = 0.0f;
	bgcolor[1] = 1.0f;
	bgcolor[2] = 0.0f;

	b2FixtureDef landfixturedef;

	landfixturedef.shape = &landshape;
	landfixturedef.friction = 1.0f;

	landshape.SetAsBox(500, 1,b2Vec2_zero,0*DEGTORAD);

	land->CreateFixture(&landfixturedef)->SetUserData(bgcolor);

	cardef.type = b2_dynamicBody;
	cardef.position = b2Vec2(0, 15.5);
	cardef.angle = 1 * DEGTORAD;

	car = CreateBody(&cardef);

	b2PolygonShape carshape;
	b2FixtureDef carfixturedef;

	carfixturedef.shape = &carshape;
	carfixturedef.density = 0.01f;
	carfixturedef.friction = 2.0f;

	carshape.SetAsBox(0.5, 2, b2Vec2(0, 0), 0);
	car->CreateFixture(&carfixturedef);

	carfixturedef.density = 0.1f;
	carshape.SetAsBox(0.5, 2, b2Vec2(0, 2), 0);
	car->CreateFixture(&carfixturedef);

	b2CircleShape carwheel;
	carwheel.m_p = b2Vec2(0.0, -2);
	carwheel.m_radius = 0.5;
	carfixturedef.friction = 0.1f;

	carfixturedef.shape = &carwheel;

	bgcolor = new GLfloat[3];

	bgcolor[0] = 0.2f;
	bgcolor[1] = 0.0f;
	bgcolor[2] = 0.2f;

	car->CreateFixture(&carfixturedef)->SetUserData(bgcolor);

	SetContactListener(this);

	glutTimerFunc(1, simulat_step, 0);

	car_move_extra_load();
}

float simulator::car_wheel_speed()
{
	return -car->GetLinearVelocityFromLocalPoint(b2Vec2(0, -2)).x;
}

b2Vec2 simulator::car_caculate_impulse(double deltime)
{
	float speed_precent;
	float force_precent;

	b2Vec2 impulse(0, 0);
	b2Vec2 ground_normal = get_ground_normal();

	/* Get Wheel Speed  */
	float wheel_speed = car_wheel_speed();

	/*the theoretical speed indicatied by pwm*/
	float pwm_speed = MAX_SPEED * PWM / 255.0;

	/* Caculate the Force at given speed  */
	if (PWM != 0)
	{
		speed_precent = (pwm_speed - wheel_speed) / pwm_speed;

		if (speed_precent >= 1)
			speed_precent = 1;
		else if (speed_precent < 0)
			return impulse;

		force_precent = speed_precent; //*speed_precent;

		float Impulse = force_precent * MAX_FORCE * PWM / 255.0 * deltime;

		// should set the force vector according to the ground vector

#if 0
		impulse.Set(-Impulse * cos(car->GetAngle()),
				Impulse * sin(car->GetAngle()));
#else
		impulse.Set(-Impulse, 0);
#endif
	}
	return impulse;
}

void simulator::step()
{
	static struct timespec pretime;
	static bool flag_pretime;
	struct timespec curtime;

	if (!flag_pretime)
	{
		clock_gettime(CLOCK_MONOTONIC, &pretime);
		flag_pretime = true;
		return;
	}

	clock_gettime(CLOCK_MONOTONIC, &curtime);

	long double deltime = (curtime.tv_sec - pretime.tv_sec)
			+ (curtime.tv_nsec - pretime.tv_nsec) / 1000000000.0;

	if (!paused)
	{
		/*
		 * Simulate PWM driven Force, only if the wheel touch the ground
		 */
		if (is_car_touched)
			car->ApplyLinearImpulse(car_caculate_impulse(deltime),
					b2Vec2(0, -1));

		Step(deltime, 1000, 500);
	}
	pretime = curtime;
}


void simulator::pause()
{
	paused = true;
}

void simulator::resume()
{
	paused = false;
	balance_reset();
	glutTimerFunc(1, balance_iter_lambda, 15);
}

void simulator::balance_iter_lambda(int interval)
{
	if (!simul.paused)
		glutTimerFunc(interval, balance_iter_lambda, interval);
	balance_iter(interval);
}

void simulator::simulat_step(int unused)
{
	glutTimerFunc(1, simulat_step, 0);

	simul.step();
}

void simulator::set_ground_normal(b2Fixture * fixture)
{
	b2PolygonShape * shape =(typeof(shape)) fixture->GetShape();

	m_ground_normal = shape->m_normals[1];
}

void simulator::BeginContact(b2Contact* _contact)
{
	for (b2Contact * pcontact = _contact; pcontact ;pcontact = pcontact->GetNext())
	{
		b2Fixture * A = pcontact->GetFixtureA();
		b2Fixture * B = pcontact->GetFixtureB();

		if(A->GetUserData() && B->GetUserData())
		{
			is_car_touched = true;

			if(B->GetBody()->GetType() == b2_staticBody)
			{
				set_ground_normal(B);
			}else if(A->GetBody()->GetType() == b2_staticBody)
			{
				set_ground_normal(A);
			}

			break;
		}
	}
}

void simulator::EndContact(b2Contact* _contact)
{
	for (b2Contact * pcontact = _contact; pcontact ;pcontact = pcontact->GetNext())
	{
		b2Fixture * A = pcontact->GetFixtureA();
		b2Fixture * B = pcontact->GetFixtureB();

		if(A->GetUserData() && B->GetUserData())
		{
			is_car_touched = false;
			break;
		}
	}
}

simulator::simulator()
:b2World(b2Vec2(0, -9.8f),false)
{
	paused = false;
	PWM = 0;
}

static void emu_do_view_draw(int interval)
{
	view_draw_frame(simul);
	glutTimerFunc(interval, emu_do_view_draw, interval);
}

/*
 * Start here
 */
int simulator::main(int argc, char*argv[])
{
	glutInit(&argc, argv);

	view_init();

	simul.init();
	simul.resume();

	glutTimerFunc(1, emu_do_view_draw, 1000 / 40);

	glutMainLoop();
	return 0;
}

EXTERN int hal_get_angle_speed(void)
{
	return simul.GetCar()->GetAngularVelocity() * 1000;
}

EXTERN int hal_get_angle_accel()
{
	return simul.GetCar()->GetAngularDamping() * 1000 + rand() % 5;
}

EXTERN int hal_get_speed()
{
	return simul.car_wheel_speed() / MAX_SPEED * 1024 + rand() % 5;
}

EXTERN void hal_set_pwm(int pwm)
{
	if (pwm >= 255)
		simul.PWM = 255;
	else if (pwm <= -255)
		simul.PWM = -255;
	else
		simul.PWM = pwm;
}

EXTERN int hal_get_pwm()
{
	return simul.PWM;
}

EXTERN void hal_delay(int ms)
{
	usleep(ms);
}

/*
 * the trunk for printf
 * int hal_printf()  :)
 */
__asm__(
		".globl hal_printf\n"
		".type	hal_printf,@function\n"
		"hal_printf:\n"
		"\t .cfi_startproc\n"
		"\t jmp printf\n"
		"\t .cfi_endproc\n"
		"\t .size hal_printf, .-hal_printf"
);

int main(int argc, char*argv[])
{
	return simulator::main(argc,argv);
}
