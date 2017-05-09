__kernel void adder(__global const float* d_frame_in,
					 __global const float* b,
					 __global float* result)
{
	int idx = get_global_id(0);
	result[idx] = d_frame_in[idx] + b[idx];
}

/*
__kernel void adder(__global const float* a,
					 __global const float* b,
					 __global float* result)
{
	int idx = get_global_id(0);
	result[idx] = a[idx] + b[idx];
}
*/
