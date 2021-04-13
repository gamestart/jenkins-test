/****************************************************************************
 *  @file     datalistwidget.h
 *  @brief    数据列表类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef DATALISTWIDGET_H
#define DATALISTWIDGET_H

#include <QHash>
#include <QWidget>

namespace Ui
{
class DataListWidget;
}

namespace vtk::display
{

class DataListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DataListWidget(const QString &title = "", QWidget *parent = nullptr);
    ~DataListWidget();
    void setTitle(QString);
    void setHeader(QStringList);

private:
    void init();
    void initWidgets();
    void initConnects();
    void resetCombox();
    void checkImageList();
    void resetNum();

private:
    Ui::DataListWidget *ui;
    QString m_title;
    QHash<QUuid, QString> m_view_info_hash;
};
}  // namespace vtk::display
#endif  // DATALISTWIDGET_H
