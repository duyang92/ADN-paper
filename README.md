## Adaptive Denoising for Network Traffic Measurement

### Introduction

Traffic measurement in high-speed networks is imperative for applications such as traffic engineering, network management, and surveillance. Restricted by the limitations of on-chip memory resources and the velocity of packet processing, most existing solutions use compact data structures, namely sketches, to facilitate line-speed measurement. Nevertheless, these sketches, due to their shared record units (bits/counters) among flows, inevitably introduce noise into the measurement result of each flow. While conventional average denoising strategies can mitigate noise from raw estimates, they fall short of providing sufficient accuracy for medium-sized flows, primarily due to the uneven distribution of noise. To complement prior work, we propose two algorithms, ADN and mADN, which can perform denoising by considering the sizes of shared flows. ADN employs an optimization algorithm to model interconnections among flows, thereby reconstructing noise propagation and accurately restoring their sizes. Meanwhile, mADN retains the benefits of ADN yet excels in being more memory-efficient and precise. We apply our estimators to five essential tasks: per-flow size estimation, heavy hitter detection, heavy change detection, distribution estimation, and entropy estimation. Experimental results based on real Internet traffic traces show that our measurement solutions surpass existing state-of-the-art approaches, reducing the mean absolute error by approximately an order of magnitude under the same on-chip memory constraints.

### About this repo

The core **adn** and **madn** structure is implemented in **Algorithms/adn.h** and **Algorithms/madn.h**, respectively.

Other baseline methods are also implemented in **Algorithms**.

The dataset files are placed under the **data**.

The general modules are placed under the **utils**.

The main function is **main.cpp**.

### Requirements

- 18.04.1-Ubuntu SMP x86_64 GNU/Linux
- g++ (gcc-version >= 7.5.0)

### Dataset

The dataset used in the paper is the CAIDA dataset.

Due to the large size of the original dataset, we do not provide the dataset.

The details of the format of the dataset can be found in the **data/README.md**.

### How to build

You can use the following commands to build and run.

```
g++ main.cpp -o main -O3 -Wall -std=c++11 -lm -w -mcmodel=medium -g
./main data/00.dat 0.025
```

> The file 'data/00.dat' specifies the dataset file.

> The num '0.025' specifies the memory size (in MB) in the experiment.