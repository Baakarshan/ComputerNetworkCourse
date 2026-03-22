# Course-3 作业实现（第25题 + 第26题）

## 1. 目录结构

```text
src/
  main.cpp
  multiplex.h
  multiplex.cpp
  modulation.h
  modulation.cpp
CMakeLists.txt
README.md
```

## 2. 题目25：多路复用与解复用

题目要求接口：

```cpp
int multiplex(unsigned char *c, const int c_size, const unsigned char *a, const int a_len,
              const unsigned char *b, const int b_len);
int demultiplex(unsigned char *a, const int a_size, unsigned char *b, const int b_size,
                const unsigned char *c, const int c_len);
```

实现说明：

- 默认接口 `multiplex/demultiplex` 使用 **同步时分复用**。
- 额外提供四种方式的独立函数：
  - 同步时分复用（Sync TDM）
  - 统计时分复用（Stat TDM）
  - 频分复用（FDM，教学抽象）
  - 码分复用（CDM，长度为2的正交码）

### 各方式最简策略

1. 同步时分复用：按时隙交替写入 `a[i]` 和 `b[i]`。
2. 统计时分复用：仅记录值为 `1` 的样本，编码为 `[源通道, 索引低字节, 索引高字节]`。
3. 频分复用：每个采样点用一个字节打包 `a` 和 `b`（bit0/bit1）。
4. 码分复用：`a=[1,1]`，`b=[1,-1]`，叠加后做相关解码。

## 3. 题目26：载波与调制

题目给出的 `unsigned double` 在 C/C++ 中非法，已统一改为 `double`：

```cpp
int generate_cover_signal(double *cover, const int size);
int simulate_digital_modulation_signal(unsigned char *message, const int size);
int simulate_analog_modulation_signal(double *message, const int size);

int modulate_digital_frequency(double *cover, const int cover_len, const unsigned char *message,
                               const int msg_len);
int modulate_analog_frequency(double *cover, const int cover_len, const double *message, const int msg_len);

int modulate_digital_amplitude(double *cover, const int cover_len, const unsigned char *message,
                               const int msg_len);
int modulate_analog_amplitude(double *cover, const int cover_len, const double *message, const int msg_len);

int modulate_digital_phase(double *cover, const int cover_len, const unsigned char *message, const int msg_len);
int modulate_analog_phase(double *cover, const int cover_len, const double *message, const int msg_len);
```

实现说明（最简可运行）：

- 载波：固定频率正弦波。
- 数字调制信号：简单方波比特流（0/1）。
- 模拟调制信号：低频正弦。
- 数字调频：BFSK（0/1 对应两种频率）。
- 模拟调频：`f_inst = fc + kf * m(t)`。
- 数字调幅：ASK（0/1 对应低/高幅值）。
- 模拟调幅：`(1 + ka*m(t)) * cos(2*pi*fc*t)`。
- 数字调相：BPSK（0 -> 0 相位，1 -> pi 相位）。
- 模拟调相：`sin(2*pi*fc*t + kp*m(t))`。

## 4. 构建与运行

```powershell
cmake -S . -B build
cmake --build build
./build/course3_demo.exe
```

程序会输出：

- 第25题四种复用/解复用的序列结果
- 第26题载波/调制信号以及 FM、AM、PM 的前若干采样

也支持自定义第25题输入序列（只允许0/1）：

```powershell
./build/course3_demo.exe 101101 0110
```

含义：

- 第一个参数是序列 `a`
- 第二个参数是序列 `b`
