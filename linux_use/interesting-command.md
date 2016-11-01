### 几个Linux终端下超赞的小命令 

*致谢：本文来自longerzone专栏，非常感谢云龙兄无私奉献*

*ref: http://blog.csdn.net/longerzone/article/details/8637530*

编者注： 如下几个小命令可能是这些Linux大牛们一时兴起，玩的东东，不过这也证明了我们Nichol的一句话 “ Programmer are just like children  no matter  how old  he is ”（好吧，大致是这样，就是说：程序员们不管多大都像小孩一样爱玩）

一、  cowsay 和 xcowsay

这两个小工具挺好玩的，使用apt-get install cowsay安装即可。使用也很简单不用多说直接上图 ： 


基本上，我们会看到一头牛告诉我们所输入的信息。尽管这个命令中只出现了cow，但其实它还可以有羊的效果:
         
只需用 '-l 就能看到它能提供的所有动物。输入：

```
$cowsay -l
```

系统会弹出下列信息：

```
Cow files in /usr/cowsay/cows:
apt beavis.zen bong bud-frogs bunny calvin cheese cock cower daemon default
dragon dragon-and-cow duck elephant elephant-in-snake eyes flaming-sheep
ghostbusters gnu head-in hellokitty kiss kitty koala kosh luke-koala
mech-and-cow meow milk moofasa moose mutilated pony pony-smaller ren sheep
skeleton snowman sodomized-sheep stegosaurus stimpy suse three-eyes turkey
turtle tux unipony unipony-smaller vader vader-koala www
```

因此，我们可以用 ' -f '参数输入以上信息中的任何动物。

对于xcowsay使用与之类似(好像还不支持其他动物)，只做演示，不深研究：


使用xcowsay命令后，你看到那个可能会出现在屏幕中任何地方的小奶牛（好吧，很像一个小狗狗）了么？哈哈，萌翻了没有！！！
注：我试过了，很遗憾的是远程登陆其他人的机器，使用这个打印出来的是在你这端的屏幕上，而非对方……所以，想整蛊朋友的童鞋们得想想其他办法了啊！

二、sl 命令

sl?打错字了吧？是ls吧？哈哈，非也，不过也可能就是这个原因，sl被创造出来……给打错字的程序猿们也能有点小乐趣！小火车相当拉风啊！
Ubuntu/Debian用户可以直接通过 sudo apt-get install sl 安装。


三、linuxlogo 命令

使用linuxlogo可以直接查看当前比较流行的Linux发行版的Logo哦！挺好玩的！
Ubuntu用户直接 sudo  apt-get install linuxlogo 安装即可，安装好了后，使用如下命令使用“ linuxlogo -L  ubuntu ” 


四、Linux "yes" 命令

这里是一个在进程结束前一直打印输入字符串的命令。万一，用户不指定任何输入字符串，默认命令是"y"。


命令会这样运行：

```
        $yes mylinuxbook 
        mylinuxbook 
        mylinuxbook 
        mylinuxbook 
        mylinuxbook 
        mylinuxbook 
        mylinuxbook 
        mylinuxbook 
        mylinuxbook 
        mylinuxbook 
        mylinuxbook 
        mylinuxbook 
        mylinuxbook 
        mylinuxbook 
        ^C$
```
用"ctrl+c"结束前台运行进程。

很奇怪，是吗？为什么我们希望命令在进程中一直打印字符串呢？尽管这样运行的方式很傻，但却不一定没用。我们可以用特定脚本中的"yes"命令，在这些脚本中，我们可以用重复操作作答。例如，我们可以在一组文件上进行操作，而Linux会要求我们对文件进行确认。我们可以用"yes"命令自动回复。

五、Linux"rev"命令

尽管非常简单，但我发现这行命令其实非常有意思。它会一行接一行地颠倒所输入的字符串。"rev"命令的名称来源于英文reverse。可以在命令指示符中输入"rev"然后按回车
        $ rev
终端会提示我们通过标准输入提供一个字符串。那么在每行后面我们就可以同时看到输出。


比如，我们输入"mylinuxbook"然后按回车。

```
        mylinuxbook
        koobxunilym
```

为了回到命令提示符，可以使用"ctrl+c"。

我们看到颠倒的字符串直接出现在了下一行。然后，我们可以输入下一个希望出现颠倒效果的字符串，接着按回车，就可以得到标准输出了。

```
        $ rev
        google
        elgoog
        linux is fun
        nuf si xunil
        ^C$
```
这行命令也可以以文件的形式输入，它会颠倒文件中的所有字符串，然后把输出结果打成标准输出。

```
        $ cat strings.txt
        MyLinuxBook
        Linux is so much fun!
        Happy Birthday
        $ rev strings.txt
        kooBxuniLyM
        !nuf hcum os si xuniL
        yadhtriB yppaH
```

六、Linux"fortune" 命令

Fortune命令是在相同命令行中作为fortune cookies。我们运行这个命令的时候随机找了一条谚语或信息。

首先要对其进行安装：

```
        $sudo apt-get install fortune
```
安装完成后，我们运行这个命令的方式就是：

```
        $ fortune
        You have the power to influence all with whom you come in contact.
```
所以，依靠这个命令，Linux可以像个算命师一样。

为了使之更有趣，你可以让cowsay说出fortune信息，如输入：

```
          $fortune | cowsay
```
则出现下面的效果：


七、Linux"figlet"命令

这个命令会用大写方式把我们输入的字符串显示在标题栏，显示效果由ASCII字符组成。
由于它不是标准设置，所以要先安装这一功能:

```
           $sudo apt-get install figlet
```
看到字符串在终端上变成这样确实很有意思。(看着效果和上面的Linuxlogo差不多的感觉哈)

```
           $ figlet mylinuxbook
```
你可以试着用"figlet"在脚本标头或源代码中添加标题栏。

八、Linux "toilet"命令

我不知道这个命令的名字源自哪里。但是它确实个五颜六色的命令。与figlet一样，它会把文本显示称标题栏，但是效果更好。
首先，安装：                        $ sudo apt-get install toilet
然后试一试这个简单的命令： $ toilet mylinuxbook
输入效果如下图所示：


它比figlet命令的效果更有艺术感。


不过，它还可以添加颜色。我们可以运行下列命令看到颜色：
              $toilet -f mono12 -F metal mylinuxbook
效果如下图：


九、Linux "cmatrix"命令

这个命令会在终端生成ASCII字符组成的矩阵风格的动画效果。

先安装：        $ sudo apt-get install cmatrix
现在运行：     $ cmatrix
出来的效果很酷：（是不是有种黑客帝国的感觉？）


用"ctrl+c" 结束，生成动画。

十、Linux  "oneko" 命令

这个命令是最好玩的。我们都知道"猫和老鼠"的动画，这个命令会让我们想起他们的追逐游戏。有了这行命令，我们可以看到一只猫>在追老鼠（鼠标指示器）的效果。
先安装：                                   $ sudo apt-get install oneko
运行这个命令，我们输入：        $ oneko
然后你就可以看到不管鼠标指示器放到哪里，Tom猫都会追着跑。下面是截图，不过不足以显示动态的效果：


即便我们切换应用，猫也不会离开老鼠。它甚至没有局限于终端。想关掉这个的时候，只需在终端里用ctrl+c 结束进程即可。


资源收集自网络～～


