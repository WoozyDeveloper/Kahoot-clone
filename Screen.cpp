#include <graphics.h>
#include <iostream>
#include <unistd.h>

#include "Screen.h"

#define MAX_INPUT_LEN 30

void Screen::SetQuestion()
{
    //strcpy(this->question, "Oare cum fac eu sa bag din baza de date intrebari aici?");
    //Question q(this->question, this->screenWidth / 2, this->screenHeight / 4);
    /**
    Below I stored in the points leftUpPoint and downRightPoint of each button the coordonates (in [0] for button A, [1] for B and so on)
    ---> When the button is spawned the given coordonates are the middle of the button, so with some quick math
         i put in the variables the good coordonates. to recognise the click for each button.
    */
    outtextxy(this->screenWidth - 100, 50, "EXIT");

    Button b1("A", this->screenWidth / 2 - this->screenWidth / 4, this->screenHeight - 450); //scad 2 * height de buton minus inca 50 pt putina distantare
    this->leftUpPoint[0].x = this->screenWidth / 2 - this->screenWidth / 4 - ((this->screenWidth) / 2 - 200) / 2;
    this->leftUpPoint[0].y = this->screenHeight - 450 - 100;
    this->downRightPoint[0].x = this->leftUpPoint[0].x + (this->screenWidth) / 2 - 200;
    this->downRightPoint[0].y = this->leftUpPoint[0].y + 200;

    Button b2("B", this->screenWidth / 2 + this->screenWidth / 4, this->screenHeight - 450);
    this->leftUpPoint[1].x = this->screenWidth / 2 + this->screenWidth / 4 - ((this->screenWidth) / 2 - 200) / 2;
    this->leftUpPoint[1].y = this->screenHeight - 450 - 100;
    this->downRightPoint[1].x = this->leftUpPoint[1].x + (this->screenWidth) / 2 - 200;
    this->downRightPoint[1].y = this->leftUpPoint[1].y + 200;

    Button b3("C", this->screenWidth / 2 - this->screenWidth / 4, this->screenHeight - 200); //scad un height de buton
    this->leftUpPoint[2].x = this->screenWidth / 2 - this->screenWidth / 4 - ((this->screenWidth) / 2 - 200) / 2;
    this->leftUpPoint[2].y = this->screenHeight - 200 - 100;
    this->downRightPoint[2].x = this->leftUpPoint[2].x + (this->screenWidth) / 2 - 200;
    this->downRightPoint[2].y = this->leftUpPoint[2].y + 200;

    Button b4("D", this->screenWidth / 2 + this->screenWidth / 4, this->screenHeight - 200);
    this->leftUpPoint[3].x = this->screenWidth / 2 + this->screenWidth / 4 - ((this->screenWidth) / 2 - 200) / 2;
    this->leftUpPoint[3].y = this->screenHeight - 200 - 100;
    this->downRightPoint[3].x = this->leftUpPoint[3].x + (this->screenWidth) / 2 - 200;
    this->downRightPoint[3].y = this->leftUpPoint[3].y + 200;

    settextjustify(LEFT_TEXT, LEFT_TEXT);
    char screen_text[100];
    strcpy(screen_text, "Logged in as:");
    outtextxy(50, 50, screen_text);
    //daca are  nume ii scrie numele, altfel scrie guest.
    if (strcmp(this->username, "") != 0)
        outtextxy(50, 80, this->username);
    else
    {

        sprintf(this->username, "Guest_%d", this->joinedPersons);
        outtextxy(50, 80, this->username);
    }

    settextjustify(CENTER_TEXT, CENTER_TEXT);
    this->questions_set = true;
}

Screen::Screen()
{
    //strcpy(this->question, "Oare cum fac eu sa bag din baza de date intrebari aici?");
    //Question q(this->question, this->screenWidth / 2, this->screenHeight / 4);
    this->timer = 0;
    this->answer = -1;
    this->questions_set = false;
    this->joinedPersons = 0;

    char screen_text[100];
    char inputbuf[30] = "";
    int input_pos = 0;

    strcpy(this->username, "");
    this->screenWidth = getmaxwidth();
    this->screenHeight = getmaxheight();
    //printf("DIMENSIUNI %f x %f", screenWidth, screenHeight);

    initwindow(this->screenWidth, this->screenHeight);

    settextjustify(CENTER_TEXT, CENTER_TEXT);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 4);
    int the_end = 0;
    while (!the_end)
    {
        cleardevice();

        strcpy(screen_text, "Who are you? (press enter if you want to join as a guest");
        outtextxy(this->screenWidth / 2, this->screenHeight / 2 - 200, screen_text);

        strcpy(screen_text, "or write your username and press enter)");
        outtextxy(this->screenWidth / 2, this->screenHeight / 2 - 170, screen_text);

        outtextxy(this->screenWidth / 2, this->screenHeight / 2, inputbuf);
        char c = getch();
        switch (c)
        {
        case 8: //backspace
            if (input_pos)
            {
                inputbuf[input_pos] = '\0';
                input_pos--;
                inputbuf[input_pos] = 0;
            }
            break;
        default:
            if (c == 13) //enter
            {
                std::cout << this->username << '\n';
                the_end = 1;
                break;
            }
            if (input_pos < MAX_INPUT_LEN - 1 && c >= ' ' && c <= '~')
            {
                inputbuf[input_pos] = c;
                input_pos++;
                inputbuf[input_pos] = 0;
            }
        }
        strcpy(this->username, inputbuf); //get the latest update on the username
        //std::cout<<this->username<<'\n';
    }
    cleardevice();
    std::cout << "AM STERS TOT SI INTRU N LOBBY\n";
    //this->Lobby();
    cleardevice();
    std::cout << "AM STERS TOT SI INTRU N SET QUESTION\n";
    this->SetQuestion();
    cleardevice();
}

double Screen::GetWidth()
{
    return this->screenWidth;
}

double Screen::GetHeight()
{
    return this->screenHeight;
}
