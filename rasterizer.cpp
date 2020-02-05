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

void render (Model model, Mat4f P, Mat4f M) {

    std::vector<int>frameBuffer(WIDTH * HEIGHT * 3);
    std::vector<int>zBuffer(WIDTH * HEIGHT);

    int i;
    for (i = 0; i < (WIDTH * HEIGHT * 3) - 2; i++) {
        frameBuffer[i + 0] = BACKGROUND_R;
        frameBuffer[i + 1] = BACKGROUND_G;
        frameBuffer[i + 2] = BACKGROUND_B;
    }

    std::fill(zBuffer.begin(), zBuffer.end(), INT_MIN);

	//
	// iteration through all the triangles of the model
    
    for (i = 0; i < model.nTriangles(); i++) {

    	int tv0 = model.triangleVertice(i, 0), tv1 = model.triangleVertice(i, 1), tv2 = model.triangleVertice(i, 2);

        Vec3f p0 = model.vertice(tv0);
		Vec3f p1 = model.vertice(tv1);
		Vec3f p2 = model.vertice(tv2);

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

		std::cout << "Transformed (0) : " << tp0 << std::endl;

		//
		// normal of triangle calculation #1 : using the V and W lines (kinda buggy cause some triangles might have baddly initialized normals (or bad zBuffer?))
        
        /*Vec3f line1 = tp1 - tp0;
        Vec3f line2 = tp2 - tp0;
        Vec3f surfaceNormal = line1.cross(line2).normalize();*/

        //
        // normal of triangle calculation #2 : average of the 3 vertices' normals (~~~~~ kinda buggy which is legitimate cause the vertex can change a lot ~~~~~)
        
        Vec3f surfaceNormal = ((vn0 + vn1 + vn2) * (1.f / 3.f)).normalize();

        //
        // back-face culling - ?

        //if (((surfaceNormal.x * (tp0.x - camera.x)) +
        //	 (surfaceNormal.y * (tp0.y - camera.y)) +
        //	 (surfaceNormal.z * (tp0.z - camera.z)) < 0.f)) continue;

        //
		// projection of the vertices

        Vec3f pp0 = P * tp0;
		Vec3f pp1 = P * tp1;
		Vec3f pp2 = P * tp2;

		std::cout << "Projected (0) : " << pp0 << std::endl;

		pp0.x += 1.f; pp0.y += 1.f;
		pp1.x += 1.f; pp1.y += 1.f;
		pp2.x += 1.f; pp2.y += 1.f;

		pp0.x *= 0.5f * (float)WIDTH; pp0.y *= 0.5f * (float)HEIGHT;
		pp1.x *= 0.5f * (float)WIDTH; pp1.y *= 0.5f * (float)HEIGHT;
		pp2.x *= 0.5f * (float)WIDTH; pp2.y *= 0.5f * (float)HEIGHT;

		std::cout << "Projected' (0) : " << pp0 << std::endl;
        
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
                	pixel.z = (pp0.z + pp1.z + pp2.z) / 3.f;

                	// depth of the triangle - ? (taken from ssloy)
                	/*pixel.z = 0;
            		pixel.z += pp0.z * bc[0];
            		pixel.z += pp1.z * bc[1];
            		pixel.z += pp2.z * bc[2];*/

            		// drawing (only if nearest pixel visited)
                	//if (zBuffer[(int)pixel.x + ((int)pixel.y * WIDTH)] < pixel.z) {

                		// updating the zBuffer
                		//zBuffer[(int)pixel.x + ((int)pixel.y * WIDTH)] = pixel.z;

	                    frameBuffer[((pixel.x + (pixel.y * WIDTH)) * 3) + 0] = MODEL_R * dot;
	                    frameBuffer[((pixel.x + (pixel.y * WIDTH)) * 3) + 1] = MODEL_G * dot;
	                    frameBuffer[((pixel.x + (pixel.y * WIDTH)) * 3) + 2] = MODEL_B * dot;
                	//}
                }
            
            }
        }
    }

    // write the frame buffer to the ppm file
    writeImage(frameBuffer);

}

int main () {
    //Model model("obj/african_head.obj");
    Model model("obj/diablo_pose.obj");
    
    // projection matrix 	   near   far    fov   width  height
    Mat4f mProjection = projectionMatrix(0.1f, 1000.f, 90.f, WIDTH, HEIGHT);

    // world matrix, for model transformations (= model matrix - ?)
	Mat4f mWorld = rotation(0.f, 0.f, 0.f) * scale(1.f, 1.f, 1.f) * translation(0.f, 0.f, 3.f);

	render(model, mProjection, mWorld);

    return 0;
}
