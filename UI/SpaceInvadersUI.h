
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
 * Gives user control of setting, menus and options and provides a 
 * wrapper around the GUI window that holds the current emulation.
 */
class SIUI : public QMainWindow {
	Q_OBJECT

public:
	/**
	   Constructor sets up window size, layout and menus

	   @param parent - the parent widget
	   @return void
	*/
	SIUI(const QString emuPath = QString());

	enum Emulator {
		NO_EMU = 0,
		SPACE_INVADERS = 1
	};

private:
	/**
	   Looks through local directories starting one directory up
	   from current working directory and looks for emulator

	   @return QString the emulators absolute path else "Error"
	*/
	QString findEmu();

	/**
	   Generates a new cross platform process and passes it arguments.
	   Process terminates when window terminates

	   @param process - path to the process
	   @param arguments - arguments to pass to the process
	   @return void
	*/
	void startEmu(QString process, QStringList arguments);

private slots:
	/**
	   Opens filedialog, user selects ROM files then compiles them into 
	   one file and launches emulator

	   @return void
	*/
	void SelectROM();

	/**
	   Opens custom keyboard mapper window to bind game controls

	   @return void
	*/
	void OpenKeyboardMapper();

signals:
	void closeOpenEmus();

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
	Emulator currEmu = Emulator::NO_EMU;
};
