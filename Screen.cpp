#include "Screen.h"

Screen::Screen(unsigned int scr_id)
{
	this->id = scr_id;
}

Screen::~Screen()
{}

unsigned int Screen::getId()
{
	return this->id;
}
