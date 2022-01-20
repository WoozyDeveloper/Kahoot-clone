#include "Question.h"

#include <graphics.h>
#include <string>

Question::Question(char question[], double ox, double oy)
{
    this->ox = ox;
    this->oy = oy;
    outtextxy(ox, oy, question);
}
