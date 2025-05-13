
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
	// Starting application
	QApplication app = QApplication(argc, argv);
	SIUI mainWindow(argv[1]);
	mainWindow.show();
	app.setStyle(QStyleFactory::create("Fusion"));
	app.exec();

	return 0;
	
}

int SIUI::processCount = 0;

SIUI::SIUI(const QString emuPath)
{
	// Initializing window
	this->resize(448, 533);
	this->setMinimumSize(224, 277);
	currEmu = SPACE_INVADERS;

	// Finding emulators
	if (!emuPath.isEmpty()) {
		intel8080Dir = emuPath;
	} else {
		intel8080Dir = findEmu();
	}

	// Creating Menus
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

	// Setting layout
	QWidget* centralWidget = new QWidget();
	QVBoxLayout* layout = new QVBoxLayout(centralWidget);
	centralWidget->setLayout(layout);

	window = new GraphicsWindow(centralWidget);
	layout->addWidget(window);
	layout->addWidget(new QRadioButton());
	this->setCentralWidget(centralWidget);
}

QString SIUI::findEmu()
{
	QDir def("");
	def.cdUp();
	QDirIterator it(def, QDirIterator::Subdirectories);
	while (it.hasNext()) {
		QString dir = it.next();
		QString app = dir.split("/").back();
	#ifdef Q_OS_WIN
			// Windows-specific code
		if (app == "intel8080emu.exe") {
			return dir;
		}
	#elif defined(Q_OS_LINUX)
			// Linux-specific code
		if (app == "intel8080emu.elf") {
			return dir;
		}
	#elif defined(Q_OS_MAC)
			// macOS-specific code
		if (app == "intel8080emu.app") {
			return dir;
		}
	#endif
	}
	return QString();
}

void SIUI::startEmu(QString process, QStringList arguments)
{
	// Create a new process
	QProcess* emu = new QProcess(this);
	emu->start(process, arguments);

	// Close the process when this process terminates
	QObject::connect(this, &QWidget::destroyed, emu, &QProcess::terminate);

	processes.append(processCount);
	processCount++;
}

void SIUI::SelectROM()
{
	// Open file browser and get rom file location
	ROMDir = QFileDialog::getOpenFileNames(this,
		tr("Select Space Invaders ROM File(s)"), "/home");

	// Varify correctness of ROM files by comparing to correct size, maybe better way?
	int totalSize = 0;
	for (int i = 0; i < ROMDir.size(); i++) {
		QFile romFile = QFile(ROMDir[i]);
		totalSize += romFile.size();
	}
	
	if (totalSize != SI_SIZE){
		qDebug() << "Selected File(s) are incorrect Size";
	}

	// If multiple files, combine into one and save 
	QString combinedROM; // Final ROM location

	if (ROMDir.count() > 1) {
		// Opening combined file location for writing
		QFile invaders("invaders");
		if (!invaders.open(QIODevice::WriteOnly | QIODevice::Truncate))
			qDebug() << "Failed to create file";
		QDataStream outStream(&invaders);

		char rawData[SI_SIZE];
		int lenRead;
		for (int i = ROMDir.size() - 1; i >= 0; i--) {
			// Opening individual files for reading
			QFile romFile = QFile(ROMDir[i]);
			if (!romFile.open(QIODevice::ReadOnly))
				qDebug() << "Failed to create file";

			QDataStream inStream(&romFile);

			lenRead = inStream.readRawData(rawData, SI_SIZE);
			if (lenRead == -1) {
				qDebug() << "Failed to read data from file" << ROMDir[i];
			}

			// Writing individual files to combined location
			lenRead = outStream.writeRawData(rawData, lenRead);
			if (lenRead == -1) {
				qDebug() << "Failed to read data from file" << ROMDir[i];
			}
			romFile.close();
		}
		invaders.close();

		combinedROM = QFileInfo(invaders).absolutePath();
		combinedROM.append("/invaders");
	} else {
		combinedROM = ROMDir[0];
	}

	// Creating new process
	QStringList arguments;
	arguments.append(combinedROM);
	arguments.append("--debug");
	startEmu(intel8080Dir, arguments);
}

void SIUI::OpenKeyboardMapper()
{
	KeyBoardMapper* mapper = new KeyBoardMapper(this);
	QObject::connect(mapper, &KeyBoardMapper::keyBindUpdated, window, &GraphicsWindow::updateKeyBind);
	mapper->show();
}
