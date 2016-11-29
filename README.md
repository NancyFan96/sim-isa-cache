# sim-isa-cache

## LOGs

* 1.0 commit: realize ports and initialzation, need to realize r/w

* 2.0 commit: **REALIZE IT**!

## QuikStart
这是体系实习Lab3-1与Lab2的拼接，实现了带三级Cache的RISC-V ISA Simulator.

### Help info

```
    This is a simulator to execute riscv ELF!
        Usage: ./sim <filename> [--verbose|--debug]
               ./sim --help
    Multiple ELFs is NOT supported!
```

Level | Capacity | Associativity | Line Size(Bytes)|Write Update Policy|Hit Latency(cycle)
------|----------|---------------|-----------------|----------|---
L1 | 32 KB |8 ways | 64 | WriteBack|4
L2 | 256 KB |8 ways | 64 | WriteBack|5
LLC | 8 MB | 8 ways | 64 | WriteBack|11


## 文件结构

* sim-isa-cache是双通道memory，Cache不实际操作读写

* new-isa-cache-sim是最终版，simulator的memory升级到了三级Cache

```
├── readme.md   
├── src/ 							# alias as "new-isa-cache-sim"   
│   ├── system.h 					# THIS IS RISCV R64IFM
│   ├── sim.hpp 						# Contains MAIN function
│   ├── sim.cpp  
│   ├── gdb.hpp     				
│   ├── gdb.cpp     				
│   ├── count.cpp     				
│   ├── count.cpp     			   			
│   ├── decode.hpp 					# Decode instructions, get meaningful part like rd, rs, imm
│   ├── decode.cpp 					
│   ├── execute.cpp 				# Execution for RISCV64I, RISCV64M (only for std tests)
│   |								# Cotains Ecall
│   ├── execute_F.cpp 				# Execution for RISCV64F (only for std tests)
│   ├── register.hpp 				# Read and write functions for Registers
│   ├── register.cpp
│   ├── memory.hpp 					# Read and write functions for Memory
│   ├── memory.cpp
│   ├── Storage.h 					# CACHE
│   ├── cache.h 					# CACHE
│   ├── cache.cc
│   ├── Makefile					# would create executable file -- sim
│   ├── test/ 
│   	├── Itest/ 
│   	├── IOtest/ 
│   	├── Ftest/ 
│   	├── stdtest/ 
│   		├── dhry2.1/ 
│   		├── ijk 				# input: m, n, t, A[][], B[][]
│   		├── ijk.cpp
│   		├── qs 					# input: size, A[]
│   		├── quickSort.cpp 
│   		├── ack					# input: m, n
│   		├── ackermann.cpp 
│   	├── time 
│   	├── time.cpp 
│   	├── time.objdump

```

