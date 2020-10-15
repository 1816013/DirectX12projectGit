#include "Geometry.h"
#include <algorithm>
#include <vector>

 const Vector2D<float> Vector2D<float>::ZERO(0.0f, 0.0f);
 const Vector2D<float> Vector2D<float>::NIL(NAN, NAN);
 const Vector2D<float> Vector2D<float>::UP(0.0f, -1.0f);
 const Vector2D<float> Vector2D<float>::DOWN(0.0f, 1.0f);
 const Vector2D<float> Vector2D<float>::RIGHT(1.0f, 0.0f);
 const Vector2D<float> Vector2D<float>::LEFT(-1.0f, 0.0f);

 const Segment Segment::ZERO(Vector2f::ZERO, Vector2f::ZERO);
 const Segment Segment::NIL(Vector2f::NIL, Vector2f::NIL);

 

float Dot(const Vector2f& va, const Vector2f& vb)
{
	return va.x * vb.x + va.y * vb.y;
}

float Closs(const Vector2f& va, const Vector2f& vb)
{
    return va.x * vb.y - va.y * vb.x;
}

float Clamp(float val, float minVal, float maxVal)
{
	return std::min(std::max(val, minVal), maxVal);
}


bool Segment::IsNil()
{
    return isnan(vec.x) || isnan(vec.y);
}

Position2f Segment::End()
{
    return start + vec;
}

// �P�ʍs���Ԃ�
Matrix IdentityMat() {
	Matrix ret = {};		// 0�ŏ�����
	ret.m[0][0] = ret.m[1][1] = ret.m[2][2] = 1.0f;
	return ret;
}

// �Q�̍s��̏�Z��Ԃ�
Matrix MultipleMat(const Matrix& lmat, const Matrix& rmat) {
	Matrix ret = {  };
	ret.m[0][0] = lmat.m[0][0] * rmat.m[0][0] + lmat.m[0][1] * rmat.m[1][0] + lmat.m[0][2] * rmat.m[2][0];
	ret.m[0][1] = lmat.m[0][0] * rmat.m[0][1] + lmat.m[0][1] * rmat.m[1][1] + lmat.m[0][2] * rmat.m[2][1];
	ret.m[0][2] = lmat.m[0][0] * rmat.m[0][2] + lmat.m[0][1] * rmat.m[1][2] + lmat.m[0][2] * rmat.m[2][2];
	ret.m[1][0] = lmat.m[1][0] * rmat.m[0][0] + lmat.m[1][1] * rmat.m[1][0] + lmat.m[1][2] * rmat.m[2][0];
	ret.m[1][1] = lmat.m[1][0] * rmat.m[0][1] + lmat.m[1][1] * rmat.m[1][1] + lmat.m[1][2] * rmat.m[2][1];
	ret.m[1][2] = lmat.m[1][0] * rmat.m[0][2] + lmat.m[1][1] * rmat.m[1][2] + lmat.m[1][2] * rmat.m[2][2];
	ret.m[2][0] = lmat.m[2][0] * rmat.m[0][0] + lmat.m[2][1] * rmat.m[1][0] + lmat.m[2][2] * rmat.m[2][0];
	ret.m[2][1] = lmat.m[2][0] * rmat.m[0][1] + lmat.m[2][1] * rmat.m[1][1] + lmat.m[2][2] * rmat.m[2][1];
	ret.m[2][2] = lmat.m[2][0] * rmat.m[0][2] + lmat.m[2][1] * rmat.m[1][2] + lmat.m[2][2] * rmat.m[2][2];
	return ret;

}

// �x�N�g���ɑ΂��čs���Z��K�p���A���ʂ̃x�N�g����Ԃ�
Vector2f MultipleVec(const Matrix& mat, const Vector2f& vec) {
	Vector2f ret = {};
	ret.x = mat.m[0][0] * vec.x + mat.m[0][1] * vec.y + mat.m[0][2];
	ret.y = mat.m[1][0] * vec.x + mat.m[1][1] * vec.y + mat.m[1][2];
	return ret;
}

Vector2f operator*(const Matrix& mat, const Vector2f& vec)
{
	return MultipleVec(mat, vec);
}

// ���s�ړ��s���Ԃ�
Matrix TranslateMat(float x, float y) {
	Matrix ret = {};
	ret = IdentityMat();
	ret.m[0][2] = x;
	ret.m[1][2] = y;
	return ret;
}

///��]�s���Ԃ�
///@param angle ��]�p�x
Matrix RotateMat(float angle) {
	Matrix ret = {};
	ret = IdentityMat();
	ret.m[0][0] = cos(angle);
	ret.m[0][1] = -sin(angle);
	ret.m[1][0] = sin(angle);
	ret.m[1][1] = cos(angle);
	return ret;
}