#pragma once

#include <qpushbutton.h>
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
	   @param actionList - list of action names
	   @param keyList - list of key names
	   @return void
	*/
	KeyEditTable(QWidget* parent = nullptr, QStringList actionList = {}, QStringList keyList = {});

	/**
	   Assigns the KeyEditTable a set of actions to display

	   @param actionList - actions to set
	   @return void
	*/
	void setActions(QStringList actionList);

	/**
	   Returns the assigned actions for the KeyEditTable

	   @return QStringList list of actions
	*/
	QStringList getActions() { return actions; };

	/**
	   Returns the assigned action for the given index

	   @param index - index of action to return
	   @return QString action in given index
	*/
	QString getAction(int index) { return actions[index]; };

	/**
	   Assigns the KeyEditTable a set of keys to display

	   @param keyList - actions to set
	   @return void
	*/
	void setKeyList(QStringList keyList);

	/**
	   Returns the assigned keys for the KeyEditTable

	   @return QStringList list of keys
	*/
	QStringList getKeys() { return keys; };

	/**
	   Returns the assigned key for the given index

	   @param index - index of action to return
	   @return QString key in given index
	*/
	QString getKey(int index) { return keys[index]; };

	/**
	   Assigns the KeyEditTable a prefix to append to actions when returning label

	   @param prefix - new prefix
	   @return void
	*/
	void setPrefix(QString pre) { prefix = pre; };

	/**
	   Returns the assigned prefix for the KeyEditTable

	   @return QString of prefix if prefix else empty QString
	*/
	QString getPrefix() { return prefix; };

private slots:
	/**
	   Unfocuses and resets cells

	   @return void
	*/
	void editorFinished();

private:
	QStringList actions;
	QStringList keys;
	QString prefix = "";
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

protected:
	/**
	   Overrided closeEvent, asks the user if they want to save 
	   keybinds and closes if saved or discarded, ignored if cancelled.

	   @param event - the close event
	   @return void
	*/
	void closeEvent(QCloseEvent* event) override;

protected slots:
	/**
	   Loads and assigns keybinds to children from settings

	   @return void
	*/
	void loadKeybinds();

	/**
	   Saves the assigned keybinds to settings

	   @return void
	*/
	void saveKeybinds(QString keybinds);

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

	/**
	   Emits signals for all updated keybinds and saves new keybinds 
	   to settings.ini

	   @return void
	*/
	void emitKeyBindsUpdated();

signals:
	void keyBindUpdated(const QString name, const QString hotkey);

private:
	KeyEditTable* p1EditTable;
	KeyEditTable* p2EditTable;
	QVBoxLayout* mainLayout;
};
