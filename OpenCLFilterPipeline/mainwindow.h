#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "glwindow.h"
#include "OpenCLUtils.h"
#include "DeviceWidget.h"
#include "FilterWidget.h"
#include <QMainWindow>
#include <QLayout>
#include <QStatusBar>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();
	void on_filterFinished(qint64 elapsed);

private:
	void setStatusBarMessage(const QString &message);

private:
    Ui::MainWindow *ui;

    QHBoxLayout *_central_widget_layout;
	QVBoxLayout *_right_widget_layout;
    GLWindow *_gl_window;
	DeviceWidget *_device_widget;
	FilterWidget *_filter_widget;
};

#endif // MAINWINDOW_H
