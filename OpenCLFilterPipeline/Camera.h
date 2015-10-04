#pragma once

class Camera
{
public:
	Camera();
	~Camera();

	inline double CenterX() const { return _center_x; }
	inline double CenterY() const { return _center_y; }
	inline void SetCenterX(const double x) { _center_x = x; }
	inline void SetCenterY(const double y) { _center_y = y; }

	inline double Width() const { return _width; }
	inline double Height() const { return _height; }
	inline void SetWidth(const double width) { _width = width; }
	inline void SetHeight(const double height) { _height = height; }

	void Translate(double x, double y);

	inline double ZoomLevel() const { return _zoom_level; }
	void Zoom(const double factor);

	void Set(double center_x, double center_y, double width, double height, double zoom_level);

	double Left() const;
	double Right() const;
	double Top() const;
	double Bottom() const;

	void Fit(const double width, const double height, double center_x, double center_y);

private:
	double _center_x;
	double _center_y;
	double _width;
	double _height;
	double _zoom_level;
};
