/****************************************************************************
 *  @file     modelsettingpage.h
 *  @brief    设置窗口模型设置界面类
 *  @author
 *  @email
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef MODELSETTINGPAGE_H
#define MODELSETTINGPAGE_H

#include <QWidget>

#include "common/vtkcommon.h"
#include "uiframe/commonuiset/arrowwidget/arrowwidget.h"

namespace Ui
{
class ModelSettingPage;
}

namespace vtk::display
{
class ModelSettingPage : public QWidget
{
    Q_OBJECT

public:
    explicit ModelSettingPage(QWidget *parent = nullptr);
    ~ModelSettingPage();
    void initModelConfigData();
    void saveModelConfigData();
    void cancelModelConfig();
    void disableFocus();
    bool validateModelSettings();

signals:
    void signalUpdateModelData();
    bool signalCheckModelNameUsed(QString);

private slots:
    void slotToggleGpu(bool);
    void slotShowInstruction();
    void on_detection_algo_tab_clicked();
    void on_classification_algo_tab_clicked();
    void on_segmentation_algo_tab_clicked();
    void on_ocr_algo_tab_clicked();

private:
    Ui::ModelSettingPage *ui;
    ArrowWidget *m_instruction{nullptr};

    void initWidgets();
    void initConnections();
    void switchTab(int);
    bool validatePage(int);
    bool eventFilter(QObject *wcg, QEvent *event) override;
    QStringList getModelNames(vtk::common::AlgoType);  // 获取某一算法类型下的所有模型名称
    bool checkDuplicate(vtk::common::AlgoType, QString);
};
}  // namespace vtk::display
#endif  // MODELSETTINGPAGE_H
