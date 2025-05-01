#pragma once

#ifdef Q_OS_WIN
	#include <windows.h>
// Windows-specific code
#elif defined(Q_OS_LINUX)
	#include <sys/shm.h>
// Linux-specific code
#elif defined(Q_OS_MAC)
	#include <sys/shm.h>
// macOS-specific code
#endif

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

const int MEM_SIZE = 0x10003;
const int SCREEN_RESOLUTION = 57344; //(256x224)
const int FRAME_RATE = 60;
const char MAPPED_NAME[] = "/SpaceInvaders";

/**
 * Centralized openGL visuals renderer
 *
 * This class is the central widget that manages the space invaders 
 * inter-process communication and reads and displays space invaders 
 * video memory.
 */
class GraphicsWindow : public QOpenGLWidget {
	Q_OBJECT
public:
	/**
	   Constructor sets defaults and starts timer

	   @param parent - the parent widget
	   @return void
	*/
	GraphicsWindow(QWidget* parent = nullptr);

	/**
	   Deconstructor releases memory

	   @return void
	*/
	~GraphicsWindow();

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

	/**
	   Overrided initializeGL, Initializes openGL shaders and buffers

	   @return void
	*/
	void initializeGL() override;

	/**
	   Overrided resizeGL, Resizes openGL context

	   @param w - the width to resize to
	   @param h - the height to resize to
	   @return void
	*/
	void resizeGL(int w, int h) override;

	/**
	   Overrided paintGL, renders openGL graphics onto the widget

	   @return void
	*/
	void paintGL() override;

	/**
	   Initializes interprocess communication structure by setting up memory maps on a by-system basis

	   @return uchar* memory location that was allocated and shared if successful else nullptr
	*/
	uchar* setupMemMap();

private:
	// Memory variables
	uchar mem[MEM_SIZE] = { 0 };
	uchar* map = mem + 0x2400;
	uchar* memptr;
	void* handle;
	int o = 0;
	int change = 85;

	// OpenGL variables
	QImage colorMap;
	//QImage gameImage;
	int m_width;
	int m_height;
	QOpenGLTexture* m_texture = nullptr; 
	QOpenGLBuffer* buff;
	QOpenGLBuffer* vbo;
	QOpenGLShader* vertex;
	QOpenGLShader* shader;
	QOpenGLShaderProgram* program;
	int vertexLocation;
	int matrixLocation;
	int colorLocation;
	int texCoords;
	GLuint tex;
	QMatrix4x4 m_proj;
	QRect rect;
};
