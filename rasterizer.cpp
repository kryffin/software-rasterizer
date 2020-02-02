#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <limits>

#include "geometry.h" //honteusement volé à M. Sokolov
#include "matrix.cpp"
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

/*Vec4f project (Vec3f point, Mat4f P, Mat4f V, Mat4f M) {
    
    Mat4f MVP = P * V * M;
    Vec4f pScreen = MVP * point;
    Vec4f projected = point * V;
    projected = projected * P;
    //projected = projected * M;

    projected.x *= WIDTH;
    projected.y *= HEIGHT;


    pScreen.x *= WIDTH;
    pScreen.y *= HEIGHT;

	cout << "Point : " << point.x << " " << point.y << " " << point.z << endl;
    cout << "Projected : " << projected.x << " " << projected.y << " " << projected.z << endl;
    cout << "Pixel : " << pScreen.x << " " << pScreen.y << " " << pScreen.z << endl << endl;

    return projected;
}*/

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

void render (Model model, Mat4f P, Mat4f V, Mat4f M, Vec3f light) {
    vector<int>frameBuffer(WIDTH * HEIGHT * 3);
    vector<int>zBuffer(WIDTH * HEIGHT);

    int i;
    for (i = 0; i < WIDTH * HEIGHT * 3; i++) {
        frameBuffer[i + 0] = BACKGROUND_R;
        frameBuffer[i + 1] = BACKGROUND_G;
        frameBuffer[i + 2] = BACKGROUND_B;
    }

    fill(zBuffer.begin(), zBuffer.end(), INT_MIN);
    
    for (i = 0; i < model.nfaces(); i++) {
    	//cout <<i <<endl;

        std::vector<Vec3f>points(3);
        
        Vec3f 	p1 = model.point(model.vert(i, 0)),
        		p2 = model.point(model.vert(i, 1)),
        		p3 = model.point(model.vert(i, 2));

		/*p1 = p1 * M;
		p2 = p2 * M;
		p3 = p3 * M;*/

		/*p1 = p1 * V;
		p2 = p2 * V;
		p3 = p3 * V;*/


		points[0] = p1;
		points[1] = p2;
		points[2] = p3;

		cout << "Triangle :" << endl << points[0].x << " " << points[0].y << " " << points[0].z << endl;
		cout << points[1].x << " " << points[1].y << " " << points[1].z << endl;
		cout << points[2].x << " " << points[2].y << " " << points[2].z << endl << endl;

		// perspective
        points[0] = points[0] * P;
        points[1] = points[1] * P;
        points[2] = points[2] * P;

        cout << "Perspective'd :" << endl << points[0].x << " " << points[0].y << " " << points[0].z << endl;
		cout << points[1].x << " " << points[1].y << " " << points[1].z << endl;
		cout << points[2].x << " " << points[2].y << " " << points[2].z << endl << endl;

        // scale into view
        points[0].x += 1.f; points[0].y += 1.f;
        points[1].x += 1.f; points[1].y += 1.f;
        points[2].x += 1.f; points[2].y += 1.f;

        // scale into screen
        points[0].x *= 0.5f * (float)WIDTH; points[0].y *= 0.5f * (float)HEIGHT;
        points[1].x *= 0.5f * (float)WIDTH; points[1].y *= 0.5f * (float)HEIGHT;
        points[2].x *= 0.5f * (float)WIDTH; points[2].y *= 0.5f * (float)HEIGHT;

        cout << "Scaled :" << endl << points[0].x << " " << points[0].y << " " << points[0].z << endl;
		cout << points[1].x << " " << points[1].y << " " << points[1].z << endl;
		cout << points[2].x << " " << points[2].y << " " << points[2].z << endl << endl;

		/*points[0].z = -points[0].z;
		points[1].z = -points[1].z;
		points[2].z = -points[2].z;*/
        
        //Vec3f V = p2 - p1;
        //Vec3f W = p3 - p1;
        Vec3f VV = points[1] - points[0];
        Vec3f WW = points[2] - points[0];
        Vec3f surfaceNormal = cross(VV, WW).normalize();
        
        //Vec3f centroid = (p1 + p2 + p3) * (1.f / 3.f);
        Vec3f centroid = (points[0] + points[1] + points[2]) * (1.f / 3.f);
        
        Vec3f lightDirection = (light - centroid).normalize();
        float lightDistance = (light - centroid).norm();

        cout << "sidtance : " << lightDistance << endl;

        float lightIntensity = 3.5f * max(-(lightDirection * surfaceNormal) + 0.2f, 0.f);

        cout << "intensity : " << lightIntensity << endl;

        //lightIntensity += 0.1f;
        
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
            	Vec3f barycentr2 = barycenter(p1, p2, p3, pixel);

                if (barycentr.x >= 0 && barycentr.y >= 0 && barycentr.z >= 0) {
                	pixel.z = 0;
                	for (j = 0; j < 3; j++)
                		pixel.z += points[j].z * barycentr2[j];

                	if (zBuffer[(int) pixel.x + pixel.y * WIDTH] < (int) pixel.z) {
                		zBuffer[(int) pixel.x + pixel.y * WIDTH] = pixel.z;
	                    frameBuffer[ACCESS(0)] = MODEL_R * lightIntensity;
	                    frameBuffer[ACCESS(1)] = MODEL_G * lightIntensity;
	                    frameBuffer[ACCESS(2)] = MODEL_B * lightIntensity;
	                    /*frameBuffer[ACCESS(0)] = MODEL_R;
	                    frameBuffer[ACCESS(1)] = MODEL_G;
	                    frameBuffer[ACCESS(2)] = MODEL_B;*/
                	}
                }
            
            }
        }
    }
    
    writeImage(frameBuffer);
}

int main () {
    //Model duck("duck.obj");
    //Model head("african_head.obj");
    Model diablo("diablo_pose.obj");
    
    Vec3f light = Vec3f(400.f, 400.f, 0.f);
        
    Mat4f P = perspectiveProjectionMatrix(90.f, 0.1f, 100.f, (float)HEIGHT / (float)WIDTH);
    Mat4f V = viewMatrix();
    Mat4f M = translation(0.f, 0.f, 0.f) * rotation(0.f, 0.f, 0.f) * scale(1.f, 1.f, 1.f);

    render(diablo, P, V, M, light);

    cout << "Camera position : " << V[3][0] << " " << V[3][1] << " " << V[3][2] << endl;

    return 0;
}
