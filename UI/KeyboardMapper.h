#pragma once

#include <qevent.h>
#include <qkeysequence.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qstyleditemdelegate.h>
#include <qstyleoption.h>
#include <qtablewidget.h>
#include <qwidget.h>


/**
 * Custom input editor dedicated to keyboard input remapping 
 *
 * Data inputted into this editor will be modified into a "human readable"
 * representation and returned to the parent to be added to the table view.
 */
class KeyEditor : public QLineEdit {
	Q_OBJECT
public:
	/**
	   Constructor sets defaults for custom editor

	   @param parent - the parent widget
	   @return void
	*/
	KeyEditor(QWidget* parent = nullptr);

	/**
	   Helper funciton maps unconventionals uppercase keys to their lowercase 
	   counterparts

	   @param sym - the inputted char to convert
	   @return char the converted char
	*/
	char convSymbols(const char sym);

protected:
	/**
	   Overridden event to handle custom inputs by mapping key names to 
	   box rather than the corrisponding ASCII characters

	   @param event - key press event
	   @return void
	*/
	virtual void keyPressEvent(QKeyEvent* event) override;

	/**
	   Overridden event to to declare the input is done editing

	   @param event - key press event
	   @return void
	*/
	virtual void keyReleaseEvent(QKeyEvent* event) override;

private:
	QString contentToChange;

signals:
	void editedWidget(QLineEdit*);
};


/**
 * Custon delegate meant for initializing custom editor
 *
 * This class is created to override the "createEditor" function and 
 * return a custom editor when called.
 */
class KeyDelegate : public QStyledItemDelegate {
	Q_OBJECT
public:
	/**
	   Constructor sets defaults for custom delegate

	   @param parent - the parent widget
	   @return void
	*/
	KeyDelegate(QWidget* parent = nullptr);

protected:
	/**
	   Overrided createEditor, returns custom editor for keyboard mapping

	   @param parent - the parent to pass to the editor
	   @param option - syling for the returned widget
	   @param index - index in table of widget to return
	   @return QWidget* Editor to apply to the TableView
	*/
	virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private slots:
	/**
	   Closes editor and updates model to hold approprate content

	   @param editor - editor to close and extract data from
	   @return void
	*/
	void editorFinished(QLineEdit* editor);
};


/**
 * Custom table for mapping keyboard inputs
 *
 * Displays the four main input types for a space invaders
 * player and allows the user to customize the inputs.
 */
class KeyEditTable : public QTableWidget {
	Q_OBJECT
public:
	/**
	   Constructor sets defaults and style configuration for keyboard mapping

	   @param parent - the parent widget
	   @return void
	*/
	KeyEditTable(QWidget* parent = nullptr);

private slots:
	/**
	   Unfocuses and resets cells

	   @return void
	*/
	void editorFinished();

private:
	QString actions[4] = { "Left", "Right", "Shoot", "Start" };
};


/**
 * Keyboard mapping popup window 
 *
 * Holds the custom table mapping widgets and labels.
 * Allows the user to save or revert changes to custom
 * keyboard inputs.
 */
class KeyBoardMapper : public QWidget {
	Q_OBJECT
public:
	/**
	   Constructor sets defaults and initalizes tables and buttons

	   @param parent - the parent widget
	   @return void
	*/
	KeyBoardMapper(QWidget* parent = nullptr);

protected slots:
	/**
	   Indended to work with QTableWidgets currentCellChanged signal
	   tests if the current cell has changed, if so, emit keyBindUpdated

	   @param currentRow - row of the cell changed to
	   @param currentColumn - column of the cell changed to
	   @param previousRow - row of the previous cell
	   @param previousColumn - column of the previous cell
	   @return void
	*/
	void emitKeyBindUpdated(int currentRow, int currentColumn, int previousRow, int previousColumn);

signals:
	void keyBindUpdated(const QString name, const QString hotkey);

private:
	KeyEditTable* p1EditTable;
	QVBoxLayout* mainLayout;
	QHBoxLayout* buttonLayout;
};
