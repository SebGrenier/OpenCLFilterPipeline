#ifndef GLWINDOW_H
#define GLWINDOW_H

#include "Camera.h"
#include "Image.h"
#include <QOpenGLWidget>
#include <QOpenGLFunctions_2_1>
#include <QMutex>
#include <QMouseEvent>
#include <QTimer>

class GLWindow : public QOpenGLWidget, public QOpenGLFunctions_2_1
{
    Q_OBJECT

public:
    GLWindow(QWidget *parent = 0);
    ~GLWindow();

    void loadImage(const QString &path);

protected:
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int w, int h);

	virtual void mousePressEvent(QMouseEvent *Event);
	virtual void mouseReleaseEvent(QMouseEvent *Event);
	virtual void mouseMoveEvent(QMouseEvent *Event);
	virtual void wheelEvent(QWheelEvent *Event);

private:
    int _current_window_width;
    int _current_window_height;
	bool _is_dragging;
	int _last_x;
	int _last_y;
	QTimer _refresh_timer;

	Camera _camera;

	Image *_image;
	Image *_result;
    QMutex _image_access_mutex;
    GLuint _texture_id;
};

#endif // GLWINDOW_H
