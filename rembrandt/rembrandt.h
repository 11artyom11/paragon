#ifndef __REMBRANDT_H__
#define __REMBRANDT_H__

#include <GL/glut.h>
#include "point.h"
#include <vector>

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

int init_rembrandt_internal(int argc, char **argv,char* strlst[], int strcnt, const std::vector<std::vector<double>>& host_coords);
void draw_traceroutes();
void draw_string_stack(char* strlst[], int strcnt);

point to_point(GLfloat latitude, GLfloat longitude, GLfloat altitude = 0.0f, GLfloat radius = 1.0f);
point get_middlepoint (const point& p1, const point& p2);

// sphere: min sector = 3, min stack = 2

#endif /* __REMBRANDT_H__ */