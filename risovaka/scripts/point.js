
let dirs = [[1,0],[1,1],[0,1],[-1,1],[-1,0],[-1,-1],[0,-1],[1,-1]];
class Point
{
	x
	y
	constructor(x=0,y=0)
	{
		this.x = x
		this.y = y
	}

	clone()
	{
		return new Point(this.x, this.y)
	}

	sub(pt)
	{
		this.x -= pt.x
		this.y -= pt.y
		return this
	}

	add(pt)
	{
		this.x += pt.x
		this.y += pt.y
		return this
	}

	lt(pt)
	{
		return this.x < pt.x || this.y < pt.y
	}

	lte(pt)
	{
		return this.x <= pt.x || this.y <= pt.y
	}

	gt(pt)
	{
		return this.x > pt.x || this.y > pt.y
	}

	gte(pt)
	{
		return this.x >= pt.x || this.y >= pt.y
	}

	neigb(dir,max)
	{
		let p = pt(dirs[dir]).add(this);
		if(p.lt(pt(0,0)) || p.gte(max))
			return null;
		return p;
	}

	
}

function pt(x,y)
{
	let pt;
	if(x instanceof Point)
	{
		return x.clone()
	}
	else if(Array.isArray(x))
	{
		return new Point(x[0],x[1])
	}
	else
		return new Point(x,y)
}