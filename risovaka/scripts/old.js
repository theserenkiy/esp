function mkVector(i0,white,dir)
{
	cl(`mkVector ${i0}, ${white}, ${dir}`);
	let [x0,y0] = i2xy(i0);
	cl(`x0 ${x0}, y0 ${y0}`);
	let x = x0,y = y0;
	let steps = 0;
	let i;
	while(x < W && y < H)
	{
		if(steps++ > MAXSTEPS)
		{
			cl('steps out');
			return 0;
			//break;
		}
		i = xy2i(x,y);
		//cl(`x ${x}, y ${y}, i ${i}, v ${sgs[i]}`);
		if(i <= 0 || i >= RES-1)
		{
			cl('out of res');
			break;
		}

		if((white-sgs[i]) < 20)
		{
			cl('edge found');
			//putPixel(i,[255,0,0]);
			break;
		}

		if(i==i0)
			putPixel(i, i==i0 ? [255,0,0] : [255,255,0]);

		x += dir[0];
		y += dir[1];
	}

	return {x: Math.floor(x),y: Math.floor(y),i,len: steps};
}

function findVector(i,white)
{
	let lastlen = 0, res=null;
	for(let angn=0; angn < ANGS; angn++)	//Math.PI*2; ang += Math.PI/16)
	{
		let ang = angn*ANGSTEP;
		res = mkVector(i,white,[Math.cos(ang), Math.sin(ang)]);
		cl(res.len+'; '+lastlen)
		if(res.len < 3 && lastlen > 3)
		{
			putPixel(res.i,[255,0,0]);
			cl(ang);
			break;
		}

		lastlen = res.len;
		//stats.push(res);
	}

	// let lastlen = 0;
	// for(let ang=1; ang < 32; ang++)
	// {
	// 	if(Math.abs(stats[ang][2]-stats[ang-1][2]) < 3)
	// 		continue;
	// }

	// stats.sort((a,b) => a[2]-b[2])

	// cl(i2xy(i))

	// cl(stats)
	return res ? res.i : -1;
}

function vectorize(i,white)
{
	for(let j=0; j < 100; j++)
	{
		let newi = findVector(i,white);
		if(newi < 0)return;
		i = newi;
	}
}

let avga = [127,127,127,127,127,127,127,127];
	let avgp = 0;
	for(let i=0; i < sgs.length; i++)
	{
		if(1 || (avg - sgs[i]) > 20)
		{
			putPixel(i);
			avga[(avgp++)%8] = sgs[i];
			vectorize(i+(W*0),avg);
			break;
		}
		else {
			avga[(avgp++)%8] = sgs[i];
		}
		
	}