"use strict";

class Basilikum
{
	static alphabet =
	"abcdefghijklmnopqrstuvwxyz" +
	"0123456789" +
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ" +
	"-.";

	static unalpha = [];
	
	static
	{
		for(var u=0; u<256; ++u)
			this.unalpha[u] = -1;
		
		for(var u=0; u<62; ++u)
			this.unalpha[ this.alphabet.charCodeAt(u) ] = u;
		
		console.log(this.unalpha);
	}
	
	static sidx(input, idx)
	{
		var c = idx<input.length ? input.charCodeAt(idx) : 32;
		return (c<32 || c>126) ? 94 : c-32;
	}
	
	static base62(coll, length)
	{
		var out = "";
		while(length)
		{
			var rem = coll % 62;
			coll = coll / 62;
			
			out += alphabet.charAt(rem);
			--length;
		}
		return out;
	}
}
