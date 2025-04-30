#pragma once

#include <qlayout.h>
#include <qlineedit.h>
#include <qstyleditemdelegate.h>
#include <qstyleoption.h>
#include <qtablewidget.h>
#include <qwidget.h>


class KeyEditer : public QLineEdit {
public:
	KeyEditer(QWidget* parent = nullptr);

};

class KeyDelegate : public QStyledItemDelegate {
public:
	KeyDelegate(QWidget* parent = nullptr);

protected:
	virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

class KeyEditTable : public QTableWidget {
public:
	KeyEditTable(QWidget* parent = nullptr);

private:
	QString actions[4] = { "Left", "Right", "Shoot", "Start" };
};

class KeyBoardMapper : public QWidget {
public:
	KeyBoardMapper(QWidget* parent = nullptr);

private:
	KeyEditTable* p1EditTable;
	QVBoxLayout* mainLayout;
	QHBoxLayout* buttonLayout;

};
