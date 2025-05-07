# 配置方法
推荐使用ubuntu
- 在电脑上下载qt creator
- 在目标文件夹（路径不要含有中文）执行下列指令 
- 之后打开qt creator出现以下界面
![Qt](./image/1.png "Qt Creator")
- 将CORE文件夹直接拖入该界面，进入以下界面
![Qt](./image/2.png "Qt Creator")
- 点击锤子图标进行编译
![APP](./image/4.png "App")
- 说明配置完成了

# 使用说明
## UI说明
addNode按钮，以上方两个文本框输入的坐标作为节点坐标，添加新节点

addRouteMode按钮，点击后进入添加路径模式，随后按顺序点击地图上的节点可以连接两个节点

batchNode按钮可以批量增加节点，输入起始坐标startX, startY 节点数量nodeNum 节点间间隔intervalX, intervalY
顺序生成一系列节点

saveMap可以保存当前的地图到MAP文件夹中，以二进制文件形式

loadMap可以加载二进制文件

## 布置算法

`savemap.bin` is the main map <<-------------------
                                                ^
`mainMapBackup.bin` is the backup of the main map |

node可用属性{id, x(表示坐标), y}

route可用属性{id, node1ID(表示连接的两个节点ID。默认单向路径，node1ID为进入点，node2ID为退出点), node2ID, attr(路径类型：单向、双向)}