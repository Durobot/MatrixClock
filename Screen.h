#ifndef _SCREEN_H_
#define _SCREEN_H_

class Screen
{
protected:
	unsigned int id;
	int whatever;

public:
	Screen(unsigned int scr_id);
  virtual ~Screen();

	unsigned int getId();
	virtual void update(unsigned long frame_millis, unsigned long prev_frame_millis) = 0;
};

#endif
