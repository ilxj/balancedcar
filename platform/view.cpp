/*
 * Draw Simulated Results
 */

#include "constant.h"
#include "hal.h"
#include "view.hpp"
#include "balance.h"

#include <unistd.h>
#include <stdio.h>
#include <GL/glut.h>

#include "glhelper.hpp"
#include "emu.hpp"


static float scale = 40;
static float w=800,h=600;
static float centor;

static void update_glview()
{
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w/scale -centor ,w/scale -centor, -1,h*2/scale - 1,-1000,10000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
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

		if(fixtures->GetUserData())
		{
			glColor3fv(static_cast<GLfloat*>(fixtures->GetUserData()));
		}else
			glColor3f(1,1,1);

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

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,1,0,1,-1111,99999);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0,0,w,h);
	glLoadIdentity();

	glTranslated(0, 10, 0);
//	glScaled(0.03, 0.03, 1);
	{
		glDrawString str("hello");
	}

//	glTranslated(0, -110, 0);
	glDrawString str2("hello2");
}

void view_draw_frame(b2World & world)
{
	update_glview();

	glClear(GL_COLOR_BUFFER_BIT|GL_ACCUM_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

	for(b2Body * body = world.GetBodyList();body;body=body->GetNext())
	{
		draw_body(body);
	}
	
	// draw arrow to reflect the PWM settings
	if(hal_get_pwm()!=0)
	{
		glMatrixKeeper mk(emulation_get_car()->GetWorldPoint(b2Vec2(0,-2)));

		glColorKeeper ck;

		int s;

		float scale=10;

		s = 6;

		if(hal_get_pwm()<0)
			s = -s;

		glScaled(1/scale,1/scale,1);

		glBegin(GL_LINES);

		glColor3f(0,1,1);

		glVertex2f(0,0);
		glVertex2f(-hal_get_pwm(),0);

		glVertex2f(-hal_get_pwm(),0);
		glVertex2f(-hal_get_pwm() + s,fabs(s));

		glVertex2f(-hal_get_pwm(),0);
		glVertex2f(-hal_get_pwm() + s,-fabs(s));

		glEnd();
	}

	//draw some debug data
//	draw_status();

	glutSwapBuffers();
}

static void on_window_resize(int _w , int _h)
{
	w = _w;
	h = _h;
	update_glview();
}

static void on_mouse_event(int, int, int, int)
{
	printf("%s\n",__func__);
}

static void on_keyup_event( int key, int, int )
{
	hal_set_pwm(0);
}

static void on_key_event( int key, int, int )
{
	switch (key)
	{
		case GLUT_KEY_LEFT:
			centor +=1;
			break;
		case GLUT_KEY_RIGHT:
			centor -=1;
			break;
		case GLUT_KEY_UP:
			scale /=2;
			break;
		case GLUT_KEY_DOWN:
			scale *=2;
			break;
		case GLUT_KEY_F2:
			simulate_move_extra_load(-2.5);
			break;
		case GLUT_KEY_F3:
			simulate_move_extra_load(0);
			break;
		case GLUT_KEY_F4:
			simulate_move_extra_load(2.5);
			break;
		case GLUT_KEY_F12:
			glutDestroyWindow(glutGetWindow());
//			exit(0);
		case GLUT_KEY_F5:
			balance_set_speed(50);
			break;
		case GLUT_KEY_F6:
			balance_set_speed(0);
			break;
		case GLUT_KEY_F7:
			balance_set_speed(-50);
			break;
		default:
			break;
	}
	update_glview();
}

void view_init()
{
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_MULTISAMPLE);

	glutInitWindowSize(800,600);
	glutCreateWindow("balance");
	glutFullScreen();

	glEnable(GL_MULTISAMPLE);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(-10,10,-1,14);
	glMatrixMode(GL_MODELVIEW);

	glutReshapeFunc(on_window_resize);

	glutMouseFunc(on_mouse_event);
	glutSpecialFunc(on_key_event);
	glutSpecialUpFunc(on_keyup_event);

	glXSwapIntervalSGI(0);
}
