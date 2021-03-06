#include <iostream>
//#include <cstdlib>
#include <omp.h>
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
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp> 
#include <opencv2/highgui/highgui.hpp>
#include <math.h>
//#include <CL/opencl.h>
//#include <CL/CL.h>

////////////////////////////////////////////////////////////////////////////////

// Use a static data size for simplicity
//
//#define DATA_SIZE (1024)

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

cl_program load_program(cl_context context, const char* filename) {
    std::ifstream in(filename, std::ios_base::binary);
    if(!in.good()) {
        printf("err0\n");
        return 0;
    }

    // get file length
    in.seekg(0, std::ios_base::end);
    size_t length = in.tellg();
    in.seekg(0, std::ios_base::beg);

    // read program source
    std::vector<char> data(length + 1);
    in.read(&data[0], length);
    data[length] = 0;

    // create and build program 
    const char* source = &data[0];
    cl_program program = clCreateProgramWithSource(context, 1, &source, 0, 0);
    if(program == 0) {
        printf("err1\n");
        return 0;
    }

    if(clBuildProgram(program, 0, 0, 0, 0, 0) != CL_SUCCESS) {
        printf("err2\n");
        return 0;
    }

    return program;
}

int main(int argc, const char * argv[]) {
	double omp_get_wtime(void);
	double startCK, finishCK;

	startCK = omp_get_wtime();
	//////////////////////////////////////////////////////////////////
    //int height, width;
	uchar *data1;

    IplImage *Image1=cvLoadImage("Koala.jpg",1);
    
    //IplImage *RedImage,*GreenImage,*BlueImage;
    //IplImage *Channel1,*Channel2,*Channel3;
    
    //CvSize Size1;
    //Size1=cvGetSize(Image1);
    
	//int height = Image1->height;
	//int width = Image1->width;

    cl_int height = 768;
	cl_int width = 1024;

	data1 = (uchar *)Image1->imageData;

    const int DATA_SIZE = height*width*3;
    //size_t work_size = DATA_SIZE;
    size_t work_size = width*height;

    //printf("%d\n",height);
    //printf("%d\n",width);
    //printf("%d\n",DATA_SIZE);
    //printf("%d\n",work_size);
      
    cvNamedWindow("Result",CV_WINDOW_AUTOSIZE);
   
    uchar *d_frame_in;
    //uchar *d_frame_out;
/////////////////////////////////////////////////////////
	cl_int err;
    cl_uint num;
    err = clGetPlatformIDs(0, 0, &num);
    if(err != CL_SUCCESS) {
        std::cerr << "Unable to get platforms\n";
        return 0;
    }

    std::vector<cl_platform_id> platforms(num);
    err = clGetPlatformIDs(num, &platforms[0], &num);
    if(err != CL_SUCCESS) {
        std::cerr << "Unable to get platform ID\n";
        return 0;
    }

    cl_context_properties prop[] = { CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(platforms[0]), 0 };
    cl_context context = clCreateContextFromType(prop, CL_DEVICE_TYPE_DEFAULT, NULL, NULL, NULL);
    if(context == 0) {
        std::cerr << "Can't create OpenCL context\n";
        return 0;
    }

    size_t cb;
    clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &cb);
    std::vector<cl_device_id> devices(cb / sizeof(cl_device_id));
    clGetContextInfo(context, CL_CONTEXT_DEVICES, cb, &devices[0], 0);

    clGetDeviceInfo(devices[0], CL_DEVICE_NAME, 0, NULL, &cb);
    std::string devname;
    devname.resize(cb);
    clGetDeviceInfo(devices[0], CL_DEVICE_NAME, cb, &devname[0], 0);
    std::cout << "Device: " << devname.c_str() << "\n";

    cl_command_queue queue = clCreateCommandQueue(context, devices[0], 0, 0);
    if(queue == 0) {
        std::cerr << "Can't create command queue\n";
        clReleaseContext(context);
        return 0;
    }

    cl_mem cl_in = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(uchar) * DATA_SIZE, &data1[0], NULL);
    cl_mem cl_height = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_int), &height, NULL);
    cl_mem cl_width = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_int), &width, NULL);

    cl_mem cl_res = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(uchar) * DATA_SIZE, NULL, NULL);

    if(cl_in == 0 || cl_height == 0 || cl_width == 0 || cl_res == 0) {
        std::cerr << "Can't create OpenCL buffer\n";
        clReleaseMemObject(cl_in);
        clReleaseMemObject(cl_height);
        clReleaseMemObject(cl_width);
        clReleaseMemObject(cl_res);
        clReleaseCommandQueue(queue);
        clReleaseContext(context);
        return 0;
    }

    cl_program program = load_program(context, "shader2.cl");
    if(program == 0) {
        std::cerr << "Can't load or build program\n";
        clReleaseMemObject(cl_in);
        clReleaseMemObject(cl_height);
        clReleaseMemObject(cl_width);
        clReleaseMemObject(cl_res);
        clReleaseCommandQueue(queue);
        clReleaseContext(context);
        return 0;
    }

    cl_kernel adder = clCreateKernel(program, "adder", 0);
    if(adder == 0) {
        std::cerr << "Can't load kernel\n";
        clReleaseProgram(program);
        clReleaseMemObject(cl_in);
        clReleaseMemObject(cl_height);
        clReleaseMemObject(cl_width);
        clReleaseMemObject(cl_res);
        clReleaseCommandQueue(queue);
        clReleaseContext(context);
        return 0;
    }

    /*clSetKernelArg(adder, 0, sizeof(cl_mem), &cl_in);
    clSetKernelArg(adder, 1, sizeof(cl_mem), &cl_height);
    clSetKernelArg(adder, 2, sizeof(cl_mem), &cl_width);
    clSetKernelArg(adder, 3, sizeof(cl_mem), &cl_res);*/

    clSetKernelArg(adder, 0, sizeof(cl_in), &cl_in);
    clSetKernelArg(adder, 1, sizeof(cl_height), &cl_height);
    clSetKernelArg(adder, 2, sizeof(cl_width), &cl_width);
    clSetKernelArg(adder, 3, sizeof(cl_res), &cl_res);

    err = clEnqueueNDRangeKernel(queue, adder, 1, 0, &work_size, 0, 0, 0, 0);

    if(err == CL_SUCCESS) {
        err = clEnqueueReadBuffer(queue, cl_res, CL_TRUE, 0, sizeof(uchar) * DATA_SIZE, &data1[0], 0, 0, 0);
    }

    /*if(err == CL_SUCCESS) {
        bool correct = true;
        for(int i = 0; i < DATA_SIZE; i++) {
            if(a[i] + b[i] != res[i]) {
                correct = false;
                break;
            }
        }

        if(correct) {
            std::cout << "Data is correct\n";
        } else {
            std::cout << "Data is incorrect\n";
        }
    } else {
        std::cerr << "Can't run kernel or read back data\n";
    }*/

    clReleaseKernel(adder);
    clReleaseProgram(program);
    clReleaseMemObject(cl_in);
    clReleaseMemObject(cl_height);
    clReleaseMemObject(cl_width);
    clReleaseMemObject(cl_res);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    ////////////////////////////////////////////////
    printf("\n");

	finishCK = omp_get_wtime();
	double duration = (double)(finishCK - startCK);

	if (duration > 1) {
		printf("It took me clicks (%f s).\n\n", duration);
	} else {
		printf("It took me clicks (%f ms).\n\n", duration * 1000);
	}
    ////////////////////////////////////////////////

    cvShowImage("Result",Image1);
    cvWaitKey(0);

    system( "read -n 1 -s -p \"Press any key to continue...\"; echo" );
	return 0;
}
