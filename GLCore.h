#pragma once

#include <QtWidgets/QWidget>
#include <QOpenGLWidget>
#include <QProgressBar>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QFrame>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <map>
#include <string>
#include <QLabel>
#include "ProjectSources/Inc/AudioOutput.h"

// 热键配置结构体
struct HotkeyConfig {
    QString expressionFile;   // 表情文件名（如 "Happy1.exp3.json"）
    QString trigger1;         // 主要触发键
    QString trigger2;         // 次要触发键
    QString trigger3;         // 第三触发键
    bool isActive;           // 是否激活
};

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
    void keyPressEvent(QKeyEvent* event) override;   // 添加键盘事件处理

private slots:
    void updateFavorability(); // 更新好感度的槽函数
    void onSendButtonClicked(); // 发送按钮点击槽函数
    void onTextChanged();       // 文本改变槽函数
    void onResponseTimeout();   // 响应超时槽函数

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
    
    // 对话框相关
    QFrame* chatContainer;         // 对话框容器
    QTextEdit* inputTextEdit;      // 文本输入框
    QPushButton* sendButton;       // 发送按钮
    QHBoxLayout* chatLayout;       // 对话框布局
    bool isReacted;                // 是否已反应（控制对话框显示）
    bool debugChatPosition;        // 是否启用对话框位置调试
    
    // 超时机制相关
    QTimer* timeoutTimer;          // 超时定时器
    QLabel* timeoutLabel;          // 超时提示标签
    bool isWaitingForResponse;     // 是否正在等待响应
    
    // 热键系统相关
    std::map<QString, HotkeyConfig> hotkeyConfigs;  // 键盘映射配置
    QString currentModelDirectory;                   // 当前模型目录
    bool isHotkeySystemReady;                       // 热键系统是否准备就绪
    
    // 音频监测相关
    AudioOutput* audioPlayer;                       // 音频播放器实例
    
    // 私有方法
    int readFavorabilityFromConfig(); // 从配置文件读取好感度
    void setupFavorabilityUI();      // 设置好感度UI
    void updateProgressBarStyle();   // 更新进度条样式
    void triggerExpression(const QString& expressionName); // 触发表情
    void initKeyMapping();           // 初始化键盘映射
    
    // 对话框相关方法
    void setupChatUI();              // 设置对话框UI
    void updateChatUIStyle();        // 更新对话框样式
    void adjustInputHeight();        // 调整输入框高度
    void sendMessage();              // 发送消息
    bool readIsReactedFromConfig();  // 从配置文件读取isReacted状态
    void updateConfigIsReacted(bool reacted); // 更新配置文件的isReacted值
    void updateChatPosition();       // 更新对话框位置
    
    // 超时机制相关方法
    void setupTimeoutUI();           // 设置超时UI
    void startResponseTimeout();     // 开始响应超时计时
    void handleResponseTimeout();    // 处理响应超时
    void hideTimeoutMessage();       // 隐藏超时消息
    
    // 新的热键系统方法
    bool loadVTubeHotkeyConfig(const QString& modelDirectory);  // 加载VTube热键配置
    bool loadExpressionFile(const QString& modelDirectory, const QString& expressionFile, const QString& expressionName); // 手动加载表情文件
    QString mapQtKeyToVTubeKey(int qtKey);                       // Qt按键映射到VTube按键格式
    void printHotkeyMappings();                                  // 打印热键映射信息
    void detectAndInitializeModel();                            // 检测并初始化模型
    
    // 音频监测相关方法
    void checkAndPlayAudioFile();                               // 检查并播放音频文件
    void cleanupTemporaryAudioFiles();                          // 清理临时音频文件

};