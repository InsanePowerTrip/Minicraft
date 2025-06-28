#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_EASY_FONT_IMPLEMENTATION
#include "stb_easy_font.h"

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <math.h>

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

// globals
int pointerLocked = 1;
int windowWidth  = 800;
int windowHeight = 600;

void togglePointerLock(GLFWwindow* window) {
    pointerLocked = !pointerLocked;
    if (pointerLocked) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        // Move cursor to center of window when unlocked
        int w, h;
        glfwGetWindowSize(window, &w, &h);
        glfwSetCursorPos(window, w / 2.0, h / 2.0);
    }
}

// enable on click if off
void mouse_button_callback(GLFWwindow* w, int b, int a, int m) {
    if (b==GLFW_MOUSE_BUTTON_LEFT && a==GLFW_PRESS && !pointerLocked)
        togglePointerLock(w);
}
// ESC handler
void key_callback(GLFWwindow* w, int k, int s, int a, int m) {
    if (k==GLFW_KEY_ESCAPE && a==GLFW_PRESS)
        togglePointerLock(w);
}
// resize handler
void framebuffer_size_callback(GLFWwindow* w, int width, int height) {
    windowWidth  = width;
    windowHeight = height;
    glViewport(0,0,width,height);
}

// load a simple RGB texture
GLuint loadTexture(const char* fn) {
    int w,h,ch;
    unsigned char* data = stbi_load(fn,&w,&h,&ch,3);
    if(!data){ fprintf(stderr,"Failed: %s\n",fn); return 0; }
    GLuint id; glGenTextures(1,&id);
    glBindTexture(GL_TEXTURE_2D,id);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,w,h,0,GL_RGB,GL_UNSIGNED_BYTE,data);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    stbi_image_free(data);
    return id;
}

// draw a unit cube centered at origin
void drawCube(){
    glBegin(GL_QUADS);
    // front
    glTexCoord2f(0,0);glVertex3f(-1,-1,1);
    glTexCoord2f(1,0);glVertex3f( 1,-1,1);
    glTexCoord2f(1,1);glVertex3f( 1, 1,1);
    glTexCoord2f(0,1);glVertex3f(-1, 1,1);
    // back
    glTexCoord2f(1,0);glVertex3f(-1,-1,-1);
    glTexCoord2f(1,1);glVertex3f(-1, 1,-1);
    glTexCoord2f(0,1);glVertex3f( 1, 1,-1);
    glTexCoord2f(0,0);glVertex3f( 1,-1,-1);
    // top
    glTexCoord2f(0,1);glVertex3f(-1,1,-1);
    glTexCoord2f(0,0);glVertex3f(-1,1, 1);
    glTexCoord2f(1,0);glVertex3f( 1,1, 1);
    glTexCoord2f(1,1);glVertex3f( 1,1,-1);
    // bottom
    glTexCoord2f(1,1);glVertex3f(-1,-1,-1);
    glTexCoord2f(0,1);glVertex3f( 1,-1,-1);
    glTexCoord2f(0,0);glVertex3f( 1,-1, 1);
    glTexCoord2f(1,0);glVertex3f(-1,-1, 1);
    // right
    glTexCoord2f(1,0);glVertex3f(1,-1,-1);
    glTexCoord2f(1,1);glVertex3f(1, 1,-1);
    glTexCoord2f(0,1);glVertex3f(1, 1, 1);
    glTexCoord2f(0,0);glVertex3f(1,-1, 1);
    // left
    glTexCoord2f(0,0);glVertex3f(-1,-1,-1);
    glTexCoord2f(1,0);glVertex3f(-1,-1, 1);
    glTexCoord2f(1,1);glVertex3f(-1, 1, 1);
    glTexCoord2f(0,1);glVertex3f(-1, 1,-1);
    glEnd();
}

void drawFPS(double fps) {
    char buf[64];
    sprintf(buf, "FPS: %.1f", fps);
    // Generate quads at (10,20) with origin assumed top-left by our flipped ortho
    char verts[9999];
    int quads = stb_easy_font_print(
        10, 20,       // 10px right, 20px down from top-left
        buf, NULL, verts, sizeof(verts)
    );

    // Set up an ortho where Y goes downwards from the top
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
      glLoadIdentity();
      gluOrtho2D(0, windowWidth, windowHeight, 0);  // note: top=0, bottom=windowHeight
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
      glLoadIdentity();

      glDisable(GL_TEXTURE_2D);
      glColor3f(1,1,1);
      glEnableClientState(GL_VERTEX_ARRAY);
      glVertexPointer(2, GL_FLOAT, 16, verts);
      glDrawArrays(GL_QUADS, 0, quads * 4);
      glDisableClientState(GL_VERTEX_ARRAY);
      glEnable(GL_TEXTURE_2D);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}
int main(){
    if(!glfwInit())return -1;
    GLFWwindow* w=glfwCreateWindow(windowWidth,windowHeight,
                                   "Clean Fix",NULL,NULL);
    if(!w){ glfwTerminate(); return -1; }
    glfwMakeContextCurrent(w);
    glfwSetKeyCallback(w,key_callback);
    glfwSetMouseButtonCallback(w,mouse_button_callback);
    glfwSetFramebufferSizeCallback(w,framebuffer_size_callback);
    // start locked
    pointerLocked=1;
    glfwSetInputMode(w,GLFW_CURSOR,GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    GLuint tex=loadTexture("block\\stone.png");
    if(!tex){ glfwTerminate(); return -1;}

    // camera
    float cx=0,cy=0,cz=5, tx=0,ty=0,tz=0;
    const float SPEED = 5.0f;      // units per second (tweak this)
    double last=glfwGetTime();
    double fpsTimer=last;
    int frames=0;
    double fps=0;

    while(!glfwWindowShouldClose(w)){
        double now=glfwGetTime();
        double dt= now - last;
        if(dt>0.1) dt=0.1;      // clamp huge dt
        last=now;

        // recalc FPS every second
        frames++;
        if(now - fpsTimer >= 1.0){
            fps = frames / (now - fpsTimer);
            frames=0;
            fpsTimer=now;
        }

        if(pointerLocked){
            // forward vector
            float fx=tx-cx, fy=ty-cy, fz=tz-cz;
            float fl=sqrtf(fx*fx+fy*fy+fz*fz);
            fx/=fl; fy/=fl; fz/=fl;
            // right = forward × up
            float ux=0,uy=1,uz=0;
            float rx=fz*uy-fy*uz,
                  ry=fx*uz-fz*ux,
                  rz=fy*ux-fx*uy;
            float rl=sqrtf(rx*rx+ry*ry+rz*rz);
            rx/=rl; ry/=rl; rz/=rl;
            // WASD
            if(glfwGetKey(w,GLFW_KEY_W)==GLFW_PRESS){
                cx+=fx*SPEED*dt; cy+=fy*SPEED*dt; cz+=fz*SPEED*dt;
                tx+=fx*SPEED*dt; ty+=fy*SPEED*dt; tz+=fz*SPEED*dt;
            }
            if(glfwGetKey(w,GLFW_KEY_S)==GLFW_PRESS){
                cx-=fx*SPEED*dt; cy-=fy*SPEED*dt; cz-=fz*SPEED*dt;
                tx-=fx*SPEED*dt; ty-=fy*SPEED*dt; tz-=fz*SPEED*dt;
            }
            if(glfwGetKey(w,GLFW_KEY_A)==GLFW_PRESS){
                cx+=rx*SPEED*dt; cy+=ry*SPEED*dt; cz+=rz*SPEED*dt;
                tx+=rx*SPEED*dt; ty+=ry*SPEED*dt; tz+=rz*SPEED*dt;
            }
            if(glfwGetKey(w,GLFW_KEY_D)==GLFW_PRESS){
                cx-=rx*SPEED*dt; cy-=ry*SPEED*dt; cz-=rz*SPEED*dt;
                tx-=rx*SPEED*dt; ty-=ry*SPEED*dt; tz-=rz*SPEED*dt;
            }
            // up/down
            if(glfwGetKey(w,GLFW_KEY_SPACE)==GLFW_PRESS){
                cy+=SPEED*dt; ty+=SPEED*dt;
            }
            if(glfwGetKey(w,GLFW_KEY_LEFT_SHIFT)==GLFW_PRESS){
                cy-=SPEED*dt; ty-=SPEED*dt;
            }
        }

        // render
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, (double)windowWidth/windowHeight, 0.1, 100.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(cx,cy,cz, tx,ty,tz, 0,1,0);

        glBindTexture(GL_TEXTURE_2D, tex);
        drawCube();

        drawFPS(fps);

        glfwSwapBuffers(w);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
