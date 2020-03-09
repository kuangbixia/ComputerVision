## 多线程
#### WIN多线程
调用AfxBeginThread()创建Windows线程。
``` C++
    // 第一个参数为此线程调用的函数，第二个参数为调用函数需要传入的参数
    AfxBeginThread((AFX_THREADPROC)&ImageProcess::addNoise, &m_pThreadParam[i]);
```
#### [OpenMP多线程](https://blog.csdn.net/zhongkejingwang/article/details/40350027)
- #pragma omp parallel
    - 每个线程都执行后面的代码块
    - 也可以这样设置线程数：#pragma omp parallel num_threads(4)
    ``` C++
        //设置线程数，一般设置的线程数不超过CPU核心数，这里开4个线程执行并行代码段
    	omp_set_num_threads(4);
    #pragma omp parallel
    	{
    		cout << "Hello" << ", I am Thread " << omp_get_thread_num() <<  endl;
    	}
    ```
    ```
    Hello, I am Thread 1
    Hello, I am Thread 0
    Hello, I am Thread 2
    Hello, I am Thread 3 
    ```
- #pragma omp parallel for
    - 后面的代码块必须接for循环
    - 多线程分别执行循环中的一次
    - 也可以这样设置线程数：#pragma omp parallel for num_threads(m_nThreadNum)
    ``` C++
    	omp_set_num_threads(4);
    #pragma omp parallel for
    	for (int i = 0; i < 6; i++)
    		printf("i = %d, I am Thread %d\n", i, omp_get_thread_num());
    ```
    ```
    i = 4, I am Thread 2
    i = 2, I am Thread 1
    i = 0, I am Thread 0
    i = 1, I am Thread 0
    i = 3, I am Thread 1
    i = 5, I am Thread 3
    ```
#### OpenCL/Cuda
待学习