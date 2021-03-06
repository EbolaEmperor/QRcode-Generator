# QR-code生成器

### 基本信息

系统环境：`Windows`

语言标准：`c++17`

主程序：`main.cpp`

编译指令：`g++ main.cpp -o main`

使用方法：将文本输入`input.txt`（暂不支持中文），然后双击打开`main.exe`，生成的二维码会被输出到`code.png`，若内容为以`http://`或`https://`开头的有效网址，常用的二维码扫描器扫描后会自动跳转

默认使用最高纠错等级`H`，可以通过修改`config.ini`更改纠错等级：

| 纠错等级 | 容错率 | 代码 |
| ---- | ---- | ---- |
| 低 | 7% | L |
| 中 | 15% | M |
| 高 | 25% | Q |
| 极高 | 30% | H |

纠错等级越高，容错率越高，同时信息容量越低

### 技术细节

RS-code采用高斯消元法计算，版本信息的BCH-code直接使用官方给出的表

二维码以png图像的形式输出，png图像生成是我以前做过的一个简单程序，可以通过RGB矩阵生成png图像

mask选择算法：以四联通相邻的相同颜色块对计数，作为估价函数，选择一个估价最小的mask，估价相同的情况下优先选择编号更大的mask