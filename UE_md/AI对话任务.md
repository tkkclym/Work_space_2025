对话使用的struct 数据结构供datatable的使用，这样的datatable可以存储各种需要的信息，本次教程中仅使用了boolean IsNPC和string dialog



### 罗列下对话用到的相关蓝图：

首先是UI，玩家的Interaction组件，NPC上的datatable 增强输入转换逻辑



### 对话逻辑梳理

在player上的interaction 组件中有射线检测，射线检测到的actor保存为变量，然后再有dialog组件获取检测到的actor身上的datatable。然后再根据敌人身上datatable的信息进行显示提示按下空格继续交流，然后按下空格之后开始进行剧情推动 



### 为了实现逐行获取数据表中的对话信息：

dialog过程获取datatable，我们如果要使用的话，需要向将其存储为数组，然后再根据下标进行逐行显示。创建使用current Row，使其后续自加，然后实现逐行显示



### intercation交互组件

组件其实是为了显示一些EF交互以及射线检测的。和剧情对话的dialog要分开



### UI

Ui上的文本控件是能改变对齐方式的justification 如果想再一个文本框的左右两边分别显示文本的话，就使用这个



### 2025年2月16日

对话与任务系统进行绑定，并且在任务进行的同时，其他NPC应该有不同的反馈





剧情线对话，本质是流程图 

任务组件中应该有保存任务进度的地方。然后进行NPC的检测

### 如何实现线性任务？

前置：

Linear Mission数据表格：数据表中三个结构

 Rowname：对应处在任务的第几环。

missionNPC：该环任务需要交互的NPC

DialogTable:与该NPC交互时线性任务的对话信息



首先是在player controller或者是game instance中存储当前任务ID,然后每次检测这个任务ID

在与敌人交互的时候，通过检测任务ID获取一个数据表Liner Mission的表数据。

再判断是不是和missionNPC相同，相同的话再去显示线性任务的对话

如果检测到的敌人和表中要交流的敌人相同，则进行任务对话，并且任务index++，这样任务就会向下进行了



然后敌人身上的话，因为要将线性任务对话和非线性任务对话区分开，有不同的对话内容，所以我们需要一系列的datatable存放对话内容，那怎么管理这些datatable？

### 创建非线性任务的对话datatable

游戏主任务线中对应的有index，所以可以创建一个datatable只存储对话的dataTable，但是存储的是下标不等于主任务下标的内容，就比如跟吕布对话的主线任务下标为2，那么我创建一个datatable中只需要存放1和3的相关对话dataTable，如果任务进行到2的时候，就执行主线中的对话信息了



### 在主角MissionSystem中逻辑实现:

在MissionSystem的EqualLinearCurrentNPC中，当其中判断NPC的时候为false的时候，就用NPC上存放的属于任务线之外的对话table

