#pragma once

#include <stdarg.h>

class glMatrixKeeper{
public:
	glMatrixKeeper(){glPushMatrix();}
	glMatrixKeeper(b2Vec2 point){
		glPushMatrix();
		glTranslatef(point.x,point.y,0);
	}
	~glMatrixKeeper(){glPopMatrix();}
};

class glColorKeeper{
public:
	glColorKeeper(){ }

	~glColorKeeper(){glColor3f(1,1,1);}
};

class glOrthoMode{
public:
	glOrthoMode(){
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();

		glLoadIdentity();
		glOrtho(0,30,40,0,-1111,99999);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
	}
	virtual ~glOrthoMode(){
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}
};

class glStringDrawer: public glOrthoMode
{
	int row;
	int line;
	void _pos(){
		glRasterPos2i(row,line+1);
	}
public:
	glStringDrawer():row(0),line(0){

		_pos();
	};

	void pos(int _line,int _row){
		row = _row;
		line = _line;
		_pos();
	}

	void draw_str(const char * string){
		const char * c = string;
		while(*c){
			switch (*c) {
				case '\r':
					row = 0;
					_pos();
					break;
				case '\n':
					row = 0;
					line ++;
					_pos();
					break;
				default:
					glutBitmapCharacter(GLUT_BITMAP_9_BY_15,*c);
					row ++;
					break;
			}
			c++;
		}
	}

	int printf(const char *__restrict __format, ...)
	{
		va_list ap;
		va_start(ap,__format);

		char	buffer[1024];

		vsnprintf(buffer,sizeof(buffer),__format,ap);
		buffer[sizeof(buffer)-1]=0;

		draw_str(buffer);
	}

	~glStringDrawer(){
		glRasterPos2f(0,0);
	}
};
