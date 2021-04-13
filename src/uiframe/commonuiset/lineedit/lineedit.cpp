/****************************************************************************
 *  @file     lineedit.cpp
 *  @brief    通用文本编辑类
 *  @author   ZHU Leyan
 *  @email
 *  @version
 *  @date     2021.01.15
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "lineedit.h"
#include "common/vtkcommon.h"

namespace vtk::display
{

LineEdit::LineEdit(QWidget *parent) : QLineEdit(parent) {}

LineEdit::~LineEdit() {}

bool LineEdit::isEmpty()
{
    if (this->text().isEmpty())
        return true;
    return false;
}

bool LineEdit::exceedLength(int length)
{
    if (this->text().length() > length)
        return true;
    return false;
}

bool LineEdit::containsSpecial()
{
    if (vtk::common::containsAllSpecial(this->text()))
        return true;
    return false;
}

bool LineEdit::containsSpace()
{
    if (this->text().contains(" "))
        return true;
    return false;
}

bool LineEdit::containsChinese()
{
    if (this->text().contains(QRegExp("[\\x4e00-\\x9fa5]+")))
        return true;
    if (vtk::common::containsChineseSpecial(this->text()))
        return true;
    return false;
}

bool LineEdit::isNum()
{
    const auto &str = this->text().toStdString();
    if (!str.size())
    {
        return false;
    }
    for (int i = 0; i < str.size(); i++)
    {
        if (str.at(0) == '-')
        {
            if (str.size() > 1)
            {
                if (i > 0 && (str.at(i) > '9' || str.at(i) < '0'))
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
        else if (str.at(i) > '9' || str.at(i) < '0')
            return false;
    }
    return true;
}

bool LineEdit::isInt()
{
    bool is_int;
    this->text().toInt(&is_int);
    return is_int;
}

int LineEdit::toInt()
{
    return this->text().toInt();
}

}  // namespace vtk::display
