#include "Image.h"
#include <assert.h>

Image::Image(int width, int height, int depth)
	: _data(0)
	, _width(width)
	, _height(height)
	, _depth(depth)
{
	_size = width * height * depth;
	_data = new unsigned char[_size];
}

Image::~Image()
{
	if (_data)
	{
		delete[] _data;
	}
}

unsigned char& Image::operator[](int index)
{
	assert(index >= 0 && index < _size);
	return _data[index];
}