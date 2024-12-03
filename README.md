# UE5_Action-RPG
UE5_0307

学习成果

来自Udemy中，Stephen Ulibarri的课程

https://www.udemy.com/course/unreal-engine-5-the-ultimate-game-developer-course/

# 新增内容

## 16 Tiling in Landscape Textures在景观纹理平铺
Are you unhappy with the tiling in your landscape material? 
你对景观材料中的瓷砖不满意吗？

Several students have requested a tutorial for fixing that. If you're up for an optional video on fixing the tiling in your landscape layers, check out this supplemental video on doing just that!
一些学生已经要求一个教程来解决这个问题。如果你想看一个关于在你的风景图层中修复平铺的可选视频，看看这个补充视频吧！

Link in the resources for this video.

Fix Tiling Textures on your Landscape in Unreal Engine 5
在虚幻引擎5中修复景观上的平铺纹理
https://www.youtube.com/watch?v=rOQE5kzNENI


## 37_Jetbrains Rider现在可以免费用于非商业用途

Jetbrains Rider is now Free for non-commercial use

While in this course, I use Visual Studio as the IDE, Jetbrains has recently made the Rider IDE free for non-commercial use.
虽然在本课程中，我使用Visual Studio作为IDE，但Jetbrains最近将Rider IDE免费用于非商业用途。

If you would like to give Rider a try (I will still be using Visual Studio in this course), you can download the IDE here:
如果你想给Rider一个尝试（我仍然会在这个课程中使用Visual Studio），你可以在这里下载IDE：
https://www.jetbrains.com/rider/

and you can read the article announcing the IDE now free for non-commercial use:
你可以阅读一篇文章，宣布IDE现在可以免费用于非商业用途：
https://blog.jetbrains.com/blog/2024/10/24/webstorm-and-rider-are-now-free-for-non-commercial-use/

If you do decide to give Rider a try, make sure to follow the instructions for setting up Rider for Unreal Engine projects. Here is a video explaining the process:
如果你决定给Rider一个尝试，确保按照说明设置骑手虚幻引擎项目。这里有一段视频解释了这个过程：
https://www.youtube.com/watch?v=zUFd5mN_dMY

## 45 How to Properly Compile如何正确编译

1、不建议使用UE的实时编码，可能会存在一些问题，会破坏代码，它实际上仅适用于函数体更改以及其他任何内容（头文件），如果你需要修改头文件，建议是关闭编辑器进行编译，例如有些UPROPERTY属性的变量，在没有关闭编辑器的情况下编译，可能出问题。

2、关闭UE的实时编码，打开编辑器设置，搜索reinstancing， 取消勾选Enable Reinstrncing

3、如果你使用Visual Studio，可以找到右侧有一个Unreal Engine uproject support(Preview)的组件，他可以支持在VS里面直接创建UE的类


## 102 Retargeting is now Easier in 5.4!重定向现在更容易在5.4！
The following lectures will cover how to retarget animations from one skeleton to another.
下面的讲座将介绍如何重新定位动画从一个骨架到另一个。

If you are using Unreal Engine 5.4, the process has become simplified. I have created a new example video showing how you can retarget animations from one skeletal mesh to another, and if you're interested in this newer, easier way, check out the link in the resources for this lecture!
如果你使用的是虚幻引擎5.4，这个过程已经变得简化了。我创建了一个新的示例视频，展示了如何将动画从一个骨骼网格重新定位到另一个，如果您对这种更新，更简单的方式感兴趣，请查看本讲座资源中的链接！

Keep in mind that this new method is simpler, but offers less fine-tuning. To be fully equipped for all scenarios (including the ones we'll encounter throughout this project), you'll still need to learn how to retarget with the methods we use in this course. But it's great to know the newer, simpler method as well!
请记住，这种新方法更简单，但提供的微调较少。要为所有场景（包括我们在整个项目中遇到的场景）做好充分的准备，您仍然需要学习如何使用我们在本课程中使用的方法重新定位。但知道更新、更简单的方法也很棒！

Stephen

https://www.youtube.com/watch?v=iE474cUpR-o

## 134 Notes on the Blender Addon关于Blender插件的说明
In the following video, we will be using a Blender Addon to add a bone to our Mixamo Skeletal Meshes.
在下面的视频中，我们将使用Blender插件添加骨骼到我们的Mixamo骨骼网格。

Please only use the version of Blender made for the plugin! Students have been getting issues with the plugin in the latest version of Blender.
请只使用为插件制作的Blender版本！学生们在最新版本的Blender中遇到了插件问题。

Blender 3.3 appears to be the latest version compatible with the plugin.
Blender 3.3似乎是与该插件兼容的最新版本。

