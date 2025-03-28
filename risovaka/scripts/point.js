

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

    lt(pt)
    {
        return this.x < pt.x || this.y < pt.y
    }

    gt(pt)
    {
        return this.x > pt.x || this.y > pt.y
    }

}

function pt(x,y)
{
    let pt;
    if(x instanceof Point)
    {
        return x.clone()
    }
    else
        return new Point(x,y)
}