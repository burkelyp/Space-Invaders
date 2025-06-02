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

#include <qaction.h>
#include <qbytearray.h>
#include <qcolortransform.h>
#include <qevent.h>
#include <qimage.h>
#include <qlayout.h>
#include <qpaintevent>
#include <qpainter.h>
#include <qpixmap.h>
#include <qsettings.h>
#include <qtransform.h>
#include <qwidget.h>

const int MEM_SIZE = 0x10007;
const int SCREEN_RESOLUTION = 57344; //(256x224)
const int FRAME_RATE = 60;
const char MAPPED_NAME[] = "/SpaceInvaders";
const int KEYBIND_AMOUNT = 9;

// TODO make this class less proprietary so it can be used with all emulators

/**
 * Action manager for the Space Invaders hotkeys
 *
 * Instantiates, assigns, modifies, and retrieves actions for the 
 * game Space Invaders
 */
class SICombinations : public QObject {
public:
	/**
	   Constructor initializes actions

	   @param parent - the parent widget
	   @return void
	*/
	SICombinations(QWidget* parent = nullptr);

	/**
	   Retrieves the action specified by action

	   @param action - the action to retrieve
	   @return QAction* action that was specified else nullptr
	*/
	QKeySequence* getCombination(QString action);

	/**
	   Sets the provided hotkey key combination to the provided action

	   @param action - the action to assign
	   @param hotkey - the key combination to assign to the action
	   @return true on sucess else false
	*/
	bool setCombination(QString action, QString hotkey);

protected:
	QKeySequence combos[KEYBIND_AMOUNT] = { 0 };
	// Contains keybind Names
	QStringList comboNames = { "p1Left", "p1Right", "p1Shoot", "p1Start", "p2Left", "p2Right", "p2Shoot", "p2Start", "Insert Coin"};
	// Contains default Bindings
	QStringList defaultCombos = { "Left", "Right", "Up", "Return", "Left", "Right", "Up", "Num+Enter", "C"};
};

/**
 * Centralized visuals renderer
 *
 * This class is the central widget that manages the space invaders 
 * inter-process communication and reads and displays space invaders 
 * video memory.
 */
class GraphicsWindow : public QWidget {
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

	/**
	   Updates the specified bind to the hotkey hotkey

	   @param bind - the map to update
	   @param hotkey - the new key squence to bind
	   @return void
	*/
	void updateKeyBind(const QString bind, const QString hotkey);

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
	   Initializes interprocess communication structure by setting up memory maps on a by-system basis

	   @return uchar* memory location that was allocated and shared if successful else nullptr
	*/
	uchar* setupMemMap();

	/**
	   Overridden event to handle custom inputs by mapping key names to
	   box rather than the corrisponding ASCII characters

	   @param event - key press event
	   @return void
	*/
	virtual void keyPressEvent(QKeyEvent* event) override;

	/**
	   Overridden event to to declare the input is done editing

	   @param event - key press event
	   @return void
	*/
	virtual void keyReleaseEvent(QKeyEvent* event) override;

private:
	/**
	   Edits the given bit on the input ports stored in memory

	   @param bit - the bit offset to edit
	   @param set - if true set bit to 1 else set to 0
	   @return void
	*/
	void editMemInputBit(int bit, bool set);

public:
	// Holds port bit offsets to access and modify appropriate input bits
	enum SIPortLocations {
		Insert_Coin = 8,
		P2Start = 9,
		P1Start = 10,
		P1Shoot = 12,
		P1Left = 13,
		P1Right = 14,
		P2Shoot = 20,
		P2Left = 21,
		P2Right = 22
	};

private:
	// Memory variables
	uchar mem[MEM_SIZE] = { 0 };
	uchar* map = mem + 0x2400;
	uchar* ports = mem + 0x10000;
	uchar* memptr;
	void* handle;
	int o = 0;
	int change = 85;

	// Keyboard Binds
	SICombinations keyBinds;

	// Variables
	QImage colorMap;
	QRect rect;
};
