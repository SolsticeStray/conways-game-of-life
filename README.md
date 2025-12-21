# 🧬 Conway's Game of Life - Ultimate AI Edition

![Project Status](https://img.shields.io/badge/Status-Completed-success)
![Language](https://img.shields.io/badge/Language-C%2FC%2B%2B-blue)
![Platform](https://img.shields.io/badge/Platform-Windows-0078D6)
![Library](https://img.shields.io/badge/Library-EasyX-orange)

> **“如果不把数据可视化，生命游戏仅仅是一堆闪烁的像素。”**
>
> 这是一个基于 C 语言与 EasyX 图形库构建的、深度改良版的康威生命游戏。除了经典的细胞自动机模拟外，本项目引入了**计算机视觉（CV）识别子系统**、**实时热力渲染引擎**以及**生态趋势分析仪表盘**，将传统的模拟器升级为具备初步感知能力的“数字生物实验室”。

---

## ✨ 核心特性 (Key Features)

本项目不再局限于简单的规则演化，而是引入了多层面的感知与控制系统：

### 🧠 1. 智能感知系统 (Computer Vision System)
不同于传统的死板模拟，我们的系统能够“看懂”画面：
- **基于规则的计算机视觉 (Rule-based CV)**：实时卷积扫描整个细胞网格。
- **结构识别**：能够自动定位并框选出 **Block (静止块)**、**Oscillator (振荡器/信号灯)**、**Glider (滑翔机)** 等经典结构。
- **智能防重叠**：引入访问标记算法（Visited Mask），防止识别框重叠，确保视觉输出的整洁。

### 🔥 2. 生态热力渲染 (Thermal Rendering)
为了直观展示群落内部的压力分布，我们移除了二值化渲染：
- **局部密度分析**：根据每个细胞周围 8 邻域的拥挤程度动态着色。
- **色阶映射**：
  - 🔵 **蓝色**：孤独/扩张期 (Neighbors ≤ 1)
  - 🟢 **绿色**：健康/稳定期 (Neighbors = 2/3)
  - 🔴 **红色**：过载/死亡期 (Neighbors ≥ 4)

### 📊 3. 趋势分析仪表盘 (Trend Analysis HUD)
顶部集成了类似 IDE 的实时抬头显示器 (HUD)：
- **实时数据流**：显示当前代数 (Gen)、演化速度 (Speed)、各子系统状态。
- **生态预测算法**：通过环形缓冲区记录历史人口数据，计算种群的一阶导数（变化率）。
  - **GROWING**：种群正在扩张（绿色提示）。
  - **STABLE**：达到动态平衡（青色提示）。
  - **DYING**：走向灭绝（红色提示）。

### ⚙️ 4. 工业级系统架构
- **配置驱动 (Config-Driven)**：支持通过 `config.ini` 自定义分辨率、网格大小、全屏模式等。
- **响应式窗口**：支持 Windows 窗口最大化/拖拽，网格自动重算，自适应屏幕分辨率。
- **持久化存储**：完整的 Save/Load 存档系统，支持断点续存。
- **变速齿轮**：支持 1x / 2x / 5x 的物理倍速切换。

---

## 📸 运行截图 (Screenshots)

### 1. 智能识别模式 (Computer Vision Mode)
![CV Vision Demo](此处请放入你那张带有青色识别框的截图.png)
*系统自动识别并标注了画面中的 Block 和 Oscillator 结构。*

### 2. 热力图模式 (Heatmap Mode)
![Heatmap Demo](此处请放入你那张五颜六色的截图.png)
*通过颜色区分细胞的生存状态，直观展示群落的内部压力。*

---

## 🎮 操作指南 (Controls)

| 按键 | 功能描述 |
| :--- | :--- |
| **Space** | **暂停 / 继续** (核心控制) |
| **H** | **热力图开关** (Toggle Heatmap) |
| **V** | **CV 识别开关** (Toggle CV Vision) |
| **F** | **变速** (1x -> 2x -> 5x 循环切换) |
| **S** | **保存** (Save to save.txt) |
| **L** | **加载** (Load from save.txt) |
| **R** | **随机重置** (Randomize Grid) |
| **C** | **清空画布** (Clear All) |
| **ESC** | **退出程序** |
| **鼠标左键** | **上帝之手**：点击网格生成/擦除细胞 |

---

## 🛠️ 配置说明 (Configuration)

在程序同级目录下创建 `config.ini` 文件即可修改核心参数：

```ini
[Settings]
; 0=窗口模式, 1=全屏沉浸模式
fullscreen=0
; 窗口分辨率 (fullscreen=1 时此项自动忽略)
width=1280
height=720
; 网格密度
rows=80
cols=120
; 单个细胞像素大小
cell_size=10
```

🚀 编译与构建 (Build)
本项目基于 Visual Studio 2022 和 EasyX 图形库开发。

环境要求:
- IDE: Visual Studio 2022 (必须安装 "使用 C++ 的桌面开发" 工作负载)。
- SDK: EasyX Graphic Library (最新版)。
编译步骤:
- 克隆本仓库到本地。
- 使用 Visual Studio 打开 .sln 或者直接导入源文件。
关键设置：
- 右键项目属性 -> 高级 (Advanced) -> 字符集 (Character Set)。
必须设置为：使用多字节字符集 (Use Multi-Byte Character Set)。
注：这是为了兼容 EasyX 的文字输出与 TCHAR 通用类型。
点击 Local Windows Debugger 即可运行。

👨‍💻 团队成员 (Credits)
本项目由大一 C 语言项目小组开发完成：

System Brain (Leader): [Solstice] - 负责系统架构、主循环、数据分析、整合、热力图算法、CV 识别绘制。
Visual Artist: [James] - 负责 Render 渲染管线。
Logic Core: [Snow] - 负责 Core 核心演化算法、数据结构。
Input Handler: [Flynn] - 负责外设交互、文件 I/O、快捷键逻辑。
