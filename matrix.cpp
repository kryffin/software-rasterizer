
// 4x4 (floats) matrix
class Mat4f {
public:
	float m[4][4] = {0.f};

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
};

// projection matrix (testing to do)
Mat4f projectionMatrix (float near, float far, float fov, int width, int height) {
    float aspectRatio = (float)height / (float)width;
    float fovRad = 1.f / tanf(fov * 0.5f / 180.f * M_PI);

    Mat4f P;

    P.m[0][0] = aspectRatio * fovRad;
    P.m[1][1] = fovRad;
    P.m[2][2] = far / (far - near);
    P.m[3][2] = (- far * near) / (far - near);
    P.m[2][3] = 1.f;
    P.m[3][3] = 0.f;

    return P;
}

Mat4f rotationX (float angle) {
	Mat4f out;
	out.m[0][0] = 1.f;
	out.m[1][1] = cosf(angle);
	out.m[1][2] = -sinf(angle);
	out.m[2][1] = sinf(angle);
	out.m[2][2] = cosf(angle);
	out.m[3][3] = 1.f;
	return out;
}

Mat4f rotationY (float angle) {
	Mat4f out;
	out.m[1][1] = 1.f;
	out.m[0][0] = cosf(angle);
	out.m[0][2] = sinf(angle);
	out.m[2][0] = -sinf(angle);
	out.m[2][2] = cosf(angle);
	out.m[3][3] = 1.f;
	return out;
}

Mat4f rotationZ (float angle) {
	Mat4f out;
	out.m[2][2] = 1.f;
	out.m[0][0] = cosf(angle);
	out.m[0][1] = -sinf(angle);
	out.m[1][0] = sinf(angle);
	out.m[1][1] = cosf(angle);
	out.m[3][3] = 1.f;
	return out;
}
