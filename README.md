## PESC: Per System-Call Stack Canary 

该repo为PESC（https://dl.acm.org/doi/abs/10.1145/3374664.3375734）的repo，包含测试使用的内核以及PESC的代码补丁

### 一.Repo结构

该Repo共包含7个分支，其内容具体如下：

- master分支

  - 包含patch文件夹，为PESC应用到内核所需要的代码补丁
    - per_task.diff：用于ARM64 Linux 5.0以前启用per-task canary的代码补丁
    - pesc_pmc.diff：在per task canary的基础上，用于启用PESC-PMC的代码补丁
    - pesc_rng.diff：在per task canary的基础上，用于启用PESC-RNG的代码补丁
    - verify.diff：在PESC的基础上，用于验证PESC是否已经成功被应用到kernel的代码补丁
  - 包含test文件夹，包含用户空间测试程序源码
    - fork文件夹：包含了android ndk工程，用于测试fork系统调用的性能
    - read文件夹：包含了android ndk工程，用于测试read系统调用的性能
    - getpid文件夹：包含了android ndk工程，用于测试getpid系统调用的性能
    - verify文件夹：包含了android ndk工程，用于验证PESC是否已经成功被应用到kernel

  

- 其余分支：包含实验中所用的hikey960内核的源码

  - original分支：hikey960原始内核的源码
  - per_task分支：启用per task canary后的hikey960内核源码
  - pesc_pmc：启用PESC-PMC后的hikey960内核源码
  - pesc_pmc_verify：用于验证PESC-PMC是否启用的内核源码
  - pesc_rng：启用PESC-RNG后的hikey960内核源码
  - pesc_rng_verify：用于验证PESC-RNG是否启用的内核源码
  
  
  
### 二.使用方法

#### 1.前期准备

  - 系统环境：Ubuntu18.04
  - hikey960开发板（附带电源和type-c USB数据线）或其他硬件环境
  - android-10.0.0_r1源码（推荐）
  - android ndk
  - 内核源码
  - android必要的开发工具
  - GCC 9.0及以上版本ARM64交叉编译器

#### 2.内核编译

##### 启用per-task canary

对于ARM64 Linux v5.0及以上版本的内核，请跳过此步骤。对于ARM64 Linux v5.0及以下版本的内核，需要首先启用per-task canary，本repo已经给出了启用per-task canary后的hikey960内核源码，即per_task分支，对于其他ARM64 Linux v5.0及以下版本的内核，请将per_task.diff中的代码补丁添加到内核中。

##### 启用PESC

本repo已经给出了启用PESC后的hikey960内核源码，pesc_pmc分支和pesc_rng分支分别对应启用PESC-PMC和PESC-RNG的分支。对于其他内核，请添加对应的代码补丁pesc_pmc.diff或pesc_rng.diff到内核中。

##### 添加测试用系统调用

如无需进行测试，则跳过此步骤。测试使用系统调用读取当前进行的canary，因此需要添加一个新的系统调用get_canary。对于hikey960，本repo已经提供了对应的内核源码，包含在pesc_pmc_verify分支和pesc_rng_verify分支中。对于其他内核，请根据verify.diff的内容自行添加系统调用。

##### 编译内核

此步骤必须使用**GCC 9.0及以上版本**的的交叉编译器编译，对于hikey960内核，请使用如下命令编译，对于其他内核，请自行编译。

```sh
#编译内核
$ make ARCH=arm64 hikey960_defconfig
$ make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -j$(nproc)	#如有必要请更改CORSS_COMPILE变量

#拷贝编好的内核到aosp目录下,请将$(AOSP)替换成aosp的根目录
$ cp arch/arm64/boot/Image.gz-dtb $(AOSP)/device/linaro/hikey-kernel/Image.gz-dtb-hikey960-4.19
$ cp arch/arm64/boot/dts/hisilicon/hi3660-hikey960.dtb  $(AOSP)/device/linaro/hikey-kernel/hi3660-hikey960.dtb-4.19
```

##### 编译系统镜像

对于hikey960请按如下步骤进行，其余环境请自行编译

```sh
$ cd $(AOSP)	#请将$(AOSP)替换成aosp的根目录
$ . ./build/envsetup.sh
$ lunch hikey960-userdebug
$ make -j12
```

##### 刷写镜像

对于hikey960请按如下步骤进行。

1.打开开关 3，选择 fastboot 模式（有关详情，请参阅 https://www.96boards.org/documentation/consumer/hikey/hikey960/getting-started/）,连接USB数据线。

2.启动开发板。

3.刷写镜像

```sh
$ cd $(AOSP)/device/linaro/hikey/installer/hikey960 #请将$(AOSP)替换成aosp的根目录
$ ./flash-all.sh
```

4.关闭开关 3，然后重启开发板。

##### 测试

如无需测试，请跳过此步骤。测试前请切换到master分支。

- 功能测试

  对于其他内核，请先修改test/verify/jni/test.c中的系统调用号为之前添加的系统调用号

  ```sh
  $ cd test/verify/jni
  $ $(andorid-ndk)/ndk-build 	#请将$(andorid-ndk)替换成android-ndk的根目录
  $ cd ../libs/arm64-v8a
  $ adb push test /data/local/tmp
  $ adb shell
  ```

  进入adb的shell后

  ```sh
  $ cd /data/local/tmp
  $ ./test
  ```

  运行test后程序会打印出每一次得到的canary，如果各次canary互不相同且不为0，则说明PESC的功能正常。

- 性能测试

  该repo包含了对三个system call的性能测试，首先需要编译三个system call的用户态测试文件,对于getpid系统调用的测试方法如下：

  ```sh
  $ cd test/getpid/jni	
  $ $(andorid-ndk)/ndk-build 	#请将$(andorid-ndk)替换成android-ndk的根目录
  $ cd ../libs/arm64-v8a
  $ adb push getpidtester /data/local/tmp	
  $ adb shell 
  ```

  进入adb的shell后

  ```sh
  $ cd /data/local/tmp
  $ ./getpidtester	
  ```

  对于read和fork系统调用，测试方法同理，测试read系统调用时，必须将需要的数据也一同刷入开发板中

  ```sh
  $ adb push test/read/testdata /data/local/tmp
  ```

  每个程序运行之后最终都会打印出类似如下输出：

  ```sh
  avg: xxx	#xxx代表平均花费的时间
  ```

  通过比较添加PESC前后平均花费的时间，可以简单得到PESC对系统调用带来的性能开销。

