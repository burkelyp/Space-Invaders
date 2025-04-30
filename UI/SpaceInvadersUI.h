
#ifdef Q_OS_WIN
// Windows-specific code
#elif defined(Q_OS_LINUX)
// Linux-specific code
#elif defined(Q_OS_MAC)
// macOS-specific code
#endif

#pragma once
#include <qmainwindow.h>
#include <qtoolbar.h>
#include <qmenu.h>
#include <qdir.h>

#include "GraphicsWindow.h"
#include "KeyboardMapper.h"

class SIUI : public QMainWindow {
	Q_OBJECT

public:
	SIUI();
	enum Emulator {
		SPACE_INVADERS = 1
	};

private slots:
	void SelectROM();
	void OpenKeyboardMapper();

private:
	QMenu* fileMenu;
	QMenu* keyMenu;
	QDir ROMDir;
	GraphicsWindow* window;
	KeyBoardMapper* keyBoardMapper;

protected:
	Emulator currEmu;
};
