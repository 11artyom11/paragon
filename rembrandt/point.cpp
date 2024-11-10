#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <cmath>
#include "point.h"

static GLfloat anim = 0.1f;

point::point() : x{0}, y{0}, z{0} {
    // std::cout << "Default constructor called\n";
}

point::point(GLfloat x, GLfloat y, GLfloat z) : x{x}, y{y}, z{z} {
    // std::cout << "Parameterized constructor called\n";
}

point::point(const point& other) : x{other.x}, y{other.y}, z{other.z} {
    // std::cout << "Copy constructor called\n";
}

point::point(point&& other) noexcept : x{other.x}, y{other.y}, z{other.z} {
    other.x = other.y = other.z = 0;  // Optional: set other to a default state
    // std::cout << "Move constructor called\n";
}

point& point::operator=(const point& other) {
    if (this != &other) {  // Self-assignment check
        x = other.x;
        y = other.y;
        z = other.z;
        // std::cout << "Copy assignment operator called\n";
    }
    return *this;
}

point& point::operator=(point&& other) noexcept {
    if (this != &other) {  // Self-assignment check
        x = other.x;
        y = other.y;
        z = other.z;
        other.x = other.y = other.z = 0;  // Optional: set other to a default state
        // std::cout << "Move assignment operator called\n";
    }
    return *this;
}

// Destructor
point::~point() {
    // std::cout << "Destructor called\n";
}

void point::draw_point(void)
{
        glVertex3f(x,y,z);
}

GLfloat point::distance(const point& p) const 
{
    return  sqrt(pow((this->x-p.x),2) + pow((this->y-p.y),2) + pow((this->z-p.z),2));
}

void draw_bezier_3(const point& p1, const point& p2, const point& p3)
{
    glBegin(GL_LINE_STRIP);
    glPointSize(50.0);
    for (GLfloat i = 0; i <= anim; ++i) {
        if (anim < 100) anim += 0.002f;
        float t = (float)i / 100.0;
        GLfloat p[3];
        p[0] = (1 - t) * (1 - t) * p1.getx() + 2 * (1 - t) * t * p2.getx() + t * t * p3.getx();
        p[1] = (1 - t) * (1 - t) * p1.gety() + 2 * (1 - t) * t * p2.gety() + t * t * p3.gety();
        p[2] = (1 - t) * (1 - t) * p1.getz() + 2 * (1 - t) * t * p2.getz() + t * t * p3.getz();
        glVertex3fv(p);
    }
    glEnd();
}

void draw_quadratic_curve(const point& p1, const point& p2, const point& p3) {
    for (GLfloat i = 0; i <= anim; ++i) {
        if (anim < 100) anim += 0.01f;
        float t = (float)i / 100.0;
        float u = 1 - t;
        float tt = t * t;
        float uu = u * u;

        float x = uu * p1.getx() + 2 * u * t * p2.getx() + tt * p3.getx();
        float y = uu * p1.gety() + 2 * u * t * p2.gety() + tt * p3.gety();
        float z = uu * p1.getz() + 2 * u * t * p2.getz() + tt * p3.getz();

        glVertex3f(x, y, z);
    }
}

void draw_curve(painter_func_t painter_cb, const point& p1, const point& p2, const point& p3)
{
    glBegin(GL_LINE_STRIP);
    glColor3f(1.0, 1.0, 1.0);
    glPointSize(1.0f);
    painter_cb(p1,p2,p3);
    return glEnd();
}