### 有符号数和无符号数的区别?

首先看一个程序：

```
void main()
{
    short sdata1=-1;
    unsigned short sdata2=1;
   
    
    int data1=-1;
    unsigned int data2=1000;


    if(sdata1>sdata2)
    {
       printf("1");    
    }
    else
    {
       printf("2");    
    }

    printf("\n");
    
    if(data1 > data2)
    {
       printf("1");    
    }
    else
    {
       printf("2");    
    }
}
```

输出的为什么是2 1啊？这个是怎么比较的？

## 解答

在c语言中有一个规则就是：int类型的数据与
unsigned int类型的数据进行比较时会把int类
型的数据转换为unsigned int 类型的数据，然
后再进行比较。
int data1 = -1 转换为unsigned int类型的数
据为：4394967295， 这个数肯定要 > 1000 ,
因此第二个会输出1。

注意：计算机中所有的数据都是按照补码形式存
储的，负数的补码求法是：先求反码，再加1。反
码的求法是最高符号位不变，其余各位按位取反。

*ref*

*http://bbs.bccn.net/thread-334562-1-1.html*
