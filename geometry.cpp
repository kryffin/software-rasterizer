#include <ostream>

// (x, y) (floats) vector
class Vec2f {
public:
	float x = 0.f, y = 0.f;

	Vec2f () {}

	Vec2f (float x, float y) {
		this->x = x;
		this->y = y;
	}

	float length () {
		return sqrtf((x * x) + (y * y));
	}

	Vec2f normalize () {
		float len = length();
		return Vec2f(x / len, y / len);
	}

	Vec2f operator+ (Vec2f in) {
		return Vec2f(x + in.x, y + in.y);
	}

	Vec2f operator+ (float in) {
		return Vec2f(x + in, y + in);
	}

	Vec2f operator- (Vec2f in) {
		return Vec2f(x - in.x, y - in.y);
	}

	Vec2f operator- (float in) {
		return Vec2f(x - in, y - in);
	}

	float operator* (Vec2f in) {
		return (x * in.x) + (y * in.y);
	}

	Vec2f operator/ (Vec2f in) {
		return Vec2f(x / in.x, y / in.y);
	}

	Vec2f operator/ (float in) {
		return Vec2f(x / in, y / in);
	}

	std::ostream& operator<< (std::ostream& os) {
		os << x << " " << y;
		return os;
	}
};

std::ostream& operator<< (std::ostream& os, Vec2f v) {
	os << v.x << " " << v.y;
	return os;
}

class Vec3i {
public:
	int x = 0, y = 0, z = 0;

	Vec3i () {}

	Vec3i (int x, int y, int z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
};

// (x, y, z) (floats) vector
class Vec3f {
public:
	float x = 0.f, y = 0.f, z = 0.f;

	Vec3f () {}

	Vec3f (float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Vec3f cross (Vec3f in) {
		return Vec3f(
        		(y * in.z) - (z * in.y),
        		(z * in.x) - (x * in.z),
        		(x * in.y) - (y * in.x)
        	);
	}

	float dot (Vec3f in) {
		return (x * in.x) + (y * in.y) + (z * in.z);
	}

	float length () {
		return sqrtf((x * x) + (y * y) + (z * z));
	}

	Vec3f normalize () {
		float len = length();
		return Vec3f(x / len, y / len, z / len);
	}

	Vec3f operator+ (Vec3f in) {
		return Vec3f(x + in.x, y + in.y, z + in.z);
	}

	Vec3f operator+ (float in) {
		return Vec3f(x + in, y + in, z + in);
	}

	Vec3f operator- (Vec3f in) {
		return Vec3f(x - in.x, y - in.y, z - in.z);
	}

	Vec3f operator- (float in) {
		return Vec3f(x - in, y - in, z - in);
	}

	Vec3f operator* (Vec3f in) {
		return Vec3f(x * in.x, y * in.y, z * in.z);
	}

	Vec3f operator* (float in) {
		return Vec3f(x * in, y * in, z * in);
	}

	Vec3f operator/ (Vec3f in) {
		return Vec3f(x / in.x, y / in.y, z / in.z);
	}

	Vec3f operator/ (float in) {
		return Vec3f(x / in, y / in, z / in);
	}
};

std::ostream& operator<< (std::ostream& os, Vec3f v) {
	os << v.x << " " << v.y << " " << v.z;
	return os;
}

// 4x4 (floats) matrix
class Mat4f {
public:
	float m[4][4] = {0.f};

	Mat4f () {}

	Mat4f (float f) {
		m[0][0] = f;
		m[1][1] = f;
		m[2][2] = f;
		m[3][3] = f;
	}

	Vec3f operator* (Vec3f in) {
		Vec3f out = Vec3f(
			out.x = in.x * m[0][0] + in.y * m[1][0] + in.z * m[2][0] + m[3][0],
			out.y = in.x * m[0][1] + in.y * m[1][1] + in.z * m[2][1] + m[3][1],
			out.z = in.x * m[0][2] + in.y * m[1][2] + in.z * m[2][2] + m[3][2]
		);

		float w = in.x * m[0][3] + in.y * m[1][3] + in.z * m[2][3] + m[3][3];

		if (w != 0.f) {
			out.x /= w;
			out.y /= w;
			out.z /= w;
		}

		return out;
	}

	Mat4f operator* (Mat4f in) {
		Mat4f out = Mat4f();
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				for (int k = 0; k < 4; k++)
					out.m[i][j] += m[i][k] * in.m[k][j];
		return out;
	}
};

// projection matrix (testing to do)
Mat4f projectionMatrix (float near, float far, float fov, int width, int height) {
    float aspectRatio = (float)height / (float)width;
    float fovRad = 1.f / tanf(fov * 0.5f * (M_PI / 180.f));

    Mat4f P;

    P.m[0][0] = aspectRatio * fovRad;
    P.m[1][1] = fovRad;
    P.m[2][2] = -far / (far - near);
    P.m[3][2] = (- far * near) / (far - near);
    P.m[2][3] = -1.f;
    P.m[3][3] = 0.f;

    return P;
}

Mat4f viewPort (int x, int y, int w, int h) {
	float depth = 255.f;
	Mat4f VP = Mat4f(1.f);

    VP.m[0][3] = x + (w / 2.f);
    VP.m[1][3] = y + (h / 2.f);
    VP.m[2][3] = depth / 2.f;

    VP.m[0][0] = w / 2.f;
    VP.m[1][1] = h / 2.f;
    VP.m[2][2] = depth / 2.f;

    return VP;
}

Mat4f rotationZ (float angle) {
	Mat4f out;
	out.m[2][2] = 1.f;
	out.m[0][0] = cosf(angle);
	out.m[1][0] = -sinf(angle);
	out.m[0][1] = sinf(angle);
	out.m[1][1] = cosf(angle);
	out.m[3][3] = 1.f;
	return out;
}

Mat4f rotationY (float angle) {
	Mat4f out;
	out.m[1][1] = 1.f;
	out.m[0][0] = cosf(angle);
	out.m[2][0] = sinf(angle);
	out.m[0][2] = -sinf(angle);
	out.m[2][2] = cosf(angle);
	out.m[3][3] = 1.f;
	return out;
}

Mat4f rotationX (float angle) {
	Mat4f out;
	out.m[0][0] = 1.f;
	out.m[1][1] = cosf(angle);
	out.m[2][1] = -sinf(angle);
	out.m[1][2] = sinf(angle);
	out.m[2][2] = cosf(angle);
	out.m[3][3] = 1.f;
	return out;
}

Mat4f rotation (float xAngle, float yAngle, float zAngle) {
	Mat4f R;
	R = rotationZ(zAngle) * rotationY(yAngle) * rotationX(xAngle);
	return R;
}

Mat4f scale (float x, float y, float z) {
	Mat4f S;
	S.m[0][0] = x;
	S.m[1][1] = y;
	S.m[2][2] = z;
	S.m[3][3] = 1.f;
	return S;
}

Mat4f translation (float x, float y, float z) {
	Mat4f T = Mat4f(1.f);
	T.m[3][0] = x;
	T.m[3][1] = y;
	T.m[3][2] = z;
	return T;
}
