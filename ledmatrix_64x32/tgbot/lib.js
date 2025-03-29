import fs from 'fs'

function genCode(len=4,use_uppercase=false,use_all_letters=false)
{
    let badwords = /sex|seks|secs|porn|[ei]b[laiu]|h[uy][yi]|x[uy][ui]|pi[zs]|popa|go[vw]n|blya|pid[oa]?|f[au][ck]{1,2}|ped[oi]|[kc]al|[kg]ondo|mud|g[ae][iy]/i
    let c = use_all_letters ? ['bcdfghjklmnpqrstvwxz','aeiouy'] : ['bcdfgklmnprstvxz','aeiou']
    if(use_uppercase)
        c = c.map(v => v+v.toUpperCase())
    let r = c => c[Math.floor(Math.random()*c.length)]
    let types = [[0,1],[1,0],[0,1,0],[1,0,1]]
    while(1)
    {
        let pattern = []
        for(let i=0;;i++)
        {
            let p = []
            //avoiding double consonants
            while(1)
            {
                p = types[Math.floor(Math.random()*4)];
                if(!pattern.length || p[0] || pattern[pattern.length-1])
                    break
            }
            pattern.push(...p);
            if(pattern.length >= len-1)
                break;
        }
        let lastsym = '',sym;
        let res = ''
        for(let t of pattern)
        {
            while(1)
            {
                sym = r(c[t])
                if(sym != lastsym)
                    break
            }
            res += sym
            lastsym = sym
        }
        let m;
        if(m = badwords.exec(res))
        {
            // console.log('TEST FAILED: ',m[0])
            continue;
        }
        return res;
    }
}

function delay(ms)
{
    return new Promise(s => setTimeout(s, ms))
}

function getVarPath(name,user)
{
    return user ? 'users/'+user.id+'/vars/'+name+'.txt' : name+'.txt';
}

function getVar(name,user=null)
{
    let path = getVarPath(name,user)
    if(!fs.existsSync(path))
        return null;
    return fs.readFileSync(path,'utf-8');
}

function getIntVar(name,user=null)
{
    let v = +getVar(name,user);
    if(isNaN(v))v = 0;
    return v;
}

function getJSONVar(name,user=null)
{
    let v = getVar(name,user);
    try{
        return JSON.parse(v);
    }catch(e)
    {
        cl('Error parsing object '+name);
        return {}
    }
}

function setJSONVar(name,obj,user=null)
{
    return setVar(name,JSON.stringify(obj,null,"\t"),user);
}

function setVar(name, value, user=null)
{
    fs.writeFileSync(getVarPath(name,user),value+'');
}


export default {
     genCode,
     delay,
     getVar,
     getIntVar,
     getJSONVar,
     setVar,
     setJSONVar
}