#include "glwindow.h"
#include "FiltersHolder.h"
#include <QImage>

GLWindow::GLWindow(QWidget *parent)
    : QOpenGLWidget(parent)
    , QOpenGLFunctions_2_1()
    , _current_window_width(1.0)
    , _current_window_height(1.0)
    , _image(0)
	, _is_dragging(false)
{
	setMouseTracking(true);
	setAutoFillBackground(true);
	setUpdatesEnabled(true);
	setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);
	_camera.Set(0, 0, 1, 1, 1);

	connect(&_refresh_timer, SIGNAL(timeout()), this, SLOT(update()));
	_refresh_timer.start(10);
}

GLWindow::~GLWindow()
{
	_refresh_timer.stop();

    if (_image)
    {
		delete _image;
    }

	if (_result)
	{
		delete _result;
	}
}

void GLWindow::loadImage(const QString &path)
{
    QImage image(path);
    QImage image_8bit = image.convertToFormat(QImage::Format_RGBA8888);

    _image_access_mutex.lock();

    if (_image)
    {
        delete _image;
    }

	_image = new Image(image_8bit.width(), image_8bit.height(), 4);

	// Adjust camera to the new image
	_camera.Fit((double)_image->Width(), (double)_image->Height(), (double)_image->Width() / 2.0, (double)_image->Height() / 2.0);

    const unsigned char *bits = image_8bit.bits();
	const int size = _image->Size();
	for (int i = 0; i < size; ++i)
    {
		(*_image)[i] = bits[i];
    }

	_result = new Image(*_image);

    _image_access_mutex.unlock();
}

void GLWindow::initializeGL()
{
    // Set up the rendering context, load shaders and other resources, etc.:
    initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(0.0);
    glEnable(GL_TEXTURE_2D);

    glGenTextures(1, &_texture_id);
    glBindTexture(GL_TEXTURE_2D, _texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void GLWindow::resizeGL(int w, int h)
{
    // Update projection matrix and other size related settings:
    glViewport(0, 0, w, h);

    double width_increase = (double)w / (double)_current_window_width;
    _current_window_width = w;
    double height_increase = (double)h / (double)_current_window_height;
    _current_window_height = h;

    double new_camera_width = _camera.Width() * width_increase;
    double new_camera_height = _camera.Height() * height_increase;

	_camera.SetWidth(new_camera_width);
	_camera.SetHeight(new_camera_height);
}

void GLWindow::paintGL()
{
    // Draw the scene:
    glClear(GL_COLOR_BUFFER_BIT | GL_TEXTURE_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glPushAttrib(GL_CURRENT_BIT | GL_TEXTURE_BIT);
    glPushMatrix();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(_camera.Left(), _camera.Right(), _camera.Bottom(), _camera.Top(), -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);

    // Really not performant stuff!!
    // Only for demonstration purpose.
    // Use a texture instead and send it to opencl
    if (_image_access_mutex.tryLock())
    {
        if (_image)
        {
			auto filter = FiltersHolder::Instance()->CurrentFilter();
			filter->Apply(*_image, *_result);

            // Create a texture from the image data
            glBindTexture(GL_TEXTURE_2D, _texture_id);
            glActiveTexture(0);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _result->Width(), _result->Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, _result->Data());

            glBegin(GL_QUADS);

			glTexCoord2d(0, 1);
            glVertex2d(0, 0);
			glTexCoord2d(1, 1);
			glVertex2d((double)_result->Width(), 0);
			glTexCoord2d(1, 0);
			glVertex2d((double)_result->Width(), (double)_result->Height());
			glTexCoord2d(0, 0);
			glVertex2d(0, (double)_result->Height());

            glEnd();
        }

        _image_access_mutex.unlock();
    }

    glPopMatrix();
    glPopAttrib();

	glFlush();
}

void GLWindow::mousePressEvent(QMouseEvent* Event)
{
	if (Event->button() == Qt::MidButton)
	{
		_is_dragging = true;
	}

	_last_x = Event->x();
	_last_y = Event->y();

	Event->accept();
}

void GLWindow::mouseReleaseEvent(QMouseEvent* Event)
{
	if (Event->button() == Qt::MidButton)
	{
		_is_dragging = false;
	}

	Event->accept();
}

void GLWindow::mouseMoveEvent(QMouseEvent* Event)
{
	int x, y;
	x = Event->x();
	y = Event->y();

	if (_is_dragging)
	{
		const double offset_x = double(_last_x - x) * _camera.ZoomLevel();
		const double offset_y = double(y - _last_y) * _camera.ZoomLevel();
		_camera.Translate(offset_x, offset_y);
	}
	_last_x = x;
	_last_y = y;

	Event->accept();

	update();
}

void GLWindow::wheelEvent(QWheelEvent* Event)
{
	if (Event->delta() > 0)
	{
		_camera.Zoom(0.9);
	}
	else
	{
		_camera.Zoom(1.0 / 0.9);
	}

	Event->accept();

	update();
}
