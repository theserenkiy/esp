
//[x,y]
//let points = [[1,0],[1,1],[0,1],[-1,1],[-1,0],[-1,-1],[0,-1],[1,-1]];
class UVect
{
    dots
    imsz
    start
    end
    prev
    next
    dir
    dirvects = []

    constructor(dots,imsz,start,end,dir,prev)
    {
        this.dots = dots
        this.imsz = imsz
        this.start = start
        this.end = end
        this.dir = dir
        this.prev = prev
    }

    run()
    {
        UVect.findVects(this.dots, this.imsz, this.end, this)
    }

    static findVects(dots,imsz,start,parent)
    {
        let dirvects = []
        for(let dir=0; dir < 8; dir++)
        {
            let p = start.neigb(dir,imsz)
            if(!p)
                continue;
            if(!dots[p.y][p.x][0] || dots[p.y][p.x][1])
                continue;
            let v = new UVect(dots, imsz, start, p, dir, parent)
            dirvects[dir] = v
            dots[p.y][p.x][1] = 1;
        }

        for(let v of dirvects)
        {
            v.run()
        }
        return;
    }
}