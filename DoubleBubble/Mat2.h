#pragma once
#include <math.h>
#include "Point.h"
class Mat2
{
public:
	Mat2(double x0y0 = 1, double x0y1 = 0, double x1y0 = 0, double x1y1 = 1)
	{
		data[0][0] = x0y0;
		data[0][1] = x0y1;
		data[1][0] = x1y0;
		data[1][1] = x1y1;
	}
	double data[2][2];
};
inline Mat2 loadIdentity()
{
	return Mat2(1, 0, 0, 1);
}

Mat2 operator * (Mat2 A, Mat2 B)
{// A*B
	return Mat2(A.data[0][0] * B.data[0][0] + A.data[0][1] * B.data[1][0],
		A.data[0][0] * B.data[0][1] + A.data[0][1] * B.data[1][1],
		A.data[1][0] * B.data[0][0] + A.data[1][1] * B.data[1][0],
		A.data[1][0] * B.data[0][1] + A.data[1][1] * B.data[1][1]);
}
inline Mat2 matrot(double theta)
{
	return Mat2(cos(theta), -sin(theta),
		sin(theta), cos(theta));
}
inline Mat2 matscale(double amt_x, double amt_y)
{
	return Mat2(amt_x, 0, 0, amt_y);
}
inline Point vecmatmult(Mat2 A, Point B)
{ // A[] * B[]
	return Point((A.data[0][0])*B.x + (A.data[0][1])*B.y, (A.data[1][0])*B.x + (A.data[1][1])*B.y);
}