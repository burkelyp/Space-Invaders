// Citation for the following code :
// Date : 04 / 21 / 2025
// Adapted from : Qt shader documentation
// Source URL : https://doc.qt.io/qt-6/qopenglshaderprogram.html#setUniformValue

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
	setupMemMap();
	//this->setAutoFillBackground(false);
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
	// Closing Memory Mapping
#ifdef Q_OS_WIN
	// Windows-specific code
	UnmapViewOfFile(memptr);

	CloseHandle(handle);
#elif defined(Q_OS_LINUX)
	// Linux-specific code
	shm_unlink(MAPPED_NAME);
	munmap(memptr, MEM_SIZE);
#elif defined(Q_OS_MAC)
	// macOS-specific code
	shm_unlink(MAPPED_NAME);
	munmap(memptr, MEM_SIZE);
#endif

	makeCurrent();
	delete m_texture;
}

void GraphicsWindow::initializeGL()
{
	QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
	f->initializeOpenGLFunctions();
	f->glEnable(GL_TEXTURE_2D);

	buff = new QOpenGLBuffer(QOpenGLBuffer::PixelPackBuffer);
	buff->create();
	buff->bind();

	vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
	vbo->create();
	vbo->bind();

	/**/GLfloat vertices[] = {
		 0.0f,			0.0f,			0.0f,
		 rect.width(),	0.0f,			0.0f,
		 rect.width(),	rect.height(),	0.0f,
		 0.0f,			rect.height(),	0.0f };
	/*GLfloat vertices[] = {
		 0.0f,			0.0f,			0.0f, 0.0f, 0.0f,
		 rect.width(),	0.0f,			0.0f, 1.0f, 0.0f,
		 rect.width(),	rect.height(),	0.0f, 1.0f, 1.0f,
		 0.0f,			rect.height(),	0.0f, 0.0f, 1.0f };*/

	float texCo[] = {
	0.0f, 0.0f,  // lower-left corner  
	1.0f, 0.0f,  // lower-right corner
	1.0f, 1.0f,  // top-right corner
	0.0f, 1.0f   // top-left corner
	};

	vbo->allocate(vertices, sizeof(vertices));

	f->glEnableVertexAttribArray(1);
	//f->glEnableVertexAttribArray(1);
	f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	//f->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));

	qDebug() << glGetError();
	program = new QOpenGLShaderProgram(this);
	program->addShaderFromSourceCode(QOpenGLShader::Vertex,
		"#version 150 core\n"
		"attribute highp vec4 vertex;\n"
		"uniform highp mat4 matrix;\n"
        "in vec2 vertTexCoord;\n"
        "out vec2 texCoord;\n"
		"void main(void)\n"
		"{\n"
		"   gl_Position = matrix * vertex;\n"
		"	texCoord = vertTexCoord; \n"
		"}");
	program->addShaderFromSourceCode(QOpenGLShader::Fragment,
		"#version 150 core\n"
        "uniform sampler2D tex;\n"
        "in vec2 texCoord;\n"
        "void main(void)\n"
        "{\n"
        "    gl_FragColor = texture2D(tex,texCoord);\n"
        "}\n");

	program->bindAttributeLocation("vertex", 1);
	//program->bindAttributeLocation("vertTexCoord", 1);

	qDebug() << glGetError();
	program->link();
	program->bind();



	vertexLocation = program->attributeLocation("vertex");
	matrixLocation = program->uniformLocation("matrix");
	texCoords = program->attributeLocation("vertTexCoord");

	//program->enableAttributeArray(0);
	//program->setAttributeArray(vertexLocation, vertices, 3);

	QImage image = QImage("ColorMap.PNG");
	delete m_texture;
	m_texture = new QOpenGLTexture(image.flipped(), QOpenGLTexture::DontGenerateMipMaps);
	m_texture->setWrapMode(QOpenGLTexture::ClampToEdge);
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
	//paintGL();
	//((QWidget*)this->parent())->repaint();
	//QOpenGLWidget::paintEvent(event);
	
	// Simple proof of concept animation
	QPainter painter(this);
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

	painter.drawImage(rect.topLeft(), image.scaled(rect.size()));
	//QOpenGLWidget::paintEvent(event);
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

	m_texture->bind(0);
	program->bind();
	//vbo->bind();
	f->glActiveTexture(GL_TEXTURE0);
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

	float vertices[] = {
		 0.0f,			0.0f,			0.0f,
		 rect.width(),	0.0f,			0.0f,
		 rect.width(),	rect.height(),	0.0f,
		 0.0f,			rect.height(),	0.0f };

	float texCo[] = {
	0.0f, 0.0f,  // lower-left corner  
	1.0f, 0.0f,  // lower-right corner
	1.0f, 1.0f,  // top-right corner
	0.0f, 1.0f   // top-left corner
	};

	QMatrix4x4 pmvMatrix;
	pmvMatrix.ortho(rect);
	QColor color(0, 255, 0, 255);

	//program->enableAttributeArray(0); 
	//program->enableAttributeArray(vertexLocation);
	//program->setAttributeArray(vertexLocation, vertices, 3);
	program->enableAttributeArray(texCoords);
	program->setAttributeArray(texCoords, texCo, 2);
	program->setUniformValue(matrixLocation, pmvMatrix);
	//f->glEnableVertexAttribArray(0);
	f->glEnableVertexAttribArray(vertexLocation);

	//f->glActiveTexture(GL_TEXTURE0);
	program->setUniformValue("tex", 0);
	f->glDrawArrays(GL_QUADS, 0, 4);

	//qDebug() << f->glGetError();
	//program->disableAttributeArray(0);
	//program->disableAttributeArray(vertexLocation);
	//f->glDisableVertexAttribArray(0);
	f->glDisableVertexAttribArray(vertexLocation);
	program->disableAttributeArray(texCoords);

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

	//buff->write(0, image.bits(), image.sizeInBytes());

	//f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//f->glFlush();

	program->release();
	//qDebug() << f->glGetError();
}

uchar* GraphicsWindow::setupMemMap()
{
	#ifdef Q_OS_WIN
		// Windows-specific code

	// Create Mapping
	handle = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, MEM_SIZE, (LPCWSTR)MAPPED_NAME);

	if (handle == NULL) {
		qDebug() << "Error mapping memory";
		return nullptr;
	}

	// Get Map location
	memptr = (uchar*)MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, 0, 0, MEM_SIZE);

	if (memptr == NULL) {
		qDebug() << "Error accessing memory";
		CloseHandle(handle);
		return nullptr;
	}

	// Initiallizing memory
	memset(memptr, 0, MEM_SIZE);
	map = memptr + 0x2400;

	#elif defined(Q_OS_LINUX)
		// Linux-specific code

	// Create Mapping
	int handle;
	handle = shm_open(MAPPED_NAME, O_RDWR | O_CREAT, NULL);

	if (handle == -1) {
		qDebug() << "Error mapping memory";
		return nullptr;
	}

	ftruncate(handle, MEM_SIZE);

	// Get Map location
	memptr = mmap(NULL, MEM_SIZE, PROT_NONE, MAP_SHARED, handle, 0);
	if (memptr == -1) {
		qDebug() << "Error accessing memory";
		return nullptr;
	}
	close(handle);

	#elif defined(Q_OS_MAC)
		// macOS-specific code

	// Create Mapping
	int handle;
	handle = shm_open(MAPPED_NAME, O_RDWR | O_CREAT, NULL);

	if (handle == -1) {
		qDebug() << "Error mapping memory";
		return nullptr;
	}

	ftruncate(handle, MEM_SIZE);

	// Get Map location
	memptr = mmap(NULL, MEM_SIZE, PROT_NONE, MAP_SHARED, handle, 0);
	if (memptr == -1) {
		qDebug() << "Error accessing memory";
		return nullptr;
	}
	close(handle);

	#endif

	return memptr;
}

