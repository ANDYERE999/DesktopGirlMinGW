#include "LAppDelegate.hpp" // ï¿½ï¿½ï¿½ï¿½Òªï¿½ï¿½ï¿½Úµï¿½Ò»ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½È»ï¿½ï¿½ï¿½ï¿½Êµï¿½ï¿½
#include "LAppView.hpp"
#include "LAppPal.hpp"
#include "LAppLive2DManager.hpp"
#include "LAppDefine.hpp"
#include "GLCore.h"
#include "SettingsDialog.h"
#include "ChatWidget.h"
#include <QTimer>
#include <QMouseEvent>
#include <QSystemTrayIcon>
#include <QApplication>
#include <QStyle>
#include <QAction>
#include <QFile>
#include <QIcon>
#include <QDateTime>





GLCore::GLCore(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_systemTray(nullptr)
    , m_trayMenu(nullptr)
    , m_exitAction(nullptr)
    , m_showHideAction(nullptr)
    , m_settingsAction(nullptr)
    , m_chatToggleAction(nullptr)
    , m_settingsDialog(nullptr)
    , m_chatWidget(nullptr)
{
    
}

GLCore::GLCore(int w, int h, QWidget *parent)
    : QOpenGLWidget(parent)
    , m_systemTray(nullptr)
    , m_trayMenu(nullptr)
    , m_exitAction(nullptr)
    , m_showHideAction(nullptr)
    , m_settingsAction(nullptr)
    , m_chatToggleAction(nullptr)
    , m_settingsDialog(nullptr)
    , m_chatWidget(nullptr)
{
    setFixedSize(w, h);
    
    // ï¿½ï¿½ï¿½ï¿½OpenGLï¿½ï¿½Ê½ï¿½ï¿½Ö§ï¿½ï¿½Í¸ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½È¾
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setAlphaBufferSize(8);  // ï¿½Ø¼ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½alphaï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
    format.setSamples(4);          // ï¿½ï¿½ï¿½ï¿½ï¿?
    setFormat(format);
    
    //this->setAttribute(Qt::WA_DeleteOnClose);       // ï¿½ï¿½ï¿½Ú¹Ø±ï¿½Ê±ï¿½Ô¶ï¿½ï¿½Í·ï¿½ï¿½Ú´ï¿½
    this->setWindowFlag(Qt::FramelessWindowHint); // ï¿½ï¿½ï¿½ï¿½ï¿½Þ±ß¿ò´°¿ï¿½
    this->setWindowFlag(Qt::WindowStaysOnTopHint); // ï¿½ï¿½ï¿½Ã´ï¿½ï¿½ï¿½Ê¼ï¿½ï¿½ï¿½Ú¶ï¿½ï¿½ï¿½
    //this->setWindowFlag(Qt::Tool); // ï¿½ï¿½ï¿½ï¿½Ó¦ï¿½Ã³ï¿½ï¿½ï¿½Í¼ï¿½ï¿½
    this->setAttribute(Qt::WA_TranslucentBackground); // ï¿½ï¿½ï¿½Ã´ï¿½ï¿½Ú±ï¿½ï¿½ï¿½Í¸ï¿½ï¿½

    

    QTimer* timer = new QTimer();
    connect(timer, &QTimer::timeout, [=]() {
        update();
        });
    timer->start((1.0 / 60) * 1000);    // 60FPS

    // ÉèÖÃÏµÍ³ÍÐÅÌ
    setupSystemTray();

}

GLCore::~GLCore()
{
    // ÇåÀíÏµÍ³ÍÐÅÌ×ÊÔ´
    if (m_systemTray) {
        m_systemTray->hide();
        delete m_systemTray;
    }
    if (m_trayMenu) {
        delete m_trayMenu;
    }
    
    // ÇåÀí¶Ô»°¿ò
    if (m_settingsDialog) {
        delete m_settingsDialog;
    }
    if (m_chatWidget) {
        delete m_chatWidget;
    }
}



void GLCore::mouseMoveEvent(QMouseEvent* event)
{
    LAppDelegate::GetInstance()->GetView()->OnTouchesMoved(event->x(), event->y());

    if (isLeftPressed) {
        this->move(event->pos() - this->currentPos + this->pos());
    }

    

    // ï¿½ï¿½ï¿½ï¿½ï¿½Â¼ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ý£ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Â¼ï¿½ï¿½ï¿½ï¿½Ý¸ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ú£ï¿½Êµï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ï¶ï¿½ï¿½Þ±ß¿ò´°¿ï¿½
    event->ignore();
}

void GLCore::mousePressEvent(QMouseEvent* event)
{
    LAppDelegate::GetInstance()->GetView()->OnTouchesBegan(event->x(), event->y());

    if (event->button() == Qt::LeftButton) {
        this->isLeftPressed = true;
        this->currentPos = event->pos();
    }
    // TODO: ï¿½Ò¼ï¿½ï¿½Ëµï¿½ï¿½ï¿½
    if (event->button() == Qt::RightButton) {
        
        // ï¿½ï¿½ï¿½Ã´ï¿½ï¿½Ú´ï¿½Ð¡
        //LAppDelegate::GetInstance()->resize(400, 400);
        //this->setFixedSize(400, 400);
        LAppLive2DManager::GetInstance()->LoadModelFromPath("Resources/Mao/", "Mao.model3.json");

        this->isRightPressed = true;
    }


    
    // ï¿½ï¿½ï¿½ï¿½ï¿½Â¼ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
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

    
    // ï¿½ï¿½ï¿½ï¿½ï¿½Â¼ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
    event->ignore();
}

void GLCore::initializeGL()
{
    // ï¿½ï¿½ï¿½ï¿½OpenGL×´Ì¬ï¿½ï¿½Ö§ï¿½ï¿½Í¸ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½È¾
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);  // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½È²ï¿½ï¿½ï¿½ï¿½ï¿½È·ï¿½ï¿½Í¸ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿?
    
    LAppDelegate::GetInstance()->Initialize(this);
    
    // Ñ¡ï¿½ï¿½Ä£ï¿½ï¿½
}

void GLCore::paintGL()
{
    // ï¿½ï¿½ï¿½ï¿½Í¸ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);  // RGBAï¿½ï¿½A=0ï¿½ï¿½Ê¾ï¿½ï¿½È«Í¸ï¿½ï¿½
    glClear(GL_COLOR_BUFFER_BIT);
    
    LAppDelegate::GetInstance()->update();
    
}

void GLCore::resizeGL(int w, int h)
{
    LAppDelegate::GetInstance()->resize(w, h);
}

void GLCore::setupSystemTray()
{
    // ¼ì²éÏµÍ³ÊÇ·ñÖ§³ÖÍÐÅÌÍ¼±ê
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        return;
    }

    // ´´½¨ÍÐÅÌÍ¼±ê
    m_systemTray = new QSystemTrayIcon(this);
    
    // ÉèÖÃÍÐÅÌÍ¼±ê£¨Ê¹ÓÃResourcesÄ¿Â¼ÖÐµÄÍ¼±ê£©
    QString iconPath = "Resources/icon_gear.png";
    if (QFile::exists(iconPath)) {
        m_systemTray->setIcon(QIcon(iconPath));
    } else {
        // Èç¹ûÕÒ²»µ½×Ô¶¨ÒåÍ¼±ê£¬Ê¹ÓÃÄ¬ÈÏÍ¼±ê
        m_systemTray->setIcon(QApplication::style()->standardIcon(QStyle::SP_ComputerIcon));
    }
    m_systemTray->setToolTip("Desktop Girl - Live2D");

    // ´´½¨ÍÐÅÌ²Ëµ¥
    m_trayMenu = new QMenu(this);
    
    // ´´½¨ÏÔÊ¾/Òþ²Ø¶¯×÷
    m_showHideAction = new QAction("Show/Hide", this);
    m_showHideAction->setIcon(QApplication::style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    
    // ´´½¨ÁÄÌì¿ª¹Ø¶¯×÷
    m_chatToggleAction = new QAction("Open Chat", this);
    m_chatToggleAction->setIcon(QApplication::style()->standardIcon(QStyle::SP_ComputerIcon));
    
    // ´´½¨ÉèÖÃ¶¯×÷
    m_settingsAction = new QAction("Settings", this);
    m_settingsAction->setIcon(QApplication::style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    
    // ´´½¨ÍË³ö¶¯×÷
    m_exitAction = new QAction("Exit", this);
    m_exitAction->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton));
    
    // ½«¶¯×÷Ìí¼Óµ½²Ëµ¥
    m_trayMenu->addAction(m_showHideAction);
    m_trayMenu->addSeparator(); // Ìí¼Ó·Ö¸ôÏß
    m_trayMenu->addAction(m_chatToggleAction);
    m_trayMenu->addAction(m_settingsAction);
    m_trayMenu->addSeparator(); // Ìí¼Ó·Ö¸ôÏß
    m_trayMenu->addAction(m_exitAction);
    
    // ÉèÖÃÍÐÅÌµÄÉÏÏÂÎÄ²Ëµ¥
    m_systemTray->setContextMenu(m_trayMenu);
    
    // Á¬½ÓÐÅºÅºÍ²Û
    connect(m_systemTray, &QSystemTrayIcon::activated, this, &GLCore::onTrayIconActivated);
    connect(m_showHideAction, &QAction::triggered, this, &GLCore::onShowHideTriggered);
    connect(m_chatToggleAction, &QAction::triggered, this, &GLCore::onChatToggleTriggered);
    connect(m_settingsAction, &QAction::triggered, this, &GLCore::onSettingsTriggered);
    connect(m_exitAction, &QAction::triggered, this, &GLCore::onExitTriggered);
    
    // ÏÔÊ¾ÍÐÅÌÍ¼±ê
    m_systemTray->show();
}

void GLCore::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::DoubleClick:
        // Ë«»÷ÍÐÅÌÍ¼±êÊ±ÏÔÊ¾/Òþ²ØÖ÷´°¿Ú
        if (this->isVisible()) {
            this->hide();
        } else {
            this->show();
            this->raise();
            this->activateWindow();
        }
        break;
    case QSystemTrayIcon::Context:
        // ÓÒ¼üµã»÷Ê±»á×Ô¶¯ÏÔÊ¾ÉÏÏÂÎÄ²Ëµ¥
        break;
    default:
        break;
    }
}

void GLCore::onShowHideTriggered()
{
    // ÊµÏÖÏÔÊ¾/Òþ²Ø¹¦ÄÜ
    if (this->isVisible()) {
        this->hide();
    } else {
        this->show();
        this->raise();
        this->activateWindow();
    }
}

void GLCore::onChatToggleTriggered()
{
    // ´´½¨ÁÄÌì×é¼þ£¨Èç¹û»¹Ã»ÓÐ´´½¨£©
    if (!m_chatWidget) {
        m_chatWidget = new ChatWidget(this);
        m_chatWidget->resize(400, 60); // ÉèÖÃ³õÊ¼´óÐ¡
    }
    
    // ÇÐ»»ÁÄÌì´°¿ÚµÄÏÔÊ¾×´Ì¬
    if (m_chatWidget->isChatVisible()) {
        m_chatWidget->hideChatInput();
        m_chatToggleAction->setText("Open Chat");
    } else {
        m_chatWidget->showChatInput();
        m_chatToggleAction->setText("Close Chat");
    }
}

void GLCore::onSettingsTriggered()
{
    // ´´½¨ÉèÖÃ¶Ô»°¿ò£¨Èç¹û»¹Ã»ÓÐ´´½¨£©
    if (!m_settingsDialog) {
        m_settingsDialog = new SettingsDialog(this);
    }
    
    // ÏÔÊ¾ÉèÖÃ¶Ô»°¿ò
    m_settingsDialog->show();
    m_settingsDialog->raise();
    m_settingsDialog->activateWindow();
}

void GLCore::onExitTriggered()
{
    // ÍË³öÓ¦ÓÃ³ÌÐò
    QApplication::quit();
}