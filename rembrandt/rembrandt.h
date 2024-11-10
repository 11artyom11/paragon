#ifndef __REMBRANDT_H__
#define __REMBRANDT_H__

#include <GL/glut.h>
#include "point.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <utility>
/* 
    If set close **CONSEQUENT** distances are approximated to one
 */
constexpr bool approximatePoints = 1;
constexpr GLfloat minDistance = 0.1;

struct host
{
    std::pair<double, double> host_coordinates;
    std::string city;
};

void displayCB();
void reshapeCB(int w, int h);
void timerCB(int millisec);
void keyboardCB(unsigned char key, int x, int y);
void mouseCB(int button, int stat, int x, int y);
void mouseMotionCB(int x, int y);

void initGL();
int  initGLUT(int argc, char **argv);
bool initSharedMem();
void clearSharedMem();
void initLights();
void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ);
void drawString(const char *str, int x, int y, float color[4], void *font);
void drawString3D(const char *str, float pos[3], float color[4], void *font);
void toOrtho();
void toPerspective();
GLuint loadTexture(const char* fileName, bool wrap=true);

int init_rembrandt_internal(int argc, char **argv,char* strlst[], int strcnt, const std::vector<host>& hosts);
void draw_traceroutes();
void draw_string_stack(char* strlst[], int strcnt);

point to_point(GLfloat latitude, GLfloat longitude, GLfloat altitude = 0.0f, GLfloat radius = 1.0f);
point get_middlepoint (const point& p1, const point& p2);
void pinCities(const std::unordered_map<std::string, point>& map);



// sphere: min sector = 3, min stack = 2

#endif /* __REMBRANDT_H__ */