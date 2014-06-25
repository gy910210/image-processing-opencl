__kernel void Bilinear(__read_only image2d_t srcImage, __write_only image2d_t desImage, float size)
{
	const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
		CLK_ADDRESS_CLAMP| //Clamp to zeros
		CLK_FILTER_NEAREST; //Don't interpolate

	int i=get_global_id(0);
	int j=get_global_id(1);

	float x,y,n,m;
	uint temp;
	uint4 rd, ld, ru, lu, d;

	y=j/size;
	x=i/size;
	n=y - (int)y;
	m=x - (int)x;

	lu=read_imageui(srcImage,smp,(int2)(x, y));
	ld=read_imageui(srcImage,smp,(int2)(x, y+1));
	ru=read_imageui(srcImage,smp,(int2)(x+1, y));
	rd=read_imageui(srcImage,smp,(int2)(x+1, y+1));

	temp=(uint) ((1 - m) * (1 - n) * lu.x + (1 - m) * n * ld.x + m * n * rd.x + m * (1 - n) * ru.x);//双线性插值，得到变换后的像素
	d.x=temp;
	write_imageui(desImage,(int2)(i,j), d);
}