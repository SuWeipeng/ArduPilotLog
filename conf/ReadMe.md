# 配置文件的格式
## 一、绘制LOG数据
> LineStyle: 0-正常，1-加粗，2-加阴影，3-加粗加阴影，4-虚线，5-虚线加粗，6-点虚线，7-线上空心圆，8-线上实心圆，9-只有空心圆
> Color: 0-红，1-绿，2-蓝，3-紫，4-棕，5-粉，6-深天蓝，7-橙，8-深青，9-金
### 1. 无申缩或相位变换
`Table.Field.LineStyle.Color`,例如：ATT.DesRoll.0.0
### 2. 有申缩或相位变换
`Table.Field.LineStyle.Color(Scale,OffsetX,OffsetY)`,例如：ATT.DesRoll.0.0(2,0,0)
## 二、绘制参考线
`<Command> Table:value LineStyle.Color`,例如：&lt;const&gt; BARO:200 0.0
