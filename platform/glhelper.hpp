#pragma once

class glMatrixKeeper{
public:
	glMatrixKeeper(){glPushMatrix();}
	~glMatrixKeeper(){glPopMatrix();}
};
