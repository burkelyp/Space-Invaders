// Citation for the following code :
// Date : 04 / 21 / 2025
// Adapted from : Qt shader documentation
// Source URL : https://doc.qt.io/qt-6/qopenglshaderprogram.html#setUniformValue

#define _POSIX_C_SOURCE 200112L

#include <qcolorspace.h>
#include <qdebug.h>
#include <qmatrix4x4.h>
#include <qmenu.h>
#include <qmenubar.h>
#include <qmetaobject.h>
#include <qobject.h>
#include <qopenglfunctions.h>
#include <qtimer.h>
#include <qwidget.h>

#include <sys/mman.h>
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h> /* For O_* constants */
#include <unistd.h>
#include <sys/types.h>

#include "GraphicsWindow.h"

GraphicsWindow::GraphicsWindow(QWidget* parent) : QOpenGLWidget(parent)
{
	setupMemMap();
	this->setAutoFillBackground(false);
	this->setFocusPolicy(Qt::StrongFocus);
	colorMap = QImage("ColorMap.PNG");
	
	// Testing BitMap
	/*uchar* m = map;
	
	for (int i = 0; i <= SCREEN_RESOLUTION/8; i++) {
		*m = 170;
		//qDebug() << *m;
		m++;
	}*/

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

void GraphicsWindow::updateKeyBind(const QString bind, const QString hotkey)
{
	keyBinds.setCombination(bind, hotkey);
}

void GraphicsWindow::initializeGL()
{
	/*QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
	f->initializeOpenGLFunctions();
	f->glEnable(GL_TEXTURE_2D);

	//buff = new QOpenGLBuffer(QOpenGLBuffer::PixelPackBuffer);
	//buff->create();
	//buff->bind();

	//vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
	vbo.create();
	vbo.bind();
	vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);

	GLfloat vertices[] = {
		 0.0f,			0.0f,			0.0f,
		 rect.width(),	0.0f,			0.0f,
		 rect.width(),	rect.height(),	0.0f,
		 0.0f,			rect.height(),	0.0f };
	GLfloat vertices[] = {
		 0.0f,			0.0f,			0.0f, 0.0f, 0.0f,
		 rect.width(),	0.0f,			0.0f, 1.0f, 0.0f,
		 rect.width(),	rect.height(),	0.0f, 1.0f, 1.0f,
		 0.0f,			rect.height(),	0.0f, 0.0f, 1.0f };

	float texCo[] = {
	0.0f, 0.0f,  // lower-left corner  
	1.0f, 0.0f,  // lower-right corner
	1.0f, 1.0f,  // top-right corner
	0.0f, 1.0f   // top-left corner
	};

	vbo.allocate(vertices, sizeof(vertices));
	//qDebug() << sizeof(GLfloat);
	//f->glEnableVertexAttribArray(1);
	//f->glEnableVertexAttribArray(1);
	//f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	//f->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));

	vbo.release();

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

	//program->bindAttributeLocation("vertex", 1);
	//program->bindAttributeLocation("vertTexCoord", 1);

	qDebug() << glGetError();
	program->link();
	program->bind();

	vertexLocation = program->attributeLocation("vertex");
	matrixLocation = program->uniformLocation("matrix");
	texCoords = program->attributeLocation("vertTexCoord");

	//vao = new QOpenGLVertexArrayObject();
	if (vao.create()) {
		qDebug() << "No VAO error";
	}
	vao.bind();

	// Bind the vertex buffer to the VAO
	//program->enableAttributeArray(vertexLocation);
	f->glEnableVertexAttribArray(vertexLocation);
	qDebug() << glGetError();
	vbo.bind();
	f->glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	qDebug() << glGetError();
	//program->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, 0);
	vao.release();

	qDebug() << glGetError();
	//program->disableAttributeArray(0);
	//program->setAttributeArray(vertexLocation, vertices, 3);

	QImage image = QImage("ColorMap.PNG");
	delete m_texture;
	m_texture = new QOpenGLTexture(image.flipped(), QOpenGLTexture::DontGenerateMipMaps);
	m_texture->setWrapMode(QOpenGLTexture::ClampToEdge);

	vbo.release();
	program->release(); */
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

	// Using bitmap as image mask 
	uchar* m = map;
	QImage image = colorMap.copy();

	// Simple proof of concept animation
	/**(map + o) = change;
	o++;
	if (o > SCREEN_RESOLUTION / 8) {
		o = 0;
		if (change == 85) {
			change = 170;
		}
		else {
			change = 85;
		}
	}*/

	for (int i = 0; i < 224; i++) {
		for (int j = 0; j < 256; j++) {
			if ((i * 224 + j) % 8 == 0) {
				m++;
			}
			if (!(*m & (1 << (7 - ((i * 224 + j) % 8))))) {
				// Iterates over every row painting each byte backwards
				image.setPixelColor(QPoint(((j - (j % 8)) + (7 - (j % 8))), i), QColor(Qt::black));
			}
		}
	}

	// Rotating image Anti-Clockwise and mirring it for OpenGl y coordinate system
	image = image.transformed(QTransform().rotate(90.0));
	image = image.mirrored(true);

	painter.drawImage(rect.topLeft(), image.scaled(rect.size()));
	this->resize(rect.size());
	//QOpenGLWidget::paintEvent(event);
	//this->repaint();
}

void GraphicsWindow::test()
{
	update();
}

void GraphicsWindow::paintGL()
{
	return;
	QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
	QOpenGLContext* context = QOpenGLContext::currentContext();

	// Setting OpenGl options
	f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	f->glEnable(GL_TEXTURE_2D);

	program->bind();
	m_texture->bind(0);
	vao.bind();
	vbo.bind();
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
	//f->glEnableVertexAttribArray(vertexLocation);
	//program->enableAttributeArray(vertexLocation);
	//program->setAttributeArray(vertexLocation, vertices, 3);
	program->enableAttributeArray(texCoords);
	program->setAttributeArray(texCoords, texCo, 2);
	program->setUniformValue(matrixLocation, pmvMatrix);
	//f->glEnableVertexAttribArray(0);

	//f->glActiveTexture(GL_TEXTURE0);
	program->setUniformValue("tex", 0);
	qDebug() << f->glGetError();
	f->glDrawArrays(GL_QUADS, 0, 4);

	//qDebug() << f->glGetError();
	//program->disableAttributeArray(0);
	//program->disableAttributeArray(vertexLocation);
	//f->glDisableVertexAttribArray(0);
	//f->glDisableVertexAttribArray(vertexLocation);
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
				image.setPixelColor(QPoint(((j - (j % 8)) + (7 - (j % 8))), i), QColor(Qt::black));
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
	vbo.release();
	vao.release();
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
	ports = memptr + 0x10000;

    #elif defined(Q_OS_LINUX)
		// Linux-specific code
        
        // Create Mapping
        int handle = shm_open(MAPPED_NAME, O_RDWR | O_CREAT, 0600); // Permissions must be numeric, not NULL
        if (handle == -1) {
            qDebug() << "Error mapping memory";
            return nullptr;
        }

        // Set the size of the shared memory
        if (ftruncate(handle, MEM_SIZE) == -1) {
            qDebug() << "Error setting size of shared memory";
            ::close(handle);
            return nullptr;
        }

        // Get Map location
        memptr = static_cast<uchar*>(mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, handle, 0));
        if (memptr == MAP_FAILED) {
            qDebug() << "Error accessing memory";
            ::close(handle);
            return nullptr;
        }

        ::close(handle);

        // Set up specific regions
        map = memptr + 0x2400;
        ports = memptr + 0x10000;

	// // Create Mapping
	// int handle;
	// handle = shm_open(MAPPED_NAME, O_RDWR | O_CREAT, NULL);

	// if (handle == -1) {
	// 	qDebug() << "Error mapping memory";
	// 	return nullptr;
	// }

	// ftruncate(handle, MEM_SIZE);

	// // Get Map location
	// memptr = mmap(NULL, MEM_SIZE, PROT_NONE, MAP_SHARED, handle, 0);
	// if (memptr == -1) {
	// 	qDebug() << "Error accessing memory";
	// 	return nullptr;
	// }
	// close(handle);

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

void GraphicsWindow::keyPressEvent(QKeyEvent* event)
{
	if (event->isAutoRepeat()) {
		return;
	}

	// Testing to find the correctly bound key
	if (keyBinds.getCombination("p1Left")->matches(event->keyCombination()) > QKeySequence::NoMatch) { // If match or partial match
		// Set proper bit in ports
		editMemInputBit(SIPortLocations::P1Left, true);
	}
	if (keyBinds.getCombination("p1Right")->matches(event->keyCombination()) > QKeySequence::NoMatch) {
		editMemInputBit(SIPortLocations::P1Right, true);
	}
	if (keyBinds.getCombination("p1Shoot")->matches(event->keyCombination()) > QKeySequence::NoMatch) {
		editMemInputBit(SIPortLocations::P1Shoot, true);
	}
	if (keyBinds.getCombination("p1Start")->matches(event->keyCombination()) > QKeySequence::NoMatch) {
		editMemInputBit(SIPortLocations::P1Start, true);
	}
	if (keyBinds.getCombination("p2Left")->matches(event->keyCombination()) > QKeySequence::NoMatch) {
		editMemInputBit(SIPortLocations::P2Left, true);
	}
	if (keyBinds.getCombination("p2Right")->matches(event->keyCombination()) > QKeySequence::NoMatch) {
		editMemInputBit(SIPortLocations::P2Right, true);
	}
	if (keyBinds.getCombination("p2Shoot")->matches(event->keyCombination()) > QKeySequence::NoMatch) {
		editMemInputBit(SIPortLocations::P2Shoot, true);
	}
	if (keyBinds.getCombination("p2Start")->matches(event->keyCombination()) > QKeySequence::NoMatch) {
		editMemInputBit(SIPortLocations::P2Start, true);
	}
	if (keyBinds.getCombination("coin")->matches(event->keyCombination()) > QKeySequence::NoMatch) {
		editMemInputBit(SIPortLocations::Insert_Coin, true);
	}
	QOpenGLWidget::keyPressEvent(event);
}

void GraphicsWindow::keyReleaseEvent(QKeyEvent* event)
{
	if (event->isAutoRepeat()) {
		return;
	}

	// Testing to find the correctly bound key
	if (keyBinds.getCombination("p1Left")->matches(event->keyCombination()) > QKeySequence::NoMatch) { // If match or partial match
		// Set proper bit in ports
		editMemInputBit(SIPortLocations::P1Left, false);
	}
	if (keyBinds.getCombination("p1Right")->matches(event->keyCombination()) > QKeySequence::NoMatch) {
		editMemInputBit(SIPortLocations::P1Right, false);
	}
	if (keyBinds.getCombination("p1Shoot")->matches(event->keyCombination()) > QKeySequence::NoMatch) {
		editMemInputBit(SIPortLocations::P1Shoot, false);
	}
	if (keyBinds.getCombination("p1Start")->matches(event->keyCombination()) > QKeySequence::NoMatch) {
		editMemInputBit(SIPortLocations::P1Start, false);
	}
	if (keyBinds.getCombination("p2Left")->matches(event->keyCombination()) > QKeySequence::NoMatch) {
		editMemInputBit(SIPortLocations::P2Left, false);
	}
	if (keyBinds.getCombination("p2Right")->matches(event->keyCombination()) > QKeySequence::NoMatch) {
		editMemInputBit(SIPortLocations::P2Right, false);
	}
	if (keyBinds.getCombination("p2Shoot")->matches(event->keyCombination()) > QKeySequence::NoMatch) {
		editMemInputBit(SIPortLocations::P2Shoot, false);
	}
	if (keyBinds.getCombination("p2Start")->matches(event->keyCombination()) > QKeySequence::NoMatch) {
		editMemInputBit(SIPortLocations::P2Start, false);
	}
	if (keyBinds.getCombination("coin")->matches(event->keyCombination()) > QKeySequence::NoMatch) {
		editMemInputBit(SIPortLocations::Insert_Coin, false);
	}
	QOpenGLWidget::keyReleaseEvent(event);
}

void GraphicsWindow::editMemInputBit(int bit, bool set)
{
	uchar* port = ports + bit / 8;
	if (set) {
		// Create bitmask to set input bit
		int mask = 1 << (bit % 8);
		*port |= mask;
	} else {
		// Create bitmask to unset input bit
		int mask = ~(1 << (bit % 8));
		*port &= mask;
	}
}

SICombinations::SICombinations(QWidget* parent) :
	QObject(parent)
{
	// Creating or using settings to keep track of user selected settings
	QSettings settings("settings.ini", QSettings::IniFormat); // Using universal INI for cross platform use
	settings.beginGroup("settings");
	QString keybinds = settings.value("keybinds").toString(); // Getting user keybinds
	settings.endGroup();

	QStringList keybindList = keybinds.split(",");

	// Setting default hotkeys if it is the first time launching the program
	if (keybindList.size() != KEYBIND_AMOUNT) {
		settings.beginGroup("settings");
		settings.setValue("keybinds", defaultCombos.join(","));
		settings.endGroup();
		keybindList = defaultCombos;
	} 

	// Loading key combinations from user hotkeys
	for (int i = 0; i < KEYBIND_AMOUNT; i++) {
		combos[i] = QKeySequence((QString)(keybindList[i]));
		qDebug() << combos[i].toString();
	}
}

QKeySequence* SICombinations::getCombination(QString action)
{
	for (int i = 0; i < KEYBIND_AMOUNT; i++) {
		if (comboNames[i] == action) {
			return &combos[i];
		}
	}
	return nullptr;
}

bool SICombinations::setCombination(QString action, QString hotkey)
{
	for (int i = 0; i < KEYBIND_AMOUNT; i++) {
		if (comboNames[i] == action) {
			combos[i] = QKeySequence(hotkey);
			qDebug() << "Sequence set: " << comboNames[i] << hotkey;
			return true;
		}
	}
	return false;
}
