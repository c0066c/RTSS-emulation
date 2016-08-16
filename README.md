# RTSS-emulation
Created on: July 27, 2016
   Author : Kuan-Hsun Chen

This is a repository of storing a RTEMS example for RTSS'16 paper.

It should be compatible with the latest version of RTEMS source tree. (4.12 toolchain from RSB)
We enhance the kernel for handling deadline misses properly. (RTEMS ticket #2772 issue)

This folder is prepared to patch the RTEMS source tree under this path "testsuites/samples/"
All the related Makefile.am and configure.ac files are also required to be revised accordingly.

We assume the input file containing the synthesized task sets, in which the priority assignment is given as well.

The automatic testing flow is under preparing.
