
#include <qcolorspace.h>
#include <qdebug.h>
#include <qobject.h>
#include <qtimer.h>
#include <qwidget.h>

#include "GraphicsWindow.h"

GraphicsWindow::GraphicsWindow(QWidget* parent) : QOpenGLWidget(parent)
{
	this->setAutoFillBackground(false);
	colorMap = QPixmap("ColorMap.PNG");
	// Testing BitMap
	uchar* m = map;
	
	for (int i = 0; i < SCREEN_RESOLUTION/8; i++) {
		*m = 170;
		//qDebug() << *m;
		m++;
	}

	// Setting time to refresh 60 FPS
	QTimer* timer = new QTimer(this);
	QObject::connect(timer, &QTimer::timeout, this, &GraphicsWindow::test);
	timer->start(1000 / FRAME_RATE);
}

void GraphicsWindow::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	QRect rect = event->rect();
	QSize rotateSize = QSize(rect.height(), rect.width());

	// Load BitMap
	QImage gameImage = QImage(map, 256, 224, QImage::Format_Mono);

	// Set Color Table
	gameImage.setColor(0, QColor(Qt::black).rgba());
	gameImage.setColor(1, QColor(Qt::transparent).rgba());

	// Rotate 90 degrees Anti-Clockwise
	painter.translate(0, rect.height());
	painter.rotate(-90);

	// Draw BitMap and Coloring
	painter.drawPixmap(rect.topLeft(), colorMap.scaled(rotateSize));
	painter.drawImage(rect.topLeft(), gameImage.scaled(rotateSize));
	painter.end();
}

void GraphicsWindow::test()
{
	update();
}

myPainter::myPainter(QWidget* parent) : QPainter(parent)
{

}
