#ifndef _COLOR_H
#define _COLOR_H

class Color {

    double red, green, blue, special, specularity, transparency;
    public:

    Color();

    Color (double, double, double, double);
    Color(double, double, double, double,  double, double);
    //method functions

    double getRed();
    double getGreen();
    double getBlue();
    double getSpecial();
    double getSpecularity();
    double getTransparency();

    void setRed(double r);
    void setGreen(double g);
    void setBlue(double b);
    void setSpecial(double s);
    void setSpecularity(double s);
    void setTransparency(double s);

    double brightness();

    Color scalar(double scalar);
    Color add(Color color);
    Color multiply(Color color);
    Color average(Color color);
    Color clip();
};

#endif