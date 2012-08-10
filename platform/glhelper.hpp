#pragma once

class glMatrixKeeper{
public:
	glMatrixKeeper(){glPushMatrix();}
	glMatrixKeeper(b2Vec2 point){
		glMatrixKeeper();
		glTranslatef(point.x,point.y,0);
	}
	~glMatrixKeeper(){glPopMatrix();}
};

class glColorKeeper{
public:
	glColorKeeper(){ }

	~glColorKeeper(){glColor3f(1,1,1);}
};
