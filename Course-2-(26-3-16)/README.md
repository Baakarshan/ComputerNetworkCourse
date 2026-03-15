# RS232C 编解码作业

## 1. 作业要求

编写一个程序，按照以下规则模拟 RS232C：

- 字节大端序
- 位小端序
- 7-bit 编码
- 包括空闲位、起始位和终止位
- 输入一个字母，输出电压值序列

题目要求接口：

```cpp
int rs232c_encode(double *volts, int volts_size, const char *msg, int size);
int rs232c_decode(char *msg, int size, const double *volts, int volts_size);
```

## 2. 实现方案

本项目采用最简单的 RS232C 模拟方式：

- 空闲位：逻辑 `1`
- 起始位：逻辑 `0`
- 数据位：7 位
- 终止位：逻辑 `1`
- 位序：低位先发送，即位小端序
- 电压映射：
  - 逻辑 `1 -> -5.0V`
  - 逻辑 `0 -> +5.0V`

每个字符对应 10 个电压采样点：

```text
idle + start + data0 + data1 + data2 + data3 + data4 + data5 + data6 + stop
```

## 3. 文件结构

```text
src/
  main.cpp
  rs232c.cpp
  rs232c.h
CMakeLists.txt
README.md
```

## 4. 编码与解码说明

### 编码

`rs232c_encode()` 的处理流程：

1. 取字符的低 7 位
2. 写入空闲位 `1`
3. 写入起始位 `0`
4. 按最低位到最高位依次写入 7 个数据位
5. 写入终止位 `1`
6. 输出对应电压值序列

### 解码

`rs232c_decode()` 的处理流程：

1. 每 10 个电压值解析为一个字符帧
2. 检查空闲位、起始位、终止位是否合法
3. 按位小端序恢复 7 位数据
4. 还原为字符

## 5. 构建方法

```powershell
cmake -S . -B build
cmake --build build
```

生成可执行文件：

```text
build/rs232c_demo.exe
```

## 6. 运行方法

例如输入字母 `A`：

```powershell
rs232c_demo A
```

程序会输出：

- 编码后的电压序列
- 解码恢复后的字符

## 7. 说明

这个实现做了简单完成作业，不考虑复杂串口时序、波特率采样和噪声处理。
