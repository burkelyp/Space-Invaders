
#ifdef Q_OS_WIN
#include <windows.h>
#include <processthreadsapi.h>
// Windows-specific code
#elif defined(Q_OS_LINUX)
#include <sys/shm.h>
// Linux-specific code
#elif defined(Q_OS_MAC)
#include <sys/shm.h>
// macOS-specific code
#endif

#pragma once
#include <qdatastream.h>
#include <qdir.h>
#include <qdiriterator.h>
#include <qfile.h>
#include <qlist.h>
#include <qmainwindow.h>
#include <qmenu.h>
#include <qprocess.h>
#include <qtoolbar.h>

#include "GraphicsWindow.h"
#include "KeyboardMapper.h"

const int SI_SIZE = 8192;

/**
 * Main UI for the emulator, manages layout and emulator launching
 *
 * Takes
 */
class SIUI : public QMainWindow {
	Q_OBJECT

public:
	SIUI(const QString emuPath = QString());

	enum Emulator {
		SPACE_INVADERS = 1
	};

private:
	QString findEmu();
	void startEmu(QString process, QStringList arguments);

private slots:
	void SelectROM();
	void OpenKeyboardMapper();

private:
	QMenu* fileMenu;
	QMenu* keyMenu;
	QString intel8080Dir;
	QStringList ROMDir;
	GraphicsWindow* window;
	KeyBoardMapper* keyBoardMapper;
	QList<int> processes;
	static int processCount;

protected:
	Emulator currEmu;
};
