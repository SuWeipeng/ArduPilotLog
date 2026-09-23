# ArduPilotLog 设置文件与功能说明

> 本文基于源码（`src/Dialog.cpp`、`src/APLDataCache.cpp`、`mainwindow.cpp`、`src/DataAnalyze.qml` 等）整理，介绍两个 JSON 设置文件的作用，以及界面上各项功能的来源与用法。

ArduPilotLog 是一个 ArduPilot 飞行日志（`*.bin` / `*.csv`）绘图分析软件（Qt6 Widgets + QML + QCustomPlot + SQLite，架构源自 QGC）。设计目的：快速显示 Log 图线，用于排查程序 bug、积累调试经验（见根目录 `ReadMe.md`）。

**数据流**：打开日志 → `APLReadWorker` 后台线程解码 → `APLDataCache`（单例）内存缓存（按消息分表存二进制行）→ 左侧消息树勾选字段即绘图；也可经 Data Analyze 面板精细绘图、加载 `.conf` 绘图脚本、导出 CSV / SQLite DB、生成并运行 Python (matplotlib) 分析脚本。

**启动行为**：主窗口构造完成后会立即自动触发一次 "Open ArduPilotLog"，弹出日志选择对话框。

---

## 一、settings.json —— 全局设置

- 位置：**程序工作目录**（代码中以相对路径 `"settings.json"` 打开，即 exe 运行目录）。
- 首次运行若不存在，程序自动创建默认版本（"Default settings.json created successfully."）。
- 由 `Dialog::loadSettings()` 在启动和每次打开文件后读取，并同步到 `APLDataCache` 单例。

```json
{
    "opendir": "D:/Log",
    "table_split": false,
    "save_csv": false,
    "trim_from": 0,
    "trim_to": 0,
    "filter_file": null,
    "python_path": "C:/ProgramData/anaconda3/python.exe",
    "python_ingnore_db": false
}
```

| 键 | 类型 | 默认 | 作用（对应源码） |
|---|---|---|---|
| `opendir` | string | `"D:/Log"` | 打开日志对话框的**初始目录**（不存在时回退到桌面）。 |
| `table_split` | bool | `false` | **按实例拆分消息表**。为 `true` 时，凡第二个字段是实例字段（`I`/`Instance`/`C`/`IMU`/`Type`/`Id`，`EV`/`MULT`/`UNIT` 除外）的消息，不再合并进一张表，而是按实例值拆成独立子表（表名 = 消息名+实例号，如 `IMU` → `IMU0`、`IMU1`）。对应 Tools 菜单 "Split Table"，切换后会清空树和图并**重新解析日志**。 |
| `save_csv` | bool | `false` | 解析完 `.bin` 后**自动导出 CSV**：在日志同目录生成 `<日志名>_csv/` 文件夹，内含每个消息一个 `<消息名>.csv` 和一份 `metadata.json`（消息格式定义）。 |
| `trim_from` | int | `0` | **裁剪起点（TimeUS，微秒）**。与 `trim_to` 一起生效：仅当 `trim_from < trim_to` 时启用裁剪，解码时丢弃第一条字段（时间戳）不在 `[trim_from, trim_to]` 内的数据行；`PARM`、`FMTU` 两类消息豁免（始终保留）；时间超过 `trim_to` 后置 `trim_complete` 提前结束解码。 |
| `trim_to` | int | `0` | **裁剪终点（TimeUS，微秒）**，见上。 |
| `filter_file` | string/null | `null` | **过滤规则文件路径**（通常指向 `settings_filter.json`，见下一节）。为 `null` 或缺省时过滤模式置 `-1`，即**不过滤**。 |
| `python_path` | string/null | `null` | **Python 解释器路径**，用于 "Load *.py" 运行分析脚本。加载时会检查该文件是否存在，不存在则清空并提示 "Invalid python_path."。 |
| `python_ingnore_db` | bool | `false` | **Python 忽略 .db**（键名原文如此，对应 File 菜单复选框 "Python Ignore *.db"）。为 `true` 时，"Generate .py" 生成的是基于 `LogCSVParser` 的脚本（直接解析 CSV/日志），否则生成基于 `LogDBParser` 的脚本（读取已导出的 `.db`）。 |

注意：`trim()` / `split()` / `ignore_db()` 是以**逐行文本替换**的方式把对应键写回 settings.json 的，因此手动编辑时请保持 `"键": 值` 的行格式。

---

## 二、settings_filter.json —— 消息过滤规则

该文件路径由 `settings.json` 的 `filter_file` 指定（文件不存在时程序会按下面模板**自动创建**）。解析日志时按消息名过滤，减少内存占用、加快解析。

```json
{
    "filter_mode": 0, // 0-Include, 1-Exclude
    "include": "ATT, BARO0",
    "exclude": "ATT, BARO0"
}
```

| 键 | 类型 | 作用 |
|---|---|---|
| `filter_mode` | int | `0` = **Include（白名单）**：只保留 `include` 列表中的消息；`1` = **Exclude（黑名单）**：丢弃 `exclude` 列表中的消息。开启 `table_split` 时，拆分出的实例表名（如 `IMU0`）同样参与过滤。 |
| `include` | string | 白名单消息名，**逗号分隔**（解析时自动去空格），如 `"ATT, BARO0"`。 |
| `exclude` | string | 黑名单消息名，格式同上。 |

- 该文件经过 `preprocessJsonData()` 预处理，**支持 `//` 行注释和 `/* */` 块注释**（普通 settings.json 不支持，是严格 JSON）。
- `filter_file` 未配置（null）时，`filter_mode = -1`，不过滤任何消息。
- 导出文件命名会引用此文件：导出的 `.db` / CSV 目录名会附加 `_<过滤文件名去掉扩展名>-<filter_mode>`，例如 `log_001-settings_filter-0.db`，便于区分不同过滤条件下的数据。

---

## 三、界面功能（不在 JSON 设置中的）

### 3.1 File 菜单

| 菜单项 | 说明 |
|---|---|
| Open ArduPilotLog | 打开日志：`Binary files(*.bin *.BIN *.csv *.CSV)`。选 `.bin` → 后台解码；选 `.csv` → 进入 **CSV 模式**（见 3.6）。打开前会清空树和图并重新加载 settings.json。 |
| Export *.csv | 手动把当前缓存数据导出为 CSV（`<日志名>_csv/` 目录，每消息一个 .csv + metadata.json），后台线程执行，完成后弹 "Save success"。 |
| Export *.db | 把当前数据另存为 SQLite 数据库（`DB files(*.db)`）。默认文件名 `<日志名>[_<过滤文件>-<mode>][_trim].db`；由 `SaveAsWorker` 在后台线程重建 maintable + 各消息子表。 |
| Load *.conf | 加载**绘图脚本**（`Config files(*.conf)`），一键复现整套绘图。语法见 3.7；目录记忆在程序目录 `confdir.txt`。 |
| Load *.py | 用 `python_path` 指定的解释器运行 Python 脚本，`DialogPython` 窗口实时显示脚本的 `Output:` / `Error:` 和退出码。 |
| Trim | 应用裁剪：把当前内存中的 `trim_from`/`trim_to` 写回 settings.json，清空树和图，**重新解析日志**（只保留时间窗内数据）。 |
| Generate .py for DB | 生成 Python 分析脚本 `<日志目录>/Python/generated_for_db.py`，基于 ArduPilot 官方 matplotlib 工具 `utilities.LogDBParser` 从 `.db` 读数据。仅当树中勾选过字段后显示。 |
| Generate .py for CSV | 同上，生成 `generated_for_csv.py`，基于 `utilities.LogCSVParser`。 |
| ☑ Python Ignore *.db | 复选框菜单项，即 `python_ingnore_db`；导出 CSV 后会被自动勾选（因为此时数据以 CSV 形式存在）。 |

> 生成的 `.py` 是完整的多子图 matplotlib 模板：自动导入所选消息/字段、含 `PARM` 参数读取、`FSM`/飞行阶段标记、`annotate_parm` 参数标注、`plot_frequency`、低通滤波等工具函数及鼠标交互，修改后可用 "Load *.py" 反复运行（脚本依赖 ArduPilot 仓库的 `utilities/` 与 `metadata.json`）。

### 3.2 Tools 菜单（动态创建）

| 菜单项 | 说明 |
|---|---|
| ☑ Show TimeUS | 开启**游标跟踪线**：鼠标在绘图区移动时显示红色虚线竖线，顶部随动显示该处时间 `xxxxx us`。 |
| ☑ Split Table | 即 `table_split`，切换后重新解析日志。 |
| ☑ Data Analyze | 显示/隐藏 "Data Analyze" 停靠面板（QML 页面）。 |

### 3.3 Trim 的完整用法（游标记点 → 裁剪 → 导出）

1. 勾选 Tools → Show TimeUS；
2. 在图上**左键点击**两个位置：放置两条固定红色虚线（自动保留最近两条），并把两点时间（自动排序为起点/终点）设为 `trim_from`/`trim_to`；
3. File → Trim：写回 settings.json 并重新解析，得到只含该时间窗的日志；
4. File → Export *.db / Export *.csv 导出，文件名自动带 `_trim` 后缀。
   （若图中没有记点就点 Trim，则相当于把 trim 范围清零、取消裁剪。）

### 3.4 左侧消息树

- 解析完成后按消息类型建父节点、字段建子节点（带复选框）。
- **勾选字段即绘图**；父子联动：父节点三态（全选/半选/全不选）。
- 每次勾选会把 消息→字段列表 同步给 Data Analyze 面板的 Table 下拉框。

### 3.5 绘图区（QCustomPlot）

- 交互：拖动平移、滚轮缩放、点击选中曲线；自动显示图例（曲线名 = `表.字段`）。
- **X 轴时间轴**自动按日志类型取 `TimeUS`（微秒）或 `TimeMS`（毫秒），内部统一换算成秒；刻度格式按总时长自动切换：不足 1 小时显示 `Time (Min:S.MS)`，否则 `Time (H:Min:S.MS)`。
- **右键菜单**：
  - Clear —— 清空全部曲线；
  - Reset graph —— 复位视图；
  - zoom X / zoom Y —— 仅 X / Y 向缩放；
  - Zoom All —— 恢复双向缩放。
- Tools → Show TimeUS 时还有游标线与左键固定标线（见 3.3）。

### 3.6 CSV 模式

打开 `.csv` 日志时：以该文件所在**目录**为数据集，扫描目录下全部 `*.csv` 的表头作为消息/字段来源；此时 "Export *.csv / Export *.db / Trim" 菜单被隐藏，并自动勾选 "Python Ignore *.db"。

### 3.7 .conf 绘图脚本（Load *.conf）

文本脚本，每行一条曲线/参考线（详见 `conf/ReadMe.md`，仓库内置 32 个示例）：

```text
# 绘制日志数据
Table.Field.LineStyle.Color                    例：ATT.DesRoll.0.0
Table.Field.LineStyle.Color(Scale,OffsetX,OffsetY)   例：ATT.DesRoll.0.0(2,0,0)

# 绘制水平参考线
<const> Table:value LineStyle.Color            例：<const> BARO:200 0.0
```

- LineStyle：0-正常，1-加粗，2-加阴影，3-加粗加阴影，4-虚线，5-虚线加粗，6-点虚线，7-线上空心圆，8-线上实心圆，9-只有空心圆
- Color：0-红，1-绿，2-蓝，3-紫，4-棕，5-粉，6-深天蓝，7-橙，8-深青，9-金

### 3.8 Data Analyze 面板（数据分析窗）

QML 实现的 10 通道精确绘图面板，每行一个通道，列头为 **State | Table | Field | Scale | X+ | Y+ | Line | Color**：

| 列 | 作用 |
|---|---|
| State | 显示/隐藏按钮：显示行号 `1`~`10` 表示启用，`hide` 表示隐藏该通道曲线 |
| Table / Field | 选择消息类型与字段（下拉框，来源为消息树中已出现的表） |
| Scale | 纵向缩放系数（`y = (值 + Y+) × Scale`） |
| X+ / Y+ | X / Y 方向平移量，用于多条曲线相位对齐、上下错开对比 |
| Line | 线型：Normal、Line1、Line2、Line3、Dot1、Dot2、Dot3、Mark1、Mark2、Mark3（含义同上表 LineStyle） |
| Color | 颜色：Red、Green、Blue、Magenta、LawnGreen、Pink、DeepSkyBlue、Orange、DarkCyan、Gold；**已占用的颜色会从下拉中移除**，超过 10 条曲线时自动随机取色 |

底部按钮：**Init**（初始化/复位全部通道）、**Hide All**（一键隐藏全部通道）。

### 3.9 其他

- **日志分类调试**：内置 Qt 日志分类（`MainWindowLog`、`APLDBLog`、`APLReadLog`、`APLDataCacheLog`、`DialogLog`、`DialogLoadLog`、`DialogPythonLog`、`APLReadConfLog`、`DataAnalyzeLog`、`APLRunPythonLog`），经 QGC 式 `qCDebug()` + `LoggingFilters` 规则控制输出，便于排查解析/入库问题。
- **confdir.txt**：程序自动维护，记住 `.conf` 绘图脚本目录。
- **matlab/**：提供把日志数据导入 MATLAB 的脚本（面向更深层分析需求）。
- **进度条**：解析时显示进度（万分比 → 百分比）。

---

## 四、典型工作流

1. 首次运行：确认 settings.json 中 `opendir`、`python_path`；如需缩小解析范围，配置 `filter_file` → settings_filter.json 白/黑名单。
2. **点选绘图**（快速浏览）：Open ArduPilotLog 打开 `.bin` → 左侧树勾选字段看图。
3. **数据分析窗**（精确对比）：勾选相关字段后打开 Data Analyze 面板，用 Scale / X+ / Y+ / 线型 / 颜色细调，锁定现象。
4. **裁剪时间窗**：Show TimeUS → 图上点两点 → Trim 重新解析 → 得到干净的时间段。
5. **脚本绘图**（反复验证）：Generate .py for DB/CSV 生成脚本 → 按需修改 → Load *.py 运行；或直接 Load *.conf 复现历史经验。
6. **数据交换**：Export *.db（SQLite，供 MATLAB/Excel/SQLite 工具使用）或 Export *.csv（配合 metadata.json）。
