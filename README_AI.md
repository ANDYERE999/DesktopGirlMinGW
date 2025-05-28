# 🤖 Desktop Girl AI 功能使用说明

## 🌟 新的架构设计

我们已经重新设计了AI功能的架构，使用**Python后端 + Qt前端**的分离式设计：

- **🐍 Python AI服务器** - 处理所有AI相关功能（Gemini API调用、代理设置等）
- **🎨 Qt可爱聊天界面** - 在人物下方显示浏览器搜索框式的聊天UI
- **📡 HTTP通信** - Python和Qt通过本地HTTP API进行通信

## 🚀 快速开始

### 1. 启动AI服务器
```bash
# 双击运行或在命令行执行
start_ai_server.bat
```

### 2. 配置API密钥
1. 右键点击系统托盘图标
2. 选择 `Settings`
3. 输入您的Gemini API密钥
4. 配置代理设置（如需要）
5. 点击 `OK` 保存

### 3. 开始聊天
1. 右键点击系统托盘图标
2. 选择 `Open Chat`
3. 在人物下方会出现可爱的搜索框
4. 输入消息开始聊天！

## 💫 功能特性

### 🎨 **可爱的UI设计**
- 胶囊状透明搜索框
- 粉色爱心发送按钮
- 平滑的显示/隐藏动画
- 自动展开的聊天历史记录

### ⚡ **智能交互**
- 实时AI响应
- 输入状态指示器
- 自动滚动到最新消息
- 错误处理和重连机制

### 🔧 **灵活配置**
- 支持多种Gemini模型
- 代理服务器配置
- 配置热重载（无需重启）

## 📁 文件结构

```
DesktopGirlMinGW/
├── 🎯 主程序文件
│   ├── GLCore.cpp/h          # 主窗口和托盘功能
│   ├── ChatWidget.cpp/h      # 可爱的聊天组件
│   └── SettingsDialog.cpp/h  # 设置对话框
│
├── 🐍 Python AI服务器
│   ├── ai_server.py          # Flask AI服务器
│   ├── requirements.txt      # Python依赖
│   └── start_ai_server.bat   # 启动脚本
│
└── 📋 配置文件
    └── ai_config.json        # AI配置（自动生成）
```

## 🔧 API接口

Python服务器提供以下HTTP接口：

### 健康检查
```
GET /health
返回: {"status": "ok", "model": "...", "api_ready": true}
```

### 聊天
```
POST /chat
请求: {"message": "你好"}
返回: {"response": "AI回复", "timestamp": 1234567890}
```

### 重新加载配置
```
POST /reload_config
返回: {"status": "success", "message": "配置重新加载成功"}
```

## 🐛 故障排除

### 聊天框显示"AI服务器离线"
1. 确保运行了 `start_ai_server.bat`
2. 检查Python是否正确安装
3. 检查端口5000是否被占用

### AI无响应
1. 检查API密钥是否正确设置
2. 确认网络连接正常
3. 如使用代理，检查代理设置

### 聊天框不显示
1. 右键点击托盘图标选择"Open Chat"
2. 检查主窗口是否可见
3. 尝试重启应用程序

## 🎨 自定义样式

聊天组件支持CSS样式自定义，您可以修改 `ChatWidget.cpp` 中的样式代码来改变外观：

- 输入框颜色和形状
- 按钮样式和动画
- 聊天气泡样式
- 字体和大小

## 💡 未来扩展

这个架构设计为未来扩展提供了很好的基础：

- 🔄 支持更多AI模型（Claude、ChatGPT等）
- 🗣️ 语音聊天功能
- 🎵 情感分析和Live2D表情同步
- 🤖 个性化AI助手配置
- 📝 聊天记录保存和管理

---

🎉 **享受与您的桌面AI助手的愉快聊天吧！** 🎉 