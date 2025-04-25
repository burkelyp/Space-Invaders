
#include <qcolorspace.h>
#include <qdebug.h>
#include <qmatrix4x4.h>
#include <qmenu.h>
#include <qmenubar.h>
#include <qobject.h>
#include <qopenglfunctions.h>
#include <qtimer.h>
#include <qwidget.h>

#include "GraphicsWindow.h"

GraphicsWindow::GraphicsWindow(QWidget* parent) : QOpenGLWidget(parent)
{
	this->setAutoFillBackground(false);
	colorMap = QImage("ColorMap.PNG");
	
	// Testing BitMap
	uchar* m = map;
	
	for (int i = 0; i <= SCREEN_RESOLUTION/8; i++) {
		*m = 170;
		//qDebug() << *m;
		m++;
	}

	// Setting time to refresh 60 FPS
	QTimer* timer = new QTimer(this);
	//QObject::connect(timer, &QTimer::timeout, this, &GraphicsWindow::paintGL);
	QObject::connect(timer, &QTimer::timeout, this, &GraphicsWindow::test);
	timer->start(1000 / FRAME_RATE);
	

	// Load BitMap
	//gameImage = QImage(map, 256, 224, QImage::Format_Mono);

	// Set Color Table
	//gameImage.setColor(0, QColor(Qt::black).rgba());
	//gameImage.setColor(1, QColor(Qt::transparent).rgba());
}

GraphicsWindow::~GraphicsWindow()
{
	makeCurrent();
	delete m_texture;
}

void GraphicsWindow::initializeGL()
{
	QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();

	buff = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
	buff->create();
	buff->bind();

	float vertices[] = {
	-1.0f, -1.0f, 0.0f,
	 1.0f, -1.0f, 0.0f,
	-1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f, 0.0f
	}; 


	unsigned int indices[] = {  // note that we start from 0!
	0, 1, 3,   // first triangle
	1, 2, 3    // second triangle
	};

	buff->write(0, vertices, sizeof(vertices));

	program = new QOpenGLShaderProgram(this);
	program->addShaderFromSourceCode(QOpenGLShader::Vertex,
		"attribute highp vec4 vertex;\n"
		"uniform highp mat4 matrix;\n"
		"void main(void)\n"
		"{\n"
		"   gl_Position = matrix * vertex;\n"
		"}");
	program->addShaderFromSourceCode(QOpenGLShader::Fragment,
		"uniform mediump vec4 color;\n"
		"void main(void)\n"
		"{\n"
		"   gl_FragColor = color;\n"
		"}");
	program->link();
	program->bind();

	vertexLocation = program->attributeLocation("aPos");
	matrixLocation = program->uniformLocation("matrix");
	colorLocation = program->uniformLocation("color");

	QImage image = QImage("ColorMap.PNG");
	delete m_texture;
	m_texture = new QOpenGLTexture(image.flipped());

	float texCo[] = {
	0.0f, 0.0f,  // lower-left corner  
	1.0f, 0.0f,  // lower-right corner
	1.0f, 1.0f,  // top-right corner
	0.0f, 1.0f   // top-left corner
	};

	f->glGenTextures(1, &tex);
	f->glBindTexture(GL_TEXTURE_2D, tex);
	m_texture->bind(tex);


}

void GraphicsWindow::resizeGL(int w, int h)
{
	QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
	m_width = w;
	m_height = h;
	//f->glViewport(0, -13, 220, h);
}

void GraphicsWindow::paintEvent(QPaintEvent* event)
{
	//resizeGL(event->rect().width(), event->rect().height());
	rect = event->rect();
	paintGL();
	//((QWidget*)this->parent())->repaint();
	//QOpenGLWidget::paintEvent(event);
	
	// Simple proof of concept animation
	/*QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	QRect rect = event->rect();
	QSize rotateSize = QSize(rect.height(), rect.width());

	*(map + o) = change;
	o++;
	if (o > SCREEN_RESOLUTION / 8) {
		o = 0;
		if (change == 85) {
			change = 170;
		}
		else {
			change = 85;
		}
	}

	// Using bitmap as image mask 
	uchar* m = map;
	QImage image = colorMap.copy();
	for (int i = 0; i < 224; i++) {
		for (int j = 0; j < 256; j++) {
			if ((i * 224 + j) % 8 == 0) {
				m++;
			}
			if (!(*m & (1 << (7 - ((i * 224 + j) % 8))))) {
				image.setPixel(j, i, 0);
			}
		}
	}

	// Rotating image Anti-Clockwise and mirring it for OpenGl y coordinate system
	image = image.transformed(QTransform().rotate(90.0));
	image = image.mirrored(true);

	painter.drawImage(rect.topLeft(), image.scaled(rect.size()));*/
}

void GraphicsWindow::test()
{
	update();
}

void GraphicsWindow::paintGL()
{
	QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
	QOpenGLContext* context = QOpenGLContext::currentContext();

	// Setting OpenGl options
	f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	f->glEnable(GL_TEXTURE_2D);
	//f->glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_TRUE);

	// Simple proof of concept animation
	*(map + o) = change;
	o++;
	if (o > SCREEN_RESOLUTION / 8) {
		o = 0;
		if (change == 85) {
			change = 170;
		}
		else {
			change = 85;
		}
	}

	float texCo[] = {
	0.0f, 0.0f,  // lower-left corner  
	1.0f, 0.0f,  // lower-right corner
	1.0f, 1.0f,  // top-right corner
	0.0f, 1.0f   // top-left corner
	};

	static GLfloat const triangleVertices[] = {
	60.0f,  10.0f,  0.0f,
	110.0f, 110.0f, 0.0f,
	10.0f,  110.0f, 0.0f
	};

	QMatrix4x4 pmvMatrix;
	pmvMatrix.ortho(rect);
	QColor color(0, 255, 0, 255);

	program->enableAttributeArray(vertexLocation);
	program->setAttributeArray(vertexLocation, triangleVertices, 3);
	program->setUniformValue(matrixLocation, pmvMatrix);
	program->setUniformValue(colorLocation, color);

	f->glDrawArrays(GL_TRIANGLES, 0, 3);

	program->disableAttributeArray(vertexLocation);
	// Using bitmap as image mask 
	uchar* m = map;
	QImage image = colorMap.copy();
	for (int i = 0; i < 224; i++) {
		for (int j = 0; j < 256; j++) {
			if ((i * 224 + j) % 8 == 0) {
				m++;
			}
			if (!(*m & (1 << (7 - ((i * 224 + j) % 8))))) {
				image.setPixel(j, i, 0);
			}
		}
	}

	// Rotating image Anti-Clockwise and mirring it for OpenGl y coordinate system
	image = image.transformed(QTransform().rotate(90.0));
	image = image.mirrored(true);

	// Rendering compiled image
	f->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
	
	//f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	f->glFlush();
}

