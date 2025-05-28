#pragma once

#include <QtWidgets/QWidget>
#include <QOpenGLWidget>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QApplication>

// 前向声明
class SettingsDialog;
class ChatWidget;

class GLCore : public QOpenGLWidget
{
    Q_OBJECT

public:
    GLCore(QWidget *parent = nullptr);
    GLCore(int width, int height, QWidget* parent = nullptr);
    ~GLCore();


    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;


    // 重写函数
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

private slots:
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onExitTriggered();
    void onShowHideTriggered();
    void onSettingsTriggered();
    void onChatToggleTriggered();

private:
    void setupSystemTray();
    
    bool isLeftPressed; // 鼠标左键是否按下
    bool isRightPressed;// 鼠标右键是否按下
    QPoint currentPos;  // 当前鼠标位置
    
    // 系统托盘相关
    QSystemTrayIcon* m_systemTray;
    QMenu* m_trayMenu;
    QAction* m_exitAction;
    QAction* m_showHideAction;
    QAction* m_settingsAction;
    QAction* m_chatToggleAction;
    
    // 对话框
    SettingsDialog* m_settingsDialog;
    ChatWidget* m_chatWidget;

};