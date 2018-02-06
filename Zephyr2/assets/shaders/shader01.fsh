#version 330 core
in vec2 oTexC;
in vec3 oNormal;
in vec3 oWorldPos;
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec3 FragPosition;
layout (location = 2) out vec3 FragNormal;
uniform float iSmoothness;
uniform sampler2D iTexImage;
uniform sampler2D iNormImage;
uniform bool iHasNorm;

//from http://www.thetenthplanet.de/archives/1180
mat3 cotangent_frame( vec3 N, vec3 p, vec2 uv )
{
    // get edge vectors of the pixel triangle
    vec3 dp1 = dFdx( p );
    vec3 dp2 = dFdy( p );
    vec2 duv1 = dFdx( uv );
    vec2 duv2 = dFdy( uv );
 
    // solve the linear system
    vec3 dp2perp = cross( dp2, N );
    vec3 dp1perp = cross( N, dp1 );
    vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;
 
    // construct a scale-invariant frame 
    float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );
    return mat3( T * invmax, B * invmax, N );
}

//adapted from http://www.thetenthplanet.de/archives/1180
vec3 perturb_normal( vec3 N, vec3 V, vec2 texcoord )
{
    vec3 map = texture2D( iNormImage, texcoord ).xyz;
    map = map * 255./127. - 128./127.;
    mat3 TBN = cotangent_frame( N, -V, texcoord );
    return normalize( TBN * map );
}

void main()
{
	vec4 texColor = texture(iTexImage, oTexC);
	
	if(texColor.a < 0.75)
		discard;
	
	FragColor.rgb = texColor.rgb;
	FragColor.a = iSmoothness;
	FragPosition = oWorldPos;
	FragNormal = normalize(oNormal);
	if(iHasNorm)
	{
		FragNormal = perturb_normal(FragNormal, FragPosition, oTexC);
	}

   
}