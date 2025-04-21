
#include <qaction.h>
#include <qapplication.h>
#include <qfiledialog.h>
#include <qmenubar.h>
#include <qstylefactory.h>

#include <qoperatingsystemversion.h>

#include "SpaceInvadersUI.h"

int MAGIC_NUMBER = 0x23464210;
int FILE_VERSION = 100;


int main(int argc, char* argv[]) {
	QApplication app = QApplication(argc, argv);
	SIUI mainWindow;
	mainWindow.show();
	app.setStyle(QStyleFactory::create("Fusion"));
	app.exec();

	return 0;
	
}

SIUI::SIUI()
{
	this->resize(448, 533);
	this->setMinimumSize(224, 277);

	fileMenu = new QMenu("File", this);
	QAction* openROM = new QAction("Open ROM", fileMenu);
	fileMenu->addAction(openROM);

	QObject::connect(openROM, &QAction::triggered, this, &SIUI::SelectROM);

	this->menuBar()->addMenu(fileMenu);

	window = new GraphicsWindow(this);
	this->setCentralWidget(window);
}

void SIUI::SelectROM()
{
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Open ROM"), "/home");
	ROMDir = QDir(fileName);
}
