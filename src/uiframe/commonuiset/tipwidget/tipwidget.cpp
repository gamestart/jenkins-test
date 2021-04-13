/****************************************************************************
 *  @file     tipwidget.cpp
 *  @brief    弹出提示类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.10
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/

#include "tipwidget.h"
#include "ui_tipwidget.h"
#include <QMovie>
#include <QPropertyAnimation>

namespace vtk::display
{

TipWidget::TipWidget(const QString &text, const QString &movie_path, QWidget *parent)
    : QWidget(parent), ui(new Ui::TipWidget), m_text(text), m_movie_path(movie_path)
{
    ui->setupUi(this);
    init();
}

TipWidget::~TipWidget()
{
    delete ui;
}

void TipWidget::setText(const QString &text)
{
    auto parent_wid = dynamic_cast<QWidget *>(parent());
    if (parent_wid)
        move((parent_wid->width() - width()) / 2, (parent_wid->height() - height()) / 2);

    m_text = text;
    ui->tip_lab->setText(m_text);
    adjustSize();
    m_show_animation->start();
    show();
}

void TipWidget::init()
{
    initWidget();
    initConnects();
}

void TipWidget::initWidget()
{
    ui->tip_lab->setText(m_text);
    QMovie *movie = new QMovie(m_movie_path);
    movie->setParent(this);
    ui->animation_lab->setMovie(movie);
    movie->start();
    // ui->animation_lab->setPixmap(QPixmap("://uiframe/images/camera_config/loading.svg"));

    m_show_animation = new QPropertyAnimation(this, "windowOpacity", this);
    m_show_animation->setDuration(1000);
    m_show_animation->setStartValue(0);
    m_show_animation->setEndValue(1);

    m_hide_animation = new QPropertyAnimation(this, "windowOpacity", this);
    m_hide_animation->setDuration(1000);
    m_hide_animation->setStartValue(1);
    m_hide_animation->setEndValue(0);
}

void TipWidget::initConnects()
{
    connect(m_show_animation, &QPropertyAnimation::finished, [=] {
        m_hide_animation->start();
        connect(m_hide_animation, &QPropertyAnimation::finished, [=] { hide(); });
    });
}

}  // namespace vtk::display
