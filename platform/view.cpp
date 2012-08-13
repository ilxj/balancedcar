/*
 * Draw Simulated Results
 */

#include "constant.h"
#include "hal.h"
#include "view.hpp"
#include "balance.h"

#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>

#include "glhelper.hpp"
#include "simulator.hpp"


static float scale = 40;
static float w=800,h=600;
static float centor;

static int is_double=false;

static void update_glview()
{
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w/scale -centor ,w/scale -centor, -1,h*2/scale - 1,-1000,10000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glScaled(-1,1,1);
}

static void 	draw_body(b2Body * body)
{
	float angle = body->GetAngle() * RADTODEG;

	glMatrixKeeper m;

	glTranslatef(body->GetPosition().x,body->GetPosition().y,0.0);
	glRotated(angle,0,0,1);

	for(b2Fixture * fixtures = body->GetFixtureList();fixtures;fixtures = fixtures->GetNext())
	{
		b2Shape * shape = fixtures->GetShape();

		b2Color color={1,1,1};

		if(fixtures->GetUserData())
			color = *(b2Color*)(fixtures->GetUserData());

		glColorKeeper colorkeeper(color);

		switch (shape->GetType())
		{
			case b2Shape::Type::e_polygon:
				{
					glMatrixKeeper m;

					b2PolygonShape * pshape = ((typeof(pshape))shape);

					glBegin(GL_POLYGON);
					for(int i=0; i < pshape->GetVertexCount(); i ++)
					{
						b2Vec2 v = pshape->GetVertex(i);
						glVertex2f(v.x,v.y);
					}
					glEnd();
				}
				break;
			case b2Shape::Type::e_circle:
				{
					glMatrixKeeper m;
					b2CircleShape * cshape = (typeof(cshape))shape;
					glTranslatef(cshape->m_p.x,cshape->m_p.y,0);
					glutSolidSphere(cshape->m_radius,/*cshape->m_radius*/40,20);
				}
				break;
			default:
				break;
		}
	}
}

static void draw_status()
{
	glStringDrawer drawer(1.1);

	drawer.printf("Angle = %03.3f度\n",simul.GetCar()->GetAngle()*RADTODEG);

	drawer.printf("PWM(-255~+255) = %d \n轮上车速 speed = %d\n", hal_get_pwm(), hal_get_speed() );

	drawer.printf("设置的速度是:%d\n当前车速=%2.1f\n",
			balance_get_speed(),
			- 1024.0/20.0 * simul.GetCar()->GetLinearVelocityFromLocalPoint(b2Vec2_zero).x
	);
}

static void draw_pwm()
{
	glMatrixKeeper mk(simul.GetCar()->GetWorldPoint(b2Vec2(0,-2)));

	glColorKeeper ck;

	int s;

	float scale=10;

	glScaled(1/scale,1/scale,1);
	glTranslatef(0,0,1);


	s = 6;

	/*
	 * get the ground normal and rotate
	 */
	b2Vec2 normal = simul.get_ground_normal();

	float angle = 0;

	if(normal.x&&normal.y)
		angle = atan2(normal.y,normal.x);

	glRotatef(angle * RADTODEG,0,0,1);

	if(hal_get_pwm()>0){
		glScalef(-1,1,1);
	}


	glBegin(GL_LINES);

	glColor3f(0,1,1);

	glVertex2f(0,0);
	glVertex2f(fabs(hal_get_pwm()),0);

	glVertex2f(fabs(hal_get_pwm()),0);
	glVertex2f(fabs(hal_get_pwm()) - s,s);

	glVertex2f(fabs(hal_get_pwm()),0);
	glVertex2f(fabs(hal_get_pwm()) - s,-s);

	glEnd();
}

static void on_redraw()
{
//	return ;
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT|GL_ACCUM_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

	for(b2Body * body = simul.GetBodyList();body;body=body->GetNext())
	{
		draw_body(body);
	}

	// draw arrow to reflect the PWM settings
	if(hal_get_pwm()!=0)
	{
		draw_pwm();
	}

	//draw some debug data
	draw_status();

	if(is_double)
	glutSwapBuffers();
	else
	glFlush();
}

static void on_window_resize(int _w , int _h)
{
	w = _w;
	h = _h;
	update_glview();
}

static void on_mouse_event(int button,int stat,int x,int y)
{
	glutPostRedisplay();

	switch (button) {
		case GLUT_WHEEL_UP_BUTTON:
			scale *=1.5;
			update_glview();
			break;
		case GLUT_WHEEL_DOWN_BUTTON:
			scale /=1.5;
			update_glview();
			break;
		default:
			break;
	}
}

static void on_key_event( unsigned char key , int, int )
{
	glutPostRedisplay();

	static bool paused=false;
	switch (key) {
		case ' ':
			paused = !paused;
			if(paused)
				simul.pause();
			else
				simul.resume();
			break;
		default:
			break;
	}
}

static void on_specialkey_event( int key, int, int )
{
	glutPostRedisplay();

	static b2Vec2 loadcentor(0,3);

	switch (key)
	{
		case GLUT_KEY_LEFT:
			centor +=1;
			break;
		case GLUT_KEY_RIGHT:
			centor -=1;
			break;
		case GLUT_KEY_UP:
			loadcentor.y++;
			break;
		case GLUT_KEY_DOWN:
			loadcentor.y--;
			break;
		case GLUT_KEY_F2:
			loadcentor.x --;
			break;
		case GLUT_KEY_F3:
			loadcentor.x = 0;
			break;
		case GLUT_KEY_F4:
			loadcentor.x ++;
			break;
		case GLUT_KEY_F12:
			glutDestroyWindow(glutGetWindow());
			break;
		case GLUT_KEY_F5:
			balance_set_speed(50);
			break;
		case GLUT_KEY_F6:
			balance_set_speed(0);
			break;
		case GLUT_KEY_F7:
			balance_set_speed(-50);
			break;
		case GLUT_KEY_F11:
			glutFullScreen();
			break;
		default:
			break;
	}
	update_glview();
	simul.car_move_extra_load(loadcentor);
}

void view_init()
{
	glutInitDisplayMode((is_double?GLUT_DOUBLE:GLUT_SINGLE)|GLUT_DEPTH|GLUT_MULTISAMPLE|GLUT_ALPHA);

	glutInitWindowSize(800,600);
	glutCreateWindow("balance");
	glXSwapIntervalSGI(0);

	glEnable(GL_MULTISAMPLE);
	glDisable(GL_DITHER);
	glEnable (GL_BLEND);
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_LINE_SMOOTH);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_MULTISAMPLE);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glutReshapeFunc(on_window_resize);
	glutMouseFunc(on_mouse_event);
	glutSpecialFunc(on_specialkey_event);
	glutKeyboardFunc(on_key_event);
	glutDisplayFunc(on_redraw);
}
