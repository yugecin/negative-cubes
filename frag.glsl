#version 430
#define debugmov 0
#define shadertoy 0 //noexport
#define doAA 0
layout (location=0) uniform vec4 fpar[2];
#define iTime fpar[0].z/1000.
layout (location=2) uniform vec4 debug[2]; //noexport
#define MAT_GROUND 0
#define MAT_BLU 1
#define MAT_RIG 2
#define MAT_MID 3
int i;
vec3 gHitPosition = vec3(0);
float PI = acos(-1.);
float diag = sqrt(2.);

#define SIZE .5

mat2 rot2(float a){float s=sin(a),c=cos(a);return mat2(c,s,-s,c);}

vec2 m(vec2 b, vec2 a){return a.x<b.x?a:b;}

float tt = 0.;
bool isneg = false;
vec3 realblu = pow(vec3(101.,101.,190.)/255.,vec3(1./.4545));

vec2 neg(vec3 p)
{
	float ttt = tt - PI;
	//p.xy += 1.;
	p.x += SIZE * .25;
	p.z -= ttt;
	p.xy = mod(p.xy, 2.) - 1.;
	p.yz *= rot2(ttt);
	float box = length(max(abs(p)-vec3(.5),0.));
	float right = max(box, -dot(p-vec3(0.,0.,-.5),normalize(vec3(1.,1.,-1.))));
	float right2 = max(box, -dot(p-vec3(0.,0.,.5),normalize(vec3(1.,-1.,1.))));
	vec2 r = vec2(right, MAT_RIG);
	float mid = box;
	mid = max(mid, -dot(p-vec3(0.,0.,-.5),normalize(vec3(-1.,-1.,1.))));
	mid = max(mid, -dot(p-vec3(0.,0.,.5),normalize(vec3(-1.,1.,-1.))));
	r = m(r, vec2(mid, MAT_MID));
	r = m(r, vec2(right2, MAT_RIG));
	return r;
}

	/*
	blu
	p.xy *= rot2(tt);
	p.xy = mod(p.xy, 2.) - 1.;
	*/

vec2 blu(vec3 p)
{
	p.x += tt;
	p.xy = mod(p.xy, 2.) - 1.;
	//p.xz *= rot2(tt);
	p.xy *= rot2(tt);
	return vec2(length(max(abs(p)-vec3(.5),0.)), MAT_BLU);
}

vec2 map(vec3 p)
{
	//p.z -= 2.;
	//p.xy *= rot2(3.1415/4.);

	p.yz *= rot2(-atan(1./sqrt(2.)));
	p.xz *= rot2(PI/4.);

	//p.yz *= rot2(3.1415/4.);

	float x = tt/2./PI;
	//p.x += mix(.0,SIZE+.1,x);
	//p.y += mix(.0,(SIZE-.22)/2.,x);
	p.y += mix(.0,-.87,x);
	p.x += mix(.0,-.24,x);

	return isneg ? neg(p) : blu(p);
}

vec3 norm(vec3 p, float dist_to_p)
{
	vec2 e=vec2(1.,-1.)*.0035;
	return normalize(e.xyy*map(p+e.xyy).x+e.yyx*map(p+e.yyx).x+e.yxy*map(p+e.yxy).x+e.xxx*map(p+e.xxx).x);
}

// x=hit(flopineShade) y=dist_to_p z=dist_to_ro w=material(if hit)
vec4 march(vec3 ro, vec3 rd)
{
	float b,dist;
	vec4 r = vec4(0);
	for (i = 0; i < 100; i++){
		gHitPosition = ro + rd * r.z*.8;
		vec2 m = map(gHitPosition);
		dist = m.x;
		if (dist < .0001) {
			r.x = float(i)/float(200); // TODO: this can just be 1. if not using flopine shade
			r.y = dist;
			r.w = m.y;
			break;
		}
		r.z += dist;
	}
	return r;
}

vec3 getmat(vec4 r, vec3 normal)
{
	vec3 p = gHitPosition.xyz;
	switch (int(r.w)) {
	case MAT_BLU: return vec3(.2,.2,.8);
	case MAT_RIG: return pow(vec3(86.,86.,161.)/255.,vec3(1./.4545));
	case MAT_MID: {
		if (normal.x < normal.y && normal.x < normal.z) {
			return realblu;
		}
		return pow(vec3(230.)/255.,vec3(1./.4545));
	}
	}
	return vec3(1.);
}

vec3 colorHit(vec4 result, vec3 rd, vec3 normal, vec3 mat)
{
	vec3 lig = normalize(vec3(.3,.45,-3.));
	vec3 adj = mat * .3 * 4. * clamp(dot(normal, lig), .0, 1.);
	if (!isneg) return adj;
	return mat;
	/*
	vec3 lig = normalize(vec3(0.,0.,1.));
	return mat * (4. + clamp(dot(normal, lig), 0., 1.) * .5);
	*/
}

out vec4 c;
in vec2 v;
void main()
{
	tt = mod(iTime, 2.*PI);
	isneg = tt >= PI;
	vec2 normuv = (v + 1.) / 2;

	vec3 ro = vec3(0.,0.,-12.), at = vec3(12.,12.,0.), rd;

#if debugmov //noexport
	ro = debug[0].xyz/20.; //noexport
	float vertAngle = debug[1].y/20.; //noexport
	float horzAngle = debug[1].x/20.; //noexport
	if (abs(vertAngle) < .001) { //noexport
		vertAngle = .001; //noexport
	} //noexport
	float xylen = sin(vertAngle); //noexport
	vertAngle = cos(vertAngle); //noexport
	at.x = ro.x + cos(horzAngle) * xylen; //noexport
	at.y = ro.y + sin(horzAngle) * xylen; //noexport
	at.z = ro.z + vertAngle; //noexport
#endif //noexport

        /*
        vec3	cf = normalize(at-ro),
		cl = normalize(cross(cf,vec3(0,0,-1)));
	mat3 rdbase = mat3(cl,normalize(cross(cl,cf)),cf);
	*/

	vec3 resultcol = vec3(0.);
#if doAA == 1
	for (int aaa = 0; aaa < 2; aaa++) {
		for (int aab = 0; aab < 2; aab++) {
#else
	int aaa = 0, aab = 0;
#endif
#if shadertoy == 1 //noexport
			vec2 o = v + vec2(float(aab),float(aab)) / 2. - 0.5; //noexport
			vec2 uv = (o-.5*iResolution.xy)/iResolution.y; //noexport
#else //noexport
			vec2 iResolution = fpar[0].xy;
			vec2 uv = v*(iResolution + vec2(float(aaa),float(aab))/4)/iResolution;
			uv.y /= iResolution.x/iResolution.y;
#endif //noexport
			//vec3 rd = rdbase*normalize(vec3(uv,1))
			vec3 col = isneg ? realblu : vec3(.8);
			ro = vec3(uv*10.,-90.), rd=vec3(0.,0.,1.);

			vec4 result = march(ro, rd);

			if (result.x > 0.) { // hit
				vec3 normal = norm(gHitPosition, result.y);
				col = colorHit(result, rd, normal, getmat(result, normal));
			}
			resultcol += col;
#if doAA == 1
		}
	}
	resultcol /= 4.;
#endif

	resultcol = pow(resultcol, vec3(.4545));
	c = vec4(resultcol, 1.0);
}
