#pragma once
#include <cmath>
#include <cstddef>

typedef struct _rgb
{
	std::size_t r, g, b;
} RGB;

typedef struct _xyz
{
	double x, y, z;
} XYZ;

typedef struct _lab
{
	double l, a, b;
} LAB;

XYZ RGB2XYZ(RGB rgb)
{
	double var_R = static_cast<double>(rgb.r) / 255;
	double var_G = static_cast<double>(rgb.g) / 255;
	double var_B = static_cast<double>(rgb.b) / 255;

	if (var_R > 0.04045) var_R = std::pow(((var_R + 0.055) / 1.055), 2.4);
	else var_R = var_R / 12.92;

	if (var_G > 0.04045) var_G = std::pow(((var_G + 0.055) / 1.055), 2.4);
	else var_G = var_G / 12.92;

	if (var_B > 0.04045) var_B = std::pow(((var_B + 0.055) / 1.055), 2.4);
	else var_B = var_B / 12.92;

	var_R *= 100;
	var_G *= 100;
	var_B *= 100;

	double X = var_R * 0.4124 + var_G * 0.3576 + var_B * 0.1805;
	double Y = var_R * 0.2126 + var_G * 0.7152 + var_B * 0.0722;
	double Z = var_R * 0.0193 + var_G * 0.1192 + var_B * 0.9505;

	return XYZ{ X, Y, Z };
}

LAB XYZ2LAB(XYZ xyz)
{
	double var_X = xyz.x / 95.047;
	double var_Y = xyz.y / 100.000;
	double var_Z = xyz.z / 108.883;

	if (var_X > 0.008856) var_X = std::cbrt(var_X);
	else var_X = (7.787 * var_X) + (16 / 116);


	if (var_Y > 0.008856) var_Y = std::cbrt(var_Y);
	else var_Y = (7.787 * var_Y) + (16 / 116);

	if (var_Z > 0.008856) var_Z = std::cbrt(var_Z);
	else var_Z = (7.787 * var_Z) + (16 / 116);

	double L = (116 * var_Y) - 16;
	double a = 500 * (var_X - var_Y);
	double b = 200 * (var_Y - var_Z);

	return LAB{ L, a, b };
}

LAB RGB2LAB(RGB rgb)
{
	return XYZ2LAB(RGB2XYZ(rgb));
}

double LAB_DeltaE(LAB lab1, LAB lab2)
{
	return std::abs(lab1.l - lab2.l);
	/*return std::sqrt((lab1.l - lab2.l) * (lab1.l - lab2.l) +
					 (lab1.a - lab2.a) * (lab1.a - lab2.a) +
					 (lab1.b - lab2.b) * (lab1.b - lab2.b));*/
}