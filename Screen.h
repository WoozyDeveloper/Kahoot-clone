#pragma once
#include "Question.h"
#include "Button.h"

#include <string>

class Screen
{
public:
    int timer;
    char question[200];
    int joinedPersons;
    struct Point
    {
        int x, y;
    } leftUpPoint[4], downRightPoint[4];

    double screenWidth, screenHeight;
    char username[30];
    bool questions_set, time_up;
    int answer;
    int index;
    Screen();
    void SetQuestion();
    double GetWidth();
    double GetHeight();
};
