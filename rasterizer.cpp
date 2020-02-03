#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <limits>

#include "geometry.h"
#include "matrix.cpp"
#include "model.cpp" //adapted for the new obj structure (thx ssloy)

#define WIDTH 800
#define HEIGHT 800

#define ACCESS_C ((x + (y * WIDTH)) * 3) + c
#define ACCESS(i) ((pixel.x + (pixel.y * WIDTH)) * 3) + i

// background base color
#define BACKGROUND_R 75
#define BACKGROUND_G 75
#define BACKGROUND_B 75

// model base color
#define MODEL_R 200
#define MODEL_G 0
#define MODEL_B 0

#define FLT_MAX std::numeric_limits<float>::max()
#define FLT_MIN std::numeric_limits<float>::lowest()

void writeImage (std::vector<int>frameBuffer) {
    std::ofstream file;
    file.open("out.ppm");
    file << "P3" << std::endl << WIDTH << " " << HEIGHT << std::endl << 255 << std::endl;
    int x, y, c;
    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            for (c = 0; c < 3; c++) {
                file << frameBuffer[((x + (y * WIDTH)) * 3) + c] << " ";
            }
        }
        file << std::endl;
    }
    file.close();
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
}

void render (Model model, Mat4f P, Vec3f camera) {

    std::vector<int>frameBuffer(WIDTH * HEIGHT * 3);
    std::vector<int>zBuffer(WIDTH * HEIGHT);

    int i;
    for (i = 0; i < (WIDTH * HEIGHT * 3) - 2; i++) {
        frameBuffer[i + 0] = BACKGROUND_R;
        frameBuffer[i + 1] = BACKGROUND_G;
        frameBuffer[i + 2] = BACKGROUND_B;
    }

    std::fill(zBuffer.begin(), zBuffer.end(), INT_MIN);
    
    for (i = 0; i < model.nfaces(); i++) {

        Vec3f p0 = model.point(model.vert(i, 0));
		Vec3f p1 = model.point(model.vert(i, 1));
		Vec3f p2 = model.point(model.vert(i, 2));

		Vec3f vn0 = model.norm(model.vert(i, 0));
		Vec3f vn1 = model.norm(model.vert(i, 1));
		Vec3f vn2 = model.norm(model.vert(i, 2));

		Vec3f rp0 = p0;
		Vec3f rp1 = p1;
		Vec3f rp2 = p2;

		//
		// rotations

		/*rp0 = rotationZ(5.f) * rp0;
		rp1 = rotationZ(5.f) * rp1;
		rp2 = rotationZ(5.f) * rp2;*/

		/*rp0 = rotationY(15.f) * rp0;
		rp1 = rotationY(15.f) * rp1;
		rp2 = rotationY(15.f) * rp2;*/

		/*rp0 = rotationX(3.f) * rp0;
		rp1 = rotationX(3.f) * rp1;
		rp2 = rotationX(3.f) * rp2;*/

		Vec3f tp0 = rp0;
		Vec3f tp1 = rp1;
		Vec3f tp2 = rp2;

		tp0.z += 3.f;
		tp1.z += 3.f;
		tp2.z += 3.f;

		Vec3f pp0 = P * tp0;
		Vec3f pp1 = P * tp1;
		Vec3f pp2 = P * tp2;

		pp0.x += 1.f; pp0.y += 1.f;
		pp1.x += 1.f; pp1.y += 1.f;
		pp2.x += 1.f; pp2.y += 1.f;

		pp0.x *= 0.5f * (float)WIDTH; pp0.y *= 0.5f * (float)HEIGHT;
		pp1.x *= 0.5f * (float)WIDTH; pp1.y *= 0.5f * (float)HEIGHT;
		pp2.x *= 0.5f * (float)WIDTH; pp2.y *= 0.5f * (float)HEIGHT;

		//
		// vertices drawing

		/*frameBuffer[(((int)pp0.x + ((int)pp0.y * WIDTH)) * 3) + 0] = 255;
		frameBuffer[(((int)pp0.x + ((int)pp0.y * WIDTH)) * 3) + 1] = 0;
		frameBuffer[(((int)pp0.x + ((int)pp0.y * WIDTH)) * 3) + 2] = 0;

		frameBuffer[((pp1.x + ((int)pp1.y * WIDTH)) * 3) + 0] = 255;
		frameBuffer[((pp1.x + ((int)pp1.y * WIDTH)) * 3) + 1] = 0;
		frameBuffer[((pp1.x + ((int)pp1.y * WIDTH)) * 3) + 2] = 0;

		frameBuffer[(((int)pp2.x + ((int)pp2.y * WIDTH)) * 3) + 0] = 255;
		frameBuffer[(((int)pp2.x + ((int)pp2.y * WIDTH)) * 3) + 1] = 0;
		frameBuffer[(((int)pp2.x + ((int)pp2.y * WIDTH)) * 3) + 2] = 0;*/

		//
		// normal of triangle calculation #1 : using the V and W lines (kinda buggy cause some triangles might have baddly initialized normals (or bad zBuffer?))
        
        Vec3f VV = pp1 - pp0;
        Vec3f WW = pp2 - pp0;

        Vec3f surfaceNormal = Vec3f(
        		(VV.y * WW.z) - (VV.z * WW.y),
        		(VV.z * WW.x) - (VV.x * WW.z),
        		(VV.x * WW.y) - (VV.y * WW.x)
        	);

        float toNormalize = 1.f / sqrtf(surfaceNormal.x * surfaceNormal.x + surfaceNormal.y * surfaceNormal.y + surfaceNormal.z * surfaceNormal.z);
        surfaceNormal.x /= toNormalize; surfaceNormal.y /= toNormalize; surfaceNormal.z /= toNormalize;

        //
        // normal of triangle calculation #2 : average of the 3 vertices' normals (~~~~~ kinda buggy which is legitimate cause the vertex can change a lot ~~~~~)
        
        /*Vec3f surfaceNormal = (vn0 + vn1 + vn2) * (1.f / 3.f);
        float toNormalize = 1.f / sqrtf(surfaceNormal.x * surfaceNormal.x + surfaceNormal.y * surfaceNormal.y + surfaceNormal.z * surfaceNormal.z);
        surfaceNormal.x /= toNormalize; surfaceNormal.y /= toNormalize; surfaceNormal.z /= toNormalize;*/

        //
        // back-face culling - ?

        if (((surfaceNormal.x * (tp0.x - camera.x)) +
        	 (surfaceNormal.y * (tp0.y - camera.y)) +
        	 (surfaceNormal.z * (tp0.z - camera.z)) < 0.f)) continue;
        
        //
    	// constant light exposure

        Vec3f lightDirection = Vec3f(0.f, 0.f, -1.f);
        toNormalize = 1.f / sqrtf(lightDirection.x * lightDirection.x + lightDirection.y * lightDirection.y + lightDirection.z * lightDirection.z);
        lightDirection.x /= toNormalize; lightDirection.y /= toNormalize; lightDirection.z /= toNormalize;

        //
        // angle between triangle normal and light direction

        float dot = lightDirection.x * surfaceNormal.x + lightDirection.y * surfaceNormal.y + lightDirection.z * surfaceNormal.z;

        //
        // bounding box init

        Vec2f bboxMin(FLT_MAX, FLT_MAX);
        Vec2f bboxMax(FLT_MIN, FLT_MIN);
        
        if (pp0.x < bboxMin.x) bboxMin.x = pp0.x;
        if (pp0.y < bboxMin.y) bboxMin.y = pp0.y;
        if (pp0.x > bboxMax.x) bboxMax.x = pp0.x;
        if (pp0.y > bboxMax.y) bboxMax.y = pp0.y;

        if (pp1.x < bboxMin.x) bboxMin.x = pp1.x;
        if (pp1.y < bboxMin.y) bboxMin.y = pp1.y;
        if (pp1.x > bboxMax.x) bboxMax.x = pp1.x;
        if (pp1.y > bboxMax.y) bboxMax.y = pp1.y;

        if (pp2.x < bboxMin.x) bboxMin.x = pp2.x;
        if (pp2.y < bboxMin.y) bboxMin.y = pp2.y;
        if (pp2.x > bboxMax.x) bboxMax.x = pp2.x;
        if (pp2.y > bboxMax.y) bboxMax.y = pp2.y;
        
        //
        // bounding box clamping in-screen

        bboxMin.x = std::max(0.f, std::min(bboxMin.x, (float)WIDTH));
        bboxMin.y = std::max(0.f, std::min(bboxMin.y, (float)HEIGHT));
        bboxMax.x = std::max(0.f, std::min(bboxMax.x, (float)WIDTH));
        bboxMax.y = std::max(0.f, std::min(bboxMax.y, (float)HEIGHT));
                
        //
        // iterating through the bounding box

        Vec3f pixel;
        for (pixel.x = (int)bboxMin.x; pixel.x < bboxMax.x; pixel.x++) {
            for (pixel.y = (int)bboxMin.y; pixel.y < bboxMax.y; pixel.y++) {

            	// barycenter of the pixel relative to the triangle (whether or not the pixel is in the triangle)
                Vec3f bc = barycenter(pp0, pp1, pp2, pixel);

                if (bc.x >= 0 && bc.y >= 0 && bc.z >= 0) {
                	
                	// depth of the triangle - ? (taken from ssloy)
                	pixel.z = 0;
            		pixel.z += pp0.z * bc[0];
            		pixel.z += pp1.z * bc[1];
            		pixel.z += pp2.z * bc[2];

            		// drawing (only if nearest pixel visited)
                	if (zBuffer[(int)pixel.x + ((int)pixel.y * WIDTH)] < pixel.z) {

                		// updating the zBuffer
                		zBuffer[(int)pixel.x + ((int)pixel.y * WIDTH)] = pixel.z;

	                    frameBuffer[((pixel.x + (pixel.y * WIDTH)) * 3) + 0] = MODEL_R * dot; // red model color * light intensity
	                    frameBuffer[((pixel.x + (pixel.y * WIDTH)) * 3) + 1] = MODEL_G * dot; // green model color * light intensity
	                    frameBuffer[((pixel.x + (pixel.y * WIDTH)) * 3) + 2] = MODEL_B * dot; // blue model color * light intensity
                	}
                }
            
            }
        }
    }

    // write the frame buffer to the ppm file
    writeImage(frameBuffer);

}

int main () {
    //Model model("african_head.obj");
    Model model("diablo_pose.obj");
    
    // TODO
    Vec3f camera = Vec3f(0.f, 0.f, 0.f);

    // projection matrix 	   near   far    fov   width  height
    Mat4f P = projectionMatrix(0.1f, 1000.f, 90.f, WIDTH, HEIGHT);

	render(model, P, camera);

    return 0;
}
