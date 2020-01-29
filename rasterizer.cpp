#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <limits>

#include "geometry.h" //honteusement volé à M. Sokolov
#include "model.cpp" //honteusement volé à M. Sokolov

#define WIDTH 800
#define HEIGHT 800

#define ACCESS_C ((x + (y * WIDTH)) * 3) + c
#define ACCESS(i) ((pixel.x + (pixel.y * WIDTH)) * 3) + i

#define BACKGROUND_R 75
#define BACKGROUND_G 75
#define BACKGROUND_B 75

#define MODEL_R 200
#define MODEL_G 0
#define MODEL_B 0

#define FLT_MAX numeric_limits<float>::max()
#define FLT_MIN numeric_limits<float>::min()

using namespace std;

class Mat4f {
public:
    vector< vector<float> > matrix;
    
    Mat4f () {
        matrix.resize(4, vector<float>(4));
        matrix[0][0] = 1.f;
        matrix[1][1] = 1.f;
        matrix[2][2] = 1.f;
        matrix[3][3] = 1.f;
    }
    
    Mat4f (Vec4f X, Vec4f Y, Vec4f Z, Vec4f W) {
        matrix.resize(4, vector<float>(4));
        matrix[0][0] = X.x;
        matrix[0][1] = X.y;
        matrix[0][2] = X.z;
        matrix[0][3] = X.w;
        
        matrix[1][0] = Y.x;
        matrix[1][1] = Y.y;
        matrix[1][2] = Y.z;
        matrix[1][3] = Y.w;
        
        matrix[2][0] = Z.x;
        matrix[2][1] = Z.y;
        matrix[2][2] = Z.z;
        matrix[2][3] = Z.w;
        
        matrix[3][0] = W.x;
        matrix[3][1] = W.y;
        matrix[3][2] = W.z;
        matrix[3][3] = W.w;
    }
    
    const vector<float>& operator[] (int i) const {
        return matrix[i];
    }
    
    vector<float>& operator[] (int i) {
        return matrix[i];
    }
};

Vec4f operator* (const Mat4f& m, const Vec3f& v) {
    return Vec4f(
                m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0] * 1,
                m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1] * 1,
                m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2] * 1,
                m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3] * 1
                );
}

Vec4f operator* (const Vec3f& v, const Mat4f& m) {
    return Vec4f(
                v.x * m[0][0] + v.y * m[0][1] + v.z * m[0][2] + 1 * m[0][3],
                v.x * m[1][0] + v.y * m[1][1] + v.z * m[1][2] + 1 * m[1][3],
                v.x * m[2][0] + v.y * m[2][1] + v.z * m[2][2] + 1 * m[2][3],
                v.x * m[3][0] + v.y * m[3][1] + v.z * m[3][2] + 1 * m[3][3]
                );
}

Mat4f operator*( const Mat4f& m1, const Mat4f& m2 )
{
    Vec4f X(
           m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2] + m1[3][0] * m2[0][3],
           m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2] + m1[3][1] * m2[0][3],
           m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1] + m1[2][2] * m2[0][2] + m1[3][2] * m2[0][3],
           m1[0][3] * m2[0][0] + m1[1][3] * m2[0][1] + m1[2][3] * m2[0][2] + m1[3][3] * m2[0][3]
           );
    Vec4f Y(
           m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2] + m1[3][0] * m2[1][3],
           m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2] + m1[3][1] * m2[1][3],
           m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1] + m1[2][2] * m2[1][2] + m1[3][2] * m2[1][3],
           m1[0][3] * m2[1][0] + m1[1][3] * m2[1][1] + m1[2][3] * m2[1][2] + m1[3][3] * m2[1][3]
           );
    Vec4f Z(
           m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1] + m1[2][0] * m2[2][2] + m1[3][0] * m2[2][3],
           m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1] + m1[2][1] * m2[2][2] + m1[3][1] * m2[2][3],
           m1[0][2] * m2[2][0] + m1[1][2] * m2[2][1] + m1[2][2] * m2[2][2] + m1[3][2] * m2[2][3],
           m1[0][3] * m2[2][0] + m1[1][3] * m2[2][1] + m1[2][3] * m2[2][2] + m1[3][3] * m2[2][3]
           );
    Vec4f W(
           m1[0][0] * m2[3][0] + m1[1][0] * m2[3][1] + m1[2][0] * m2[3][2] + m1[3][0] * m2[3][3],
           m1[0][1] * m2[3][0] + m1[1][1] * m2[3][1] + m1[2][1] * m2[3][2] + m1[3][1] * m2[3][3],
           m1[0][2] * m2[3][0] + m1[1][2] * m2[3][1] + m1[2][2] * m2[3][2] + m1[3][2] * m2[3][3],
           m1[0][3] * m2[3][0] + m1[1][3] * m2[3][1] + m1[2][3] * m2[3][2] + m1[3][3] * m2[3][3]
           );
    
    return Mat4f( X, Y, Z, W );
}


void writeImage (std::vector<int>frameBuffer) {
    ofstream file;
    file.open("out.ppm");
    file << "P3" << endl << WIDTH << " " << HEIGHT << endl << 255 << endl;
    int x, y, c;
    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            for (c = 0; c < 3; c++) {
                file << frameBuffer[ACCESS_C] << " ";
            }
        }
        file << endl;
    }
    file.close();
}

void project (Vec3f point, Vec3f *pScreen, Mat4f projectionMatrix) {
    
    float aspectRatio = WIDTH / HEIGHT;
    
    pScreen->x = (point.x) / (aspectRatio * -point.z * tan((M_PI/2) / 2.f));
    pScreen->y = ((point.y) / (point.z * tan((M_PI/2) / 2.f))) + (0.2f);
    
    pScreen->x = pScreen->x * WIDTH;
    pScreen->y = pScreen->y * HEIGHT;
}

int orient2d(const Vec2f& a, const Vec2f& b, const Vec2f& c) {
    return (b.x-a.x)*(c.y-a.y) - (b.y-a.y)*(c.x-a.x);
}

Vec3f barycenter (Vec3f a, Vec3f b, Vec3f c, Vec3f pixel) {
	Vec3f bc = Vec3f();
	bc.x = (c.x-b.x)*(pixel.y-b.y) - (c.y-b.y)*(pixel.x-b.x);
	bc.y = (a.x-c.x)*(pixel.y-c.y) - (a.y-c.y)*(pixel.x-c.x);
	bc.z = (b.x-a.x)*(pixel.y-a.y) - (b.y-a.y)*(pixel.x-a.x);
	return bc;
	//1 2 - 2 0 - 0 1
}

void render (Model duck, Mat4f projectionMatrix, Vec3f light) {
    vector<int>frameBuffer(WIDTH * HEIGHT * 3);
    vector<int>zBuffer(WIDTH * HEIGHT);

    int i;
    for (i = 0; i < WIDTH * HEIGHT * 3; i++) {
        frameBuffer[i + 0] = BACKGROUND_R;
        frameBuffer[i + 1] = BACKGROUND_G;
        frameBuffer[i + 2] = BACKGROUND_B;
    }

    fill(zBuffer.begin(), zBuffer.end(), INT_MIN);
    
    for (i = 0; i < duck.nfaces(); i++) {
        std::vector<Vec3f>points(3);
        
        Vec3f p1 = duck.point(duck.vert(i, 0)), p2 = duck.point(duck.vert(i, 1)), p3 = duck.point(duck.vert(i, 2));
        
        project(p1, &points[0], projectionMatrix);
        project(p2, &points[1], projectionMatrix);
        project(p3, &points[2], projectionMatrix);
        
        Vec3f V = p2 - p1;
        Vec3f W = p3 - p1;
        Vec3f surfaceNormal = cross(V, W).normalize();
        
        Vec3f centroid = (p1 + p2 + p3) * (1.f / 3.f);
        
        Vec3f lightDirection = (light - centroid).normalize();
        float lightDistance = (light - centroid).norm();

        float lightIntensity = 1.3f * max(-(lightDirection * surfaceNormal), 0.f);
        
        lightIntensity = max(0.f, min(lightIntensity, 1.f)); //clamping

        Vec2f bboxMin(1000., 1000.);
        Vec2f bboxMax(-1000., -1000.);
        
        int j;
        for (j = 0; j < 3; j++) {
            if (points[j].x < bboxMin.x) bboxMin.x = points[j].x;
            if (points[j].y < bboxMin.y) bboxMin.y = points[j].y;
            
            if (points[j].x > bboxMax.x) bboxMax.x = points[j].x;
            if (points[j].y > bboxMax.y) bboxMax.y = points[j].y;
        }
        
        bboxMin.x = max(0.f, min(bboxMin.x, (float)WIDTH));
        bboxMin.y = max(0.f, min(bboxMin.y, (float)HEIGHT));
        bboxMax.x = max(0.f, min(bboxMax.x, (float)WIDTH));
        bboxMax.y = max(0.f, min(bboxMax.y, (float)HEIGHT));
                
        int c, x, y;
        Vec3f pixel;
        for (pixel.x = (int)bboxMin.x; pixel.x < bboxMax.x; pixel.x++) {
            for (pixel.y = (int)bboxMin.y; pixel.y < bboxMax.y; pixel.y++) {
                Vec3f barycentr = barycenter(points[0], points[1], points[2], pixel);

                if (barycentr.x >= 0 && barycentr.y >= 0 && barycentr.z >= 0) {
                	pixel.z = 0;
                	for (j = 0; j < 3; j++)
                		pixel.z += points[j].z * barycentr[j];

                	if (zBuffer[(int) pixel.x + pixel.y * WIDTH] < (int) pixel.z) {
                		zBuffer[(int) pixel.x + pixel.y * WIDTH] = pixel.z;
	                    frameBuffer[ACCESS(0)] = MODEL_R * lightIntensity;
	                    frameBuffer[ACCESS(1)] = MODEL_G * lightIntensity;
	                    frameBuffer[ACCESS(2)] = MODEL_B * lightIntensity;
                	}
                }
            
            }
        }
    }
    
    writeImage(frameBuffer);
}

int main () {
    Model duck("duck.obj");
    
    Vec3f light = Vec3f(0.f, 0.f, 0.f);
    
    float zFar = 1.f;
    float zNear = -1.f;
    
    Mat4f projectionMatrix = Mat4f();

    projectionMatrix[0][0] = 2 / (WIDTH - 0.f);

    projectionMatrix[1][1] = 2 / (0.f - HEIGHT);
    projectionMatrix[2][2] = -2 / (zFar - zNear);
    projectionMatrix[3][3] = 1.f;
    projectionMatrix[3][0] = (WIDTH + 0.f) / (WIDTH - 0.f);
    projectionMatrix[3][0] = (0.f + HEIGHT) / (0.f - HEIGHT);
    projectionMatrix[3][0] = (zFar + zNear) / (zFar - zNear);
    
    render(duck, projectionMatrix, light);

    return 0;
}
