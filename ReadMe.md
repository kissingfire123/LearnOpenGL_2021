### 1.介绍

记录学习openGL的过程,参照教程 [LearningOpenGL](https://learnopengl-cn.readthedocs.io/zh/latest/) ，各部分代码都加了自己的注释，某些细节bug也做了修复。

### 2.注意事项

- 当前只支持Windows系统，还请安装好`Visual Studio`；

- 项目使用CMake编译，请确保安装了`CMake` 3.6.0以上版本；

- OpenGL常用的几个Lib(包括GLFW, GLEW, SOIL, Glm)已经打包在文件`openGL_Depends`内了。

### 3.如何使用

- 双击运行配置脚本`GenerateVsProj.bat`，会调用CMake生成`build`目录，里面有能直接运行的`*.sln`工程；
- 暂时没有把位于`Src`目录的`shader`脚本`glsl` 文件在cmake阶段添加到工程，可以手动在VS里添加(不加也不影响运行，方便修改而已 )。

**Enjoy ~**

