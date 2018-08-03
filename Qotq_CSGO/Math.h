#include <cmath>
#define _USE_MATH_DEFINES
#include "SDK.h"

namespace Math {
	void VectorTransform ( const Vector in1 , matrix3x4_t in2 , Vector &out );
	Vector CalculateAngle ( const Vector& in , Vector out );
	void AngleVectors ( const QAngle& angles , Vector& forward );
	void Normalize ( Vector & vIn , Vector & vOut );
	vec_t VectorNormalize ( Vector& v );
	void AngleVectorsV ( const Vector &angles , Vector *forward );
	void NormaliseViewAngle ( Vector &angle );
	void CalcAngle ( Vector src , Vector dst , Vector &angles );
	void VectorAngles2 ( const Vector& forward , QAngle &angles );
	QAngle CalcAngle2 ( Vector src , Vector dst );
	void VectorAngles ( const Vector& forward , QAngle& angles );
	void SinCos ( float radians , float* sine , float* cosine );
	double deg2rad ( double deg );
	double rad2deg ( double rad );
};