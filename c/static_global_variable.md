### C中static全局变量与非static全局变量的区别

这是在书上看到的一个static全局变量与普通全局变量的一个区别说明

1) 全局变量(外部变量)的说明之前再冠以static 就构成了静态的全局变量。全局变量本身就是静态存储方式， 静态全局变量当然也是静态存储方式。 这两者在存储方式上并无不同。这两者的区别在于非静态全局变量的作用域是整个源程序， 当一个源程序由多个源文件组成时，非静态的全局变量在各个源文件中都是有效的。 而静态全局变量则限制了其作用域， 即只在定义该变量的源文件内有效， 在同一源程序的其它源文件中不能使用它。由于静态全局变量的作用域局限于一个源文件内，只能为该源文件内的函数公用，因此可以避免在其它源文件中引起错误。

这是源文件1.h

```
int a = 2;                //普通全局变量
static int s = 5;         //static全局变量
```

这是源文件2.h

```
int b = 3;                //普通全局变量
```

这是main.c

```
#include <stdio.h>
#include "1.h"
#include "2.h"
 
int func()
{
    return s;
}
 
int main()
{
    printf("%d\n", a);
    printf("%d\n", b);
 
    printf("%d\n", s);        //这里有问题
}
```

在printf("%d\n", s)中， s这个变量是我在1.h这个头文件中定义的一个static全局变量， 而从上面的说明中可以知道static去阿奴姐变量限制了其作用域， 即之灾定义该变量的源文件内有限(即在1.h中有限)。
在同一源程序的其它源文件中不能使用它。可是我却可以在main.c中使用这个变量， 这是不是矛盾了
这个是我现在理解不了的地方， 请各位code monkey讲解一下 

*ref*

*http://bbs.csdn.net/topics/390626289?page=1*
