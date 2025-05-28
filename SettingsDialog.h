#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

private slots:
    void onOkClicked();
    void onCancelClicked();
    void onApplyClicked();

private:
    void setupUI();
    void loadSettings();
    void saveSettings();
    void saveJsonConfig();
    void reloadPythonConfig();

    // UI组件
    QComboBox* m_aiModelCombo;
    QCheckBox* m_useProxyCheckBox;
    QLineEdit* m_proxyAddressEdit;
    QLineEdit* m_proxyPortEdit;
    QLineEdit* m_apiKeyEdit;
    
    QPushButton* m_okButton;
    QPushButton* m_cancelButton;
    QPushButton* m_applyButton;
    
    QSettings* m_settings;
}; 