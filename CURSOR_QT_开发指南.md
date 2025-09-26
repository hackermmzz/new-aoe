# Cursor 中的 Qt 开发指南

## 环境配置

当前项目已经为 Qt 5.9.2 配置好了所有必要的设置。配置文件包括：

- `.vscode/c_cpp_properties.json` - C/C++扩展配置
- `.vscode/launch.json` - 调试配置
- `.vscode/tasks.json` - 构建任务
- `.vscode/settings.json` - 编辑器设置
- `.vscode/extensions.json` - 推荐扩展
- `newAOE.code-workspace` - 工作区文件
- `qt-env.bat` - Qt 环境变量设置批处理文件

## 使用步骤

### 初始设置

1. 首先，在 Cursor 中打开工作区文件：

   - 点击 `文件 > 打开工作区`
   - 选择 `newAOE.code-workspace`

2. 安装推荐的扩展：
   - 点击左侧扩展图标
   - 查看"推荐"标签页
   - 安装所有推荐的扩展

### 代码补全

要确保代码补全正常工作，请执行以下操作：

1. 确保已安装 C/C++扩展
2. 重新加载窗口：`Ctrl+Shift+P` 然后输入 `Reload Window`
3. 如果仍然没有代码补全，请尝试重建 IntelliSense 数据库：
   - `Ctrl+Shift+P`
   - 输入并选择 `C/C++: Reset IntelliSense Database`

### 编译和运行

1. 编译项目 (Debug 版本)：

   - `Ctrl+Shift+P`
   - 输入 `Tasks: Run Task`
   - 选择 `qmake & make`

2. 编译项目 (Release 版本)：

   - `Ctrl+Shift+P`
   - 输入 `Tasks: Run Task`
   - 选择 `qmake & make release`

3. 调试项目：
   - 按 `F5` 使用 Debug 配置
   - 或在调试面板中选择"Qt Debug"或"Qt Release"配置

### 使用 Qt Designer

1. 打开 Qt Designer：

   - `Ctrl+Shift+P`
   - 输入 `Tasks: Run Task`
   - 选择 `Run Qt Designer`

2. 从 Designer 生成头文件：
   - 在 UI 文件(\*.ui)上右键
   - 选择 `Tasks: Run Task`
   - 选择 `Generate UI Headers`

### 使用 Qt Assistant 获取帮助

1. 打开 Qt Assistant：
   - `Ctrl+Shift+P`
   - 输入 `Tasks: Run Task`
   - 选择 `Run Qt Assistant`

## 常见问题解决

### 代码补全不工作

如果代码补全功能不正常工作，请尝试以下步骤：

1. 确保项目已经至少编译过一次
2. 重置 IntelliSense 数据库（如上所述）
3. 检查文件是否包含正确的头文件：
   ```cpp
   #include <QWidget>
   #include <QApplication>
   ```
4. 重启 Cursor
5. 删除.vscode 目录下的 browse.vc.db 文件然后重启 Cursor

### 找不到 Qt 类或函数

如果无法识别 Qt 类，请检查：

1. 包含的头文件是否正确
2. 项目是否编译过
3. .pro 文件中是否正确指定了 QT 模块：
   ```
   QT += core gui widgets multimedia
   ```

### 无法找到库文件

如果编译时报找不到库文件的错误，请确保：

1. 环境变量设置正确（使用 qt-env.bat）
2. 项目的.pro 文件中正确引用了所需模块

## 提示与技巧

1. 在写代码时，利用 Cursor 的智能补全功能
2. 使用 F12 可以跳转到定义
3. 使用 Shift+F12 可以查找所有引用
4. 使用 Alt+左箭头或右箭头可以在浏览历史之间前进或后退
5. 使用 Ctrl+G 可以跳转到特定行
