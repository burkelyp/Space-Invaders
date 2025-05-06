
#include <qheaderview.h>

#include "KeyboardMapper.h"

KeyBoardMapper::KeyBoardMapper(QWidget* parent)
{
	// Window Initialization
	this->setAttribute(Qt::WA_DeleteOnClose);
	this->setWindowTitle("Configure Kayboard");
	this->setFixedSize(375, 450);

	mainLayout = new QVBoxLayout(this);

	p1EditTable = new KeyEditTable(this);
	mainLayout->addWidget(p1EditTable);

	QObject::connect(p1EditTable, &QTableWidget::currentCellChanged, this, &KeyBoardMapper::emitKeyBindUpdated);

	this->setLayout(mainLayout);
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

KeyEditTable::KeyEditTable(QWidget* parent) :
	QTableWidget(parent)
{
	// Hiding column and row headers
	this->horizontalHeader()->hide();
	this->verticalHeader()->hide();

	// Setting fixed column and row counts
	this->setColumnCount(2);
	this->setRowCount(4);

	this->setEditTriggers(QAbstractItemView::AllEditTriggers);

	// Adding Hotkey Labels
	QTableWidgetItem* item;
	int i = 0;
	for (QString* a = actions; a != std::end(actions); a++) {
		item = new QTableWidgetItem(*a);
		item->setFlags(item->flags() & Qt::ItemIsEditable);
		this->setItem(i, 0, item);
		i++;
	}

	// Setting up custom delegate
	KeyDelegate* delegate = new KeyDelegate(this);
	this->setItemDelegate(delegate);

	// Setting clearing signals
	QObject::connect(delegate, &QStyledItemDelegate::closeEditor, this, &KeyEditTable::editorFinished);
	QObject::connect(this, &QTableWidget::cellChanged, this, &QWidget::clearFocus);
	QObject::connect(this, &QTableWidget::cellChanged, this, &QAbstractItemView::clearSelection);

}

void KeyEditTable::editorFinished()
{
	emit this->currentCellChanged(0, 0, this->currentRow(), this->currentColumn());
	emit this->cellChanged(0, 0);
}

KeyDelegate::KeyDelegate(QWidget* parent) :
	QStyledItemDelegate(parent)
{
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
	// I wish there was a better way to do this, but ASCII values are not consistant 
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
