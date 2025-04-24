#pragma once

#include <qbytearray.h>
#include <qcolortransform.h>
#include <qimage.h>
#include <qlayout.h>
#include <qmatrix4x4.h>
#include <qopenglbuffer.h>
#include <qopenglshaderprogram.h>
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
	GraphicsWindow(QWidget* parent = nullptr); 
	~GraphicsWindow();

private:
	uchar map[SCREEN_RESOLUTION / 8] = { 0 };
	QImage colorMap;
	//QImage gameImage;
	int o = 0;
	int change = 85;
	int m_width;
	int m_height;
	QOpenGLTexture* m_texture = nullptr; 
	QOpenGLBuffer* buff;
	QOpenGLShader* vertex;
	QOpenGLShader* shader;
	QOpenGLShaderProgram* program;
	int vertexLocation;
	int matrixLocation;
	int colorLocation;
	GLuint tex;
	QMatrix4x4 m_proj;
	QRect rect;

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
	void initializeGL();
	void resizeGL(int w, int h) override;
	void paintGL() override;
};
