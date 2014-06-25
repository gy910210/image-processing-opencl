__kernel void Bilinear_beta(__global unsigned char *pSrc,  int width,  int height, __global  unsigned char *pDes, int t_width, int t_height)
{
	int i=get_global_id(0);
	int j=get_global_id(1);

	float zx = (float)(width) / (t_width);
	float zy = (float)(height) / (t_height);

	unsigned char d,rd, ld, lu, ru;
	float x,y,n,m;

	y=j*zy;
	x=i*zx;

	n=y - (int)(y);
	m=x - (int)(x);

	ld = *(pSrc + width * (1+(int)(y)) + (int)(x));//左xia角点的像素值
	rd = *(pSrc + width * (1+(int)(y)) + (int)(x) + 1);// 右上角点的像素值
	lu = *(pSrc + width* (int)(y) + (int)(x));// 左下角点的像素值
	ru = *(pSrc + width* (int)(y) + (int)(x) + 1);// 右上角点的像素值

	d= (unsigned char)((1 - m) * (1 - n) * lu + (1 - m) * n * ld + m * n * rd + m * (1 - n) * ru);//双线性插值，得到变换后的像素
	*(pDes+j*t_width+i)=d;
}