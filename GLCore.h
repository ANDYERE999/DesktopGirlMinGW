#pragma once

#include <QtWidgets/QWidget>
#include <QOpenGLWidget>
#include <QProgressBar>
#include <QTimer>
#include <QVBoxLayout>



class GLCore : public QOpenGLWidget
{
    Q_OBJECT

public:
    GLCore(int width, int height, QWidget* parent = nullptr);
    ~GLCore();


    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;


    // 重写函数
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    void resizeEvent(QResizeEvent* event) override;  // 添加resize事件处理

private slots:
    void updateFavorability(); // 更新好感度的槽函数

private:
    bool isLeftPressed; // 鼠标左键是否按下
    bool isRightPressed;// 鼠标右键是否按下
    QPoint currentPos;  // 当前鼠标位置

    // 边框显示状态
    bool isFrameless;   // 是否无边框状态
    
    // 初始窗口大小
    int initialWidth;
    int initialHeight;
    
    // 好感度相关
    QProgressBar* favorabilityBar;  // 好感度进度条
    QTimer* favorabilityTimer;     // 定时器
    int currentFavorability;       // 当前好感度值
    
    // 私有方法
    int readFavorabilityFromConfig(); // 从配置文件读取好感度
    void setupFavorabilityUI();      // 设置好感度UI
    void updateProgressBarStyle();   // 更新进度条样式

};