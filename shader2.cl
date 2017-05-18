__kernel void adder(__global const uchar* d_frame_in, 
					int height2, 
					int width2, 
					__global uchar* d_frame_out)
{
	int idx = get_global_id(0);
	//int width = convert_int(width2);
	//int height = convert_int(height2);
	int width = 1024;
	int height = 768;
	//int width = width2;
	//int height = height2;

	float index = convert_float(idx);
	float wi = convert_float(width);
	float hi = convert_float(height);

	//printf("sss");

	int x=idx%width;
	int y=convert_int((index-convert_float(x))/1024);
	int z=0;
	double total=0;

	//float index = convert_float(idx)/3;
	
	//index=(y*width+x)*3+z;
	//index=(y*width+x)*3;
	//index/3=y*width+x;

	int wi2 = convert_int(wi/2);
	int hi2 = convert_int(hi/2);

	float rr = (x-wi2)*(x-wi2)+(y-hi2)*(y-hi2);
	//float rr = (x-512)*(x-512)+(y-384)*(y-384);
	rr = sqrt(rr);
	int rr2 = convert_int(rr);

	int tmp = (y*width+x)*3;
	total = 0.299*d_frame_in[tmp] + 0.587*d_frame_in[tmp+1] + 0.114*d_frame_in[tmp+2];

	d_frame_out[tmp] = total;
	d_frame_out[tmp+1] = total;
	d_frame_out[tmp+2] = total;
	
	/*if (rr2%100==rr2%200){
		
		d_frame_out[tmp] = 255-d_frame_in[tmp];
		d_frame_out[tmp+1] = 255-d_frame_in[tmp+1];
		d_frame_out[tmp+2] = 255-d_frame_in[tmp+2];
	} else {
		int tmp = (y*width+x)*3;
		d_frame_out[tmp] = d_frame_in[tmp];
		d_frame_out[tmp+1] = d_frame_in[tmp+1];
		d_frame_out[tmp+2] = d_frame_in[tmp+2];
	}*/
}