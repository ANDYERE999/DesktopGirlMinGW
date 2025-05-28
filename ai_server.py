#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import json
import time
from flask import Flask, request, jsonify, render_template_string
from flask_cors import CORS
import google.generativeai as genai
import threading
import sys

# 设置代理（如果需要）
def setup_proxy():
    """从配置文件读取代理设置"""
    try:
        with open('ai_config.json', 'r', encoding='utf-8') as f:
            config = json.load(f)
            if config.get('proxy', {}).get('enabled', False):
                proxy_addr = config['proxy']['address']
                proxy_port = config['proxy']['port']
                proxy_url = f"http://{proxy_addr}:{proxy_port}"
                os.environ["HTTP_PROXY"] = proxy_url
                os.environ["HTTPS_PROXY"] = proxy_url
                print(f"代理已设置: {proxy_url}")
    except FileNotFoundError:
        print("配置文件不存在，使用默认设置")

def load_config():
    """加载配置文件"""
    try:
        with open('ai_config.json', 'r', encoding='utf-8') as f:
            return json.load(f)
    except FileNotFoundError:
        # 默认配置
        default_config = {
            "api_key": "",
            "model": "gemini-2.0-flash",
            "proxy": {
                "enabled": False,
                "address": "127.0.0.1",
                "port": "7890"
            }
        }
        with open('ai_config.json', 'w', encoding='utf-8') as f:
            json.dump(default_config, f, indent=2, ensure_ascii=False)
        return default_config

# 初始化Flask应用
app = Flask(__name__)
CORS(app)  # 允许跨域请求

# 全局变量
gemini_model = None
config = None

def init_gemini():
    """初始化Gemini AI"""
    global gemini_model, config
    config = load_config()
    
    if not config['api_key']:
        print("⚠️ API密钥未设置，请在ai_config.json中配置")
        return False
    
    try:
        setup_proxy()
        genai.configure(api_key=config['api_key'])
        gemini_model = genai.GenerativeModel(config['model'])
        print(f"✅ Gemini AI 初始化成功 - 模型: {config['model']}")
        return True
    except Exception as e:
        print(f"❌ Gemini AI 初始化失败: {e}")
        return False

@app.route('/health', methods=['GET'])
def health_check():
    """健康检查接口"""
    return jsonify({
        "status": "ok",
        "model": config['model'] if config else "未配置",
        "api_ready": gemini_model is not None
    })

@app.route('/chat', methods=['POST'])
def chat():
    """聊天接口"""
    try:
        data = request.get_json()
        message = data.get('message', '').strip()
        
        if not message:
            return jsonify({"error": "消息不能为空"}), 400
        
        if not gemini_model:
            return jsonify({"error": "AI模型未初始化"}), 500
        
        # 调用Gemini API
        response = gemini_model.generate_content(message)
        
        if response.text:
            return jsonify({
                "response": response.text,
                "timestamp": time.time()
            })
        else:
            return jsonify({"error": "AI返回空响应"}), 500
            
    except Exception as e:
        print(f"聊天错误: {e}")
        return jsonify({"error": f"处理失败: {str(e)}"}), 500

@app.route('/reload_config', methods=['POST'])
def reload_config():
    """重新加载配置"""
    try:
        global config, gemini_model
        config = load_config()
        gemini_model = None
        
        if init_gemini():
            return jsonify({"status": "success", "message": "配置重新加载成功"})
        else:
            return jsonify({"status": "error", "message": "配置重新加载失败"}), 500
    except Exception as e:
        return jsonify({"status": "error", "message": str(e)}), 500

@app.route('/status', methods=['GET'])
def get_status():
    """获取状态信息"""
    return jsonify({
        "ai_ready": gemini_model is not None,
        "config": {
            "model": config['model'] if config else "未知",
            "proxy_enabled": config['proxy']['enabled'] if config else False
        }
    })

# 简单的Web界面（用于调试）
DEBUG_HTML = '''
<!DOCTYPE html>
<html>
<head>
    <title>Desktop Girl AI Server</title>
    <meta charset="utf-8">
</head>
<body>
    <h1>Desktop Girl AI Server</h1>
    <p>服务器运行中...</p>
    <div id="status"></div>
    <script>
        fetch('/status')
            .then(r => r.json())
            .then(data => {
                document.getElementById('status').innerHTML = 
                    '<pre>' + JSON.stringify(data, null, 2) + '</pre>';
            });
    </script>
</body>
</html>
'''

@app.route('/', methods=['GET'])
def index():
    """调试页面"""
    return render_template_string(DEBUG_HTML)

def start_server():
    """启动服务器"""
    print("🚀 正在启动Desktop Girl AI服务器...")
    
    if init_gemini():
        print("✅ AI模型准备就绪")
    else:
        print("⚠️  AI模型初始化失败，部分功能可能不可用")
    
    print("🌐 服务器地址: http://localhost:5000")
    print("📋 健康检查: http://localhost:5000/health")
    print("🔄 要重新加载配置，请访问: POST http://localhost:5000/reload_config")
    
    # 在后台线程中运行服务器
    app.run(host='127.0.0.1', port=5000, debug=False, threaded=True)

if __name__ == '__main__':
    start_server() 