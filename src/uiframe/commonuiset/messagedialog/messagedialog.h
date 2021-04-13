#ifndef MESSAGEDIALOG_H
#define MESSAGEDIALOG_H

#include <QDialog>
#include <QToolButton>

namespace Ui
{
class MessageDialog;
}

namespace vtk::display
{

enum ButtonType
{
    Confirm,
    Cancel,
    Yes,
    No,
    OpenFile
};

enum class DialogType
{
    InfoDialog,
    WarningDialog,
    ErrorDialog
};

class ConfirmButton : public QToolButton
{
    Q_OBJECT
public:
    ConfirmButton();
};

class CancelButton : public QToolButton
{
    Q_OBJECT
public:
    CancelButton();
};

class YesButton : public QToolButton
{
    Q_OBJECT
public:
    YesButton();
};

class NoButton : public QToolButton
{
    Q_OBJECT
public:
    NoButton();
};

class DeleteButton : public QToolButton
{
    Q_OBJECT
public:
    DeleteButton();
};

class OpenFileButton : public QToolButton
{
    Q_OBJECT
public:
    OpenFileButton();
};

class MessageDialog : public QDialog
{
    Q_OBJECT

public:
    MessageDialog(DialogType dialog_type, QString msg = "", QList<ButtonType> buttons = {Confirm},
                  QWidget *parent = nullptr);
    ~MessageDialog();

private slots:
    void slotCancelDialog();
    void slotConfirmDialog();

private:
    Ui::MessageDialog *ui;
};

}  // namespace vtk::display
#endif  // MESSAGEDIALOG_H
