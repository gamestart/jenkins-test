/****************************************************************************
 *  @file     lineedit.h
 *  @brief    通用文本编辑类
 *  @author   ZHU Leyan
 *  @email
 *  @version
 *  @date     2021.01.15
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <QLineEdit>
#include <QWidget>

namespace Ui
{
class LineEdit;
}

namespace vtk::display
{

class LineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit LineEdit(QWidget *parent = nullptr);
    ~LineEdit();
    bool isEmpty();
    bool exceedLength(int);
    bool containsSpecial();
    bool containsSpace();
    bool containsChinese();
    bool isNum();
    bool isInt();
    int toInt();
};

}  // namespace vtk::display
#endif  // LINEEDIT_H
