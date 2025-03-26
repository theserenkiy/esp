
class Vectorizer2
{
	dots
	w
	h
	dctx = null
	idata_t = null

	constructor(dots,w,h)
	{
		this.dots = dots
		this.w = w
		this.h = h
	}

	putPixel(i,color=[255,0,0])
	{
		this.idata_t.data[i*4] = color[0];//sgs[i];
		this.idata_t.data[i*4+1] = color[1];//sgs[i];
		this.idata_t.data[i*4+2] = color[2];//sgs[i];
		this.idata_t.data[i*4+3] = 255;
	}

	setDebugCtx(ctx)
	{
		this.dctx = ctx
		this.idata_t = this.dctx.getImageData(0,0,W,H);
	}

	vectorize()
	{

	}

}