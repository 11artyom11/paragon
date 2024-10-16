#ifndef __POINT_H__
#define __POINT_H__

#include <GL/glut.h>

class point;

typedef void(*painter_func_t)(const point& p1, const point& p2, const point& p3);

class point 
{
    public:
        point (GLfloat x, GLfloat y, GLfloat z);
        void draw_point (void);
        GLfloat getx() const {return x;}; 
        GLfloat gety() const {return y;}; 
        GLfloat getz() const {return z;}; 
        GLfloat getlat() const {return lat;};
        GLfloat getlon() const {return lon;};
        GLfloat getalt() const {return alt;};


        void setcord (GLfloat lat, GLfloat lon, GLfloat alt) {this->lat = lat; this->lon = lon; this->alt = alt;};
    private:
        GLfloat x;
        GLfloat y;
        GLfloat z;
        GLfloat lat;
        GLfloat lon;
        GLfloat alt;
};

void draw_curve(painter_func_t painter_cb, const point& p1, const point& p2, const point& p3);
void draw_quadratic_curve(const point& p1, const point& p2, const point& p3);
void draw_simple_curve(const point& p1, const point& p2, const point& p3);
void draw_bezier_3(const point& p1, const point& p2, const point& p3);

#endif /* __POINT_H__ */