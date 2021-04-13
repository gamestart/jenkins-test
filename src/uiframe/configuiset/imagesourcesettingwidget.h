/****************************************************************************
 *  @file     imagesourcesettingwidget.h
 *  @brief    图像数据源设置窗口部件类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2021.01.08
 *  Copyright (c) 2021 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef IMAGESOURCESETTINGWIDGET_H
#define IMAGESOURCESETTINGWIDGET_H

#include "configdatamanager/configmanager.h"
#include <QWidget>

namespace Ui
{
class ImageSourceSettingWidget;
}

namespace vtk::display
{

using SourceConfigItem = vtk::config::SourceConfigItem;

class ImageSourceSettingWidget : public QWidget
{
    Q_OBJECT

public:
    struct SourceInfo
    {
        QString camera_id;
        vtk::common::CameraState state;
    };
    explicit ImageSourceSettingWidget(const SourceConfigItem &data, QWidget *parent = nullptr);
    ~ImageSourceSettingWidget();

    void resetTriggerLevelOrderCombox(const QStringList &string_list, const QString &current_text = "");

    const QString &getCurrentImageSourceName() const;
    void setCurrentImageSourceName(const QString &source_name);

    const QString &getCurrentTriggerChar() const;
    void setCurrentTriggerChar(const QString &trigger_char);

    const QString &getCurrentImageSource() const;
    const QString &getCurrentTriggerLevel() const;
    const QString &getCurrentTriggerLevelOrder() const;
    const SourceConfigItem &getSourceConfigData() const;

    bool validateSourceSetting();
    void disableFocus(int);

signals:
    bool signalSourceNameTextChanged(const QString &old_source_name, const QString &new_source_name);
    bool signalTriggerCharTextChanged(const QString &old_char, const QString &new_char);
    void signalImageSourceTextChanged(const QString &old_source, const QString &new_source);
    void signalTriggerLevelTextChanged(const QString &source, const QString &trigger_level);
    void signalTriggerLevelOrderTextChanged(const QString &source, const QString &old_order, const QString &new_order);
    void signalDelete();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void on_delete_btn_clicked();
    void slotCheckCameraStatus(int);

private:
    void init();
    void initWidgets();
    void initConnects();
    bool validateSourceName();
    bool validateTriggerCharacter();

private:
    Ui::ImageSourceSettingWidget *ui;
    int m_id{0};
    SourceConfigItem m_data;
};
}  // namespace vtk::display
Q_DECLARE_METATYPE(vtk::display::ImageSourceSettingWidget::SourceInfo);
#endif  // IMAGESOURCESETTINGWIDGET_H
