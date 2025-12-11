# 🧬 康威生命游戏（Conway's Game of Life）

> 基于 C + EasyX 实现的细胞自动机模拟器  
> 支持交互控制、存档读档、配置自定义

![demo](demo.png)

---

## 🎮 功能需求

- **规则**：标准康威四条规则（8 邻居）
- **鼠标**：点击切换格子生死
- **键盘**：
  - `Space`：暂停/继续
  - `R`：随机重置
  - `C`：清空
  - `S`：保存到 `save.txt`
  - `L`：从 `save.txt` 加载
- **配置**：通过 `config.ini` 设置窗口、网格、速度

---

## 🏗️ 架构与接口

### 模块分工
| 文件 | 职责 | 负责人 |
|------|------|--------|
| `main.c` | 主循环、存档、配置 | Solstice |
| `core.c` | 生命演化逻辑 | snow |
| `input.c` | 鼠标/键盘处理 | 成员B |
| `render.c` | 图形绘制 | James |

### 全局状态（仅在 `main.c` 定义）
```c
int grid[100][100];   // 1=活, 0=死
int GRID_ROWS, GRID_COLS;
int paused;           // 0=运行, 1=暂停
