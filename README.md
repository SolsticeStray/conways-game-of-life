# 康威的生命游戏 (Conway's Game of Life)

NJU 大一智能C语言程序设计期末项目
基于 EasyX 图形库
支持 GUI 交互与读存档功能

## ✨ Feature

### 🖱️ 鼠标交互
- 点击任意格子切换细胞生死状态

### ⏯️ 实时控制
- **Space**：暂停 / 恢复演化  
- **R**：随机生成初始状态  
- **C**：清空世界  

### 💾 存档与读档
- **S**：将当前世界保存到 `save.txt`  
- **L**：从 `save.txt` 加载世界  

### ⚙️ 配置文件
- 通过 `config.ini` 自定义窗口大小、网格密度、演化速度等参数

## 🛠️ 运行与编译

1. 安装 [Visual Studio 2019/2022](https://visualstudio.microsoft.com/)（社区版免费）
2. 安装 [EasyX 图形库](https://easyx.cn/)（一键安装，自动集成到 VS）

### 打开项目
- **Visual Studio 用户**：  
  `文件 → 打开 → 文件夹` → 选择本项目根目录  
  右键 `main.c` → “设为启动项” → 按 F5 运行

- **CLion 用户（Windows）**：  
  直接打开项目文件夹，CMake 会自动配置（需确保已装 EasyX）

> ⚠️ 注意：EasyX 仅支持 Windows。Mac/Linux 用户可编译但无法显示图形。

## 🧩项目结构

```text
conways-game-of-life/
├── .gitignore              # Git 忽略规则（忽略 VS/CLion 临时文件）
├── README.md               # 项目说明文档
├── CMakeLists.txt          # 跨平台构建配置
├── config.ini              # 游戏配置文件
├── main.c                  # 主程序入口
├── core.c                  # 核心算法
├── input.c                 # 用户交互（鼠标/键盘）
├── render.c                # 图形渲染
└── demo.png                # 程序运行截图
