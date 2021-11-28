### 1.介绍

记录学习openGL的过程,参照教程 [LearningOpenGL](https://learnopengl-cn.readthedocs.io/zh/latest/) ，各部分代码都加了自己的注释，某些细节bug也做了修复。

相对于官方教程，主要做的修改与特点：

- CMake搞定工程生成，不用操心环境配置和依赖库编译；

- 按照功能模块拆分函数，方便理解；
- 关键代码加入自己的理解，中文注释；
- 对于某些官方未注意的细节(例如初始化)等小问题做了修复；
- 完成了官方教程的所有习题，融合到一套代码

### 2.注意事项

- 支持Windows平台和MacOS平台，还请安装好`Visual Studio`或`Xcode`编辑器；

- 项目使用CMake编译，请确保安装了`CMake` 3.12.0以上版本；

- OpenGL常用的几个Lib(包括GLFW, GLEW, SOIL, Glm)采用**源码集成**，所有工程共同依赖`Dependency`。

### 3.如何使用

- Windows系统:双击运行Configure脚本`win_GenerateVsProj.bat`，会调用CMake生成`build`目录(生成的是32位环境的工程)，里面有能直接运行的`*.sln`工程；
- MacOS系统:终端运行配置脚本`./mac_GenerateXcodeProj.sh`，会调用CMake生成`build`目录,能直接运行的`*.xcodeproj`工程；

### 4.其他
- ShaderClass.hpp是对shader常规重复操作的类封装,该hpp文件不需要修改,专注修改glsl代码即可;
- 提示:工程路径没有测试中文,如果有问题可以改成英文路径
- Xcode**中文注释乱码**的问题：在Finder页面，右键使用`系统文本编辑器`打开，然后` Command + S `保存后退出，再打开Xcode工程，问题解决!

 Enjoy ~

