#include "Color.h"

double Color::getRed(){return red;}
double Color::getGreen(){return green;}
double Color::getBlue(){ return blue;}
double Color::getSpecial(){ return special;}
double Color::getSpecularity(){ return specularity;}
double Color::getTransparency(){ return transparency;}

void Color::setRed(double r){ red = r;};
void Color::setGreen(double g){ green = g; };
void Color::setBlue(double b){ blue = b;};
void Color::setSpecial(double s){special = s;};
void Color::setSpecularity(double s){ specularity = s;}
void Color::setTransparency(double t){ transparency = t;}
double Color::brightness(){
    return(red + green + blue) /3;
}

Color Color::scalar(double scalar){
    return Color(red * scalar, green*scalar, blue*scalar, special);
}
Color Color::add(Color color){
    return Color(red + color.getRed(), green + color.getGreen(), blue + color.getBlue(), special);
}
Color Color::multiply(Color color){
    return Color( red * color.getRed(), green * color.getGreen(), blue * color.getBlue(), special);
}
Color Color::average(Color color){
    return Color((red + color.getRed())/2,(green + color.getGreen())/2,(blue + color.getBlue())/2, special );
}
Color Color::clip(){
    double allLight = red + green + blue;
    double excessLight = allLight - 3;
    if (excessLight > 0){
        red = red + excessLight * (red/allLight);
        blue = blue + excessLight * (blue/allLight);
        green = green + excessLight * (green/allLight);
    }
    if(red > 1){ red = 1;}
    if(green > 1){ green = 1;}
    if(blue > 1){ blue = 1;}
    if(red < 0){ red = 0;}
    if(green < 0){ green = 0;}
    if(blue < 0){ blue = 0;}
    return Color (red, green, blue, special);
}

Color::Color(){
    red = 0.5;
    green = 0.5;
    blue = 0.5;
}

Color::Color(double r, double g, double b, double s){
    red = r;
    green = g;
    blue = b;
    specularity = s;
    transparency = 0;
    special = 0;
    
}
Color::Color(double r, double g, double b, double s,  double t, double spec){
    red = r;
    green = g;
    blue = b;
    specularity = s;
    transparency = t;
    special = spec;
}