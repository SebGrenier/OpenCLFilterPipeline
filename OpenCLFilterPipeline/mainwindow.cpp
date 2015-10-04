#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "FiltersHolder.h"

#include <QFileDialog>
#include <QString>
#include <QImage>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _central_widget_layout = new QHBoxLayout(ui->centralWidget);

    _gl_window = new GLWindow(0);
    _central_widget_layout->addWidget(_gl_window);
    _gl_window->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	_right_widget_layout = new QVBoxLayout();
	_central_widget_layout->addLayout(_right_widget_layout);

	OpenCLUtils::Instance()->InitOpenCL();
	FiltersHolder::Instance()->InitFilters();

	// Init the rest of the widgets depending on OpenCL
	_device_widget = new DeviceWidget();
	_right_widget_layout->addWidget(_device_widget);
	_filter_widget = new FilterWidget();
	_right_widget_layout->addWidget(_filter_widget);
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
