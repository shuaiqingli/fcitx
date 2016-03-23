# 利用包管理器安装 #
多数发行版都提供了fcitx的预编译版本，建议安装。
以ubuntu为例
```
apt-get install 
```

同时在[这里](http://code.google.com/p/fcitx/downloads/list)，提供一些svn snapshot的编译，可能存在问题，使用时风险请自己承担，并且欢迎到[Issues](http://code.google.com/p/fcitx/issues/list)留下bug report。

# 从源代码安装 #

FCITX通常以源码的方式发布，您需要编译安装后才能使用。编译FCITX需要(并不仅限于)以下包：
GCC-2.95.3或更新版本
XFree86-4.3.0/xorg或更新(及其开发包)

首先下载FCITX的源码包，文件名为 fcitx-3.6.3.tar.bz2，存放在某个临时的目录中(如/temp)。执行：

```
cd /temp
tar jxvf fcitx-3.6.3.tar.bz2
```

这样会建立目录fcitx-3.6.3，所有的源码都在该目录中。
执行以下命令编译安装：

```
cd fcitx-3.6.3
./configure –prefix=<安装目录>
make && make install
```

如果您的系统安装XFT并配置正确，上述命令将编译安装XFT版。如果您不想使用XFT，请执行以下命令：

```
cd fcitx-3.6.3
./configure –prefix=<安装目录> --disable-xft
make && make install
```

如果您的系统没有安装XFT，安装程序会自动关闭XFT支持。
FCITX支持系统托盘，默认情况下该功能是打开的，如果您不希望打开该功能，可在configure时增加选项“--disable-tray”。
在configure时增加选项“--enable-recording”将打开FCITX的输入记录功能（参见后面的“输入记录”）。
在configure时增加选项“--enable-dbus”将打开FCITX的DBus支持，此时如果您的系统中安装了kimpanel，kimpanel将接管fcitx的用户界面（参见[KimpanelIntroduction](KimpanelIntroduction.md)）。