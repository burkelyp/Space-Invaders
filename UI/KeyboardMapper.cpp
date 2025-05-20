
#include <qheaderview.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qscrollbar.h>
#include <qsettings.h>

#include "KeyboardMapper.h"

KeyBoardMapper::KeyBoardMapper(QWidget* parent)
{
	// Window Initialization
	this->setAttribute(Qt::WA_DeleteOnClose);
	this->setWindowTitle("Configure Keyboard");
	this->setFixedSize(400, 450);

	// Maybe make this its own function?
	// Getting setting keybind information to populate tables with
	QSettings settings("settings.ini", QSettings::IniFormat); // Using universal INI for cross platform use
	settings.beginGroup("settings");
	QString keybinds = settings.value("keybinds").toString(); // Getting user keybinds
	settings.endGroup();

	QStringList keybindList = keybinds.split(",");

	mainLayout = new QVBoxLayout(this);
	QHBoxLayout* sublayout = new QHBoxLayout();
	mainLayout->addLayout(sublayout);

	// Adding player 1 keybinds
	QVBoxLayout* p1Layout = new QVBoxLayout();
	p1Layout->setAlignment(Qt::AlignTop);
	sublayout->addLayout(p1Layout);

	QLabel* p1header = new QLabel("Player 1:", this);
	p1Layout->addWidget(p1header, 0, Qt::AlignHCenter | Qt::AlignTop);

	p1EditTable = new KeyEditTable(this, { "Left", "Right", "Shoot", "Start" }, keybindList.mid(0, 4));
	p1EditTable->setPrefix("p1");
	p1Layout->addWidget(p1EditTable, 0, Qt::AlignTop);

	// Adding player 2 keybinds
	QVBoxLayout* p2Layout = new QVBoxLayout();
	p2Layout->setAlignment(Qt::AlignTop);
	sublayout->addLayout(p2Layout);

	QLabel* p2header = new QLabel("Player 2:", this);
	p2Layout->addWidget(p2header, 0, Qt::AlignHCenter | Qt::AlignTop);

	p2EditTable = new KeyEditTable(this, { "Left", "Right", "Shoot", "Start" }, keybindList.mid(4, 4));
	p2EditTable->setPrefix("p2");
	p2Layout->addWidget(p2EditTable, 0, Qt::AlignTop);

	// Adding save or cancel buttons
	QHBoxLayout* buttonLayout = new QHBoxLayout();
	mainLayout->addLayout(buttonLayout);

	QPushButton* resetButton = new QPushButton("Reset", this);
	QObject::connect(resetButton, &QPushButton::pressed, this, &KeyBoardMapper::loadKeybinds);
	//buttonLayout->addWidget(resetButton);

	QPushButton* saveButton = new QPushButton("Save", this);
	QObject::connect(saveButton, &QPushButton::pressed, this, &KeyBoardMapper::emitKeyBindsUpdated);
	buttonLayout->addWidget(saveButton, 0, Qt::AlignHCenter);

	this->setLayout(mainLayout);
}

void KeyBoardMapper::loadKeybinds() {
	// Getting setting keybind information to populate tables with
	QSettings settings("settings.ini", QSettings::IniFormat); // Using universal INI for cross platform use
	settings.beginGroup("settings");
	QString keybinds = settings.value("keybinds").toString(); // Getting user keybinds
	settings.endGroup();

	QStringList keybindList = keybinds.split(",");
	p1EditTable->setKeyList(keybindList.mid(0, 4));
	p2EditTable->setKeyList(keybindList.mid(4, 4));
}

void KeyBoardMapper::saveKeybinds(QString keybinds) {
	// Getting setting keybind information to populate tables with
	QSettings settings("settings.ini", QSettings::IniFormat); // Using universal INI for cross platform use
	settings.beginGroup("settings");
	settings.setValue("keybinds", keybinds); // Saving user keybinds
	settings.endGroup();
}

void KeyBoardMapper::closeEvent(QCloseEvent* event)
{
	QStringList keys = p1EditTable->getKeys() + p2EditTable->getKeys();
	keys.append("C");

	// Getting setting keybind information
	QSettings settings("settings.ini", QSettings::IniFormat);
	settings.beginGroup("settings");
	QString keybinds = settings.value("keybinds").toString(); // Getting user keybinds
	settings.endGroup();

	QStringList keybindList = keybinds.split(",");

	// Verify if unsaved changes
	if (keys != keybindList) {
		// Ask if user wants to save
		int ret = QMessageBox::warning(this, tr("Save"),
			tr("Would you like to save your changes before closing?"),
			QMessageBox::Save | QMessageBox::Discard
			| QMessageBox::Cancel,
			QMessageBox::Save);

		switch (ret) {	
		case QMessageBox::Save:
			emitKeyBindsUpdated();
		case QMessageBox::Discard:
			event->accept();
			break;
		default:
			event->ignore();
		}
	}
}

void KeyBoardMapper::emitKeyBindUpdated(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
	// There might be a better way to do this, but this will work for now
	if (currentRow == 0 && currentColumn == 0) {
		// Grab changed hotkey and the action label and emit them when changed
		QString label = p1EditTable->item(previousRow, 0)->text();
		label.prepend("p1");
		QString sequence = p1EditTable->item(previousRow, previousColumn)->text();
		if (!sequence.isEmpty())
			emit keyBindUpdated(label, sequence);
	}


}

void KeyBoardMapper::emitKeyBindsUpdated()
{
	QStringList p1Keys = p1EditTable->getKeys();
	QStringList p1Actions = p1EditTable->getActions();
	
	// Iterate over p1 keys and actions and emit signal
	for (QString* i = p1Keys.begin(),* j = p1Actions.begin(); i != p1Keys.end(); i++, j++) {
		j->prepend("p1");
		emit keyBindUpdated(*j, *i);
	}

	QStringList p2Keys = p2EditTable->getKeys();
	QStringList p2Actions = p2EditTable->getActions();

	// Iterate over p2 keys and actions and emit signal
	for (QString* i = p2Keys.begin(),* j = p2Actions.begin(); i != p2Keys.end(); i++, j++) {
		j->prepend("p2");
		emit keyBindUpdated(*j, *i);
	}

	// Saving new keybinds
	QString keybinds = p1Keys.join(",");
	keybinds += ",";
	keybinds += p2Keys.join(",");
	keybinds += ",C"; // Temporary until I add coin input settings
	saveKeybinds(keybinds);
}

KeyEditTable::KeyEditTable(QWidget* parent, QStringList actionList, QStringList keyList) :
	QTableWidget(parent)
{
	this->setFixedHeight(122);

	// Hiding column and row headers
	this->horizontalHeader()->hide();
	this->verticalHeader()->hide();
	this->horizontalScrollBar()->hide();

	// Setting fixed column and row counts
	this->setColumnCount(2);
	this->setRowCount(4);

	// Adjusting column size
	this->setColumnWidth(0, 50);
	this->setColumnWidth(1, 134);

	this->setEditTriggers(QAbstractItemView::AllEditTriggers);

	if (!actionList.isEmpty()) {
		this->setActions(actionList);
	}
	if (!keyList.isEmpty()) {
		this->setKeyList(keyList);
	}

	// Setting up custom delegate
	KeyDelegate* delegate = new KeyDelegate(this);
	this->setItemDelegate(delegate);

	// Setting clearing signals
	QObject::connect(delegate, &QStyledItemDelegate::closeEditor, this, &KeyEditTable::editorFinished);
	QObject::connect(this, &QTableWidget::cellChanged, this, &QWidget::clearFocus);
	QObject::connect(this, &QTableWidget::cellChanged, this, &QAbstractItemView::clearSelection);

}

void KeyEditTable::setActions(QStringList actionList)
{	
	// Adding Hotkey Labels
	QTableWidgetItem* item;
	int i = 0;
	for (QString* a = actionList.begin(); a != actionList.end(); a++) {
		item = new QTableWidgetItem(*a);
		item->setFlags(item->flags() & Qt::ItemIsEditable);
		this->setItem(i, 0, item);
		i++;
	}
	actions = actionList;
}

void KeyEditTable::setKeyList(QStringList keyList)
{
	// Adding Keys Labels
	QTableWidgetItem* item;
	int i = 0;
	for (QString* a = keyList.begin(); a != keyList.end(); a++) {
		item = new QTableWidgetItem(*a);
		//item->setFlags(item->flags() & Qt::ItemIsEditable);
		this->setItem(i, 1, item);
		i++;
	}
	keys = keyList;
}

void KeyEditTable::editorFinished()
{
	// Update current stored key
	keys[this->currentRow()] = this->currentItem()->text();

	// Emit signals
	emit this->currentCellChanged(0, 0, this->currentRow(), this->currentColumn());
	emit this->cellChanged(0, 0);
}

KeyDelegate::KeyDelegate(QWidget* parent) :
	QStyledItemDelegate(parent)
{
	// Empty for now
}

QWidget* KeyDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	KeyEditor* editor = new KeyEditor(parent);

	// Connecting editor to closing signals
	QObject::connect(editor, &KeyEditor::editedWidget, this, &KeyDelegate::editorFinished);

	return editor;
}

void KeyDelegate::editorFinished(QLineEdit* editor)
{
	emit this->commitData(editor);
	emit this->closeEditor(editor);
}

KeyEditor::KeyEditor(QWidget* parent) :
	QLineEdit(parent)
{
	this->setReadOnly(true);
}

char KeyEditor::convSymbols(const char sym)
{
	// I wish there was a better way to do this, but ASCII values are not consistant with qwerty caps
	switch (sym) {
	case '!':
		return '1';
	case '@':
		return '2';
	case '#':
		return '3';
	case '$':
		return '4';
	case '%':
		return '5';
	case '^':
		return '6';
	case '&':
		return '7';
	case '*':
		return '8';
	case '(':
		return '9';
	case ')':
		return '0';
	case '_':
		return '-';
	case '+':
		return '=';
	case '{':
		return '[';
	case '}':
		return ']';
	case '|':
		return '\\';
	case ':':
		return ';';
	case '"':
		return '\'';
	case '<':
		return ',';
	case '>':
		return '.';
	case '?':
		return '/';
	case '~':
		return '`';
	default:
		return sym; // Returns symbol otherwise
	}
}

void KeyEditor::keyPressEvent(QKeyEvent* event)
{
	if (event->isAutoRepeat()) {
		return;
	} 

	// Editing content to appropriate values
	contentToChange = QString("");
	QString ctrl = "Control + ";
	Qt::KeyboardModifiers modifiers = event->keyCombination().keyboardModifiers();
#if defined(Q_OS_MAC)
	// Mac specific code for command key
	ctrl = "Command + ";
	if ((modifiers & Qt::MetaModifier) == Qt::MetaModifier) {
		contentToChange.append("Control + ");
	}
#endif
	// Universal key modifiers
	if ((modifiers & Qt::ShiftModifier) == Qt::ShiftModifier) {
		contentToChange.append("Shift + ");
	}
	if ((modifiers & Qt::AltModifier) == Qt::AltModifier) {
		contentToChange.append("Alt + ");
	}
	if ((modifiers & Qt::ControlModifier) == Qt::ControlModifier) {
		contentToChange.append(ctrl);
	}

	// Adding Keys
	int key = event->key();
	if (key != Qt::Key_Control && key != Qt::Key_Shift && key != Qt::Key_Alt && key != Qt::Key_Meta) {
		QString k = QKeySequence(key).toString();
		k.toUpper();
		if (k.length() == 1)
			k = convSymbols(*(k.toStdString().c_str()));
		contentToChange.append(k);
	}
	this->clear();
}

void KeyEditor::keyReleaseEvent(QKeyEvent* event)
{
	if (event->isAutoRepeat()) {
		return;
	}

	// Set text
	this->setText(contentToChange);
	emit this->editingFinished();
	emit this->editedWidget(this);
}
