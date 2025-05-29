#include "LAppDelegate.hpp" // 必须要放在第一个，不然包老实的
#include "LAppView.hpp"
#include "LAppPal.hpp"
#include "LAppLive2DManager.hpp"
#include "LAppDefine.hpp"
#include "GLCore.h"
#include <QTimer>
#include <QMouseEvent>
#include <QWheelEvent>

#ifdef _WIN32
#include <windows.h>
#include <dwmapi.h>
#endif

GLCore::GLCore(int w, int h, QWidget *parent)
    : QOpenGLWidget(parent), isFrameless(true), initialWidth(w), initialHeight(h)  // 保存初始大小
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
    this->setAttribute(Qt::WA_TranslucentBackground); // 设置窗口背景透明
    

    // 显示窗口以获取有效的窗口句柄
    this->show();
    
    // 使用Windows DWM API设置透明背景
#ifdef _WIN32
    auto viewId = this->winId();
    DWM_BLURBEHIND bb = { 0 };
    HRGN hRgn = CreateRectRgn(0, 0, -1, -1); // 应用透明的矩形范围，整个窗口客户区
    bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
    bb.hRgnBlur = hRgn;
    bb.fEnable = TRUE;
    DwmEnableBlurBehindWindow((HWND)viewId, &bb);
    DeleteObject(hRgn); // 清理资源
#endif

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
    LAppDelegate::GetInstance()->GetView()->OnTouchesMoved(event->position().x(), event->position().y());

    if (isLeftPressed) {
        this->move(event->pos() - this->currentPos + this->pos());
    }

    

    // 允许事件继续传递，将鼠标事件传递给主窗口，实现鼠标拖动无边框窗口
    event->ignore();
}

void GLCore::mousePressEvent(QMouseEvent* event)
{
    LAppDelegate::GetInstance()->GetView()->OnTouchesBegan(event->position().x(), event->position().y());

    if (event->button() == Qt::LeftButton) {
        this->isLeftPressed = true;
        this->currentPos = event->pos();
    }
    // 右键切换边框显示
    if (event->button() == Qt::RightButton) {
        // 切换边框状态
        isFrameless = !isFrameless;
        
        if (isFrameless) {
            // 设置为无边框
            this->setWindowFlag(Qt::FramelessWindowHint, true);
            // 使用当前窗口大小而不是初始大小
            QSize currentSize = this->size();
            this->setFixedSize(currentSize.width(), currentSize.height());
        } else {
            // 显示边框
            this->setWindowFlag(Qt::FramelessWindowHint, false);
            // 移除固定大小限制，允许调整大小
            this->setMinimumSize(200, 150);  // 设置最小大小
            this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);  // 移除最大大小限制
        }
        
        // 重新显示窗口以应用更改
        this->show();
        
        this->isRightPressed = true;
    }

    // 允许事件继续传递
    event->ignore();
}

void GLCore::mouseReleaseEvent(QMouseEvent* event)
{
    LAppDelegate::GetInstance()->GetView()->OnTouchesEnded(event->position().x(), event->position().y());

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
    // 设置完全透明背景以配合DWM透明效果
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);  // RGBA，A=0表示完全透明
    glClear(GL_COLOR_BUFFER_BIT);
    
    LAppDelegate::GetInstance()->update();
    
}

void GLCore::resizeGL(int w, int h)
{
    LAppDelegate::GetInstance()->resize(w, h);
}

void GLCore::wheelEvent(QWheelEvent* event)
{
    // 不再处理缩放，直接接受事件防止传递
    event->accept();
}