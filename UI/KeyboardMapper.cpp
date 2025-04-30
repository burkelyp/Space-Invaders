
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

	this->setLayout(mainLayout);
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

	// Setting clearing signals
	QObject::connect(this, &QTableWidget::cellChanged, this, &QWidget::clearFocus);
	QObject::connect(this, &QTableWidget::cellChanged, this, &QAbstractItemView::clearSelection);

	// Setting up custom delegate
	KeyDelegate* delegate = new KeyDelegate(this);
	this->setItemDelegate(delegate);
}

KeyDelegate::KeyDelegate(QWidget* parent) :
	QStyledItemDelegate(parent)
{
}

QWidget* KeyDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	KeyEditer* editer = new KeyEditer(parent);
	return editer;
}

KeyEditer::KeyEditer(QWidget* parent) :
	QLineEdit(parent)
{
}
