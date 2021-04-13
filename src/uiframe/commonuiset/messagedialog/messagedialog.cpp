#include "messagedialog.h"
#include "messagedialogtitlebar.h"
#include "ui_messagedialog.h"

#include <QApplication>
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QTranslator>
#include <QVBoxLayout>

namespace vtk::display
{

ConfirmButton::ConfirmButton()
{
    this->setFixedSize(100, 40);
    this->setStyleSheet("background: #006db2; border-radius: 4px; color: white; font-size: 16px;");
    this->setText(tr("OK"));
    this->setCursor(Qt::PointingHandCursor);
}

CancelButton::CancelButton()
{
    this->setFixedSize(100, 40);
    this->setStyleSheet(
        "background: white; border-radius: 4px; color: rgba(0,0,0,0.5); border: 1px solid #eeeeee; font-size: 16px;");
    this->setText(tr("Cancel"));
    this->setCursor(Qt::PointingHandCursor);
}

YesButton::YesButton()
{
    this->setFixedSize(100, 40);
    this->setStyleSheet("background: #f44747; border-radius: 4px; color: white; font-size: 16px;");
    this->setText(tr("Yes"));
    this->setCursor(Qt::PointingHandCursor);
}

NoButton::NoButton()
{
    this->setFixedSize(100, 40);
    this->setStyleSheet(
        "background: white; border-radius: 4px; color: rgba(0,0,0,0.5); border: 1px solid #eeeeee; font-size: 16px;");
    this->setText(tr("No"));
    this->setCursor(Qt::PointingHandCursor);
}

DeleteButton::DeleteButton()
{
    this->setFixedSize(100, 40);
    this->setStyleSheet("background: #f44747; border-radius: 4px; color: white; font-size: 16px;");
    this->setText(tr("Delete"));
    this->setCursor(Qt::PointingHandCursor);
}

OpenFileButton::OpenFileButton()
{
    this->setFixedSize(170, 40);
    this->setStyleSheet("background: #006db2; border-radius: 4px; color: white; font-size: 16px;");
    this->setText(tr("Open Folder"));
    this->setCursor(Qt::PointingHandCursor);
}

MessageDialog::MessageDialog(DialogType dialog_type, QString msg, QList<ButtonType> buttons, QWidget *parent)
    : QDialog(parent), ui(new Ui::MessageDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setWindowModality(Qt::ApplicationModal);
    this->setFixedSize(500, 230);

    ui->text_area->setStyleSheet("font-size: 16px;");

    MessageDialogTitlebar *titlebar = new MessageDialogTitlebar(this);
    ui->titlebar_layout->addWidget(titlebar);
    ui->button_layout->addStretch();

    // add box shadow
    this->setAttribute(Qt::WA_TranslucentBackground);
    QGraphicsDropShadowEffect *pEffect = new QGraphicsDropShadowEffect(ui->message_dialog_frame);
    pEffect->setOffset(0, 0);
    pEffect->setColor(QColor(100, 100, 100));
    pEffect->setBlurRadius(10);
    ui->message_dialog_frame->setGraphicsEffect(pEffect);
    ui->main_layout->setMargin(10);

    for (auto button : buttons)
    {
        switch (button)
        {
            case Confirm:
            {
                ConfirmButton *confirm_btn = new ConfirmButton();
                ui->button_layout->addWidget(confirm_btn);
                connect(confirm_btn, SIGNAL(clicked()), this, SLOT(slotConfirmDialog()));
                break;
            }
            case Cancel:
            {
                CancelButton *cancel_btn = new CancelButton();
                ui->button_layout->addWidget(cancel_btn);
                connect(cancel_btn, SIGNAL(clicked()), this, SLOT(slotCancelDialog()));
                break;
            }
            case Yes:
            {
                YesButton *yes_btn = new YesButton();
                ui->button_layout->addWidget(yes_btn);
                connect(yes_btn, SIGNAL(clicked()), this, SLOT(slotConfirmDialog()));
                break;
            }
            case No:
            {
                NoButton *no_btn = new NoButton();
                ui->button_layout->addWidget(no_btn);
                connect(no_btn, SIGNAL(clicked()), this, SLOT(slotCancelDialog()));
                break;
            }
            case OpenFile:
            {
                OpenFileButton *open_file_btn = new OpenFileButton();
                ui->button_layout->addWidget(open_file_btn);
                // connect(open_file_btn, SIGNAL(clicked()), this, SLOT(slotOpenFile()));
                break;
            }
        }
    }

    ui->text_area->setText(msg);

    if (dialog_type == DialogType::InfoDialog)
    {
        titlebar->m_title->setText(tr("Info"));
    }
    else if (dialog_type == DialogType::WarningDialog)
    {
        titlebar->m_title->setText(tr("Warning"));
    }
    else if (dialog_type == DialogType::ErrorDialog)
    {
        titlebar->m_title->setText(tr("Error"));
    }
}

MessageDialog::~MessageDialog() {}

void MessageDialog::slotCancelDialog()
{
    this->reject();
}

void MessageDialog::slotConfirmDialog()
{
    this->accept();
}

}  // namespace vtk::display
