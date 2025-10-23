ASC-EC-SecondCheck
ASC智能车实验室电控二轮考核任务

目前模块分配：
TIM1 定时器定时中断
PA0 非阻塞式按键
=================================================

详细工程日志

2025.10.18
第一次正式使用git，并创建了该项目。
单独使用了vscode来编写该readme.txt文件
项目主要部分仍然在keil5上编写

上传了ASC电控一轮考核的代码

2025.10.20
发现了自己对任务理解的错误
重新建立了Github上的库YINGYYUN/ASC-EC-SecondRoound-Check
仍然考虑以江协的全功能按键实现（清空主函数部分）作为空白模板，方便后续操作

2025.10.22
（今日操作基于江协PID课程以及验收任务描述）
第一次提交
更新了模块 （由于功能 完备/简单，未开分支，在测试成功后上传）
-江协OLED V2.0 UFT-8（此版本中保留测试实例）
    支持OLED_8X16和OLED_6X8两种字体
    支持中英文
    支持类printf格式化输出
    [Hardware/OLED.c+OLED.h+OLED_Data.c+OLED.Data.h]

-PC13端口LED（方便测试程序？）
    [Hardware/LED.c+Led.h]

第二次提交
更新了模块（已测试完毕，不开分支）
-定时器定时中断
    从由TIM2定时器改为TIM1负责
    定时时间暂定为1ms
    [System/Timer.c+Timer.h]
-非阻塞式按键PA0
    目前仅有单击的功能
    [Hardware/Key.c+Key.h]

2025.10.23
*增加了'feature/uart-chuan-kou'分支
-串口模块
    TX:PB10;RX:PB11

    备注：使用了Vofa+来作为上位机辅助
    成功判定示例：Vofa+选择\r\n（发送按钮左），同时内容不换行
