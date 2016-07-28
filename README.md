# RTSS-emulation
Created on: July 27, 2016
   Author : Kuan-Hsun Chen

This is a repository of storing a RTEMS example for RTSS'16 paper.

We use 4280dff7cfbc7c14c4231ac68637650f39f792b1 version of RTEMS source tree. (4.12 toolchain from RSB)

This folder is prepared to patch the RTEMS source tree under this path "testsuites/samples/"
All the related Makefile.am and configure.ac files are also required to be revised accordingly.

We assume the input file containing the synthesized task sets, in which the priority assignment is given as well.

The automatic testing flow is preparing.
