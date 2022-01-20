#pragma once
#include<string>

class Button
{
    private:
        double x_coord,y_coord;
        std::string text;
    public:
        Button(std::string,double ox,double oy);
};
