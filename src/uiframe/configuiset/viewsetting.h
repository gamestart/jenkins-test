/****************************************************************************
 *  @file     viewsetting.h
 *  @brief    设置窗口单个显示设置窗口类
 *  @author
 *  @email
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef VIEWSETTING_H
#define VIEWSETTING_H

#include "camerasettingpage.h"
#include "configdatamanager/configmanager.h"
#include "modelitem.h"
#include "ui_viewsetting.h"
#include <QListWidget>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class ViewSetting;
}
QT_END_NAMESPACE

namespace vtk::display
{
class ViewSetting : public QWidget
{
    Q_OBJECT

public:
    ViewSetting(const QUuid &view_uuid, const QString &view_name, const QString &view_source,
                const QStringList &view_algo, QWidget *parent = nullptr);
    ~ViewSetting();

    QUuid getUuid();
    QString getViewName();
    void setViewName(QString);
    QString getViewSource();
    QStringList getViewAlgo();
    void updateViewSources();
    void updateViewModels();
    bool validateViewSetting();
    void disableFocus();
    void enableDeleteBtn(bool enable);

signals:
    void signalDeleteView();
    bool signalCheckDuplicate();

protected:
    bool eventFilter(QObject *wcg, QEvent *event) override;

private slots:
    void slotDeleteView();
    void on_add_model_item_btn_clicked();
    void on_delete_model_item_btn_clicked();
    void slotChangeDeleteModelBtnStatus();

private:
    void initWidgets(const QString &view_name, const QString &view_source, const QStringList &algo_type);
    void initConnections();
    void initModelListWidget(QStringList);
    void addOneModelItem(QString model_name);
    bool validateViewName();
    bool validateViewSource();

private:
    Ui::ViewSetting *ui;
    QUuid m_uuid;
    bool is_deleted{false};
};
}  // namespace vtk::display

#endif  // VIEWSETTING_H
