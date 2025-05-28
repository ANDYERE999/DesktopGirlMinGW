#include "SettingsDialog.h"

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , m_settings(new QSettings("DesktopGirl", "Settings", this))
{
    setWindowTitle("Settings");
    setFixedSize(400, 300);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    
    setupUI();
    loadSettings();
}

SettingsDialog::~SettingsDialog()
{
}

void SettingsDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // AI模型设置组
    QGroupBox* aiGroup = new QGroupBox("AI Model Settings", this);
    QFormLayout* aiLayout = new QFormLayout(aiGroup);
    
    m_aiModelCombo = new QComboBox(this);
    m_aiModelCombo->addItem("Gemini 2.0 Flash", "gemini-2.0-flash");
    m_aiModelCombo->addItem("Gemini 1.5 Pro", "gemini-1.5-pro");
    m_aiModelCombo->addItem("Gemini 1.5 Flash", "gemini-1.5-flash");
    aiLayout->addRow("AI Model:", m_aiModelCombo);
    
    m_apiKeyEdit = new QLineEdit(this);
    m_apiKeyEdit->setEchoMode(QLineEdit::Password);
    m_apiKeyEdit->setPlaceholderText("Enter your Gemini API key");
    aiLayout->addRow("API Key:", m_apiKeyEdit);
    
    // 代理设置组
    QGroupBox* proxyGroup = new QGroupBox("Proxy Settings", this);
    QFormLayout* proxyLayout = new QFormLayout(proxyGroup);
    
    m_useProxyCheckBox = new QCheckBox("Enable Proxy", this);
    proxyLayout->addRow(m_useProxyCheckBox);
    
    m_proxyAddressEdit = new QLineEdit(this);
    m_proxyAddressEdit->setPlaceholderText("127.0.0.1");
    proxyLayout->addRow("Proxy Address:", m_proxyAddressEdit);
    
    m_proxyPortEdit = new QLineEdit(this);
    m_proxyPortEdit->setPlaceholderText("7890");
    proxyLayout->addRow("Proxy Port:", m_proxyPortEdit);
    
    // 启用/禁用代理设置
    connect(m_useProxyCheckBox, &QCheckBox::toggled, [this](bool enabled) {
        m_proxyAddressEdit->setEnabled(enabled);
        m_proxyPortEdit->setEnabled(enabled);
    });
    
    // 按钮布局
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_okButton = new QPushButton("OK", this);
    m_cancelButton = new QPushButton("Cancel", this);
    m_applyButton = new QPushButton("Apply", this);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_okButton);
    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(m_applyButton);
    
    // 连接信号
    connect(m_okButton, &QPushButton::clicked, this, &SettingsDialog::onOkClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &SettingsDialog::onCancelClicked);
    connect(m_applyButton, &QPushButton::clicked, this, &SettingsDialog::onApplyClicked);
    
    // 主布局
    mainLayout->addWidget(aiGroup);
    mainLayout->addWidget(proxyGroup);
    mainLayout->addLayout(buttonLayout);
}

void SettingsDialog::loadSettings()
{
    // 加载AI模型设置
    QString aiModel = m_settings->value("ai/model", "gemini-2.0-flash").toString();
    int index = m_aiModelCombo->findData(aiModel);
    if (index >= 0) {
        m_aiModelCombo->setCurrentIndex(index);
    }
    
    // 加载API密钥
    QString apiKey = m_settings->value("ai/apikey", "").toString();
    m_apiKeyEdit->setText(apiKey);
    
    // 加载代理设置
    bool useProxy = m_settings->value("proxy/enabled", false).toBool();
    m_useProxyCheckBox->setChecked(useProxy);
    
    QString proxyAddress = m_settings->value("proxy/address", "127.0.0.1").toString();
    m_proxyAddressEdit->setText(proxyAddress);
    
    QString proxyPort = m_settings->value("proxy/port", "7890").toString();
    m_proxyPortEdit->setText(proxyPort);
    
    // 设置代理控件状态
    m_proxyAddressEdit->setEnabled(useProxy);
    m_proxyPortEdit->setEnabled(useProxy);
}

void SettingsDialog::saveSettings()
{
    // 保存AI模型设置到Qt设置
    QString aiModel = m_aiModelCombo->currentData().toString();
    m_settings->setValue("ai/model", aiModel);
    
    // 保存API密钥到Qt设置
    m_settings->setValue("ai/apikey", m_apiKeyEdit->text());
    
    // 保存代理设置到Qt设置
    m_settings->setValue("proxy/enabled", m_useProxyCheckBox->isChecked());
    m_settings->setValue("proxy/address", m_proxyAddressEdit->text());
    m_settings->setValue("proxy/port", m_proxyPortEdit->text());
    
    m_settings->sync();
    
    // 同时保存到JSON文件供Python服务器使用
    saveJsonConfig();
    
    // 通知Python服务器重新加载配置
    reloadPythonConfig();
}

void SettingsDialog::saveJsonConfig()
{
    QJsonObject config;
    config["api_key"] = m_apiKeyEdit->text();
    config["model"] = m_aiModelCombo->currentData().toString();
    
    QJsonObject proxy;
    proxy["enabled"] = m_useProxyCheckBox->isChecked();
    proxy["address"] = m_proxyAddressEdit->text();
    proxy["port"] = m_proxyPortEdit->text();
    config["proxy"] = proxy;
    
    QJsonDocument doc(config);
    
    QFile file("ai_config.json");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

void SettingsDialog::reloadPythonConfig()
{
    // 向Python服务器发送重新加载配置的请求
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request(QUrl("http://127.0.0.1:5000/reload_config"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QNetworkReply *reply = manager->post(request, QByteArray());
    connect(reply, &QNetworkReply::finished, [reply]() {
        reply->deleteLater();
    });
}

void SettingsDialog::onOkClicked()
{
    saveSettings();
    accept();
}

void SettingsDialog::onCancelClicked()
{
    reject();
}

void SettingsDialog::onApplyClicked()
{
    saveSettings();
} 