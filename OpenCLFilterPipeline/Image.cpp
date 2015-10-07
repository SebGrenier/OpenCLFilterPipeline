#include "Image.h"
#include <assert.h>
#include <scoped_allocator>

Image::Image(int width, int height, int depth)
	: _data(0)
	, _width(width)
	, _height(height)
	, _depth(depth)
	, _size(0)
{
	_size = width * height * depth;
	_data = new unsigned char[_size];
	memset(_data, 0, _size);
}

Image::Image(const Image& other)
	: _data(0)
	, _width(0)
	, _height(0)
	, _depth(0)
	, _size(0)
{
	Copy(other);
}

Image::~Image()
{
	if (_data)
	{
		delete[] _data;
	}
}

Image& Image::operator=(const Image& other)
{
	Copy(other);
	return *this;
}

unsigned char& Image::operator[](int index)
{
	assert(index >= 0 && index < _size);
	return _data[index];
}

unsigned char& Image::operator()(int i, int j, int k)
{
	return _data[(i * _width + j) * _depth + k];
}

const unsigned char& Image::operator()(int i, int j, int k) const
{
	return _data[(i * _width + j) * _depth + k];
}

void Image::Copy(const Image& other)
{
	if (_data)
	{
		delete[] _data;
	}

	_width = other.Width();
	_height = other.Height();
	_depth = other.Depth();
	_size = other.Size();
	_data = new unsigned char[_size];
	std::copy(other.Data(), other.Data() + _size, _data);
}