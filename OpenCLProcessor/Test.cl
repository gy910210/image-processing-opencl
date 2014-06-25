__kernel void Test(__global unsigned char *pSrc, int width, int height,  __global unsigned char *pDes,int t_width, int t_height, __local unsigned char *ImageIn, int localColSize, int localRowSize)
{
	float zx = (float)(width) / (t_width);
	float zy = (float)(height) / (t_height);

	int localCol=get_local_id(0);
	int localRow=get_local_id(1);

	int groupCol=get_group_id(0);
	int groupRow=get_group_id(1);

	int globalCol=groupCol*get_local_size(0)+localCol;
	int globalRow=groupRow*get_local_size(1)+localRow;

	int tempx=(int)(globalCol*zx);
	int tempy=(int)(globalRow*zy);

	ImageIn[localRow*localColSize+localCol]=*(pSrc+tempy*width+tempx);

	barrier(CLK_LOCAL_MEM_FENCE);

	unsigned char d,rd, ld, lu, ru;
	float x,y,n,m;

	y=localRow*zy;
	x=localCol*zx;

	n=y - (int)(y);
	m=x - (int)(x);

	ld = *(ImageIn+ localColSize * (1+(int)(y)) + (int)(x));//左xia角点的像素值
	rd = *(ImageIn + localColSize * (1+(int)(y)) + (int)(x) + 1);// 右上角点的像素值
	lu = *(ImageIn + localColSize* (int)(y) + (int)(x));// 左下角点的像素值
	ru = *(ImageIn + localColSize* (int)(y) + (int)(x) + 1);// 右上角点的像素值

	d= (unsigned char)((1 - m) * (1 - n) * lu + (1 - m) * n * ld + m * n * rd + m * (1 - n) * ru);//双线性插值，得到变换后的像素
	*(pDes+globalRow*t_width+globalCol)=d;
}