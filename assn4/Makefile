################################################################################
#
# Copyright 1993-2006 NVIDIA Corporation.  All rights reserved.
#
# NOTICE TO USER:   
#
# This source code is subject to NVIDIA ownership rights under U.S. and 
# international Copyright laws.  
#
# NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE 
# CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR 
# IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH 
# REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF 
# MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.   
# IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL, 
# OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS 
# OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE 
# OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE 
# OR PERFORMANCE OF THIS SOURCE CODE.  
#
# U.S. Government End Users.  This source code is a "commercial item" as 
# that term is defined at 48 C.F.R. 2.101 (OCT 1995), consisting  of 
# "commercial computer software" and "commercial computer software 
# documentation" as such terms are used in 48 C.F.R. 12.212 (SEPT 1995) 
# and is provided to the U.S. Government only as a commercial end item.  
# Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through 
# 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the 
# source code with only those rights set forth herein.
#
################################################################################
#
# Build script for project
#
################################################################################

# Add source files here
EXECUTABLE	:= 5kk70-assignment-gpu
# Cuda source files (compiled with cudacc)
CUFILES		:= 
# C/C++ source files (compiled with gcc / c++)
CCFILES		:= main.cpp histogram-equalization.cpp contrast-enhancement.cpp

################################################################################
# Rules and targets

LIBS := -I /usr/local/cuda/samples/common/inc -L /usr/local/cuda/samples/common/lib
NVCC := /usr/local/cuda/bin/nvcc

all: a.out

histogram-equalization.o: histogram-equalization.cu
	$(NVCC) -arch=sm_20 $(LIBS) -o $@ -c $<
contrast-enhancement.o: contrast-enhancement.cu
	$(NVCC) -arch=sm_20 $(LIBS) -o $@ -c $<
main.o: main.cpp
	$(NVCC) -arch=sm_20 $(LIBS) -o $@ -c $<

a.out: contrast-enhancement.o histogram-equalization.o main.o
	$(NVCC) -lcudart $(LIBS) -o $@ $+

clean:
	rm -f a.out *.o

#include ../../common/common.mk
