#pragma once

#include <qbytearray.h>
#include <qcolortransform.h>
#include <qopenglwidget.h>
#include <qpaintevent>
#include <qpainter.h>
#include <qpixmap.h>
#include <qtransform.h>
#include <qwidget.h>

#include "SDL3/SDL.h"

const int SCREEN_RESOLUTION = 57344; //(256x224)
const int FRAME_RATE = 60;


class GraphicsWindow : public QOpenGLWidget {
	Q_OBJECT
public:
	/**
	   Constructor sets defaults and starts timer

	   @param parent - the parent widget
	   @return void
	*/
	GraphicsWindow(QWidget* parent);

private:
	uchar map[SCREEN_RESOLUTION / 8] = { 0 };
	QPixmap colorMap;

protected:
	/**
	   Overrided paintEvent, draws map rotated 90 degrees anti-clockwise

	   @param event - the paintevent that was triggered
	   @return void
	*/
	void paintEvent(QPaintEvent* event) override;

	/**
	   Helper function for calling update() because direct slotting wasn't working TODO

	   @return void
	*/
	void test();
};

class myPainter : public QPainter {
public:
	myPainter(QWidget* parent);
};