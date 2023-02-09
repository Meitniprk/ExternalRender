# Narlebone客户端 外部模块之一

## 网络
默认连接127.0.0.1:61311

## 结构
前4字节为data数据长度

data数据示例:
```json
{
  "size": 5,
  "list": [
    {"y": 200.0,"x": 600.0,"name": "test1"},
    {"y": 300.0,"x": 500.0,"name": "test2"},
    {"y": 400.0,"x": 500.0,"name": "test3"},
    {"y": 500.0,"x": 500.0,"name": "test4"},
    {"y": 600.0,"x": 500.0,"name": "test5"}
  ]
}
```
## 效果预览:
![Image text](Preview.png)