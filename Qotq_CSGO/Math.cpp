#include "math.h"
void Math::VectorTransform ( const Vector in1 , matrix3x4_t in2 , Vector &out )
{
	out [ 0 ] = in1.Dot ( in2 [ 0 ] ) + in2 [ 0 ] [ 3 ];
	out [ 1 ] = in1.Dot ( in2 [ 1 ] ) + in2 [ 1 ] [ 3 ];
	out [ 2 ] = in1.Dot ( in2 [ 2 ] ) + in2 [ 2 ] [ 3 ];
}
Vector Math::CalculateAngle ( const Vector& in , Vector out )
{
	double delta [ 3 ] = { ( in [ 0 ] - out [ 0 ] ), ( in [ 1 ] - out [ 1 ] ), ( in [ 2 ] - out [ 2 ] ) };
	double hyp = sqrt ( delta [ 0 ] * delta [ 0 ] + delta [ 1 ] * delta [ 1 ] );
	Vector ret = Vector ( );
	ret.x = ( float ) ( asinf ( delta [ 2 ] / hyp ) * 57.295779513082f );
	ret.y = ( float ) ( atanf ( delta [ 1 ] / delta [ 0 ] ) * 57.295779513082f );
	ret.z = 0.0f;

	if ( delta [ 0 ] >= 0.0 )
		ret.y += 180.0f;
	return ret;
}
void Math::AngleVectorsV ( const Vector &angles , Vector *forward )
{
#define DEG2RAD( x ) ( ( float )( x ) * ( float )( ( float )( 3.1415926f ) / 180.0f ) )

	Assert ( s_bMathlibInitialized );
	Assert ( forward );

	float	sp , sy , cp , cy;

	sy = sin ( DEG2RAD ( angles [ 1 ] ) );
	cy = cos ( DEG2RAD ( angles [ 1 ] ) );

	sp = sin ( DEG2RAD ( angles [ 0 ] ) );
	cp = cos ( DEG2RAD ( angles [ 0 ] ) );

	forward->x = cp * cy;
	forward->y = cp * sy;
	forward->z = -sp;
}
void Math::NormaliseViewAngle ( Vector &angle )
{
	while ( angle.y <= -180 ) angle.y += 360;
	while ( angle.y > 180 ) angle.y -= 360;
	while ( angle.x <= -180 ) angle.x += 360;
	while ( angle.x > 180 ) angle.x -= 360;

	if ( angle.x > 89 ) angle.x = 89;
	if ( angle.x < -89 ) angle.x = -89;
	if ( angle.y < -180 ) angle.y = -179.999;
	if ( angle.y > 180 ) angle.y = 179.999;

	angle.z = 0;
}

void Math::VectorAngles ( const Vector& forward , QAngle& angles )
{
	if ( forward [ 1 ] == 0.0f && forward [ 0 ] == 0.0f )
	{
		angles [ 0 ] = ( forward [ 2 ] > 0.0f ) ? 270.0f : 90.0f;
		angles [ 1 ] = 0.0f;
	}
	else
	{
		angles [ 0 ] = atan2 ( -forward [ 2 ] , forward.Length2D ( ) ) * -180 / M_PI;
		angles [ 1 ] = atan2 ( forward [ 1 ] , forward [ 0 ] ) * 180 / M_PI;

		if ( angles [ 1 ] > 90 )
			angles [ 1 ] -= 180;
		else if ( angles [ 1 ] < 90 )
			angles [ 1 ] += 180;
		else if ( angles [ 1 ] == 90 )
			angles [ 1 ] = 0;
	}

	angles [ 2 ] = 0.0f;
}

void Math::AngleVectors ( const QAngle& angles , Vector &forward )
{
	float sp , sy , cp , cy;

	SinCos ( deg2rad ( angles.yaw ) , &sy , &cy );
	SinCos ( deg2rad ( angles.pitch ) , &sp , &cp );

	forward.x = cp * cy;
	forward.y = cp * sy;
	forward.z = -sp;
}
void Math::VectorAngles2 ( const Vector& forward , QAngle &angles )
{
	if ( forward [ 1 ] == 0.0f && forward [ 0 ] == 0.0f )
	{
		angles [ 0 ] = ( forward [ 2 ] > 0.0f ) ? 270.0f : 90.0f; // Pitch (up/down)
		angles [ 1 ] = 0.0f;  //yaw left/right
	}
	else
	{
		angles [ 0 ] = atan2 ( -forward [ 2 ] , forward.Length2D ( ) ) * -180 / 3.1415926;
		angles [ 1 ] = atan2 ( forward [ 1 ] , forward [ 0 ] ) * 180 / 3.1415926;

		if ( angles [ 1 ] > 90 ) angles [ 1 ] -= 180;
		else if ( angles [ 1 ] < 90 ) angles [ 1 ] += 180;
		else if ( angles [ 1 ] == 90 ) angles [ 1 ] = 0;
	}

	angles [ 2 ] = 0.0f;
}

QAngle Math::CalcAngle2 ( Vector src , Vector dst )
{
	QAngle angles;
	Vector delta = src - dst;
	VectorAngles ( delta , angles );
	delta.Normalize ( );
	return angles;
}

void Math::CalcAngle ( Vector src , Vector dst , Vector &angles )
{
	Vector delta = src - dst;
	double hyp = delta.Length2D ( );
	angles.y = ( atan ( delta.y / delta.x ) * 57.295779513082f );
	angles.x = ( atan ( delta.z / hyp ) * 57.295779513082f );
	angles [ 2 ] = 0.00;

	if ( delta.x >= 0.0 )
		angles.y += 180.0f;
}
void Math::Normalize ( Vector &vIn , Vector &vOut )
{
	float flLen = vIn.Length ( );
	if ( flLen == 0 ) {
		vOut.Init ( 0 , 0 , 1 );
		return;
	}
	flLen = 1 / flLen;
	vOut.Init ( vIn.x * flLen , vIn.y * flLen , vIn.z * flLen );
}
vec_t Math::VectorNormalize ( Vector& v )
{
	vec_t l = v.Length ( );

	if ( l != 0.0f )
	{
		v /= l;
	}
	else
	{
		v.x = v.y = 0.0f; v.z = 1.0f;
	}

	return l;
}
void Math::SinCos ( float radians , float* sine , float* cosine )
{
	*sine = sin ( radians );
	*cosine = cos ( radians );
}
double Math::deg2rad ( double deg )
{
	return deg * M_PI / 180.;
}
double Math::rad2deg ( double rad )
{
	double deg = 0;
	deg = rad * ( 180 / M_PI );
	return deg;
}