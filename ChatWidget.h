#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QTextEdit>
#include <QScrollArea>

class ChatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWidget(QWidget *parent = nullptr);
    ~ChatWidget();

    void showChatInput();
    void hideChatInput();
    void toggleChat();
    bool isChatVisible() const { return m_isVisible; }

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onSendMessage();
    void onServerResponse();
    void onNetworkError(QNetworkReply::NetworkError error);
    void onTypingAnimation();
    void checkServerStatus();

private:
    void setupUI();
    void setupAnimations();
    void setupNetworking();
    void sendToAIServer(const QString &message);
    void showResponse(const QString &response);
    void showTypingIndicator();
    void hideTypingIndicator();
    void updateChatHistory(const QString &message, bool isUser = true);
    
    // UI组件
    QLineEdit *m_inputEdit;
    QPushButton *m_sendButton;
    QPushButton *m_toggleButton;
    QLabel *m_typingLabel;
    QTextEdit *m_historyDisplay;
    QScrollArea *m_historyScroll;
    
    // 布局
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_inputLayout;
    QWidget *m_inputContainer;
    QWidget *m_historyContainer;
    
    // 动画
    QPropertyAnimation *m_showAnimation;
    QPropertyAnimation *m_hideAnimation;
    QTimer *m_typingTimer;
    
    // 网络
    QNetworkAccessManager *m_networkManager;
    QNetworkReply *m_currentReply;
    
    // 状态
    bool m_isVisible;
    bool m_isTyping;
    QString m_serverUrl;
    
    // 样式
    QString m_inputStyle;
    QString m_buttonStyle;
    QString m_containerStyle;
}; 