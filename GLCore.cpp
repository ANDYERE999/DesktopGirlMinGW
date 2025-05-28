#include "LAppDelegate.hpp" // 必须要放在第一个，不然包老实的
#include "LAppView.hpp"
#include "LAppPal.hpp"
#include "LAppLive2DManager.hpp"
#include "LAppDefine.hpp"
#include "GLCore.h"
#include <QTimer>
#include <QMouseEvent>







GLCore::GLCore(int w, int h, QWidget *parent)
    : QOpenGLWidget(parent)
{
    setFixedSize(w, h);
    
    // 设置OpenGL格式以支持透明度
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setAlphaBufferSize(8);  // 关键：设置alpha缓冲区
    format.setSamples(4);          // 抗锯齿
    setFormat(format);
    
    //this->setAttribute(Qt::WA_DeleteOnClose);       // 窗口关闭时自动释放内存
    this->setWindowFlag(Qt::FramelessWindowHint); // 设置无边框窗口
    this->setWindowFlag(Qt::WindowStaysOnTopHint); // 设置窗口始终在顶层
    //this->setWindowFlag(Qt::Tool); // 不在应用程序图标
    //this->setAttribute(Qt::WA_TranslucentBackground); // 设置窗口背景透明 - 注释掉以显示OpenGL背景色

    

    QTimer* timer = new QTimer();
    connect(timer, &QTimer::timeout, [=]() {
        update();
        });
    timer->start((1.0 / 60) * 1000);    // 60FPS



}

GLCore::~GLCore()
{
    
}



void GLCore::mouseMoveEvent(QMouseEvent* event)
{
    LAppDelegate::GetInstance()->GetView()->OnTouchesMoved(event->x(), event->y());

    if (isLeftPressed) {
        this->move(event->pos() - this->currentPos + this->pos());
    }

    

    // 允许事件继续传递，将鼠标事件传递给主窗口，实现鼠标拖动无边框窗口
    event->ignore();
}

void GLCore::mousePressEvent(QMouseEvent* event)
{
    LAppDelegate::GetInstance()->GetView()->OnTouchesBegan(event->x(), event->y());

    if (event->button() == Qt::LeftButton) {
        this->isLeftPressed = true;
        this->currentPos = event->pos();
    }
    // TODO: 右键菜单等
    if (event->button() == Qt::RightButton) {
        
        // 设置窗口大小
        //LAppDelegate::GetInstance()->resize(400, 400);
        //this->setFixedSize(400, 400);
        LAppLive2DManager::GetInstance()->LoadModelFromPath("Resources/Mao/", "Mao.model3.json");

        this->isRightPressed = true;
    }


    
    // 允许事件继续传递
    event->ignore();
}

void GLCore::mouseReleaseEvent(QMouseEvent* event)
{
    LAppDelegate::GetInstance()->GetView()->OnTouchesEnded(event->x(), event->y());

    if (event->button() == Qt::LeftButton) {
        isLeftPressed = false;
    }
    if (event->button() == Qt::RightButton) {
        isRightPressed = false;
    }

    
    // 允许事件继续传递
    event->ignore();
}

void GLCore::initializeGL()
{
    // 设置OpenGL状态以支持透明度渲染
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);  // 禁用深度测试以确保透明度正常工作
    
    LAppDelegate::GetInstance()->Initialize(this);
    
    // 选择模型
}

void GLCore::paintGL()
{
    // 设置背景颜色 - 可以选择以下任意一种颜色
    // glClearColor(0.0f, 0.0f, 0.0f, 0.0f);  // 完全透明（原设置）
    // glClearColor(1.0f, 1.0f, 1.0f, 1.0f);  // 白色背景
    // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // 黑色背景
    // glClearColor(0.2f, 0.3f, 0.8f, 1.0f);  // 蓝色背景
    // glClearColor(0.8f, 0.2f, 0.2f, 1.0f);  // 红色背景
    glClearColor(0.2f, 0.8f, 0.2f, 1.0f);  // 绿色背景
    // glClearColor(0.5f, 0.5f, 0.5f, 1.0f);  // 灰色背景
    //glClearColor(0.9f, 0.9f, 0.9f, 1.0f);  // 浅灰色背景
    glClear(GL_COLOR_BUFFER_BIT);
    
    LAppDelegate::GetInstance()->update();
    
}

void GLCore::resizeGL(int w, int h)
{
    LAppDelegate::GetInstance()->resize(w, h);
}