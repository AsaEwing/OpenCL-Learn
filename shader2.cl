__kernel void adder(__global const uchar* d_frame_in, 
					 const int height, 
					 const int width, 
					__global uchar* d_frame_out)
{
	int idx = get_global_id(0);
	int y=0;
	int z=0;
	double total=0;

	for(y=0;y<height;y++) {
		for(z=0;z<3;z++) {
			int index=(y*width+idx)*3+z;
			d_frame_out[index] = 255 - d_frame_in[index];
		}
	}

	/*for(int y=0;y<height;y++) {
		int index=(y*width+idx)*3;
		total = 0.299*d_frame_in[index] + 0.587*d_frame_in[index+1] + 0.114*d_frame_in[index+2]; 

		for(int z=0;z<3;z++) {
			index=(y*width+idx)*3+z;
			d_frame_out[index] = (uchar)total;
		}
	}*/

}