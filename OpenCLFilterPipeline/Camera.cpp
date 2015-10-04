#include "Camera.h"

Camera::Camera()
	: _center_x(0)
	, _center_y(0)
	, _width(1)
	, _height(1)
	, _zoom_level(1)
{ }

Camera::~Camera()
{ }

void Camera::Translate(double x, double y)
{
	_center_x += x;
	_center_y += y;
}

void Camera::Zoom(const double factor)
{
	_width = _width * factor;
	_height = _height * factor;
	_zoom_level *= factor;
}

void Camera::Set(double center_x, double center_y, double width, double height, double zoom_level)
{
	_center_x = center_x;
	_center_y = center_y;
	_width = width;
	_height = height;
	_zoom_level = zoom_level;
}

double Camera::Left() const
{
	return _center_x - _width / 2.0;
}

double Camera::Right() const
{
	return _center_x + _width / 2.0;
}

double Camera::Top() const
{
	return _center_y + _height / 2.0;
}

double Camera::Bottom() const
{
	return _center_y - _height / 2.0;
}

void Camera::Fit(const double width, const double height, double center_x, double center_y)
{
	_center_x = center_x;
	_center_y = center_y;
	_zoom_level = 1.0;

	double aspect_ratio = _width / _height;

	if (height > width)
	{
		_height = height;
		_width = _height * aspect_ratio;
	}
	else
	{
		_width = width;
		_height = _width / aspect_ratio;
	}
}
