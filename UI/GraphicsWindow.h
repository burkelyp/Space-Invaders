#pragma once

#include <qbytearray.h>
#include <qcolortransform.h>
#include <qopenglbuffer.h>
#include <qopenglshaderprogram.h>
#include <qopenglcontext.h>
#include <qopengltexture.h>
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
	~GraphicsWindow();

private:
	uchar map[SCREEN_RESOLUTION / 8] = { 0 };
	QImage colorMap;
	int o = 0;
	int change = 85;
	QRect rect;
	GLuint tex;
	QOpenGLBuffer* buff;
	QOpenGLShaderProgram* program;
	int vertexLocation;
	int matrixLocation;
	int colorLocation;
	QOpenGLTexture* m_texture = nullptr;

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
	void paintGL() override;
	void resizeGL(int w, int h) override;
	void initializeGL() override;

};

class myPainter : public QPainter {
public:
	myPainter(QWidget* parent);
};