# 计算机网络课程仓库

这个仓库存放计算机网络课程的作业内容。

## 目录结构

- `Course-1-(26-3-15)`
  - 光通信模拟作业
  - 使用 C++ 和 OpenCV 实现
  - 包含源码、CMake 配置和作业说明文档

- `Course-2-(26-3-16)`
  - RS232C 编解码作业
  - 使用 C++ 实现 7-bit 编码模拟
  - 包含源码、CMake 配置和作业说明文档

- `Course-3-(26-3-22)`
  - 多路复用与调制作业
  - 使用 C++ 实现 TDM/FDM/CDM 与 FM/AM/PM 模拟
  - 包含源码、CMake 配置和作业说明文档

- `Course-4-(26-3-29)`
  - 奇偶校验码检验作业
  - 使用 C++ 实现偶校验检测函数
  - 包含源码、CMake 配置和作业说明文档

## 当前作业

### 作业 1

`Course-1-(26-3-15)` 实现了：

- 黑白二值通信：黑色表示 `1`，白色表示 `0`
- 八色八进制通信
- 共同接口：
  - `encode(int msg)`
  - `decode(cv::Scalar color)`
  - `send(int msg)`
  - `receive()`

### 作业 2

`Course-2-(26-3-16)` 实现了：

- RS232C 电压序列编码与解码
- 7-bit 数据位编码
- 空闲位、起始位、终止位处理
- 共同接口：
  - `rs232c_encode(double *volts, int volts_size, const char *msg, int size)`
  - `rs232c_decode(char *msg, int size, const double *volts, int volts_size)`

### 作业 3

`Course-3-(26-3-22)` 实现了：

- 双路消息的复用与解复用（同步时分、统计时分、频分、码分）
- 载波、数字/模拟调制信号生成
- 数字与模拟的调频、调幅、调相
- 主要接口：
  - `multiplex(...) / demultiplex(...)`
  - `generate_cover_signal(...)`
  - `simulate_digital_modulation_signal(...)`
  - `simulate_analog_modulation_signal(...)`
  - `modulate_*_frequency(...)`
  - `modulate_*_amplitude(...)`
  - `modulate_*_phase(...)`

### 作业 4

`Course-4-(26-3-29)` 实现了：

- 奇偶校验码（偶校验）检测
- 消息元素按 `0` 和非 `0` 两类处理（非 `0` 视为 `1`）
- 题目接口：
  - `int parity_check(const unsigned char *msg, const int msg_length)`

具体构建方法和运行方式见各自目录下的 `README.md`。
