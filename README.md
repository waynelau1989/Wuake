Wuake - A Drop-down Terminal For Windows
========================================
# 简介
Wuake是[mintty](https://github.com/mintty/mintty)的“外壳”，它在mintty的基础上提供了“下拉”和“标签页”的功能。

# 截图
![Git](https://github.com/vale666/Wuake/raw/master/screenshots/screenshot2.jpg)

![自定义主题](https://github.com/vale666/Wuake/raw/master/screenshots/screenshot1.jpg)

# 开发
Wuake采用Qt5开发，你需要安装Qt5和QtCreator。

# 使用
将mintty.exe加入环境变量Path中

## 快捷键
|快捷键|功能|
|:---|:---|
|F12|显示/隐藏|
|Ctrl+Shift+N|新建标签|
|Ctrl+Shift+Left|当前标签页向左移动|
|Ctrl+Shift+Right|当前标签页向右移动|
|Shift+Left|切换到左边的标签页|
|Shift+Right|切换到右边的标签页|

注意:在Windows系统上F12默认是无法单独作为全局快捷的，需要修改注册表：

```bat
将Windows系统调试键设置为Pause，以便Wuake可以使用F12作为全局快捷键
[HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\AeDebug]
"UserDebuggerHotKey"=dword:00000013
```