#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <limits>

//#include "geometry.h"
#include "geometry.cpp"
#include "parser.cpp" //adapted for the new obj structure (thx ssloy)

#define WIDTH 800
#define HEIGHT 800

// background base color
#define BACKGROUND_COLOR 0

// model base color
#define MODEL_R 255
#define MODEL_G 255
#define MODEL_B 255

#define FLT_MAX std::numeric_limits<float>::max()
#define FLT_MIN std::numeric_limits<float>::lowest()

void writeImage (std::vector<int>frameBuffer) {
    std::ofstream file;
    file.open("out.ppm");
    file << "P3" << std::endl << WIDTH << " " << HEIGHT << std::endl << 255 << std::endl;
    int x, y, c;
    for (y = 0; y < HEIGHT; y++) {
        for (x = WIDTH-1; x >= 0; x--) { //reverse course to inverse the rendered image along the x-axis
            for (c = 0; c < 3; c++) {
                file << frameBuffer[((x + (y * WIDTH)) * 3) + c] << " ";
            }
        }
        file << std::endl;
    }
    file.close();
}

Vec3f barycenter (Vec3f a, Vec3f b, Vec3f c, Vec3f pixel) {
	Vec3f bc = Vec3f();
	bc.x = (c.x-b.x)*(pixel.y-b.y) - (c.y-b.y)*(pixel.x-b.x);
	bc.y = (a.x-c.x)*(pixel.y-c.y) - (a.y-c.y)*(pixel.x-c.x);
	bc.z = (b.x-a.x)*(pixel.y-a.y) - (b.y-a.y)*(pixel.x-a.x);
	return bc;
}

std::vector<int> render (Model model, Mat4f P, Mat4f M, Mat4f C) {

    std::vector<int>frameBuffer(WIDTH * HEIGHT); //single colored pixels for the grayscale
    std::vector<float>zBuffer(WIDTH * HEIGHT);

    std::fill(frameBuffer.begin(), frameBuffer.end(), BACKGROUND_COLOR);
    std::fill(zBuffer.begin(), zBuffer.end(), FLT_MIN);

	//
	// iteration through all the triangles of the model
    
    int i;
    for (i = 0; i < model.nTriangles(); i++) {

    	//triangle indices
    	int tv0 = model.triangleVertice(i, 0), tv1 = model.triangleVertice(i, 1), tv2 = model.triangleVertice(i, 2);

    	// triangle vertices
        Vec3f p0 = model.vertice(tv0);
		Vec3f p1 = model.vertice(tv1);
		Vec3f p2 = model.vertice(tv2);

		// vertices normals
		Vec3f vn0 = model.normal(tv0);
		Vec3f vn1 = model.normal(tv1);
		Vec3f vn2 = model.normal(tv2);

		Vec3f tp0 = p0;
		Vec3f tp1 = p1;
		Vec3f tp2 = p2;

		//
		// world transformation

		tp0 = M * tp0;
		tp1 = M * tp1;
		tp2 = M * tp2;

		//
		// transformation to get along with the camera

        tp0 = C * tp0;
		tp1 = C * tp1;
		tp2 = C * tp2;

        //
        // normal of triangle calculation : average of the 3 vertices' normals
        
        Vec3f surfaceNormal = ((vn0 + vn1 + vn2) * (1.f / 3.f)).normalize();

        //
		// projection of the vertices - clean that somehow

        Vec3f pp0 = P * tp0;
		Vec3f pp1 = P * tp1;
		Vec3f pp2 = P * tp2;

		pp0.x += 1.f; pp0.y += 1.f;
		pp1.x += 1.f; pp1.y += 1.f;
		pp2.x += 1.f; pp2.y += 1.f;

		pp0.x /= 2.f; pp0.y /= 2.f;
		pp1.x /= 2.f; pp1.y /= 2.f;
		pp2.x /= 2.f; pp2.y /= 2.f;

		pp0.x *= (float)WIDTH; pp0.y *= (float)HEIGHT; pp0.z *= 2.f;
		pp1.x *= (float)WIDTH; pp1.y *= (float)HEIGHT; pp1.z *= 2.f;
		pp2.x *= (float)WIDTH; pp2.y *= (float)HEIGHT; pp2.z *= 2.f;
        
        //
    	// constant light exposure

        Vec3f lightDirection = Vec3f(0.f, 0.f, 1.f).normalize();

        //
        // angle between triangle normal and light direction

        float dot = surfaceNormal.dot(lightDirection);

        //
        // bounding box init -- clean that somehow

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
        // bounding box clamping in-screen -- clean that somehow

        bboxMin.x = std::max(0.f, std::min(bboxMin.x, (float)WIDTH));
        bboxMin.y = std::max(0.f, std::min(bboxMin.y, (float)HEIGHT));
        bboxMax.x = std::max(0.f, std::min(bboxMax.x, (float)WIDTH));
        bboxMax.y = std::max(0.f, std::min(bboxMax.y, (float)HEIGHT));

        //
        // iterating through the bounding box -- maybe clean that?

        Vec3f pixel;
        for (pixel.x = (int)bboxMin.x; pixel.x < bboxMax.x; pixel.x++) {
            for (pixel.y = (int)bboxMin.y; pixel.y < bboxMax.y; pixel.y++) {

            	// barycenter of the pixel relative to the triangle (whether or not the pixel is in the triangle)
                Vec3f bc = barycenter(pp0, pp1, pp2, pixel);

                if (bc.x >= 0 && bc.y >= 0 && bc.z >= 0) {
                	
                	// depth of the triangle by average of z coordinates
                	pixel.z = (p0.z + p1.z + p2.z) / 3.f;

            		// drawing (only if nearest pixel visited)
                	if (zBuffer[(int)pixel.x + ((int)pixel.y * WIDTH)] < pixel.z) {

                		// updating the zBuffer
                		zBuffer[(int)pixel.x + ((int)pixel.y * WIDTH)] = pixel.z;

                		if (dot < 0.f) dot = 0.f;

	                    frameBuffer[pixel.x + (pixel.y * WIDTH)] = (0.21f * (MODEL_R * dot)) + (0.72f * (MODEL_G * dot)) + (0.07f * (MODEL_B * dot));
                	}
                }

            }
        }
    }

    // return the frame buffer write image outside
    return frameBuffer;

}



std::vector<int> mix_channels(std::vector<int> buffR, std::vector<int> buffB){
    std::vector<int>frameBuffer(WIDTH * HEIGHT * 3); //vector representing the anaglyph render
    
    //red buffer goes in red channel, blue buffer in blue channel
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            frameBuffer[((x + (y * WIDTH)) * 3) + 0] = buffR[x + (y * WIDTH)]; //red
            frameBuffer[((x + (y * WIDTH)) * 3) + 1] = 0; //green
            frameBuffer[((x + (y * WIDTH)) * 3) + 2] = buffB[x + (y * WIDTH)]; //blue
        }
    }

    return frameBuffer;
}

int main () {
    Model model("obj/diablo_pose.obj");
    
    // projection matrix 	   near   far    fov   width  height
    Mat4f mProjection = projectionMatrix(0.1f, 1000.f, 90.f, (float)WIDTH, (float)HEIGHT);

    // world matrix, for model transformations (= model matrix - ?)
	Mat4f mWorld = rotation(0.f, 0.f, 0.f) * scale(1.f, 1.f, 1.f) * translation(0.f, 0.f, -5.f);

	// camera matrix, determines where the camera is, what it is facing and where does its upward vector is
	Mat4f mCameraR = lookAt(Vec3f(-0.05f, 0.f, 5.f), Vec3f(0.f, 0.f, 0.f), Vec3f(0.f, 1.f, 0.f));
    Mat4f mCameraB = lookAt(Vec3f(0.05f, 0.f, 5.f), Vec3f(0.f, 0.f, 0.f), Vec3f(0.f, 1.f, 0.f));

    //catching the two buffered image in order to construct the main image
    std::vector<int> frameBufferR = render(model, mProjection, mWorld, mCameraR); //red buffer
	std::vector<int> frameBufferB = render(model, mProjection, mWorld, mCameraB); //blue buffer

    //mixing the channels to render the anaglyph image
    std::vector<int>frameBuffer = mix_channels(frameBufferR, frameBufferB);

    writeImage(frameBuffer);
    
    return 0;
}
