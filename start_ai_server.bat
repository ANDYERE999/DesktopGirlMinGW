@echo off
echo =================================
echo     Desktop Girl AI Server
echo =================================
echo.

:: 检查Python是否安装
python --version >nul 2>&1
if errorlevel 1 (
    echo 错误: Python未安装或未添加到PATH
    echo 请安装Python 3.8+并确保添加到环境变量
    pause
    exit /b 1
)

:: 检查依赖是否安装
echo 检查Python依赖...
pip show flask >nul 2>&1
if errorlevel 1 (
    echo 正在安装Python依赖...
    pip install -r requirements.txt
    if errorlevel 1 (
        echo 依赖安装失败
        pause
        exit /b 1
    )
)

echo.
echo 🚀 启动AI服务器...
echo 📋 地址: http://localhost:5000
echo 🔧 配置文件: ai_config.json
echo ⏹️  按Ctrl+C停止服务器
echo.

python ai_server.py

pause 