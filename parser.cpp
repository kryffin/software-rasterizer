#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>

// Model class taken from ssloy (thanks) and modified for comprehension purposes as well as vertices normals parsing
class Model {
private:
	std::vector<Vec3f> vertices;
	std::vector<Vec3i> triangles;
	std::vector<Vec3f> normals;

public:
	Model (const char* filename) {
		std::ifstream in;
	    in.open(filename, std::ifstream::in);
	    if (in.fail()) {
	        std::cerr << "Failed to open " << filename << std::endl;
	        return;
	    }

	    std::string line;
	    while (!in.eof()) {

	        std::getline(in, line);
	        std::istringstream iss(line.c_str());
	        char trash;
	        if (!line.compare(0, 2, "v ")) { //vertices

	            iss >> trash;
	            Vec3f v;
	            iss >> v.x; iss >> v.y; iss >> v.z;
	            vertices.push_back(v);

	        } else if (!line.compare(0, 2, "f ")) { //triangles (f for faces)

	            Vec3i t;
	            std::string str, token;
	            int idx, cnt = 0;
	            iss >> trash;
	            while (iss >> str) {
	                token = str.std::string::substr(0, str.std::string::find('/'));
	                idx = std::stoi(token);
	                idx--; // in wavefront obj all indices start at 1, not zero
	                if (cnt == 0) t.x = idx;
	                if (cnt == 1) t.y = idx;
	                if (cnt == 2) t.z = idx;
	                cnt++;
	            }
	            if (cnt == 3) triangles.push_back(t);

	        } else if (!line.compare(0, 3, "vn ")) { //vertices normals

	            iss >> trash;
	            iss >> trash;
	            Vec3f n;
	            iss >> n.x; iss >> n.y; iss >> n.z;
	            normals.push_back(n);

	        }

	    }
	    std::cout << "# v# " << vertices.size() << " f# "  << triangles.size() << " vn# " << normals.size() << std::endl;
	}

	int nVertices () {
		return (int)vertices.size();
	}

	int nTriangles () {
		return (int)triangles.size();
	}

	int nNormals () {
		return (int)normals.size();
	}

	Vec3f vertice (int i) {
    	assert(i >= 0 && i < nVertices());
    	return vertices[i];
	}

	int triangleVertice (int ti, int i) {
	    assert(ti >= 0 && ti < nTriangles() && i >= 0 && i < 3);
	    if (i == 0) return triangles[ti].x;
	    if (i == 1) return triangles[ti].y;
	    return triangles[ti].z;
	}

	Vec3f normal (int vi) {
	    assert(vi >= 0 && vi < nNormals());
	    return normals[vi];
	}
};