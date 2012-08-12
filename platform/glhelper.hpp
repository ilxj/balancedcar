#pragma once

#include <stdarg.h>
#include <FTGL/ftgl.h>

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
	glColorKeeper(const b2Color & color){
		glColor3f(color.r,color.g,color.b);
	}

	~glColorKeeper(){glColor3f(1,1,1);}
};

class glOrthoMode{
public:
	glOrthoMode(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top){
		glMatrixMode (GL_PROJECTION);
		glPushMatrix();

		glLoadIdentity();
		glOrtho(left,right,bottom,top,-10,10);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glClear(GL_DEPTH_BUFFER_BIT);
	}
	virtual ~glOrthoMode(){
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}
};

class glStringDrawer: protected glOrthoMode, protected FTPolygonFont , protected FTOutlineFont
{
private:
	b2Color color;
	FTPoint position;
	int char_size(const char *utf8){
		if(!(*utf8 & 0x80))
		{
			return 1;
		}
		else if ((*utf8 & 0xE0) == 0xC0)
		{
			return 2;
		}else if((*utf8 & 0xF0) == 0xE0)
		{
			return 3;
		}else if((*utf8 & 0xF8) == 0xF0)
		{
			return 4;
		}
		else if ((*utf8 & 0xFC) == 0xF8)
		{
			return 5;
		}
		else if ((*utf8 & 0xFE) == 0xFC)
		{
			return 6;
		}
		::printf("error decode utf8 string\n");
		exit(-1);
	}
public:
	glStringDrawer(float fontsize=1,const char *fontFilePath = "/usr/share/fonts/wqy-microhei/wqy-microhei.ttc")
		:FTPolygonFont(fontFilePath),
		 FTOutlineFont(fontFilePath),
		 glOrthoMode(0,40/fontsize,30/fontsize-1,-1)
	{
		FTOutlineFont::FaceSize(1);
		FTPolygonFont::FaceSize(1);
	};

	void draw_str(const char * string){
		char * copyed_string = strdup(string);

		char * str = copyed_string;

		char * ready = str;

		while(*ready){
			ready = strchrnul(str,'\n');

			glPushMatrix();
			glScalef(1,-1,1);

			int c = *ready;
			*ready = 0;
			FTPoint oldposition=position;

			glColor3f(0,0,0);
			position = FTOutlineFont::Render(str,-1,oldposition);
			glColor3f(1,1,1);
			position = FTPolygonFont::Render(str,-1,oldposition);
			glPopMatrix();
			str = ready+1;
			if( c ){
				position.X(0);
				position.Y(position.Y()-1);
				ready = str;
			}
		}
		free(copyed_string);
	}

	void draw_str(int x, int y, const char * string){
		const char * c = string;
		position = FTPoint(x,y,0);

		draw_str(string);
	}

	int printf(const char *__restrict __format, ...)
	{
		va_list ap;
		va_start(ap,__format);

		char	buffer[1024]={0};

		vsnprintf(buffer,sizeof(buffer),__format,ap);
		buffer[sizeof(buffer)-1]=0;

		draw_str(buffer);
	}

	~glStringDrawer(){
		glRasterPos2f(0,0);
	}
};
