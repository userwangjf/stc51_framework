# 标准的STC 8051程序框架模板
有以下优点
1. 方便使用vscode编辑。
2. 内置非抢占的操作系统。
3. 通过增加必要的文件，实现不同的功能。

# 几个需要修改的地方
1. 在app目录添加app_xxx.h,包含自己需要使用的应用头
2. 在app目录添加app_xxx.c，并app需要的内容
3. 在module目录里添加module_xxx.c，并在module.c里include
4. 在bsp目录里添加bsp_xxx.c，编写自己需要的初始化代码，然后在bsp.c里include该文件。

