#include <iostream>
//#include <cstdlib>
//#include <omp.h>
//#include <stdio.h>
//#include <stdlib.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <OpenCL/opencl.h>
//#include <CL/opencl.h>
//#include <CL/CL.h>

////////////////////////////////////////////////////////////////////////////////

// Use a static data size for simplicity
//
#define DATA_SIZE (1024)

////////////////////////////////////////////////////////////////////////////////

// Simple compute kernel which computes the square of an input array 
//
/*const char *KernelSource = "\n" \
"__kernel void square(                                                       \n" \
"   __global float* input,                                              \n" \
"   __global float* output,                                             \n" \
"   const unsigned int count)                                           \n" \
"{                                                                      \n" \
"   int i = get_global_id(0);                                           \n" \
"   if(i < count)                                                       \n" \
"       output[i] = input[i] * input[i];                                \n" \
"}                                                                      \n" \
"\n";*/
////////////////////////////////////////////////////////////////////////////////
int main(int argc, const char * argv[]) {

	cl_context context;
    cl_device_id *devices;
    char *devname;
    size_t cb;
    
    // create a GPU context
    context = clCreateContextFromType(NULL, CL_DEVICE_TYPE_GPU, NULL, NULL, NULL);
    if(context == 0) {
        printf("Can't create GPU context\n");
        return 0;
    }
    
    // get a list of devices
    clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &cb);
    devices = (cl_device_id*) malloc(cb);
    clGetContextInfo(context, CL_CONTEXT_DEVICES, cb, devices, 0);
    
    // show the name of the first device
    clGetDeviceInfo(devices[0], CL_DEVICE_NAME, 0, NULL, &cb);
    devname = (char*) malloc(cb);
    clGetDeviceInfo(devices[0], CL_DEVICE_NAME, cb, devname, 0);
    printf("Device GPU : %s\n", devname);
    
    // release everything
    free(devname);
    free(devices);
    clReleaseContext(context);

    // create a CPU context
    context = clCreateContextFromType(NULL, CL_DEVICE_TYPE_CPU, NULL, NULL, NULL);
    if(context == 0) {
        printf("Can't create GPU context\n");
        return 0;
    }
    
    // get a list of devices
    clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &cb);
    devices = (cl_device_id*) malloc(cb);
    clGetContextInfo(context, CL_CONTEXT_DEVICES, cb, devices, 0);
    
    // show the name of the first device
    clGetDeviceInfo(devices[0], CL_DEVICE_NAME, 0, NULL, &cb);
    devname = (char*) malloc(cb);
    clGetDeviceInfo(devices[0], CL_DEVICE_NAME, cb, devname, 0);
    printf("Device CPU : %s\n", devname);
    
    // release everything
    free(devname);
    free(devices);
    clReleaseContext(context);

	system( "read -n 1 -s -p \"Press any key to continue...\"; echo" );
	return 0;
}
