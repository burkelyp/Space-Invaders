
#include <qaction.h>
#include <qapplication.h>
#include <qfiledialog.h>
#include <qmenubar.h>
#include <qradiobutton.h>
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
	currEmu = SPACE_INVADERS;

	fileMenu = new QMenu("File", this);
	QAction* openROM = new QAction("Open ROM", fileMenu);
	fileMenu->addAction(openROM);
	
	QObject::connect(openROM, &QAction::triggered, this, &SIUI::SelectROM);

	keyMenu = new QMenu("Settings", this);
	QAction* mapKeys = new QAction("Configure Inputs", fileMenu);
	keyMenu->addAction(mapKeys);

	QObject::connect(mapKeys, &QAction::triggered, this, &SIUI::OpenKeyboardMapper);

	this->menuBar()->addMenu(fileMenu);
	this->menuBar()->addMenu(keyMenu);

	QWidget* centralWidget = new QWidget();
	QVBoxLayout* layout = new QVBoxLayout(centralWidget);
	centralWidget->setLayout(layout);

	window = new GraphicsWindow(centralWidget);
	layout->addWidget(window);
	layout->addWidget(new QRadioButton());
	this->setCentralWidget(centralWidget);
}

void SIUI::SelectROM()
{
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Open ROM"), "/home");
	ROMDir = QDir(fileName);
}

void SIUI::OpenKeyboardMapper()
{
	KeyBoardMapper* mapper = new KeyBoardMapper(this);
	mapper->show();
}
