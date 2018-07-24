###安装OTAWA前必须首先安装的工具集

sudo apt install cmake

sudo apt-get install ocaml

sudo apt-get install graphviz

sudo apt-get install doxygen

sudo apt-get install flex

sudo apt-get install bison

sudo apt-get install libxml2

sudo apt-get install libxslt

sudo apt-get install libxml2-dev

sudo apt-get install libxslt-dev

sudo apt-get install mercurial 

###编译otawa

- 参照 ```build-no-clone.sh``` 来进行编译（部分命令不完善，不要直接执行）

###编译待分析ELF二进制文件（powerpc指令集,elf_20二进制文件格式）

-  Windows下安装powerpc-elf-gcc编译器并生成二进制文件。

    工具来源：http://download.ronetix.info/toolchains/powerpc/

>     $ get-file <binary> 查看生成的二进制文件是否有效，
>       二进制文件必须是link后最终的可执行文件，而不能是单个C文件的编译结果
>     $ oipet -v linear_powerpc_elf  带verbose选项的查看结果
>     $ oipet linear_powerpc_elf main
>     $ otawa-config --list-scripts
>     $ owcet -s generic linear_powerpc_elf
 
   <font color="red">参考oipet -v过程展示的loader加载和ELF的格式规范，貌似默认安装版本仅支持powerpc</font>

### 调试跟踪WCET程序的执行

- 重新编译WCET工具链
>     凡事用CMake编译的程序，需要带有cmake参数 -DCMAKE_BUILD_TYPE=Debug
>     (gdb)symbol-file <path> 测试文件是否包含有效的debug信息
>     makefile编译的文件确保参数带有 -g 等选项

- 安装cdt-stand-alone-debugger (9.2)最新版进行调试
>     先安装Oracle Java （百度有说明)
>     再从Eclipse CDT官网下载调试器最新版，解压后可直接运行(注意需要设置命令行参数和working director)

- 安装kdbg并调试 (另一种候选方案）
>     $sudo apt-get install kdbg
>     $kdbg (开始kdbg调试)
>     通过kdbg工具栏的"Executable"选中目标可执行程序 /usr/local/bin/oipet或/usr/local/bin/otawa
>     菜单栏"Execution"->"Arguments"中设定工作目录Working Directory和Arguments
>         以oipet为例，Working Directory为被分析程序所在目录，Arguments为-v linear_powerpc_elf
>     下断点到适当位置，并通过F5等快捷键开始跟踪程序执行
>     如果出现”dwarf error wrong version in compilation unit header (is 4 should be 2) ”错误，很可能是安装了两个gdb，检查/usr/bin/gdb和/usr/local/bin/gdb，将老版本改为gdb.backup


 