#include "ChatWidget.h"
#include <QPainter>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QApplication>
#include <QScreen>
#include <QScrollBar>
#include <QDateTime>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

ChatWidget::ChatWidget(QWidget *parent)
    : QWidget(parent)
    , m_inputEdit(nullptr)
    , m_sendButton(nullptr)
    , m_toggleButton(nullptr)
    , m_typingLabel(nullptr)
    , m_historyDisplay(nullptr)
    , m_isVisible(false)
    , m_isTyping(false)
    , m_serverUrl("http://127.0.0.1:5000")
{
    setupUI();
    setupAnimations();
    setupNetworking();
    
    // 初始时隐藏
    setVisible(false);
    
    // 检查服务器状态
    QTimer::singleShot(1000, this, &ChatWidget::checkServerStatus);
}

ChatWidget::~ChatWidget()
{
    if (m_currentReply) {
        m_currentReply->abort();
    }
}

void ChatWidget::setupUI()
{
    // 设置窗口属性
    setWindowFlags(Qt::Widget);
    setAttribute(Qt::WA_TranslucentBackground);
    
    // 主布局
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(10, 5, 10, 10);
    m_mainLayout->setSpacing(8);
    
    // 历史记录容器（可展开）
    m_historyContainer = new QWidget(this);
    m_historyContainer->setFixedHeight(0); // 初始隐藏
    m_historyContainer->setStyleSheet(
        "QWidget { "
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 rgba(255, 255, 255, 240), stop:1 rgba(248, 250, 255, 240)); "
        "border-radius: 15px; "
        "border: 2px solid rgba(200, 220, 255, 180); "
        "}"
    );
    
    QVBoxLayout *historyLayout = new QVBoxLayout(m_historyContainer);
    historyLayout->setContentsMargins(12, 8, 12, 8);
    
    m_historyDisplay = new QTextEdit(m_historyContainer);
    m_historyDisplay->setFixedHeight(200);
    m_historyDisplay->setReadOnly(true);
    m_historyDisplay->setStyleSheet(
        "QTextEdit { "
        "background: transparent; "
        "border: none; "
        "font-family: 'Microsoft YaHei', Arial; "
        "font-size: 11px; "
        "color: #333; "
        "}"
        "QScrollBar:vertical { "
        "background: rgba(200, 200, 200, 100); "
        "width: 6px; border-radius: 3px; "
        "}"
        "QScrollBar::handle:vertical { "
        "background: rgba(150, 150, 150, 150); "
        "border-radius: 3px; "
        "}"
    );
    
    historyLayout->addWidget(m_historyDisplay);
    
    // 输入容器
    m_inputContainer = new QWidget(this);
    m_inputContainer->setFixedHeight(50);
    
    // 设置输入容器样式 - 可爱的胶囊形状
    m_containerStyle = 
        "QWidget { "
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 rgba(255, 255, 255, 250), stop:1 rgba(248, 250, 255, 250)); "
        "border-radius: 25px; "
        "border: 3px solid rgba(200, 220, 255, 200); "
        "box-shadow: 0 4px 15px rgba(0, 0, 0, 20); "
        "}";
    
    m_inputContainer->setStyleSheet(m_containerStyle);
    
    // 添加阴影效果
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(15);
    shadowEffect->setColor(QColor(0, 0, 0, 30));
    shadowEffect->setOffset(0, 3);
    m_inputContainer->setGraphicsEffect(shadowEffect);
    
    // 输入布局
    m_inputLayout = new QHBoxLayout(m_inputContainer);
    m_inputLayout->setContentsMargins(15, 8, 8, 8);
    m_inputLayout->setSpacing(8);
    
    // 输入框
    m_inputEdit = new QLineEdit(m_inputContainer);
    m_inputEdit->setPlaceholderText("和我聊天吧~ 💭");
    
    m_inputStyle = 
        "QLineEdit { "
        "background: transparent; "
        "border: none; "
        "font-family: 'Microsoft YaHei', Arial; "
        "font-size: 12px; "
        "color: #333; "
        "padding: 5px; "
        "}"
        "QLineEdit:focus { "
        "background: rgba(255, 255, 255, 100); "
        "border-radius: 8px; "
        "}";
    
    m_inputEdit->setStyleSheet(m_inputStyle);
    
    // 发送按钮
    m_sendButton = new QPushButton("💕", m_inputContainer);
    m_sendButton->setFixedSize(34, 34);
    
    m_buttonStyle = 
        "QPushButton { "
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 rgba(255, 182, 193, 200), stop:1 rgba(255, 160, 180, 200)); "
        "border: 2px solid rgba(255, 105, 180, 150); "
        "border-radius: 17px; "
        "font-size: 14px; "
        "color: white; "
        "font-weight: bold; "
        "}"
        "QPushButton:hover { "
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 rgba(255, 105, 180, 220), stop:1 rgba(255, 20, 147, 220)); "
        "}"
        "QPushButton:pressed { "
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 rgba(255, 20, 147, 240), stop:1 rgba(219, 112, 147, 240)); "
        "}";
    
    m_sendButton->setStyleSheet(m_buttonStyle);
    
    // 打字指示器
    m_typingLabel = new QLabel("AI正在思考中... 💭", m_inputContainer);
    m_typingLabel->setStyleSheet(
        "QLabel { "
        "color: rgba(100, 100, 100, 180); "
        "font-family: 'Microsoft YaHei', Arial; "
        "font-size: 10px; "
        "background: transparent; "
        "}"
    );
    m_typingLabel->setVisible(false);
    
    // 布局
    m_inputLayout->addWidget(m_inputEdit, 1);
    m_inputLayout->addWidget(m_sendButton);
    
    m_mainLayout->addWidget(m_historyContainer);
    m_mainLayout->addWidget(m_inputContainer);
    
    // 连接信号
    connect(m_inputEdit, &QLineEdit::returnPressed, this, &ChatWidget::onSendMessage);
    connect(m_sendButton, &QPushButton::clicked, this, &ChatWidget::onSendMessage);
}

void ChatWidget::setupAnimations()
{
    // 显示动画
    m_showAnimation = new QPropertyAnimation(this, "geometry", this);
    m_showAnimation->setDuration(300);
    m_showAnimation->setEasingCurve(QEasingCurve::OutCubic);
    
    // 隐藏动画
    m_hideAnimation = new QPropertyAnimation(this, "geometry", this);
    m_hideAnimation->setDuration(250);
    m_hideAnimation->setEasingCurve(QEasingCurve::InCubic);
    
    // 打字动画定时器
    m_typingTimer = new QTimer(this);
    connect(m_typingTimer, &QTimer::timeout, this, &ChatWidget::onTypingAnimation);
}

void ChatWidget::setupNetworking()
{
    m_networkManager = new QNetworkAccessManager(this);
    m_currentReply = nullptr;
}

void ChatWidget::showChatInput()
{
    if (m_isVisible) return;
    
    m_isVisible = true;
    setVisible(true);
    
    // 计算位置（在父窗口下方居中）
    if (parentWidget()) {
        QWidget *parent = parentWidget();
        int x = parent->width() / 2 - width() / 2;
        int y = parent->height() - height() - 20;
        
        QRect startGeometry(x, parent->height(), width(), height());
        QRect endGeometry(x, y, width(), height());
        
        setGeometry(startGeometry);
        
        m_showAnimation->setStartValue(startGeometry);
        m_showAnimation->setEndValue(endGeometry);
        m_showAnimation->start();
    }
    
    // 聚焦输入框
    m_inputEdit->setFocus();
}

void ChatWidget::hideChatInput()
{
    if (!m_isVisible) return;
    
    if (parentWidget()) {
        QWidget *parent = parentWidget();
        QRect startGeometry = geometry();
        QRect endGeometry(startGeometry.x(), parent->height(), startGeometry.width(), startGeometry.height());
        
        m_hideAnimation->setStartValue(startGeometry);
        m_hideAnimation->setEndValue(endGeometry);
        
        connect(m_hideAnimation, &QPropertyAnimation::finished, this, [this]() {
            setVisible(false);
            m_isVisible = false;
        }, Qt::UniqueConnection);
        
        m_hideAnimation->start();
    }
}

void ChatWidget::toggleChat()
{
    if (m_isVisible) {
        hideChatInput();
    } else {
        showChatInput();
    }
}

void ChatWidget::paintEvent(QPaintEvent *event)
{
    // 透明背景绘制
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 绘制背景（可选）
    painter.fillRect(rect(), QColor(0, 0, 0, 0));
    
    QWidget::paintEvent(event);
}

void ChatWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    
    // 调整历史记录显示区域
    if (m_historyContainer && m_historyContainer->height() > 0) {
        // 重新计算位置
        if (parentWidget() && m_isVisible) {
            QWidget *parent = parentWidget();
            int x = parent->width() / 2 - width() / 2;
            int y = parent->height() - height() - 20;
            move(x, y);
        }
    }
}

void ChatWidget::onSendMessage()
{
    QString message = m_inputEdit->text().trimmed();
    if (message.isEmpty()) return;
    
    // 更新聊天历史
    updateChatHistory(message, true);
    
    // 清空输入框
    m_inputEdit->clear();
    
    // 显示正在输入指示器
    showTypingIndicator();
    
    // 发送到AI服务器
    sendToAIServer(message);
}

void ChatWidget::sendToAIServer(const QString &message)
{
    QJsonObject json;
    json["message"] = message;
    
    QJsonDocument doc(json);
    QByteArray data = doc.toJson();
    
    QNetworkRequest request(QUrl(m_serverUrl + "/chat"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    m_currentReply = m_networkManager->post(request, data);
    connect(m_currentReply, &QNetworkReply::finished, this, &ChatWidget::onServerResponse);
    connect(m_currentReply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred),
            this, &ChatWidget::onNetworkError);
}

void ChatWidget::onServerResponse()
{
    hideTypingIndicator();
    
    if (!m_currentReply) return;
    
    if (m_currentReply->error() == QNetworkReply::NoError) {
        QByteArray data = m_currentReply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject response = doc.object();
        
        if (response.contains("response")) {
            QString aiResponse = response["response"].toString();
            updateChatHistory(aiResponse, false);
        } else if (response.contains("error")) {
            QString error = response["error"].toString();
            updateChatHistory("❌ " + error, false);
        }
    } else {
        updateChatHistory("❌ 网络连接错误", false);
    }
    
    m_currentReply->deleteLater();
    m_currentReply = nullptr;
}

void ChatWidget::onNetworkError(QNetworkReply::NetworkError error)
{
    hideTypingIndicator();
    
    QString errorMsg;
    switch (error) {
    case QNetworkReply::ConnectionRefusedError:
        errorMsg = "❌ 无法连接到AI服务器，请确保Python服务器正在运行";
        break;
    case QNetworkReply::HostNotFoundError:
        errorMsg = "❌ 找不到AI服务器";
        break;
    default:
        errorMsg = "❌ 网络错误";
        break;
    }
    
    updateChatHistory(errorMsg, false);
}

void ChatWidget::showTypingIndicator()
{
    m_isTyping = true;
    m_sendButton->setEnabled(false);
    m_sendButton->setText("⏳");
    m_typingTimer->start(500);
}

void ChatWidget::hideTypingIndicator()
{
    m_isTyping = false;
    m_sendButton->setEnabled(true);
    m_sendButton->setText("💕");
    m_typingTimer->stop();
}

void ChatWidget::onTypingAnimation()
{
    static int dots = 0;
    QString dotStr = QString(".").repeated((dots % 3) + 1);
    m_sendButton->setText("⏳");
    dots++;
}

void ChatWidget::updateChatHistory(const QString &message, bool isUser)
{
    if (!m_historyDisplay) return;
    
    // 展开历史记录区域（如果还没展开）
    if (m_historyContainer->height() == 0) {
        QPropertyAnimation *expandAnim = new QPropertyAnimation(m_historyContainer, "maximumHeight", this);
        expandAnim->setDuration(300);
        expandAnim->setStartValue(0);
        expandAnim->setEndValue(220);
        expandAnim->start();
        
        m_historyContainer->setFixedHeight(220);
    }
    
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm");
    QString prefix = isUser ? "💭 你" : "🤖 AI";
    QString color = isUser ? "#4A90E2" : "#5CB85C";
    
    QString html = QString(
        "<div style='margin: 5px 0; padding: 8px; "
        "background: %1; border-radius: 10px; "
        "border-left: 3px solid %2;'>"
        "<b style='color: %2;'>%3</b> "
        "<span style='color: #999; font-size: 9px;'>[%4]</span><br>"
        "<span style='color: #333; line-height: 1.4;'>%5</span>"
        "</div>"
    ).arg(isUser ? "rgba(74, 144, 226, 50)" : "rgba(92, 184, 92, 50)")
     .arg(color)
     .arg(prefix)
     .arg(timestamp)
     .arg(message);
    
    m_historyDisplay->append(html);
    
    // 滚动到底部
    QScrollBar *scrollBar = m_historyDisplay->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void ChatWidget::checkServerStatus()
{
    QNetworkRequest request(QUrl(m_serverUrl + "/health"));
    QNetworkReply *reply = m_networkManager->get(request);
    
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            m_inputEdit->setPlaceholderText("和我聊天吧~ 💭 (AI已就绪)");
        } else {
            m_inputEdit->setPlaceholderText("AI服务器离线 😴 (请启动Python服务器)");
        }
        reply->deleteLater();
    });
} 