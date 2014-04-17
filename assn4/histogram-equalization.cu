#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hist-equ.h"

#define BLOCK_NUM 256
#define THREAD_NUM 256
void histogram(int * hist_out, unsigned char * img_in, int img_size, int nbr_bin){
    int i;
    for ( i = 0; i < nbr_bin; i ++){
        hist_out[i] = 0; //construct an array with one entry for each color grey
    }

    for ( i = 0; i < img_size; i ++){
        hist_out[img_in[i]] ++; //fill array with counts of pixels of that color in image
    }
}

void histogram_equalization(unsigned char * img_out, unsigned char * img_in, 
                            int * hist_in, int img_size, int nbr_bin){
    int *lut = (int *)malloc(sizeof(int)*nbr_bin);
    int i, cdf, min, d;
    /* Construct the LUT by calculating the CDF */
    cdf = 0;
    min = 0;
    i = 0;
    while(min == 0){
        min = hist_in[i++]; //find the number of darkest pixels in the image
    }
    d = img_size - min;
    for(i = 0; i < nbr_bin; i ++){
        cdf += hist_in[i];
        //lut[i] = (cdf - min)*(nbr_bin - 1)/d;
        lut[i] = (int)(((float)cdf - min)*255/d + 0.5);
        if(lut[i] < 0){
            lut[i] = 0;
        }
        
        
    }
    
    /* Get the result image */
    for(i = 0; i < img_size; i ++){
        if(lut[img_in[i]] > 255){
            img_out[i] = 255;
        }
        else{
            img_out[i] = (unsigned char)lut[img_in[i]];
        }
        
    }
}

__global__ void histogram_gpu(int * hist_out, unsigned char * img_in, int img_size, int nbr_bin){
    
    __shared__ int temp[256];

    temp[threadIdx.x] = 0;
    __syncthreads();

    int id =  blockIdx.x * blockDim.x + threadIdx.x;
    int offset = blockDim.x * gridDim.x
    if (id >= img_size)
    {
        return;
    }

    while (id < img_size){
        atomicAdd(&temp[img_in[id]],1);
        i+= offset;
    }
    __syncthreads();

    //unsigned char value = img_in[id];

    //int bin = value % nbr_bin;
    atomicAdd(&(hist_out[threadIdx.x]), temp[threadIdx.x]);
}

void getHist(int * hist_out, unsigned char* img_in, int img_size, int nbr_bin){

    unsigned char * dArray;
    cudaMalloc(&dArray, img_size);
    cudaMemcpy(dArray, img_in, img_size,cudaMemcpyHostToDevice);

    int * dHist;
    cudaMalloc(&dHist, nbr_bin * sizeof(int));
    cudaMemset(dHist,0,nbr_bin * sizeof(int));

    dim3 block(32);
    dim3 grid((img_size + block.x - 1)/block.x);

    histogram_gpu<<<grid,block>>>(dHist,dArray,img_size,nbr_bin);

    cudaMemcpy(hist_out,dHist, nbr_bin * sizeof(int), cudaMemcpyDeviceToHost);
    cudaFree(dArray);
    cudaFree(dHist);



}

__global__ void histogram_image_compile_gpu(unsigned char * img_out, unsigned char * img_in,
                            int * lut, int image_size, int nbr_bin) {
        __shared__ unsigned int memlut[255];
     
        for(int i = 0; i < 255; i++){
            memlut[i] = lut[i]; //don't know if pointer is correct but I want a local copy of lut
        }
        int chunk_size = blockIdx.x; //need code here, we need to split the image array into local parts to run high performance calcs on
        int offset = image_size/blockIdx.x; //when getting a chunk of the in image, or writing a chunk to the out image, offset+i should map to the correct location
        __shared__ unsigned int local_img[9999]; //create a local version of a segment of the image to work against so the whole image isn't stored in gpu memory per core
        for(int i = 0; i < chunk_size; i ++) {
           local_img[i] = img_in[offset+i];
        }
        __syncthreads();
        for(int i = 0; i < chunk_size; i++) {
           local_img[i] = lut[local_img[i]];
        }
        __syncthreads();
        for(int i = 0; i < chunk_size; i++) {
       img_out[offset+i] = local_img[i];
       }
}


__host__ static void histogram_equalization_gpu(unsigned char * img_out, unsigned char * img_in, 
                            int * hist_in, int img_size, int nbr_bin){
    /* Calculating the lut doesn't really make sense as a massively parallel thing, as it's only going through a maximum of 255 steps
    so lets only cudaize the result image formation step	*/
    unsigned int lut[nbr_bin]; //look up table, same size as hist
    int i, cdf, min, d;
    /* Construct the LUT by calculating the CDF */
    cdf = 0;
    min = 0;
    i = 0;
    while(min == 0){
        min = hist_in[i++]; //find the number of darkest pixels in the image

    }
	d = img_size - min;
    for(i = 0; i < nbr_bin; i ++){
        cdf += hist_in[i];
        //lut[i] = (cdf - min)*(nbr_bin - 1)/d;
        lut[i] = (int)(((float)cdf - min)*255/d + 0.5);
        if(lut[i] < 0){
            lut[i] = 0;
        }
    }    
       

	
	/* Get the result image*/
	
	//histogram_image_compile_gpu(img_out, img_in, lut, img_size, nbr_bin);
    
}

