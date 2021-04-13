/****************************************************************************
 *  @file     modelitem.h
 *  @brief    设置窗口显示设置检测模型列表单个模型类
 *  @author   ZHU Leyan
 *  @email
 *  @version
 *  @date     2021.01.08
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef MODELITEM_H
#define MODELITEM_H

#include "common/vtkcommon.h"
#include <QWidget>

namespace Ui
{
class ModelItem;
}

namespace vtk::display
{
class ModelItem : public QWidget
{
    Q_OBJECT

public:
    struct ItemData
    {
        QString model_name{""};
        vtk::common::AlgoType model_type{0};
    };
    ModelItem(QString model_name, QWidget *parent = nullptr);
    ~ModelItem();
    QString getText();
    void setText(QString);
    void setId(int);
    void resetList();
    void changeItemStatus();

signals:
    QString signalGetModelType();
    void signalNameSelected(int);
    bool signalIsNameUsed(QString);
    void signalDelete();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void on_model_item_list_currentTextChanged(const QString &arg1);

private:
    Ui::ModelItem *ui;
    QString convertAlgoType(vtk::common::AlgoType);
    void setTypeText(QString);
    QUuid getUuid(QString);
};
}  // namespace vtk::display
Q_DECLARE_METATYPE(vtk::display::ModelItem::ItemData)
#endif  // MODELITEM_H
