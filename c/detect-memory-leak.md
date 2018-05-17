# Linux开发中c/c++内存问题检测

*Author: Wang Xiaoqiang wang_xiaoq@126.com*

标签（空格分隔）： 内存泄漏 内存越界 段错误

---

## 讨论范围

本文只讨论Linux下c/c++中遇到的各种内存问题。

## 内存问题分类

1. 内存泄漏
2. 内存越界访问（段错误）

## 应用程序内存问题检测

用户态内存问题检测需要用到工具valgrind，它的使用步骤如下：

1. 安装：`sudo apt-get install valgrind`；
2. 应用程程序在gcc/g++编译时，需要添加-g选项；
3. 使用valgrind运行应用程序（假设应用程序的名字为a.out）：
```
valgrind -v --log-file=valgrind.log --tool=memcheck --leak-check=full --show-mismatched-frees=yes ./a.out
```
-v：打印详细的信息；
--log-file：指定输出的日志文件；
--tool：指定内存检测工具，memcheck是对内存泄漏、越界和非法指针检测的工具；
--leak-check：内存泄漏检测力度，它有如下取值：
```
no - 不检测内存泄漏；
summary - 仅报告总共内存泄漏的数量，不报告内存泄漏的具体位置；
yes/full - 报告泄漏总数，泄漏的数量；
```
--show-mismatched-frees：检测释放内存的函数与分配内存的函数是否一致，分配函数与释放函数的对应关系人下表：

内存分配函数 | 内存释放函数
:-:|:-:
malloc | free
new | delete
new [] | delete []
例如，不能使用malloc分配内存，而使用delete释放；
4. 等待程序运行结束，如果是长期运行的程序，在运行一段时间后，可以使用<Ctrl+c>中断程序，查看输出日志信息。

注意：valgrind输出的消息，有99%的准确率，一定要认真对待。

## Linux内核开发内存泄漏检测

内核中内存泄漏的检测工具是kmemleak，它的基本使用步骤如下：

1. 下载对应的内核源码，配置.config文件，选中CONFIG_DEBUG_KMEMLEAK， CONFIG_DEBUG_KMEMLEAK_EARLY_LOG_SIZE参数应设置为2000或者更大，使用默认值会造成/sys/kernel/debug/kmemleak文件不被创建；

2. 编译内核并安装；

3. 重启新内核：
查看内存泄漏情况的详细信息：
```
cd /sys/kernel/debug
cat kmemleak
```
kmemleak每隔十分钟扫描一次内存，也可以手动触发：
```
echo scan > kmemleak
```
清除之前检测到的内存错误信息：
```
echo clear > kmemleak
```
；
4. kmemleak基本原理

kmemleak构建了一棵树，当分配内存时，会将分配的内存地址添加到树中；释放内存时，会将对应的内存地址从树种删除。每当扫描时间到了之后，就会根据树中的地址，在内存中进行寻找，如果内存中不存在数值等于要找的地址，并且不存在数值落在这块分配的地址空间之内，就认为这块内存泄露了，因为没有办法通过直接或间接的方式释放这块内存了。

这种判断方式造成kmemleak有可能会误报：

* 内存实际已经泄漏，却无法报告：但此时内存中恰好存在一个数值等于已分配的内存块的地址；
* 内存未泄漏，却报告内存泄漏：通过比较特殊的间接访问的方式；




