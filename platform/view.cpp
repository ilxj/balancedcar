/*
 * Draw Simulated Results
 */

#include "constant.h"
#include "view.hpp"

#include <unistd.h>
#include <stdio.h>
#include <GL/glut.h>

static void 	draw_body(b2Body * body)
{
	float angle = body->GetAngle() * RADTODEG;

	glPushMatrix();

	glTranslatef(body->GetPosition().x,body->GetPosition().y,0.0);
	glRotated(angle,0,0,-1);

	for(b2Fixture * fixtures = body->GetFixtureList();fixtures;fixtures = fixtures->GetNext())
	{
		b2Shape * shape = fixtures->GetShape();

		switch (shape->GetType())
		{
			case b2Shape::Type::e_polygon:
			{
				b2PolygonShape * pshape = ((b2PolygonShape*)shape);

				glBegin(GL_POLYGON);
				for(int i=0; i < pshape->GetVertexCount(); i ++)
				{
					b2Vec2 v = pshape->GetVertex(i);
					glVertex2f(v.x,v.y);
				}
				glEnd();
			}
			break;
			default:
				break;
		}
	}
	glPopMatrix();
}

void view_draw_frame(b2World & world)
{
	glClear(GL_COLOR_BUFFER_BIT|GL_ACCUM_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

	glLoadIdentity();

	for(b2Body * body = world.GetBodyList();body;body=body->GetNext())
	{
		draw_body(body);
	}
	
	glutSwapBuffers();
}

static void on_window_resize(int w , int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-w/80.0,w/80.0, -1,h/40.0 - 1);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0,0,w,h);
	return ;
}

void view_init()
{
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_MULTISAMPLE);

	glutInitWindowSize(800,600);
	glutCreateWindow("balance");

	glEnable(GL_MULTISAMPLE);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(-10,10,-1,14);
	glMatrixMode(GL_MODELVIEW);

	glutReshapeFunc(on_window_resize);
}
