# STM32&Letter Shell

## 前言

在进行调试程序（例如电机的角度速度、PID参数的整定、测试函数之间的联动等）时通常会使用串口来进行指令的收发，但是这个每个指令都需要进行单独的编写，并且编写出来的会拥有一个特定的格式，过一段时间再看程序的时候要傻眼很长时间。

- 就像这样（摘录自匿名上位机）需要一个位一个位的对照，很是不方便

​	![屏幕截图 2024-03-19 012646](D:\Pictures\Screenshots\屏幕截图 2024-03-19 012646.png)

所以一直在思考有没有一个程序可以直接在串口工具中输入函数（函数名就是更具其功能来设置的）然后直接执行。这就是本期的主角 `Letter Shell`了。

![shell_info](D:\Downloads\shell_info.png)

## 关于`Letter Shell`

话不多说直接上[链接](https://github.com/NevermindZZT/letter-shell)，作者已经写到了3.1版本了，但是基础的功能2.x版本就可以实现了，所以最终我选择了2.x版本进行移植。[这个是2.x版本的链接](https://github.com/NevermindZZT/letter-shell/tree/shell2.x)。**注意！！！2.x版本和3.x版本的函数之间不通用，如果需要3.x版本的功能，需要重新移植对应版本**

## 移植

- 既然是串口调试那么第一步就是打开串口了！

  ![屏幕截图 2024-03-19 012717](D:\Pictures\Screenshots\屏幕截图 2024-03-19 012717.png)

​	同时需要打开串口中断，设置中断优先级。

​	![屏幕截图 2024-03-19 002249](D:\Pictures\Screenshots\屏幕截图 2024-03-19 002249.png)

​	**注意！！！这个串口的优先级，最好给它一个单独的中断！！！**

​	 有次我把它和定时器中断放在了一个优先级里面，它抢占优先级虽然比定时器高但是，在输入一些特殊字符方面还是会出出现问题。

- 接下来是`Printf`的重定向，常规操作，我这里用的是Clion+Cmake编译的，Keil的可以参考我之前写的一篇[文章](https://blog.csdn.net/weixin_44484415/article/details/136825272?spm=1001.2014.3001.5502)

- 有了`Printf`当然还不够，需要下载[Letter-Shell-2.x](https://github.com/NevermindZZT/letter-shell/tree/shell2.x),直接下载并且丢到对应使用STM32文件里面，添加好头文件路径就行。

  ![屏幕截图 2024-03-19 004214](D:\Pictures\Screenshots\屏幕截图 2024-03-19 004214.png)

  - 这时候如果直接使用会出现错误，因为我的主文件用的是`.cpp`文件，而`Letter Shell`用的是`.c`文件进行编写的，所以需要在`Letter Shell`的`.h`·中将函数使用下面的函数包裹起来这样就不会报错了。

    ```c++
    #ifdef __cplusplus
    extern "C" {
    #endif
    
    // 相关C语言函数
    
    #ifdef __cplusplus
    }
    #endif
    ```
    
    接下来就是需要给`Letter Shell`分配一下串口，并且告诉它接受函数是什么，发送函数是什么。
    

- 先来搞一下发送函数，也就是单片机发送给上位机的函数，这里我是直接在`shell.c`的文件下面创建了`shell`的结构体以及发送函数

  ```c++
  SHELL_TypeDef shell;
  
  /**
   * @brief shell写数据函数原型
   *
   * @param const char 需写的字符
   */
  void ShellWrite(char data) {
      HAL_UART_Transmit(&huart1, (uint8_t *)&data, 1, 0xFFFF);
  }
  ```

  注意这里需要添加串口的头文件，以及`.h`文件中要声明一下。

  然后回到主函数里，分配一下函数的作用，这样发送的函数就完成了。

  ```C++
   shell.write = ShellWrite;
  
   shellInit(&shell);
  ```

  编译！！！！这时候发现报了个大错！

  ![屏幕截图 2024-03-19 010609](D:\Pictures\Screenshots\屏幕截图 2024-03-19 010609.png)

​		这个错在对应的说明文档里面写了，需要在`.ld`文件中的只读数据区(建议)添加

​			![屏幕截图 2024-03-19 011149](D:\Pictures\Screenshots\屏幕截图 2024-03-19 011149.png)

​		如果使用的是Keil编译的话，需要在keil的target option中增加--keep shellCommand*，防止定义的命令被优化掉。

​		这时候在编译就没有问题了。烧录并且打开串口助手就可以看见输出出来的语言了。

- 接收函数，使用中断来进行编写

  ```c++
  // 串口中断
  void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
      if (huart->Instance == USART1) {
          shellInput(&shell, shellRxBuffer);
          HAL_UART_Receive_IT(&huart1, (uint8_t *)&shellRxBuffer, 1);
      }
  }
  ```

  这里的`shellRxBuffer`定义在创建shell的文件下面。同时需要在`shell_cfg.h`

文件中`#define     SHELL_USING_TASK            0`把任务关闭。

- 编译下载测试！

## 串口上位机的选择

这里我使用的是MobaXterm这一款上位机。`Letter Shell`库的串口是你每输入一个字符都会发送给单片机，单片机缓存这个字符直到输入`Enter`再对输入的字符进行处理，所以需要选择一款按下一个按键就会发送的串口上位机。![屏幕截图 2024-03-19 133606](D:\Pictures\Screenshots\屏幕截图 2024-03-19 133606.png)

OK！
