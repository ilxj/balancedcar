#pragma once

#include "hal.h"

#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/b2WorldCallbacks.h>


class simulator: public b2World,public b2ContactListener
{
friend void hal_set_pwm(int pwm);
friend int hal_get_pwm();

private: // for balance.c
	int volatile PWM;

private:
	b2Body	*car;

	bool is_car_touched;
	b2Vec2 m_ground_normal;

	bool volatile paused;

public:
	void init();
	void car_move_extra_load(b2Vec2 centor = b2Vec2(0,3));
	float car_wheel_speed();
	b2Body	*GetCar(){return car;};

	void pause();
	void resume();

	static int main(int argc,char *argv[]);

private:
	b2Vec2 car_caculate_impulse(double);
	static void balance_iter_lambda(int interval);
	static void simulat_step(int unused);
	void step();

private: // for b2ContactListener
	/// Called when two fixtures begin to touch.
	void BeginContact(b2Contact* contact);
	/// Called when two fixtures cease to touch.
	void EndContact(b2Contact* contact);

public: // for b2ContactListener
	//get the gound normal vector
	b2Vec2 get_ground_normal(){return m_ground_normal;}
private:
	void set_ground_normal(b2Fixture * fixture);

public:
	simulator();
	~simulator(){}
};


extern simulator simul;
