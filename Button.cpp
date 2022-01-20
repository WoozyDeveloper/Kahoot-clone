#include "Button.h"

#include <string>
#include <graphics.h>

#define TEXT_SCALE 20

Button::Button(std::string text, double x, double y)
{
    //scale of the button
    double ox_scale = getmaxwidth() / 2 - 200;
    double oy_scale = 200;

    //put values
    this->text = text;
    this->x_coord = x;
    this->y_coord = y;

    //display the rectangle of the button
    rectangle(x - ox_scale / 2, y - oy_scale / 2 - 10, x + ox_scale / 2, y + oy_scale / 2);

    //set the text inside the rectangle
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    char *my_text = (char *)malloc(text.size() + 1);
    int i, current_position = 0;

    //put in my_text[] the text
    for (i = 0; i < text.size(); i++)
        my_text[i] = text[i];
    my_text[i] = '\0';

    outtextxy(x, y, my_text);
}