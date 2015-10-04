#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QString>
#include <QImage>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _central_widget_layout = new QGridLayout(ui->centralWidget);
    _gl_window = new GLWindow(0);
    _central_widget_layout->addWidget(_gl_window);
    _gl_window->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	OpenCLUtils::Instance()->InitOpenCL();

	// Init the rest of the widgets depending on OpenCL
	_device_widget = new DeviceWidget();
	_central_widget_layout->addWidget(_device_widget, 0, 1);
}

MainWindow::~MainWindow()
{
    // Let the ui delete its children
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                "",
                                                tr("Images (*.png *.bmp *.jpg)"));
    if (!path.isEmpty())
    {
		_gl_window->loadImage(path);
    }
}
