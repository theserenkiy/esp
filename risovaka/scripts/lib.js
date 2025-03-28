function createIm(w,h)
{
    if(!w)w = W
    if(!h)h = H
	let im = []
	for(let row=0;row < H;row++)
		im.push([])
	return im;
}