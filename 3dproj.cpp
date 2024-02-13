#include "raylib.h"
#include "raymath.h"
#include <cmath>

Vector3 mtrxMult(Vector3 (&mtrx)[3], Vector3 point){
    return (Vector3){
            mtrx[0].x * point.x + mtrx[0].y * point.y + mtrx[0].z * point.z,
            mtrx[1].x * point.x + mtrx[1].y * point.y + mtrx[1].z * point.z,
            mtrx[2].x * point.x + mtrx[2].y * point.y + mtrx[2].z * point.z
    };
}

int main(){
    
    ///// INIT /////
    const int kWidth  = 800;
    const int kHeight = 600;
    InitWindow(kWidth, kHeight, "3D Projection");
    
    const int kScale = 250;
    const int kPoints = 8;
    const int kDistance = 2;
    const bool kPerspective = true;
    const Vector3 kAngleIncrement = {1.f, 1.f, 1.f};
    const Vector2 center = { kWidth / 2.f, kHeight / 2.f };
    
    Vector2 drawablePoints[kPoints];
    Vector3 points[kPoints] = {
        {-0.5f, -0.5f, -0.5f},
        { 0.5f, -0.5f, -0.5f},
        { 0.5f,  0.5f, -0.5f},
        {-0.5f,  0.5f, -0.5f},
        {-0.5f, -0.5f,  0.5f},
        { 0.5f, -0.5f,  0.5f},
        { 0.5f,  0.5f,  0.5f},
        {-0.5f,  0.5f,  0.5f}
    };
    Vector3 projectedPoints[kPoints];
    Vector3 rotatedPoints[kPoints];
    Vector3 projectionMatrix[3] = {
        {1.f, 0.f, 0.f},
        {0.f, 1.f, 0.f},
        {0.f, 0.f, 0.f}
    };
    Vector3 angle = Vector3Zero();
    Vector3 radAngle = Vector3Zero();
    Vector3 rotationZ[3] = {
        {cosf(radAngle.z), -sinf(radAngle.z), 0.f},
        {sinf(radAngle.z),  cosf(radAngle.z), 0.f},
        {             0.f,               0.f, 1.f}
    };
    Vector3 rotationX[3] = {
        {1.f,              0.f,               0.f},
        {0.f, cosf(radAngle.x), -sinf(radAngle.x)},
        {0.f, sinf(radAngle.x),  cosf(radAngle.x)}
    };
    Vector3 rotationY[3] = {
        { cosf(radAngle.y), 0.f, sinf(radAngle.y)},
        { 0.f,              1.f,              0.f},
        {-sinf(radAngle.y), 0.f, cosf(radAngle.y)}
    };
    
    ///// LOOP /////
    SetTargetFPS(60);
    while(!WindowShouldClose())
    {
        ///// UPDATE /////
        angle.x = (int)(angle.x + kAngleIncrement.x) % 360;
        angle.y = (int)(angle.y + kAngleIncrement.y) % 360;
        angle.z = (int)(angle.z + kAngleIncrement.z) % 360;
        radAngle = Vector3Scale(angle, DEG2RAD);
        rotationZ[0] = {cosf(radAngle.z), -sinf(radAngle.z), 0.f};
        rotationZ[1] = {sinf(radAngle.z),  cosf(radAngle.z), 0.f};
        rotationX[1] = {0.f, cosf(radAngle.x), -sinf(radAngle.x)};
        rotationX[2] = {0.f, sinf(radAngle.x),  cosf(radAngle.x)};
        rotationY[0] = {cosf(radAngle.y) , 0.f, sinf(radAngle.y)};
        rotationY[2] = {-sinf(radAngle.y), 0.f, cosf(radAngle.y)};
        
        for(int i = 0; i < kPoints; i++){
            rotatedPoints[i] = mtrxMult(rotationY, points[i]);
            rotatedPoints[i] = mtrxMult(rotationX, rotatedPoints[i]);
            rotatedPoints[i] = mtrxMult(rotationZ, rotatedPoints[i]);
            if(kPerspective){
                projectionMatrix[0].x = 1 / (kDistance - rotatedPoints[i].z);
                projectionMatrix[1].y = 1 / (kDistance - rotatedPoints[i].z);
            }
            projectedPoints[i] = mtrxMult(projectionMatrix, rotatedPoints[i]);
            drawablePoints[i] = {
                projectedPoints[i].x * kScale + center.x,
                projectedPoints[i].y * kScale + center.y
            };
        }
            
        
        ///// DRAW /////
        BeginDrawing();
        
            ClearBackground(BLACK);
            // DrawFPS(10, 10);
            
            // Vertices
            for(const auto &point : drawablePoints)
                DrawCircle(point.x, point.y, 10, GREEN);
            // Edges
            for(int i = 0; i < kPoints / 2; i++){
                DrawLineEx(drawablePoints[i]    , drawablePoints[(i + 1) % (kPoints / 2)]    , 2.f, RED);
                DrawLineEx(drawablePoints[i + 4], drawablePoints[(i + 1) % (kPoints / 2) + 4], 2.f, RED);
                DrawLineEx(drawablePoints[i]    , drawablePoints[i + 4]                      , 2.f, RED);
            }
            // Faces
            Vector2 facePoints[4] = {drawablePoints[0], drawablePoints[1], drawablePoints[2], drawablePoints[3]};
            DrawTriangleStrip(facePoints, 4, WHITE);
            
        EndDrawing();
    }
    
    ///// SHUT /////
    CloseWindow();
    return 0;
}