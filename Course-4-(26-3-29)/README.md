# Course-4 作业实现（奇偶校验）

## 1. 作业要求

编写一个检验奇偶校验码的程序。

- 消息 `msg` 的每个元素只区分两类：
  - `0` 表示比特 `0`
  - 非 `0` 表示比特 `1`
- 返回值：
  - `0` 表示校验失败
  - `1` 表示校验通过

题目接口：

```cpp
int parity_check(const unsigned char *msg, const int msg_length);
```

## 2. 实现说明

本实现采用**偶校验**规则：

- 将 `msg` 中所有非 `0` 元素视为 `1`
- 统计 `1` 的总数
- 若总数为偶数，返回 `1`；否则返回 `0`

边界处理：

- `msg == nullptr` 或 `msg_length <= 0` 时返回 `0`

## 3. 目录结构

```text
src/
  main.cpp
  parity_check.h
  parity_check.cpp
CMakeLists.txt
README.md
```

## 4. 构建与运行

```powershell
cmake -S . -B build
cmake --build build
./build/course4_demo.exe 1010
```

示例说明：

- 输入 `1010`，其中 `1` 的个数为 `2`（偶数），校验通过。
- 输入 `1011`，其中 `1` 的个数为 `3`（奇数），校验失败。
