
#include "rembrandt.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string.h>
#include <fstream>
#include <cmath>
#include <time.h>
#include <unistd.h>
#include <chrono>
#include "bmp.h"
#include "sphere.h"
#include "point.h"

// constants
const int   SCREEN_WIDTH    = 1920;
const int   SCREEN_HEIGHT   = 1136;
const float CAMERA_DISTANCE = 4.0f;
const int   TEXT_WIDTH      = 8;
const int   TEXT_HEIGHT     = 13;


// global variables
void *font = GLUT_BITMAP_8_BY_13;
int screenWidth;
int screenHeight;
bool mouseLeftDown;
bool mouseRightDown;
bool mouseMiddleDown;
float mouseX, mouseY;
float cameraAngleX;
float cameraAngleY;
float cameraDistance;
int drawMode;
GLuint texId;
int imageWidth;
int imageHeight;
sphere sphere2(1.0f, 36*2, 18*2, true,  2);    // radius, sectors, stacks, smooth(default), Y-up
std::string hostnames[30]; /* Magic 30 */
std::vector<host> hosts;

int hostname_count;
int magic_lat = 0;
int magic_lon = 0;
///////////////////////////////////////////////////////////////////////////////
int init_rembrandt_internal(int argc, char **argv, char* strlst[], int strcnt, const std::vector<host>& host_c)
{
    hostname_count = strcnt;
    hosts = host_c;
    std::cout << "[RMBRDT] hostnames\n";
    for (int i = 0; i < hostname_count; i++) {
        hostnames[i] = strlst[i];
        std::cout << "hostname["<<i<<"] : " << hostnames[i] << "\n";
        std::cout << "city ["<<i<<"] : " << hosts[i].city << "\n";
        std::cout << "coordinates["<<i<<"][lat] : " << hosts[i].host_coordinates.first << "\n";
        std::cout << "coordinates["<<i<<"][lon] : " << hosts[i].host_coordinates.second << "\n";
    }
    // init global vars
    initSharedMem();

    // init GLUT and GL
    initGLUT(argc, argv);
    initGL();

    // load BMP image
    // texId = loadTexture("earth2048.bmp", true);
    texId = loadTexture("/home/tyom/workspace/paragon/rembrandt/8081_earthmap4k.bmp", true);
    if (texId == 0) 
    {
        return 12;
    }
    // the last GLUT call (LOOP)
    // window will be shown and display callback is triggered by events
    // NOTE: this call never return main().
       glutMainLoop(); /* Start GLUT event-processing loop */

    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// initialize GLUT for windowing
///////////////////////////////////////////////////////////////////////////////
int initGLUT(int argc, char **argv)
{
    // GLUT stuff for windowing
    // initialization openGL window.
    // it is called before any other GLUT routine
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);   // display mode

    glutInitWindowSize(screenWidth, screenHeight);  // window size

    glutInitWindowPosition(100, 100);               // window location

    // finally, c // Set display mode (double buffering for smooth animation)
    // Window will not displayed until glutMainLoop() is called
    // it returns a unique ID
    int handle = glutCreateWindow(argv[0]);     // param is the title of window

    // register GLUT callback functions
    glutDisplayFunc(displayCB);
    glutTimerFunc(33, timerCB, 33);             // redraw only every given millisec
    glutReshapeFunc(reshapeCB);
    glutKeyboardFunc(keyboardCB);
    glutMouseFunc(mouseCB);
    glutMotionFunc(mouseMotionCB);
    glutIdleFunc(update_progress);
    return handle;
}



///////////////////////////////////////////////////////////////////////////////
// initialize OpenGL
// disable unused features
///////////////////////////////////////////////////////////////////////////////
void initGL()
{
    glShadeModel(GL_SMOOTH);                    // shading mathod: GL_SMOOTH or GL_FLAT
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);      // 4-byte pixel alignment

    // enable /disable features
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    //glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);

    // track material ambient and diffuse from surface color, call it before glEnable(GL_COLOR_MATERIAL)
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    glClearColor(0, 0, 0, 0);                   // background color
    glClearStencil(0);                          // clear stencil buffer
    glClearDepth(1.0f);                         // 0 is near, 1 is far
    glDepthFunc(GL_LEQUAL);

    initLights();
}



///////////////////////////////////////////////////////////////////////////////
// write 2d text using GLUT
// The projection matrix must be set to orthogonal before call this function.
///////////////////////////////////////////////////////////////////////////////
void drawString(const char *str, int x, int y, float color[4], void *font)
{
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
    glDisable(GL_LIGHTING);     // need to disable lighting for proper text color
    glDisable(GL_TEXTURE_2D);

    glColor4fv(color);          // set text color
    glRasterPos2i(x, y);        // place text position

    // loop all characters in the string
    while(*str)
    {
        glutBitmapCharacter(font, *str);
        ++str;
    }

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glPopAttrib();
}



///////////////////////////////////////////////////////////////////////////////
// draw a string in 3D space
///////////////////////////////////////////////////////////////////////////////
void drawString3D(const char *str, float pos[3], float color[4], void *font)
{
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
    glDisable(GL_LIGHTING);     // need to disable lighting for proper text color
    glDisable(GL_TEXTURE_2D);

    glColor4fv(color);          // set text color
    glRasterPos3fv(pos);        // place text position

    // loop all characters in the string
    while(*str)
    {
        glutBitmapCharacter(font, *str);
        ++str;
    }

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glPopAttrib();
}



///////////////////////////////////////////////////////////////////////////////
// initialize global variables
///////////////////////////////////////////////////////////////////////////////
bool initSharedMem()
{
    screenWidth = SCREEN_WIDTH;
    screenHeight = SCREEN_HEIGHT;

    mouseLeftDown = mouseRightDown = mouseMiddleDown = false;
    mouseX = mouseY = 0;

    cameraAngleX = cameraAngleY = 0.0f;
    cameraDistance = CAMERA_DISTANCE;

    drawMode = 0; // 0:fill, 1: wireframe, 2:points

    // change up axis to +Y
    //sphere1.setUpAxis(2);
    //sphere2.setUpAxis(2);

    // debug
    sphere2.printSelf();
    

    return true;
}



///////////////////////////////////////////////////////////////////////////////
// clean up global vars
///////////////////////////////////////////////////////////////////////////////
void clearSharedMem()
{
}



///////////////////////////////////////////////////////////////////////////////
// initialize lights
///////////////////////////////////////////////////////////////////////////////
void initLights()
{
    // set up light colors (ambient, diffuse, specular)
    GLfloat lightKa[] = {.3f, .3f, .3f, 1.0f};  // ambient light
    GLfloat lightKd[] = {.7f, .7f, .7f, 1.0f};  // diffuse light
    GLfloat lightKs[] = {1, 1, 1, 1};           // specular light
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightKa);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightKd);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightKs);

    // position the light
    float lightPos[4] = {1, 1, 1, 1}; // directional light
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    glEnable(GL_LIGHT0);                        // MUST enable each light source after configuration
}



///////////////////////////////////////////////////////////////////////////////
// set camera position and lookat direction
///////////////////////////////////////////////////////////////////////////////
void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(posX, posY, posZ, targetX, targetY, targetZ, 0, 1, 0); // eye(x,y,z), focal(x,y,z), up(x,y,z)
}



///////////////////////////////////////////////////////////////////////////////
// load raw image as a texture
///////////////////////////////////////////////////////////////////////////////
GLuint loadTexture(const char* fileName, bool wrap)
{
    Image::bmp bmp;
    if(!bmp.read(fileName))
        return 0;     // exit if failed load image

    // get bmp info
    int width = bmp.getWidth();
    int height = bmp.getHeight();
    const unsigned char* data = bmp.getDataRGB();
    GLenum type = GL_UNSIGNED_BYTE;    // only allow BMP with 8-bit per channel

    // We assume the image is 8-bit, 24-bit or 32-bit BMP
    GLenum format;
    int bpp = bmp.getBitCount();
    if(bpp == 8)
        format = GL_LUMINANCE;
    else if(bpp == 24)
        format = GL_RGB;
    else if(bpp == 32)
        format = GL_RGBA;
    else
        return 0;               // NOT supported, exit

    // gen texture ID
    GLuint texture;
    glGenTextures(1, &texture);

    // set active texture and configure it
    glBindTexture(GL_TEXTURE_2D, texture);

    // select modulate to mix texture with color for shading
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

    // if wrap is true, the texture wraps over at the edges (repeat)
    //       ... false, the texture ends at the edges (clamp)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap ? GL_REPEAT : GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap ? GL_REPEAT : GL_CLAMP);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // copy texture data
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, type, data);
    //glGenerateMipmap(GL_TEXTURE_2D);

    // build our texture mipmaps
    switch(bpp)
    {
    case 8:
        gluBuild2DMipmaps(GL_TEXTURE_2D, 1, width, height, GL_LUMINANCE, type, data);
        break;
    case 24:
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, type, data);
        break;
    case 32:
        gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGBA, type, data);
        break;
    }

    bmp.printSelf();
    return texture;
}



///////////////////////////////////////////////////////////////////////////////
// display info messages
///////////////////////////////////////////////////////////////////////////////
void showInfo()
{
    // backup current model-view matrix
    glPushMatrix();                     // save current modelview matrix
    glLoadIdentity();                   // reset modelview matrix

    // set to 2D orthogonal projection
    glMatrixMode(GL_PROJECTION);        // switch to projection matrix
    glPushMatrix();                     // save current projection matrix
    glLoadIdentity();                   // reset projection matrix
    //gluOrtho2D(0, screenWidth, 0, screenHeight); // set to orthogonal projection
    glOrtho(0, screenWidth, 0, screenHeight, -1, 1); // set to orthogonal projection

    float color[4] = {1, 1, 1, 1};

    std::stringstream ss;
    ss << std::fixed << std::setprecision(3);

    ss << "sphere Radius: " << sphere2.getRadius() << std::ends;
    drawString(ss.str().c_str(), 1, screenHeight-TEXT_HEIGHT, color, font);
    ss.str("");

    ss << "Sector Count: " << sphere2.getSectorCount() << std::ends;
    drawString(ss.str().c_str(), 1, screenHeight-(2*TEXT_HEIGHT), color, font);
    ss.str("");

    ss << "Stack Count: " << sphere2.getStackCount() << std::ends;
    drawString(ss.str().c_str(), 1, screenHeight-(3*TEXT_HEIGHT), color, font);
    ss.str("");

    ss << "Vertex Count: " << sphere2.getVertexCount() << std::ends;
    drawString(ss.str().c_str(), 1, screenHeight-(4*TEXT_HEIGHT), color, font);
    ss.str("");

    ss << "Index Count: " << sphere2.getIndexCount() << std::ends;
    drawString(ss.str().c_str(), 1, screenHeight-(5*TEXT_HEIGHT), color, font);
    ss.str("");

    for (size_t i = 0; i < hostname_count; ++i) {
        ss << hostnames[i] << std::ends;
        drawString(ss.str().c_str(), 1, screenHeight-((6+i)*TEXT_HEIGHT), color, font);
        // std::cout << hostnames[i] << "  << hostname\n";
        ss.str("");
    }

    // unset floating format
    ss << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);

    // restore projection matrix
    glPopMatrix();                   // restore to previous projection matrix

    // restore modelview matrix
    glMatrixMode(GL_MODELVIEW);      // switch to modelview matrix
    glPopMatrix();                   // restore to previous modelview matrix
}



///////////////////////////////////////////////////////////////////////////////
// set projection matrix as orthogonal
///////////////////////////////////////////////////////////////////////////////
void toOrtho()
{
    // set viewport to be the entire window
    glViewport(0, 0, (GLsizei)screenWidth, (GLsizei)screenHeight);

    // set orthographic viewing frustum
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, screenWidth, 0, screenHeight, -1, 1);

    // switch to modelview matrix in order to set scene
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}



///////////////////////////////////////////////////////////////////////////////
// set the projection matrix as perspective
///////////////////////////////////////////////////////////////////////////////
void toPerspective()
{
    // set viewport to be the entire window
    glViewport(0, 0, (GLsizei)screenWidth, (GLsizei)screenHeight);

    // set perspective viewing frustum
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(400.0f, (float)(screenWidth)/screenHeight, 1.0f, 1000.0f); // FOV, AspectRatio, NearClip, FarClip

    // switch to modelview matrix in order to set scene
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

//=============================================================================
// CALLBACKS
//=============================================================================

void displayCB()
{
    // clear buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // save the initial ModelView matrix before modifying ModelView matrix
    glPushMatrix();

    // tramsform modelview matrix
    glTranslatef(0, 0, -cameraDistance);

    // set material
    float ambient[]  = {0.5f, 0.5f, 0.5f, 1};
    float diffuse[]  = {0.7f, 0.7f, 0.7f, 1};
    float specular[] = {1.0f, 1.0f, 1.0f, 1};
    float shininess  = 128;
    glMaterialfv(GL_FRONT, GL_AMBIENT,   ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    // line color
    float lineColor[] = {0.2f, 0.2f, 0.2f, 1};

    // draw right sphere with texture
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse); // reset diffuse
    glPushMatrix();
    glTranslatef(0, 0, 0);
    glRotatef(cameraAngleX, 1, 0, 0);
    glRotatef(cameraAngleY, 0, 1, 0);
    glBindTexture(GL_TEXTURE_2D, texId);
    // glDisable(GL_COLOR_MATERIAL);
    sphere2.draw();
    
    // glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    // glEnable(GL_COLOR_MATERIAL);
    
    draw_traceroutes ();
    glRotatef(0.1f, 1,0,0);
    glPointSize(3.0);

    glPopMatrix();


    showInfo();     // print max range of glDrawRangeElements
    
    glPopMatrix();

    glutSwapBuffers();
}


void reshapeCB(int w, int h)
{
    screenWidth = w;
    screenHeight = h;
    toPerspective();
    std::cout << "window resized: " << w << " x " << h << std::endl;
}


void timerCB(int millisec)
{
    glutTimerFunc(millisec, timerCB, millisec);
    glutPostRedisplay();
}


void keyboardCB(unsigned char key, int x, int y)
{
    switch(key)
    {
    case 27: // ESCAPE
        clearSharedMem();
        exit(0);
        break;

    case 'b': // switch rendering modes (fill -> wire -> point)
    case 'B':
        ++drawMode;
        drawMode %= 3;
        if(drawMode == 0)        // fill mode
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
        }
        else if(drawMode == 1)  // wireframe mode
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
        }
        else                    // point mode
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
        }
        break;

    case ' ':
        sphere2.reverseNormals();
        break;
    case 'w':
        magic_lat += 10;
        std::cout << "magic_lat: " << magic_lat << '\n';
        break;
    case 's':
        magic_lat -= 10;
        std::cout << "magic_lat: " << magic_lat << '\n';
        break;
    case 'd':
        magic_lon += 10;
        std::cout << "magic_lon: " << magic_lon << '\n';
        break;
    case 'a':
        magic_lon -= 10;
        std::cout << "magic_lon: " << magic_lon << '\n';
        break;
    default:
        ;
    }
}


void mouseCB(int button, int state, int x, int y)
{
    mouseX = x;
    mouseY = y;

    if(button == GLUT_LEFT_BUTTON)
    {
        if(state == GLUT_DOWN)
        {
            mouseLeftDown = true;
        }
        else if(state == GLUT_UP)
            mouseLeftDown = false;
    }

    else if(button == GLUT_RIGHT_BUTTON)
    {
        if(state == GLUT_DOWN)
        {
            mouseRightDown = true;
        }
        else if(state == GLUT_UP)
            mouseRightDown = false;
    }

    else if(button == GLUT_MIDDLE_BUTTON)
    {
        if(state == GLUT_DOWN)
        {
            mouseMiddleDown = true;
        }
        else if(state == GLUT_UP)
            mouseMiddleDown = false;
    }
}


void mouseMotionCB(int x, int y)
{
    if(mouseLeftDown)
    {
        cameraAngleY += (x - mouseX);
        cameraAngleX += (y - mouseY);
        mouseX = x;
        mouseY = y;
    }
    if(mouseRightDown)
    {
        cameraDistance -= (y - mouseY) * 0.2f;
        mouseY = y;
    }
}
std::vector<point> points;
int currentSegment = 0;      // Index of the current segment being drawn
float progress = 0.0f;       // Progress of the current segment (0.0 to 1.0)
const float segmentDuration = 1.0f; // Duration to draw each segment in seconds

auto lastTime = std::chrono::high_resolution_clock::now(); // Track time


void update_progress() {
    auto currentTime = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();

    // Update the progress for the current segment
    progress += deltaTime / segmentDuration;

    // Check if we need to move to the next segment
    if (progress >= 1.0f) {
        progress = 0.0f;
        currentSegment++;

        // Reset if all segments are drawn
        if (currentSegment >= points.size() - 1) {
            currentSegment = 0;  // Loop the animation
        }
    }

    lastTime = currentTime;
    glutPostRedisplay();
}

void draw_traceroutes(void)
{

    glBegin(GL_POINTS);
    glColor3f(1, 0, 0);
        
    std::unordered_map<std::string, point> map;
    int city_count = 0;
    for (auto i = 0; i < hosts.size(); ++i){
        point p = to_point(hosts[i].host_coordinates.first + magic_lat, hosts[i].host_coordinates.second + magic_lon);
        if(hosts[i].host_coordinates.first == 0 && hosts[i].host_coordinates.second == 0){
            continue;
        } 

        if (points.size() >= 1) {
            point last_point = points.back();
            if constexpr (approximatePoints) {
                /* If current point is close enough to its former point, skip -- approximate */
                if (last_point.distance(p) < minDistance) {
                    continue;
                }
            }
        }
        points.push_back(p);
        std::cout << "city_count :" << i << "| " << "name : " << hosts[i].city << "\n";
        map[hosts[i].city] = p;
    }
    
    { /* TEST Points */
        // points.push_back(to_point(40.1772 + magic_lat, 44.526 + magic_lon));    // Armenia
        // points.push_back(to_point(40.3772 + magic_lat, 44.226 + magic_lon));    // Armenia
        // points.push_back(to_point(60.1699 + magic_lat, 24.9384 + magic_lon));   // Finland
        // points.push_back(to_point(35.694 + magic_lat, 139.754 + magic_lon));    // Japan
        // points.push_back(to_point(52.2297 + magic_lat, 21.0122 + magic_lon));   // Poland
        // points.push_back(to_point(52.3759 + magic_lat, 4.8975 + magic_lon));    // The Netherlands
        // points.push_back(to_point(52.3676 + magic_lat, 4.90414 + magic_lon));   // The Netherlands (alternative coordinate)
        // points.push_back(to_point(40.7128 + magic_lat, -74.006 + magic_lon));   // United States (New York)
    }

    for (auto i = 0; i < points.size(); i++) {
        if (i == 0) {
            glPointSize(10.0f);
            glColor3f(0.0, 1.0, 0.0);
        } else {
            glColor3f(0.0, 0.0, 1.0);
        }
        points[i].draw_point();
    } 


    glEnd();

    pinCities(map);

    for (int i = 0; i < currentSegment; ++i) {
        point p1 = points[i];
        point p2 = get_middlepoint(points[i], points[i + 1]);
        point p3 = points[i + 1];

        glBegin(GL_LINE_STRIP);
        glColor3f(1.0, 1.0, 1.0);
        draw_quadratic_curve(p1, p2, p3, 1.0f); // Draw full curve for completed segments
        glEnd();
    }

    if (currentSegment < points.size() - 1) {
        point p1 = points[currentSegment];
        point p2 = get_middlepoint(points[currentSegment], points[currentSegment + 1]);
        point p3 = points[currentSegment + 1];

        glBegin(GL_LINE_STRIP);
        glColor3f(1.0, 1.0, 1.0);
        draw_curve(draw_quadratic_curve, p1, p2, p3, progress); // Animate only the current segment
        glEnd();
    }
}

void draw_string_stack(char* strlst[], int strcnt)
{
    glPushMatrix();                     // save current modelview matrix
    glLoadIdentity();                   // reset modelview matrix

    // set to 2D orthogonal projection
    glMatrixMode(GL_PROJECTION);        // switch to projection matrix
    glPushMatrix();                     // save current projection matrix
    glLoadIdentity();                   // reset projection matrix
    //gluOrtho2D(0, screenWidth, 0, screenHeight); // set to orthogonal projection
    glOrtho(0, screenWidth, 0, screenHeight, -1, 1); // set to orthogonal projection

    float color[4] = {1, 1, 1, 1};
    for (int i = 0; i < strcnt; i++) {
        drawString(strlst[i], 1, screenHeight-((i+6)*TEXT_HEIGHT), color, font);
    }

    // restore projection matrix
    glPopMatrix();                   // restore to previous projection matrix

    // restore modelview matrix
    glMatrixMode(GL_MODELVIEW);      // switch to modelview matrix
    glPopMatrix();                   // restore to previous modelview matrix
}

point to_point(GLfloat latitude, GLfloat longitude, GLfloat altitude, GLfloat radius)
{
    
    GLfloat latRad = latitude * M_PI / 180.0;
    GLfloat lonRad = longitude * M_PI / 180.0;
    
    GLfloat y = cos(latRad) * cos(lonRad);
    GLfloat x = sin(latRad);
    GLfloat z = cos(latRad) * sin(lonRad);
    


    GLfloat theta = M_PI / 2; /* Need to rotate 90 degrees */
    GLfloat x_rotated = x * cos(theta) - y * sin(theta);
    GLfloat y_rotated = x * sin(theta) + y * cos(theta);

    point p(x_rotated, y_rotated, z);
    p.setcord(latitude, longitude, altitude);
    return p;
}

point get_middlepoint (const point& p1, const point& p2)
{
    const point M ((p2.getx()+p1.getx())/2, (p2.gety()+p1.gety())/2, (p2.getz()+ p1.getz())/2);
    
    /* Distance between origin and M(middlepoint) */
    GLfloat OM_dist = sqrt(pow(M.getx(),2) + pow(M.gety(),2) + pow(M.getz(),2)) + 0.0001;
    /* The less the distance OM, means that points are further on the surface, thus magnitude needs to be bigger */
    const GLfloat MAGIC_MAGNITUDE =  (2+(0.2/OM_dist)) - (OM_dist);
    return point((M.getx()/OM_dist*MAGIC_MAGNITUDE), (M.gety()/OM_dist*MAGIC_MAGNITUDE), (M.getz()/OM_dist*MAGIC_MAGNITUDE));
}

void pinCities(const std::unordered_map<std::string, point>& map)
{
    float color[4] = {1, 1, 1, 1};
    for (const auto &i : map) {
        float pos[] = {i.second.getx(), i.second.gety(), i.second.getz() + 0.04};
        drawString3D(i.first.c_str(), pos, color, font);
    }
}