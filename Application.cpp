#include <Arduino.h>

#include "Application.h"

Application::Application()
{}

Application::~Application()
{
    for(int i = 0; i < sizeof(this->screens) / sizeof(this->screens[0]); i++)
        delete this->screens[i];
}

void Application::update()
{
    this->prev_frame_millis = this->frame_millis;
    this->frame_millis = millis();

    if(this->active_screen != NULL)
      this->active_screen->update(this->frame_millis, this->prev_frame_millis);
}

Screen* Application::findScreen(unsigned int scr_id)
{
    for(int i = 0; i < sizeof(this->screens) / sizeof(this->screens[0]); i++)
        if(this->screens[i]->getId() == scr_id)
            return this->screens[i];

    return NULL;
}

bool Application::switchToScreen(unsigned int scr_id)
{
    Screen* new_scr = this->findScreen(scr_id);
    if(new_scr == NULL)
        return false;

    this->active_screen = new_scr;
    return true;
}
