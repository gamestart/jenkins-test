/****************************************************************************
 *  @file     modelsetting.h
 *  @brief    设置窗口单个模型设置窗口类
 *  @author   ZHU Leyan
 *  @email
 *  @version
 *  @date     2021.01.07
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef MODELSETTING_H
#define MODELSETTING_H

#include "common/vtkcommon.h"

#include <QUuid>
#include <QWidget>

namespace Ui
{
class ModelSetting;
}

namespace vtk::display
{
class ModelSetting : public QWidget
{
    Q_OBJECT

public:
    ModelSetting(QWidget *parent, int index, const QUuid &model_id, const QString &model_name,
                 const QString &model_path, vtk::common::AlgoType type);
    ~ModelSetting();
    int getId();
    void setId(int);
    QString getModelName();
    void setModelName(QString);
    QString getModelPath();
    QUuid getModelId();
    bool validateModelSetting();
    void disableFocus();

signals:
    void signalDeleteModel(int);
    bool signalCheckDuplicate(int);

private slots:
    void on_delete_model_btn_clicked();
    void on_model_path_btn_clicked();

private:
    Ui::ModelSetting *ui;
    int m_id;
    vtk::common::AlgoType m_type;
    QUuid m_uuid;

private:
    void initWidgets(const QString &model_name, const QString &model_path);
    void initConnections();
    bool validateModelName();
    bool validateModelPath();
    bool eventFilter(QObject *wcg, QEvent *event);
};
}  // namespace vtk::display
#endif  // MODELSETTING_H
