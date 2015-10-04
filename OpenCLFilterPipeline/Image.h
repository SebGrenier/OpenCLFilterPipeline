#pragma once

class Image
{
public:
	Image(int width, int height, int depth);
	Image(const Image &other);
	~Image();

	Image& operator = (const Image &other);

	const unsigned char* Data() const { return _data; }
	unsigned char* Data() { return _data; }

	inline int Width() const { return _width; }
	inline int Height() const { return _height; }
	inline int Depth() const { return _depth; }
	inline int Size() const { return _size; }

	unsigned char& operator[] (int index);

private:
	void Copy(const Image &other);

private:
	unsigned char *_data;
	int _width;
	int _height;
	int _depth;
	int _size;
};
