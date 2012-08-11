#pragma once

#include <Box2D/Box2D.h>

#define	GLUT_WHEEL_UP_BUTTON		0x0003
#define	GLUT_WHEEL_DOWN_BUTTON		0x0004

void view_init();
void view_draw_frame(b2World & world);

extern "C" {
int glXSwapIntervalSGI(unsigned int interval);
};
