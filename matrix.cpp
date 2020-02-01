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

Mat4f perspectiveProjectionMatrix (float fov, float near, float far) {
	Mat4f P = Mat4f();

	float scale = 1.f / (tan((fov / 2.f) * (M_PI / 180.f)));

	P[0][0] = scale;
    P[1][1] = scale;
    P[2][2] = - (far / (far - near));
    P[3][3] = 0.f;
    P[2][3] = -1.f;
    P[3][2] = - ((far * near) / (far - near));

    return P;
}

Mat4f lookAt (Vec3f eye, Vec3f target, Vec3f up) {
	Vec3f zaxis = (eye - target).normalize();
	Vec3f xaxis = cross(up, zaxis).normalize();
	Vec3f yaxis = cross(zaxis, xaxis);

	Mat4f orient = Mat4f();

	orient[0][0] = xaxis.x;
	orient[0][1] = yaxis.x;
	orient[0][2] = zaxis.x;
	orient[1][0] = xaxis.y;
	orient[1][1] = yaxis.y;
	orient[1][2] = zaxis.y;
	orient[2][0] = xaxis.z;
	orient[2][1] = yaxis.z;
	orient[2][2] = zaxis.z;

	Mat4f trans = Mat4f();

	trans[3][0] = -eye.x;
	trans[3][1] = -eye.y;
	trans[3][2] = -eye.z;

	return orient * trans;
}

Mat4f viewMatrix () {
	Mat4f V = Mat4f();

	V[1][1] = -1.f;

	return V;
}

Mat4f translation (float x, float y, float z) {
	Mat4f T = Mat4f();

	T[3][0] = x;
	T[3][1] = y;
	T[3][2] = z;

	return T;
}

Mat4f scale (float x, float y, float z) {
	Mat4f S = Mat4f();

	S[0][0] = x;
	S[1][1] = y;
	S[2][2] = z;

	return S;
}

Mat4f rotationX (float x) {
	Mat4f R = Mat4f();

	R[1][1] = cos(x);
	R[2][2] = cos(x);
	R[1][2] = -sin(x);
	R[2][1] = sin(x);

	return R;
}

Mat4f rotationY (float y) {
	Mat4f R = Mat4f();

	R[0][0] = cos(y);
	R[2][2] = cos(y);
	R[0][2] = sin(y);
	R[2][0] = -sin(y);

	return R;
}

Mat4f rotationZ (float z) {
	Mat4f R = Mat4f();

	R[0][0] = cos(z);
	R[1][1] = cos(z);
	R[0][1] = -sin(z);
	R[1][0] = sin(z);

	return R;
}

Mat4f rotation (float x, float y, float z) {
	return rotationX(x) * rotationY(y) * rotationZ(z);
}
