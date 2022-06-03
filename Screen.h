#ifndef _SCREEN_H_
#define _SCREEN_H_

class Screen
{
protected:
	unsigned int id;
	int whatever;

public:
	Screen(unsigned int scr_id);
	unsigned int getId();
	virtual void update() = 0;
};

#endif

