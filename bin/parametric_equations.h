#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int f1(double u, double xy[2]); //Circle

int f2(double u, double xy[2]);
//Rounded Square
//x^4 + y^4 = 1

int f3(double u, double xy[2]);
//Square
//|x| + |y| = 1

int f4(double u, double xy[2]);
//Asteroid
//sqrt(|x|) + sqrt(|y|) = 1

int f5(double u, double xy[2]);
//Hyperbola
//x^2 - y^2 = 1

int f6(double u, double xy[2]);
//Parabola
//x^2 = y

int f7(double u, double xy[2]);
//Lemon
//x^2 - (1 - (y^2))^3 = 0

int f8(double u, double v, double xyz[3]);
//Sphere

int f9(double u, double v, double xyz[3]);
//Hyperboloid
