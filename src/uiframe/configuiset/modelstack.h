/****************************************************************************
 *  @file     modelstack.h
 *  @brief    设置窗口单类模型设置窗口类
 *  @author   ZHU Leyan
 *  @email
 *  @version
 *  @date     2021.01.07
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef MODELSTACK_H
#define MODELSTACK_H

#include "addmodelwidget.h"
#include "common/vtkcommon.h"
#include "configdatamanager/configmanager.h"
#include "modelsetting.h"
#include <QVBoxLayout>
#include <QWidget>

namespace Ui
{
class ModelStack;
}

namespace vtk::display
{
using AlgoConfigItem = vtk::config::AlgoConfigItem;
class ModelStack : public QWidget
{
    Q_OBJECT

public:
    explicit ModelStack(QWidget *parent = nullptr);
    ~ModelStack();
    void setModelType(vtk::common::AlgoType);
    bool validatePage();
    QStringList getModelNames();
    void disableFocus();
    void addModel(const QUuid &model_id, const QString &model_name, const QString &model_path);
    QVector<AlgoConfigItem> getModelData();
    void clearModels();

signals:
    bool signalCheckDuplicate(QString);

private slots:
    void slotDeleteModel(int);
    bool slotCheckDuplicate(int);

private:
    Ui::ModelStack *ui;
    QVector<QSharedPointer<ModelSetting>> m_model_settings;
    vtk::common::AlgoType m_type;
    QVBoxLayout *m_model_stack_container{nullptr};
    AddModelWidget *m_add_model_widget{nullptr};

private:
    void initWidgets();
    void initConnections();
    bool checkModelNameUsed(QString);
};
}  // namespace vtk::display
#endif  // MODELSTACK_H
