/****************************************************************************
 *  @file     imagesourcepage.h
 *  @brief    图像数据源设置页面类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2021.01.08
 *  Copyright (c) 2021 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef IMAGESOURCEPAGE_H
#define IMAGESOURCEPAGE_H

#include "imagesourcesettingwidget.h"
#include <QHash>
#include <QMap>
#include <QWidget>

namespace Ui
{
class ImageSourcePage;
}

namespace vtk::display
{

class ImageSourcePage : public QWidget
{
    Q_OBJECT

public:
    explicit ImageSourcePage(QWidget *parent = nullptr);
    ~ImageSourcePage();

    typedef QHash<QString, QMap<QString, bool>> TriggerHash;

    void initImageSourceConfigData();
    void saveImageSourceConfigData();
    void cancelImageSourceConfig();
    void disableFocus();
    bool validateImageSourceSettings();

private slots:
    bool slotCheckSourceName(const QString &old_source_name, const QString &new_source_name);
    bool slotCheckTriggerChar(const QString &old_char, const QString &new_char);

private:
    void init();
    void initWidgets();
    void initConnects();
    void addItem(const SourceConfigItem &data);
    void resetSameLevelOfTriggerLevelOrderCombox(const QString &source, const QString &same_level);
    bool checkSourceNameUsed(QString);

private:
    Ui::ImageSourcePage *ui;
    QHash<QString, TriggerHash> m_source_hash;  //<Ln , <Ln-1, true/false>> n=1,2,3...10 true:已占用 false:未使用
    QList<QString> m_source_name_list;
    QList<QString> m_trigger_char_list;
};
}  // namespace vtk::display
#endif  // IMAGESOURCEPAGE_H
