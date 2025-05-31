#include "LAppDelegate.hpp" // 必须要放在第一个，不然包老实的
#include "LAppView.hpp"
#include "LAppPal.hpp"
#include "LAppLive2DManager.hpp"
#include "LAppDefine.hpp"
#include "GLCore.h"
#include <QTimer>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDir>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QDebug>

#ifdef _WIN32
#include <windows.h>
#include <dwmapi.h>
#endif

GLCore::GLCore(int w, int h, QWidget *parent)
    : QOpenGLWidget(parent), isFrameless(true), initialWidth(w), initialHeight(h), 
      favorabilityBar(nullptr), favorabilityTimer(nullptr), currentFavorability(0)  // 简化初始化
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
    
    // 初始化好感度UI
    setupFavorabilityUI();

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
    // 清理好感度相关资源
    if (favorabilityTimer) {
        favorabilityTimer->stop();
    }
    // Qt的父子关系会自动清理QProgressBar和QTimer等组件
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
        
        // 调试输出
        qDebug() << "Switching to frameless:" << isFrameless;
        
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
        
        // 在show()之后设置进度条可见性，防止被重置
        if (favorabilityBar) {
            bool shouldShow = !isFrameless;  // 有边框时显示
            favorabilityBar->setVisible(shouldShow);
            qDebug() << "Setting progress bar visible:" << shouldShow;
            qDebug() << "Progress bar actual visible:" << favorabilityBar->isVisible();
        }
        
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

void GLCore::resizeEvent(QResizeEvent* event)
{
    QOpenGLWidget::resizeEvent(event);
    
    // 调整进度条位置和大小
    if (favorabilityBar) {
        int margin = 5;
        favorabilityBar->setGeometry(margin, margin, width() - 2 * margin, 20);
    }
}

void GLCore::wheelEvent(QWheelEvent* event)
{
    // 不再处理缩放，直接接受事件防止传递
    event->accept();
}

// 从配置文件读取好感度
int GLCore::readFavorabilityFromConfig() {
    QString configPath = QDir::currentPath() + "/contact/config.json";
    QFile file(configPath);
    
    if (!file.open(QIODevice::ReadOnly)) {
        return currentFavorability; // 如果读取失败，返回当前值
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    
    if (error.error != QJsonParseError::NoError) {
        return currentFavorability; // 如果解析失败，返回当前值
    }
    
    QJsonObject obj = doc.object();
    int favorability = obj["favorability"].toInt(0);
    
    // 确保好感度在0-100范围内
    return qBound(0, favorability, 100);
}

// 设置好感度UI
void GLCore::setupFavorabilityUI() {
    // 创建好感度进度条作为GLCore的子widget
    favorabilityBar = new QProgressBar(this);  // 重要：设置this作为父对象
    favorabilityBar->setRange(0, 100);
    favorabilityBar->setValue(0);
    favorabilityBar->setFixedHeight(20);
    favorabilityBar->setTextVisible(true);
    favorabilityBar->setFormat("好感度: %p%");
    
    // 设置进度条样式
    updateProgressBarStyle();
    
    // 初始状态下隐藏进度条（因为默认是无边框）
    favorabilityBar->setVisible(false);
    qDebug() << "Initial setup: frameless=" << isFrameless << ", progress bar visible=" << favorabilityBar->isVisible();
    
    // 创建并启动定时器
    favorabilityTimer = new QTimer(this);
    connect(favorabilityTimer, &QTimer::timeout, this, &GLCore::updateFavorability);
    favorabilityTimer->start(1000); // 每1秒更新一次
    
    // 立即读取一次好感度
    updateFavorability();
    
    // 设置进度条的初始位置
    int margin = 5;
    favorabilityBar->setGeometry(margin, margin, width() - 2 * margin, 20);
}

// 更新进度条样式
void GLCore::updateProgressBarStyle() {
    if (!favorabilityBar) return;
    
    QString style = R"(
        QProgressBar {
            border: 2px solid #555555;
            border-radius: 10px;
            background-color: #CC4444;
            text-align: center;
            color: white;
            font-weight: bold;
            font-size: 12px;
            padding: 0px;
        }
        QProgressBar::chunk {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1, 
                                       stop:0 #88DD88, stop:1 #66BB66);
            border-radius: 8px;
            margin: 0px;
            border: none;
        }
    )";
    
    favorabilityBar->setStyleSheet(style);
}

// 更新好感度槽函数
void GLCore::updateFavorability() {
    int newFavorability = readFavorabilityFromConfig();
    if (newFavorability != currentFavorability) {
        currentFavorability = newFavorability;
        if (favorabilityBar) {
            favorabilityBar->setValue(currentFavorability);
        }
    }
}