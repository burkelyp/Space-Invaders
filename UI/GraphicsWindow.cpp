
#include <qcolorspace.h>
#include <qdebug.h>
#include <qmenu.h>
#include <qmenubar.h>
#include <qmetaobject.h>
#include <qobject.h>
#include <qtimer.h>
#include <qwidget.h>

#include "GraphicsWindow.h"

GraphicsWindow::GraphicsWindow(QWidget* parent) : QWidget(parent)
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

}

void GraphicsWindow::updateKeyBind(const QString bind, const QString hotkey)
{
	keyBinds.setCombination(bind, hotkey);
}

void GraphicsWindow::paintEvent(QPaintEvent* event)
{
	
	rect = event->rect();
	
	// Simple proof of concept animation
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	QRect rect = event->rect();
	QSize rotateSize = QSize(rect.height(), rect.width());

	// Using bitmap as image mask 
	uchar* m = map;
	QImage image = colorMap.copy();

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
}

void GraphicsWindow::test()
{
	update();
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
	if (keyBinds.getCombination("Insert Coin")->matches(event->keyCombination()) > QKeySequence::NoMatch) {
		editMemInputBit(SIPortLocations::Insert_Coin, true);
	}
	QWidget::keyPressEvent(event);
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
	/*if (keyBinds.getCombination("Insert Coin")->matches(event->keyCombination()) > QKeySequence::NoMatch) {
		editMemInputBit(SIPortLocations::Insert_Coin, false);
	}*/
	QWidget::keyReleaseEvent(event);
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
