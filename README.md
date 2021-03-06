# DataHiding
贵济大三上信息隐藏实验个人项目 by钟计东


[toc]



# 一、伪随机数发生器



## 1.1 实验背景



- 根据密码学原理，随机数的随机性检验可以分为三个标准：

1、 统计学伪随机性：在给定的随机比特流样本中，1的数量大致等于0的数量。同理，00，11，01，10四者数量大致相等；

2、 密码学安全伪随机性：给定随机样本的一部分和随机算法，不能有效地演算出随机样本的剩余部分；

3、 真随机性：随机样本不可重现。

 

- 相应的，随机数也分为三类：

1、 伪随机数：满足第一个条件的随机数；

2、 密码学安全的伪随机数：同时满足前两个条件的随机数，可以通过密码学安全伪随机数生成器计算得出；

3、 真随机数：同时满足三个条件的随机数。

 

真随机数通过物理实验得出，要满足**随机性，不可预测性和不可重现性。**

伪随机数通过一定的算法和种子得出。本次实验实现的正是为随机数。

 

只要随机数是由确定算法生成的，那就是伪随机数。**不断优化算法只能使随机数接近随机。现代计算机中，**目前无法通过一个纯算法来生成真正的随机数。无论是哪种语言，单纯的算法生成的数字都是伪随机数，都是由可确定的函数通过一个种子产生的伪随机数。

这也就意味着如果知道了种子，就可以推断接下来的随机数序列的信息。

 

伪随机数发生器采用特定的算法，将随机数种子seed转换成一系列的伪随机数。伪随机数依赖于seed的值，给定相同的seed值总是生成相同的随机数。伪随机数的生成过程只依赖CPU，不依赖任何外部设备，生成速度快，不会阻塞。

**本次实验将通过时间种子生成符合一定分布规律的伪随机数。**



 

 

## 1.2 实验人员和开发平台介绍

- 成员：

- 开发平台：

Visual studio 2019，使用C++语言进行开发。



 

 

## 1.3 实验原理

 

### 1.3.1 生成符合指数分布的伪随机数：

指数分布本质上是gamma分布的一种特殊形式，可以通过均匀函数的变化实现。

生成符合0-1分布的均匀函数则通过Messene Twister算法。

MT19937算法是利用线性反馈移位寄存器产生随机数的。整个算法主要分为三个阶段：

1） 获得基础的梅森旋转链；

2） 对于旋转链进行旋转算法；

3） 对于旋转算法得到的结果进行处理。

 

- 从C++ 11开始，可以直接使用此算法。本实验中将直接使用random.h文件中自带的MT19937算法。

![img](F:/大三上/信息隐藏/实验一/pics/1.png)

 

- 随机数则根据公式生成：

![img](F:/大三上/信息隐藏/实验一/pics/2.png)

 

- 尺度参数的估计：

由于随机数符合指数分布，那么尺度参数beta即为随机数的期望。

![img](F:/大三上/信息隐藏/实验一/pics/3.png)

### 1.3.2 生成符合广义高斯分布的伪随机数

1） c=1.0时：

此时广义高斯分布即为Laplace分布。

![img](F:/大三上/信息隐藏/实验一/pics/4.png)

2） c=0.5时：

E的gamma分布的形状参数为2.0，尺度函数为1/(sqrt beta) 。

先生成符合伯努利分布的伪随机数w，

![img](F:/大三上/信息隐藏/实验一/pics/5.png)

再生成符合gamma分布的伪随机数E。

![img](F:/大三上/信息隐藏/实验一/pics/6.png)

**若w=1，符合广义高斯分布的伪随机数z=E^c；反之z=-E^c。**

![img](F:/大三上/信息隐藏/实验一/pics/7.png)



 

 

## 1.4 实验结果比较和分析讨论

### 1.4.1 指数分布测试：

以beta=2进行测试。

- 输出十个值时，计算得出的beta值误差很大：

 

![img](F:/大三上/信息隐藏/实验一/pics/8.png)

 

- 当样本数量变大时（如1000个），误差减小：

![img](F:/大三上/信息隐藏/实验一/pics/9.png)

 

### 1.4.2 广义高斯分布测试c=1.0

![img](F:/大三上/信息隐藏/实验一/pics/10.png)

 

### 1.4.3 广义高斯分布测试c=0.5

 

![img](F:/大三上/信息隐藏/实验一/pics/11.png)



 

 

## 1.5 实验感想

- 实验中出现的bug有：

（1）   调用random头文件中的模板报错：

![img](F:/大三上/信息隐藏/实验一/pics/12.png)

排查后发现在MTRandInt函数中实例化错误，应为int型。

 

（2）   缓冲区溢出

溢出原因为数组申请的空间不足。

 

 

- 小结：

本次实验一个月前就布置了，但一直没有着手开始完成，因为觉得实验要求看起来很难。但开始实现之后，发现实验思路很清晰，实现起来也没有什么难以解决的技术问题。

本次实验最大的挫折就是uniform_real_distribution模块实例化错误，浪费了大量时间，而错误原因仅仅是实例化时数据类型写错。

![img](F:/大三上/信息隐藏/实验一/pics/13.png)

 

 

## 1.7 参考

（1）   梅森旋转算法：https://blog.csdn.net/dianshu1593/article/details/101524003

（2）   伯努利随机数：https://blog.csdn.net/caroline_wendy/article/details/17335871

（3）   求指数分布的期望：

[https://baike.baidu.com/item/%E6%8C%87%E6%95%B0%E5%88%86%E5%B8%83/776702?fr=aladdin](https://baike.baidu.com/item/指数分布/776702?fr=aladdin)

 

# 二、扩频水印实验



# 三、MonteCarlo仿真

