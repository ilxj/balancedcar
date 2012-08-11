#pragma once

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

class glDrawString{
	glMatrixKeeper restorer;
public:
	glDrawString(const char * string){
		const char * c = string;
		while(*c){
//			glutBitmapCharacter(GLUT_BITMAP_9_BY_15,*c);
			glutStrokeCharacter(GLUT_STROKE_ROMAN,*c);
			c++;
		}
	}
	~glDrawString(){
		glRasterPos2f(0,0);
	}
};
