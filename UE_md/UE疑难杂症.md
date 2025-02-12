他妈的，场景中有一个Actor（coin），玩家碰到他的话，会将信息发送给playerController，然后playerController调用playerState中的一个addCoin的函数，增加1个金币。如何实现？

这是MVC设计模式，然后碰到困难，妈的默认的PlayerState找不到，也打不开，我怎么在里面设置数据？

然后重新创建了一个，妈的又一直在playerController中找不到PlayerState中我创建的函数，一直找不到，然后一个及其优秀的思路来了。

在playerController中添加变量，然后变量类型用你新创建的PlayerState的对象引用，然后就能获取他的函数了。

获取金币功能没有成功



【如何在虚幻引擎 5 中制作自定义碰撞】 https://www.bilibili.com/video/BV1Mc411q7Tn/?share_source=copy_web&vd_source=448909cdfe7ff87e464eb123889e9d9a
