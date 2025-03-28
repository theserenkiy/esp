



class Cursor
{
	sz
	pos
	im
	dim
	start
	end

	constructor(im,dim,sz)
	{
		sz = (sz % 2) ? sz : sz+1;
		this.setImage(im,dim)
		this.sz     = pt(sz,sz)
		this.hsz    = pt(Math.floor(this.sz.x/2),Math.floor(this.sz.y/2))
		this.start  = pt(this.hsz)
		this.end    = dim.clone().sub(this.hsz)
		this.pos    = pt(this.start)

		cl(this.dim, this.end, this.pos, this.sz, this.hsz)
	}

	setImage(im,dim)
	{
		this.dim = dim
		this.im = createIm(dim.x, dim.y)
		for(let r = 0; r < dim.y; r++)
		{
			for(let c = 0; c < dim.x; c++)
			{
				this.im[r][c] = [im[r][c],0]
			}
		}
	}

	//dirs: 0 = R, 1 = RD, 2 = D, 3 = LD, 4 = L, 5 = LU, 6 = U, 7 = RU
	dirs = [[1,0],[1,1],[0,1],[-1,1],[-1,0],[-1,-1],[0,-1],[1,-1]]
	mkStep(dir,steps=1)
	{
		let newpos = this.pos.clone()
		newpos.x += this.dirs[dir][0]*steps
		newpos.y += this.dirs[dir][1]*steps

		if(newpos.lt(this.start))
		{
			cl('lt')
			return 0;
		}
		if(newpos.gt(this.end))
		{
			cl('gt',newpos,this.end)
			return 0;
		}
		this.pos = newpos;
		return 1;
	}

	weightSector(from,to)
	{
		cl('wtSector',from,to)
		let wt = 0;
		for(let r=from.y; r <= to.y; r++)
		{
			for(let c = from.x; c <= to.x; c++)
			{
				if(this.im[r][c] > 0)wt++;
			}
		}
		return wt
	}

	weight()
	{
		let cc = this.corners()
		cc.xh = cc.x0+this.hsz.x
		cc.yh = cc.y0+this.hsz.y
		let sectors = {
			TL: this.weightSector(pt(cc.x0,cc.y0), pt(cc.xh, cc.yh)), //tl
			TR: this.weightSector(pt(cc.xh,cc.y0), pt(cc.x1, cc.yh)), //tr
			BL: this.weightSector(pt(cc.x0,cc.yh), pt(cc.xh, cc.y1)), //bl
			BR: this.weightSector(pt(cc.xh,cc.yh), pt(cc.x1, cc.y1))  //br
		}
		cl(sectors)
		return sectors
	}

	moveToLine()
	{
		for(let y=this.start.y; y < this.end.y; y++)
		{
			cl({y})
			while(this.mkStep(0,5))
			{
				let sectors = this.weight()
				let wt = Object.values(sectors).reduce((a,v) => a + v, 0)
				cl({wt})
				if(wt)return sectors
			}
			this.pos.y++;
			this.pos.x = this.start.x;
		}
	}

	getNormalDir(sectorWeights)
	{
		let sw = sectorWeights;
		let vects = [
			(sw.TR+sw.BR)/2,
			sw.BR,
			(sw.BL+sw.BR)/2,
			sw.BL,
			(sw.TL+sw.BL)/2,
			sw.TL,
			(sw.TL+sw.TR)/2,
			sw.TR
		]

		let avects = []
		let maxv = [0,0]
		for(let i=0;i < 4;i++)
		{
			avects[i] = vects[i]-vects[i+4];
			let absv = Math.abs(avects[i]);
			if(absv > maxv[0])
				maxv = [absv,avects[i] >= 0 ? i : i+4]
		}
		return maxv[1];
	}

	stabilizeOnLine(steps)
	{
		for(let i = 0; i < steps; i++)
		{
			let sww = this.weight()
			cl({sww})
			let dir = this.getNormalDir(sww)
			cl({dir})
			this.mkStep(dir)
		}
	}

	corners()
	{
		return {
			x0: this.pos.x-this.hsz.x,
			x1: this.pos.x+this.hsz.x,
			y0: this.pos.y-this.hsz.y,
			y1: this.pos.y+this.hsz.y
		}
	}

	drawMe(im)
	{
		cl('drawMe')
		let x,y;
		let cc = this.corners();

		for(let x = cc.x0; x < cc.x1; x++)
		{
			im[cc.y0][x] = 1;
			im[cc.y1][x] = 1;

			// cl(x,this.pos.y-this.hsz.y)
			// cl(x,this.pos.y+this.hsz.y)
		}

		for(let y = cc.y0; y < cc.y1; y++)
		{
			im[y][cc.x0] = 1;
			im[y][cc.x1] = 1;

			// cl(this.pos.x-this.hsz.x, y)
			// cl(this.pos.x+this.hsz.x, y)
		}
	}
	
}