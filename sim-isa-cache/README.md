# sim-isa-cache-toy

这是体系实习的LAB3-1作业

由小组合作完成

要求实现RISCV64I 的模拟器,带3层cache


## LOGs

0 commit: 分工发布

1 commit: 初次合并，修正了大坑bug 

1.3 commit: fixed bug in fscanf; can run my.trace correctly, 1.trace segmentation fault, 2.trace done(not surely correct)

2.0 commit: presudo cache r/w, result is RIGHT~

3.0 commit: real done

## QuikStart

### Help info

```
    This is a simulator to execute riscv ELF!
        Usage: ./sim <filename> [--verbose|--debug]
               ./sim --help
    Multiple ELFs is NOT supported!
```

## 作业要求

### 实现
根据提供的配置框架,实现一个可配置 Cache Size、Block Size、Set Associativity、 Replacement Policy(暂定 LRU)、Hit Latency 等参数的 Cache Simulator。具体要求如下:

a) 该 simulator 应提供读取 trace 文件的接口(格式参照样例 trace 文件)以及与 Lab2 的 CPU simulator 的对接实现,并可以支持多层 cache 的架构;

b) 该 simulator 需要同时提供下层 Memory 的仿真支持,比如能够体现 last level cache (LLC)的 miss latency 等(对应于主存访问平均延时);

c) 仿真结果需要正确统计访问总次数、Miss 总次数、Miss Rate、replacement 总次数、 对下层存储的访问次数、访问总延时周期数等参数;

d) 主存访问平均延时设置为 100 个时钟周期,CPU 和主存频率均默认为 2GHz。

e) 各层Cache的延迟保留两位小数,请根据需要从http://quid.hpl.hp.com:9081/cacti/
获得:

*	Nr. of Banks 固定为 1。
*	Technology Node 固定为 65nm。
	
f) 各层存储介质之间的传输延时暂不考虑。

g) Replacement Policy 统一定为 LRU,其他优化策略暂不考虑。

### 测试内容
测试内容包含两个部分,实验的代码及相应的测试结果请分开提交:

a) 使用附件中给定的 trace 通过单一层次 cache 测试

* 观察在不同的 Cache Size(32KB – 32MB)的条件下,Miss Rate 随 Block Size 变化的趋势,收集数据并绘制折线图。
* 观察在不同的CacheSize的条件下,MissRate随Associativity(1-32)变化的 趋势,收集数据并绘制折线图。
* 比较 Write Through 和 Write Back、Write Allocate 和 No-write allocate 的总访 问延时的差异。


<mark>**b) 与 Lab2 中的 CPU Simulator 结合,运行 Dhrystone Benchmark。**</mark>

Level | Capacity | Associativity | Line Size(Bytes)|Write Update Policy
------|----------|---------------|-----------------|----------
L1 | 32 KB |8 ways | 64 | WriteBack
L2 | 256 KB |8 ways | 64 | WriteBack
LLC | 8 MB | 8 ways | 64 | WriteBack


