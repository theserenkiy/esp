
function vectorize2(dots,w,h)
{
    let v = new Vectorizer(dots,w,h)
    v.vectorize()
}

class Vectorizer
{
    dots
    w
    h
    dctx = null

    constructor(dots,w,h)
    {
        this.dots = dots
        this.w = w
        this.h = h
    }

    setDebugCtx(ctx)
    {
        this.dctx = ctx
    }

    vectorize()
    {
        
    }

}