static char *fragSource =
"\n#version 430\n"
"#define debugmov 0\n"
"#define doAA 0\n"
"layout (location=0) uniform vec4 fpar[2];"
"\n#define iTime fpar[0].z/1000.\n"
"#define MAT_BLU 0\n"
"#define MAT_DB 1\n"
"#define MAT_WH 2\n"
"#define MAT_LB 3\n"
"int i;"
"vec3 gHitPosition = vec3(0);"
"float PI = acos(-1.);"
"float diag = sqrt(2.);"
"\n#define SIZE .5\n"
"mat2 rot2(float a){float s=sin(a),c=cos(a);return mat2(c,s,-s,c);}"
"vec2 m(vec2 b, vec2 a){return a.x<b.x?a:b;}"
"float tt = 0.;"
"bool isneg = false;"
"vec3 realblu = pow(vec3(101.,101.,190.)/255.,vec3(1./.4545));"
"float sdBox( vec3 p, vec3 b )"
"{"
"vec3 q = abs(p) - b;"
"return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);"
"}"
"float neg2x(vec3 p)"
"{"
"float box = length(max(abs(p)-vec3(.5),0.));"
"float one = box;"
"one = max(one, -dot(p,normalize(vec3(1.,-1.,0.))));"
"one = max(one, -dot(p,normalize(vec3(-1.,-1.,0.))));"
"one = max(one, -dot(p,normalize(vec3(0.,1.,-1.))));"
"return one;"
"}"
"vec2 neg2(vec3 p)"
"{"
"float ttt = tt - PI;"
"p.x += SIZE * .25 + .015;"
"p.z -= ttt;"
"p.xy = mod(p.xy, 2.) - 1.;"
"p.yz *= rot2(ttt);"
"float box = sdBox(p,vec3(.5)); "
"vec3 q;"
"q = p;"
"vec2 r = vec2(neg2x(q), MAT_WH);"
"q = p; q.xy *= rot2(PI/2.);"
"r = m(r, vec2(neg2x(q), MAT_DB));"
"q = p; q.xy *= rot2(PI);"
"r = m(r, vec2(neg2x(q), MAT_DB));"
"q = p; q.xy *= rot2(-PI/2.);"
"r = m(r, vec2(neg2x(q), MAT_WH));"
"q = p; q.xz *= rot2(-PI/2.);"
"r = m(r, vec2(neg2x(q), MAT_LB));"
"q = p; q.xz *= rot2(-PI/2.); q.xy *= rot2(PI/2.);"
"r = m(r, vec2(neg2x(q), MAT_LB));"
"q = p; q.xz *= rot2(-PI/2.); q.xy *= rot2(PI);"
"r = m(r, vec2(neg2x(q), MAT_LB));"
"q = p; q.xz *= rot2(-PI/2.); q.xy *= rot2(-PI/2.);"
"r = m(r, vec2(neg2x(q), MAT_LB));"
"q = p; q.yz *= rot2(PI/2.);"
"r = m(r, vec2(neg2x(q), MAT_DB));"
"q = p; q.yz *= rot2(PI/2.); q.xy *= rot2(PI/2.);"
"r = m(r, vec2(neg2x(q), MAT_DB));"
"q = p; q.yz *= rot2(PI/2.); q.xy *= rot2(-PI/2.);"
"r = m(r, vec2(neg2x(q), MAT_WH));"
"q = p; q.yz *= rot2(PI/2.); q.xy *= rot2(PI);"
"r = m(r, vec2(neg2x(q), MAT_WH));"
"q = p; q.yz *= rot2(-PI/2.);"
"r = m(r, vec2(neg2x(q), MAT_DB));"
"q = p; q.yz *= rot2(-PI/2.); q.xy *= rot2(PI/2.);"
"r = m(r, vec2(neg2x(q), MAT_DB));"
"q = p; q.yz *= rot2(-PI/2.); q.xy *= rot2(-PI/2.);"
"r = m(r, vec2(neg2x(q), MAT_WH));"
"q = p; q.yz *= rot2(-PI/2.); q.xy *= rot2(PI);"
"r = m(r, vec2(neg2x(q), MAT_WH));"
"q = p; q.yz *= rot2(PI);"
"r = m(r, vec2(neg2x(q), MAT_WH));"
"q = p; q.yz *= rot2(PI); q.xy *= rot2(-PI/2.);"
"r = m(r, vec2(neg2x(q), MAT_WH));"
"q = p; q.yz *= rot2(PI); q.xy *= rot2(PI/2.);"
"r = m(r, vec2(neg2x(q), MAT_DB));"
"q = p; q.yz *= rot2(PI); q.xy *= rot2(PI);"
"r = m(r, vec2(neg2x(q), MAT_DB));"
"return r;"
"}"
"/*"
"blu"
"p.xy *= rot2(tt);"
"p.xy = mod(p.xy, 2.) - 1.;"
"*/"
"vec2 blu(vec3 p)"
"{"
"p.x += tt;"
"p.xy = mod(p.xy, 2.) - 1.;"
"p.xy *= rot2(tt);"
"return vec2(length(max(abs(p)-vec3(.492),0.)), MAT_BLU);"
"}"
"vec2 map(vec3 p)"
"{"
"p.yz *= rot2(-atan(1./sqrt(2.)));"
"p.xz *= rot2(PI/4.);"
"float x = tt/2./PI;"
"p.y += mix(.0,-.86,x);"
"p.x += mix(.0,-.268,x);"
"return isneg ? neg2(p) : blu(p);"
"}"
"vec3 norm(vec3 p, float dist_to_p)"
"{"
"vec2 e=vec2(1.,-1.)*.0035;"
"return normalize(e.xyy*map(p+e.xyy).x+e.yyx*map(p+e.yyx).x+e.yxy*map(p+e.yxy).x+e.xxx*map(p+e.xxx).x);"
"}"
"vec4 march(vec3 ro, vec3 rd)"
"{"
"float b,dist;"
"vec4 r = vec4(0);"
"gHitPosition = ro;"
"for (i = 0; i < 500; i++){"
"gHitPosition = ro + rd * r.z;"
"vec2 m = map(gHitPosition);"
"dist = m.x * .8;"
"if (dist < (isneg ? .001 : .01)) {"
"r.x = float(i)/float(200); "
"r.y = dist;"
"r.w = m.y;"
"break;"
"}"
"r.z += dist;"
"}"
"return r;"
"}"
"vec3 getmat(vec4 r, vec3 normal)"
"{"
"vec3 p = gHitPosition.xyz;"
"switch (int(r.w)) {"
"case MAT_BLU: return vec3(.2,.2,.8);"
"case MAT_DB: return pow(vec3(86.,86.,161.)/255.*1.25,vec3(1./.4545));"
"case MAT_WH: return vec3(.8)*1.57;"
"case MAT_LB: return vec3(.2,.2,.8)*1.4;"
"}"
"return vec3(1.);"
"}"
"vec3 colorHit(vec4 result, vec3 rd, vec3 normal, vec3 mat)"
"{"
"vec3 lig = normalize(vec3(.3,.45,-3.));"
"vec3 adj = mat * .3 * 4. * clamp(dot(normal, lig), .0, 1.);"
"return adj;"
"/*"
"vec3 lig = normalize(vec3(0.,0.,1.));"
"return mat * (4. + clamp(dot(normal, lig), 0., 1.) * .5);"
"*/"
"}"
"out vec4 c;"
"in vec2 v;"
"void main()"
"{"
"tt = mod(iTime, 2.*PI);"
"isneg = tt >= PI;"
"vec2 normuv = (v + 1.) / 2;"
"vec3 ro = vec3(0.,0.,-12.), at = vec3(12.,12.,0.), rd;"
"/*"
"vec3	cf = normalize(at-ro),"
"cl = normalize(cross(cf,vec3(0,0,-1)));"
"mat3 rdbase = mat3(cl,normalize(cross(cl,cf)),cf);"
"*/"
"vec3 resultcol = vec3(0.);"
"\n#if doAA == 1\n"
"for (int aaa = 0; aaa < 2; aaa++) {"
"for (int aab = 0; aab < 2; aab++) {"
"\n#else\n"
"int aaa = 0, aab = 0;"
"\n#endif\n"
"vec2 iResolution = fpar[0].xy;"
"vec2 uv = v*(iResolution + vec2(float(aaa),float(aab))/4)/iResolution;"
"uv.y /= iResolution.x/iResolution.y;"
"vec3 col = isneg ? realblu : vec3(.8);"
"ro = vec3(uv*10.,-90.), rd=vec3(0.,0.,1.);"
"vec4 result = march(ro, rd);"
"if (result.x > 0.) { "
"vec3 normal = norm(gHitPosition, result.y);"
"col = colorHit(result, rd, normal, getmat(result, normal));"
"if (col.x<.1&&col.y<.1&&col.z<.1) col = realblu;"
"}"
"resultcol += col;"
"\n#if doAA == 1\n"
"}"
"}"
"resultcol /= 4.;"
"\n#endif\n"
"resultcol = pow(resultcol, vec3(.4545));"
"c = vec4(resultcol, 1.0);"
"}"
;