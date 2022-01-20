#include "Button.h"
#include "Screen.h"

#include <fstream>
#include <graphics.h>
#include <unistd.h>
#include <iostream>

using namespace std;

int main()
{
    int testing_array[100] = {-100};

    ofstream file("output.txt");

    int gdriver = DETECT, gmode, errorcode;
    /* initialize graphics mode */

    initgraph(&gdriver, &gmode, "");

    /* read result of initialization */
    errorcode = graphresult();
    if (errorcode != grOk) /* an error occurred */
    {
        printf("Graphics error: %s\n", grapherrormsg(errorcode));
        printf("Press any key to halt:");
        getch();
        exit(1); /* return with error code */
    }

    Screen screen;
    int i = 0;
    if (screen.questions_set == true)
    {
        std::cout << "ACM AM INTRAT IN NEBUNIE\n";
        while (1)
        {
            screen.GetButtonClick();
            switch (screen.answer)
            {
            case 0:
                std::cout << "A\n";

                break;
            case 1:
                std::cout << "B\n";
                return 0;
                break;
            case 2:
                std::cout << "C\n";
                return 0;
                break;
            case 3:
                std::cout << "D\n";
                return 0;
                break;
            default:
                std::cout << "NIMIC\n";
                break;
            }
            std::cout << "Answer " << screen.answer << '\n';
        }
    }
    getch();
    closegraph();
    return 0;
}
