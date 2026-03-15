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

具体构建方法和运行方式见各自目录下的 `README.md`。
