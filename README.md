# 1 项目
加载和显示bvh动作文件
![image](https://github.com/HW140701/LoadandDisplayBVH/blob/master/assest/example1.png)
![image](https://github.com/HW140701/LoadandDisplayBVH/blob/master/assest/example.gif)

# 2 项目结构
- bin:可执行文件目录；
- projectfile:visual studio 2015工程文件目录；
- src:源码文件目录；
- third/freeglut:OpenGL库freeglut文件目录；

可直接使用visual studio打开projectfile目录中的项目文件，编译运行即可。如果使用其他C++编译器，需根据freeglut目录自行设置好第三方库的包含目录以及链接目录，同时预定义WIN32，_CRT_SECURE_NO_WARNINGS。

# 3 跨平台
目前弹出文件选择对话框依赖于windows api,如果需要在linux上运行需自行修改文件加载处的代码：
    
	if ( key == 'l' )
	{
		#ifdef  WIN32
		const int  file_name_len = 256;
		char  file_name[ file_name_len ] = "";

		OPENFILENAME	open_file;
		memset( &open_file, 0, sizeof(OPENFILENAME) );
		open_file.lStructSize = sizeof(OPENFILENAME);
		open_file.hwndOwner = NULL;
		open_file.lpstrFilter = "BVH Motion Data (*.bvh)\0*.bvh\0All (*.*)\0*.*\0";
		open_file.nFilterIndex = 1;
		open_file.lpstrFile = file_name;
		open_file.nMaxFile = file_name_len;
		open_file.lpstrTitle = "Select a BVH file";
		open_file.lpstrDefExt = "bvh";
		open_file.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

		BOOL  ret = GetOpenFileName( &open_file );

		if( ret )
		{
			if ( bvh )
				delete  bvh;
			bvh = new BVH( file_name );

			if ( !bvh->IsLoadSuccess() )
			{
				delete  bvh;
				bvh = NULL;
			}

			animation_time = 0.0f;
			frame_no = 0;
		}
	#endif
	}

# 4 联系我
- 我的个人网站：[www.stubbornhuang.com](www.stubbornhuang.com "www.stubbornhuang.com")
- 我的个人博客：[https://blog.csdn.net/hw140701](https://blog.csdn.net/hw140701 "https://blog.csdn.net/hw140701")
- 我的邮箱：stubbornhuang@qq.com