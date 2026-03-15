# 光通信模拟作业

## 1. 作业内容

使用两台笔记本电脑相对放置来模拟光通信：

- 发送端：显示器模拟发光二极管
- 接收端：摄像头模拟光敏感应器

要求使用 C++ 和 OpenCV 完成以下两题：

1. 用黑色和白色分别表示比特 `1` 和 `0`
2. 用黑、白、红、蓝、绿、紫、黄、青表示八进制位 `0` 到 `7`

题目要求接口：

```cpp
cv::Scalar encode(int msg);
int decode(cv::Scalar color);

void send(int msg);
int receive();
```

## 2. 实现说明

本项目采用最简单可用的实现方式。

### 2.1 两种模式

程序内部使用两种模式：

- `Binary`：黑白二值通信
- `Octal`：八色八进制通信

### 2.2 编码规则

#### 题目一：二值模式

- `1 -> black -> Scalar(0, 0, 0)`
- `0 -> white -> Scalar(255, 255, 255)`

解码时计算颜色平均亮度：

- 亮度 `< 128` 认为是 `1`
- 否则认为是 `0`

#### 题目二：八色模式

使用 OpenCV 的 BGR 顺序定义颜色：

- `0 -> black   -> Scalar(0, 0, 0)`
- `1 -> white   -> Scalar(255, 255, 255)`
- `2 -> red     -> Scalar(0, 0, 255)`
- `3 -> blue    -> Scalar(255, 0, 0)`
- `4 -> green   -> Scalar(0, 255, 0)`
- `5 -> purple  -> Scalar(255, 0, 255)`
- `6 -> yellow  -> Scalar(0, 255, 255)`
- `7 -> cyan    -> Scalar(255, 255, 0)`

解码时计算输入颜色和这 8 个标准颜色之间的欧氏距离，距离最小者作为结果。

### 2.3 send / receive 设计

- `send(int msg)`
  - 先调用 `encode(msg)` 得到颜色
  - 再生成一张纯色图像
  - 可保存为图像文件
  - 也支持显示窗口模拟发送端屏幕

- `receive()`
  - 可从图像文件读取
  - 也可从摄像头读取
  - 取图像中心区域的平均颜色
  - 再调用 `decode(color)` 得到消息

## 3. 文件结构

```text
src/
  main.cpp
  light_comm.h
  light_comm.cpp
CMakeLists.txt
README.md
```

## 4. 主要接口

头文件位置：[light_comm.h](E:\CodeSpace\计算机网络\src\light_comm.h)

```cpp
namespace light_comm {

enum class Mode {
    Binary,
    Octal
};

void setMode(Mode mode);
Mode getMode();

cv::Scalar encode(int msg);
int decode(cv::Scalar color);

void send(int msg);
int receive();

}
```

## 5. 构建方法

本项目已经在本机通过以下环境验证：

- Visual Studio Build Tools 2022
- OpenCV 4.12.0
- CMake
- vcpkg

构建命令：

```powershell
cmd /c "call ""E:\BuildTools\VC\Auxiliary\Build\vcvarsall.bat"" x64 && cmake -S ""E:\CodeSpace\计算机网络"" -B ""E:\CodeSpace\计算机网络\build-msvc"" -G Ninja -DCMAKE_CXX_COMPILER=cl -DCMAKE_TOOLCHAIN_FILE=""D:\vcpkg\scripts\buildsystems\vcpkg.cmake"" -DVCPKG_TARGET_TRIPLET=x64-windows -DCMAKE_BUILD_TYPE=Release"

cmd /c "cd /d ""E:\CodeSpace\计算机网络\build-msvc"" && call ""E:\BuildTools\VC\Auxiliary\Build\vcvarsall.bat"" x64 && cmake --build . --config Release"
```

生成的可执行文件：

```text
E:\CodeSpace\计算机网络\build-msvc\light_comm.exe
```

## 6. 运行方法

### 6.1 自动演示

```powershell
E:\CodeSpace\计算机网络\build-msvc\light_comm.exe demo
```

### 6.2 发送图像

二值模式发送：

```powershell
E:\CodeSpace\计算机网络\build-msvc\light_comm.exe send binary 1 binary.png
```

八进制模式发送：

```powershell
E:\CodeSpace\计算机网络\build-msvc\light_comm.exe send octal 6 octal.png
```

### 6.3 接收图像

二值模式接收：

```powershell
E:\CodeSpace\计算机网络\build-msvc\light_comm.exe receive binary binary.png
```

八进制模式接收：

```powershell
E:\CodeSpace\计算机网络\build-msvc\light_comm.exe receive octal octal.png
```

### 6.4 摄像头模式

如果需要从摄像头读取，可使用：

```powershell
E:\CodeSpace\计算机网络\build-msvc\light_comm.exe receive binary camera
E:\CodeSpace\计算机网络\build-msvc\light_comm.exe receive octal camera
```

## 7. 验证结果

本项目已完成自动验证，运行结果如下：

```text
binary demo, send 1, receive 1
octal demo, send 6, receive 6
```

说明当前实现的 `encode / decode / send / receive` 在图像文件模拟方式下工作正常。

## 8. 总结

本作业完成了两种颜色编码方案：

- 黑白二值通信
- 八色八进制通信

并实现了题目要求的共同接口，支持：

- 生成颜色图像模拟发送
- 从图像文件解析模拟接收
- 使用摄像头进行接收实验

整体实现保持简洁，便于演示和提交。
