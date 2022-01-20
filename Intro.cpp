#include <graphics.h>

#include "Intro.h"

Intro::Intro()
{
    this->screenWidth = getmaxwidth();
    this->screenHeight = getmaxheight();
    //initwindow(this->screenWidth, this->screenHeight);
}
