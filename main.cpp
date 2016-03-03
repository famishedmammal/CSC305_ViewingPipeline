#include "Canvas.h"
#include <math.h>
#include "Eigen/Dense"
#include <vector>
using namespace Eigen;

unsigned int width = 512;
unsigned int height = 512;
float mouse_x = 0;
float mouse_y = 0;
bool leftButtonPressed = false;
bool rightButtonPressed = false;
float last_x = 0;
float last_y = 0;
int timercount = 0;
Canvas canvas;

float DISTANCE = 5.0f;
float near = 1.0f;
float far = 10.0f;
std::vector<Vector4d> pointList;
Matrix4d perspectiveMatrix;
Matrix4d M0;
Vector3d eyePosition_perm(1, 0, 0);

void MouseMove(double x, double y)
{

   //the pointer has moved
   mouse_x = (float)(x) / 256 - 1;
   mouse_y = 1 - (float)(y) / 256;
}

void DrawCube()
{

    for(int i=0; i<pointList.size(); i+=2) {

        // pre, Calculate MV
        Vector3d eyePosition = eyePosition_perm*DISTANCE;
        Vector3d gazeDirection(-eyePosition[0], -eyePosition[1], -eyePosition[2]);
        Vector3d upVector(0, 1, 0);
        Vector3d w = -(gazeDirection/gazeDirection.norm());
        Vector3d u = ((upVector.cross(w))/(upVector.cross(w)).norm());
        Vector3d v = w.cross(u);

        // Calculate MV
        Matrix4d MVrot;
        MVrot <<
                 u[0], u[1], u[2], 0,
                 v[0], v[1], v[2], 0,
                 w[0], w[1], w[2], 0,
                 0, 0, 0, 1; //extra dimension?
        Matrix4d temp;
        temp <<
                 1, 0, 0, -eyePosition[0],
                 0, 1, 0, -eyePosition[1],
                 0, 0, 1, -eyePosition[2],
                 0, 0, 0, 1;
        Matrix4d MV = MVrot*temp;

        // Calculate the rest.
        Matrix4d M = M0*perspectiveMatrix*MV;
        Vector4d p = M*pointList[i];
        Vector4d q = M*pointList[i+1];
        float hp = p[2] / near; //change this to p instead of pointlist?
        float hq = q[2] / near;
        canvas.AddLine(p[0]/hp, p[1]/hp, q[0]/hq, q[1]/hq);
    }
}

void OnPaint()
{
    canvas.Clear();
    DrawCube();
}

void MouseButton(MouseButtons mouseButton, bool press)
{
    if (mouseButton == LeftButton)
    {
        if (press == true) { leftButtonPressed = true; last_x = mouse_x; last_y = mouse_y;}
        else leftButtonPressed = false;
    }
    if (mouseButton == RightButton)
    {
        if (press == true) { rightButtonPressed = true; last_x = mouse_x; last_y = mouse_y;}
        else rightButtonPressed = false;
    }
}

void OnTimer()
{
    timercount ++;

    if (leftButtonPressed) {

        float dx = -(mouse_x-last_x);
        float dy = -(mouse_y-last_y);

        Matrix3d mA;
        mA = AngleAxisd(dy, (eyePosition_perm.cross(Vector3d(0,1,0)).normalized()))
          * AngleAxisd(dx,  Vector3d(0, 1, 0))
          * AngleAxisd(0, Vector3d(0, 0, 1));

        eyePosition_perm = mA * eyePosition_perm;
        last_x = mouse_x;
        last_y = mouse_y;

    }
    else if (rightButtonPressed) {
        float d = mouse_y-last_y;
        DISTANCE += d;
        last_y = mouse_y;
    }

}

int main(int, char **){

    Vector4d pA(1, 1, 1, 1);
    Vector4d pB(1, -1, 1, 1);
    Vector4d pC(1, -1, -1, 1);
    Vector4d pD(1, 1, -1, 1);
    Vector4d pE(-1, 1, 1, 1);
    Vector4d pF(-1, -1, 1, 1);
    Vector4d pG(-1, -1, -1, 1);
    Vector4d pH(-1, 1, -1, 1);
    pointList.push_back(pA); pointList.push_back(pB);
    pointList.push_back(pB); pointList.push_back(pC);
    pointList.push_back(pC); pointList.push_back(pD);
    pointList.push_back(pD); pointList.push_back(pA);
    pointList.push_back(pE); pointList.push_back(pF);
    pointList.push_back(pF); pointList.push_back(pG);
    pointList.push_back(pG); pointList.push_back(pH);
    pointList.push_back(pH); pointList.push_back(pE);
    pointList.push_back(pA); pointList.push_back(pE);
    pointList.push_back(pB); pointList.push_back(pF);
    pointList.push_back(pC); pointList.push_back(pG);
    pointList.push_back(pD); pointList.push_back(pH);

    perspectiveMatrix <<
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, (near+far)/near, -far,
        0, 0, 1/near, 0; //change last to 1?
    float r = 1.0f;
    float t = 1.0f;
    float b = -1.0f;
    float l = -1.0f;
    M0 <<
        2/(r-l), 0, 0, -((r+l)/(r-l)),
        0, 2/(t-b), 0, -((t+b)/(t-b)),
        0, 0, 2/(near-far), -((near+far)/(near-far)),
        0, 0, 0, 1;

    //Link the call backs
    canvas.SetMouseMove(MouseMove);
    canvas.SetMouseButton(MouseButton);
    canvas.SetOnPaint(OnPaint);
    //canvas.SetKeyPress(KeyPress);
    canvas.SetTimer(0.01, OnTimer);//trigger OnTimer every 0.1 second
    canvas.Show(width, height, "Canvas Demo");
    return 0;
}


/*
void KeyPress(char keychar)
{
    //A key is pressed! print a message
    fprintf(stderr, "The \"%c\" key is pressed!\n", keychar);
}
*/

