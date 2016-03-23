# FAQ #
这里收集的是关于fcitx一些常见问题。有些其实并不是bug，有些问题可能短期难以解决，请等待我们修复。

有关Kimpanel的问题请查看 [KimpanelIntroduction](KimpanelIntroduction.md)

当然我们建议你在阅读这些内容之前，抽出几分钟的时间，阅读一下fcitx自带的pdf格式的文档，也许有些问题也迎刃而解了。

## 安装完成后，按ctrl+space 无法打开fcitx 的输入条啊？ ##
这个的原因有很多。你可能需要一一排查以下情况：

如果其他程序可以成功使用fcitx，那么也许是ctrl space的快捷键被抢占了，想办法设置这个程序的快捷键保证不冲突。例如CodeBlocks有这个问题。

如果所有程序都无法正常启用fcitx，请参看说明文档的“环境配置”部分。如果仍然有问题，那么也许是你的locale设置有问题，在某些系统上，默认的locale可能是C或者POSIX，如果你仍然想使用英文界面，应当将locale设置为en\_US.UTF-8。en\_US.UTF-8的gtk程序默认不会使用xim，一定要设置GTK\_IM\_MODULE，否则只能每次右键选择。

## 为什么有时候fcitx 会卡几秒？ ##
fcitx是在第一次输入时读入码表，如果码表比较大的话，读入的时间可能会比较久一些，感觉有些
卡，但后面输入就不会再有这种现象了(如果切换了码表输入法的话，切换后的第一次输入也会重新读入码表)。

## 为什么我只按了一个键，输入条上却出现两个相同的字符？ ##
这个问题应该已经在4.0里面修复。

## 当我使用en\_US.UTF-8 时，为什么无法激活fcitx？ ##
这种情况应该是GTK2的应用程序。当locale为en\_US.UTF-8时。GTK2默认的输入模块不是XIM，因此无法激活fcitx。此时在文字输入框中按鼠标右键，把Input Method设置为X input method，然后就可以使用fcitx了。比较好的解决方法是，在~/.bash\_profile或者~/.xprofile中加入以下两行：
export GTK\_IM\_MODULE=xim
export XMODIFIERS=”@im=fcitx”

## fcitx 的默认字体并不是我常用字体 ##
由于fcitx并没有使用一些高层的字体处理，默认字体仅仅是利用Fontconfig找到的一个系统当
中的中文字体（并无优先级关系，也不服从/etc/fonts.conf）。可以通过配置文件指定具体的字体
解决。或者你应该考虑编译时enable-pango，这样字体设置就有更好的支持了。

## 有些皮肤周围为什么有不透明的黑边 ##
由于Xserver需要可以支持混成的窗口管理器才能处理真透明效果，因此需要对窗口管理器本身进
行设置。相应的开启混成之后会消耗更多的CPU和GPU性能，因此是否开启这个效果请自己权衡。
分为以下情况：

### Gnome ###
Metacity可以支持混成，但由于不是所有的显卡和驱动程序都能很好地支持混成，因此混成在gnome中默认是未开启的。如果你的显卡和驱动程序支持3D加速，则可以开启之。使用gconf-editor进行编辑，编辑/apps/metacity/general/compositing\_manager，设置为真即可。
或者直接使用下面的命令启用：
```
gconftool-2 -s --type bool /apps/metacity/general/compositing_manager true
```
如果你想禁用它，修改“true”为“false”，并重新执行该命令。

### KDE4 ###
Kwin可以支持混成，在系统设置，桌面效果内设置。

### XFCE ###
Xfwm4支持混成，在高级窗口管理器内进行设置。

### Compiz ###
Compiz >= 0.9.0可能默认没有开启混成，可以通过安装ccsm进行配置。

### 其他 ###
其他窗口管理器可以通过一个额外的混成管理器xcompmgr进行混成管理。只需要安装它即可。另外
也可以将之前所述的窗口管理器在不开启混成的情形下同时使用xcompmgr，但相应的可能不如原生的
效果那么好。

## fcitx 启动后图标背景为黑色，但关闭后手动启动背景颜色正常 ##
这个问题应该已经修复，在ubuntu10.04的虚拟机下测试没有问题。如果你仍然遇见了这个问题，你可以通过设置DelayStart延迟几秒启动fcitx，在支持混成的窗口管理器和面板启动之后再启动fcitx。

## 有时Firefox中输入的第一个字母会丢失 ##
我们确认存在这个问题。也许在实现了gtk-im-module之后这类问题会得到改善。

## fcitx在某些的程序中无法光标跟随/光标跟随存在问题 ##
这是这些程序在实现xim的时候并没有提示光标在哪里的缘故，所有使用xim的输入法都会有这样的问题，而不是只有fcitx。现在fcitx对于这些窗口的处理就是现实在这个窗口的下方。这时你可以拖动fcitx的输入窗口到你想要的位置上继续输入，在关闭这个程序之前，fcitx会记住你拖动的位置。

## 在flash中无法输入 ##
我们对于adobe的做法倍感疑惑，在10.0.45.2这个版本，fcitx是可以正常输入的。但是升级之后却又出了问题。不过既然使用gtk-im-module的输入法可以支持flash了，那么在fcitx实现了gtk-im-module之后，这个问题也应该可以解决。

## 我对于fcitx拼音词库很不满 ##
有两个折中的解决方案。

1、利用createPYMB创建自己的词库，放在~/.config/fcitx/pinyin下面。

2、尝试fcitx-sunpinyin，sunpinyin是一个基于语言模型的pinyin开源输入法项目，对于整句输入有很大提升。

## 我是kde4用户，kimpanel的问题太多了，可是我又想界面协调 ##
你可以尝试这里的小工具http://csslayer.tk/wordpress/fcitx开发/fcitx-skingenerator/

## 我想输入日语怎么办？ ##
你可以尝试这里：http://forum.ubuntu.org.cn/viewtopic.php?f=8&t=290712&start=0 5楼的码表。
或者请等待基于anthy的fcitx的输入法wrapper，这个项目目前在计划中。

## 我想输入颜文字 ##
fcitx的QuickPhrase在你有想象力的前提下，可以发挥很多功能，例如用来输入颜文字，例如用来根据Latex的标记输入特殊符号。

## 我在使用vnc/nomachine远程连接时，fcitx好像不太好使？ ##
vnc下fcitx似乎确实有问题，但我们尚未验证是否这是基于xim的通病。

nomachine/nx下是没有问题的，不过由于你可能是使用windows进行远程连接，那么ctrl+space会被windows的输入法抢走而不会传递到远程的计算机上，你可以考虑将fcitx的输入法切换快捷键设置为shift+space。在linux的nx客户端没有问题。