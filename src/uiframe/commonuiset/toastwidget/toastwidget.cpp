#include "toastwidget.h"

#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QGuiApplication>
#include <QPainter>
#include <QScreen>
#include <QStyle>
#include <QStyleOption>
#include <QTimer>

namespace vtk::display
{

ToastWidget::ToastWidget(QWidget *parent)
    : QWidget(parent), m_timer(nullptr), m_show_animation(nullptr), m_hide_animation(nullptr)
{
    ui.setupUi(this);
    ui.icon->hide();
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::Tool);  // 无边框 无任务栏
    // add box shadow
    this->setAttribute(Qt::WA_TranslucentBackground);
    QGraphicsDropShadowEffect *pEffect = new QGraphicsDropShadowEffect(ui.frame);
    pEffect->setOffset(0, 0);
    pEffect->setColor(QColor(100, 100, 100));
    pEffect->setBlurRadius(10);
    ui.frame->setGraphicsEffect(pEffect);
    ui.main_layout->setMargin(10);
}

ToastWidget::~ToastWidget()
{
    if (m_show_animation)
    {
        m_show_animation->stop();
        delete m_show_animation;
        m_show_animation = nullptr;
    }
    if (m_timer)
    {
        m_timer->stop();
        delete m_timer;
        m_timer = nullptr;
    }
    if (m_hide_animation)
    {
        m_hide_animation->stop();
        delete m_hide_animation;
        m_hide_animation = nullptr;
    }
    close();
}

void ToastWidget::setText(const QString &text)
{
    ui.label->setText(text);
}

void ToastWidget::showAnimation(int timeout /*= 1000*/)
{
    // 开始动画
    m_show_animation = new QPropertyAnimation(this, "windowOpacity");
    m_show_animation->setDuration(500);
    m_show_animation->setStartValue(0);
    m_show_animation->setEndValue(1);
    m_show_animation->start();
    show();

    m_hide_animation = new QPropertyAnimation(this, "windowOpacity");
    m_hide_animation->setDuration(500);
    m_hide_animation->setStartValue(1);
    m_hide_animation->setEndValue(0);
    connect(m_hide_animation, &QPropertyAnimation::finished, [&] { close(); });

    m_timer = new QTimer;
    m_timer->start(timeout);
    connect(m_timer, &QTimer::timeout, [&] {
        // 结束动画
        if (m_hide_animation)
        {
            m_hide_animation->start();
        }
    });
}

void ToastWidget::showTip(const QString &text, QWidget *parent /*= nullptr*/)
{
    ToastWidget *toast = new ToastWidget(parent);
    toast->setWindowFlags(toast->windowFlags() | Qt::WindowStaysOnTopHint);  // 置顶
    toast->ui.frame->setStyleSheet("background-color: #006db2;"
                                   "border-radius: 15px;");

    toast->setText(text);
    toast->ui.label->setStyleSheet("color: #FFFFFF; font-size: 12px;");
    toast->adjustSize();  //设置完文本后调整下大小

    // 测试显示位于主屏的50%高度位置
    QScreen *pScreen = QGuiApplication::primaryScreen();
    toast->move((pScreen->size().width() - toast->width()) / 2, (pScreen->size().height() - toast->height()) / 2);
    toast->showAnimation();
}

void ToastWidget::showSaveTip(const QString &text, const QString &icon_uri, QWidget *parent /*= nullptr*/)
{
    ToastWidget *toast = new ToastWidget(parent);
    toast->setWindowFlags(toast->windowFlags() | Qt::WindowStaysOnTopHint);  // 置顶
    toast->setText(text);

    toast->ui.frame->setStyleSheet("background-color: #FFFFFF;border-radius: 4px;");
    toast->ui.frame_layout->setContentsMargins(16, 0, 0, 0);
    toast->ui.icon->setMaximumSize(14, 14);
    toast->ui.icon->setPixmap(QPixmap(icon_uri));
    toast->ui.icon->show();
    toast->ui.label->setStyleSheet("color: #000000;padding-right: 10px; font-size: 12px;");

    toast->adjustSize();  //设置完文本后调整下大小

    QScreen *pScreen = QGuiApplication::primaryScreen();
    toast->move((pScreen->size().width() - toast->width()) / 2, (pScreen->size().height() - toast->height()) / 4);
    toast->showAnimation(4000);
}

void ToastWidget::showErrorTip(const QString &text, QWidget *parent /*= nullptr*/)
{
    ToastWidget *toast = new ToastWidget(parent);
    toast->setWindowFlags(toast->windowFlags() | Qt::WindowStaysOnTopHint);  // 置顶
    toast->ui.frame->setStyleSheet("background-color: #f44747;"
                                   "border-radius: 15px;");

    toast->setText(text);
    toast->ui.label->setStyleSheet("color: white;font-size: 12px;");
    toast->adjustSize();  //设置完文本后调整下大小

    // 测试显示位于主屏的50%高度位置
    QScreen *pScreen = QGuiApplication::primaryScreen();
    toast->move((pScreen->size().width() - toast->width()) / 2, (pScreen->size().height() - toast->height()) / 2);
    toast->showAnimation(2000);
}

void ToastWidget::paintEvent(QPaintEvent *e)
{
    (void)e;
    QStyleOption opt;
    opt.init(this);
    QPainter paint(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &paint, this);
}

}  // namespace vtk::display
