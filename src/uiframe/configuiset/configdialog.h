/****************************************************************************
 *  @file     configdialog.h
 *  @brief    设置窗口类
 *  @author
 *  @email
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QWidget>

#include "camerasettingpage.h"
#include "configtitlebar.h"
#include "imagesourcepage.h"
#include "modelsettingpage.h"
#include "savesettingpage.h"
#include "systemsettingpage.h"
#include "viewsettingpage.h"

namespace Ui
{
class ConfigDialog;
}

namespace vtk::display
{
class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(QWidget *parent = nullptr);
    ~ConfigDialog();

signals:
    void signalCloseWaitingDialog();

protected:
    bool eventFilter(QObject *wcg, QEvent *event) override;

private slots:
    void slotCheckAndClose();

private:
    void initWidgets();
    void initConnections();
    void switchPage(int);
    void initTitleBar();
    bool validatePage(int);
    void saveConfig();
    void cancelConfig();

private:
    Ui::ConfigDialog *ui;
    ConfigTitleBar *m_titleBar{nullptr};
    SystemSettingPage *m_system_setting_page{nullptr};
    CameraSettingPage *m_camera_setting_page{nullptr};
    ModelSettingPage *m_model_setting_page{nullptr};
    ImageSourcePage *m_image_source_page{nullptr};
    ViewSettingPage *m_view_setting_page{nullptr};
    SaveSettingPage *m_save_setting_page{nullptr};
    QLabel *m_view_tab_text{nullptr};
    QLabel *m_save_tab_text{nullptr};
    QLabel *m_system_tab_text{nullptr};
    QLabel *m_camera_tab_text{nullptr};
    QLabel *m_model_tab_text{nullptr};
    QLabel *m_image_source_tab_text{nullptr};
};
}  // namespace vtk::display
#endif  // CONFIGDIALOG_H
