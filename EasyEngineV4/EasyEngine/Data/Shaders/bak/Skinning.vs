#ifdef PIXEL_LIGHTING

varying vec4 vVertexPos;
varying vec3 N;
varying vec3 V;
varying vec2 Texcoord;

void main()
{
	vVertexPos = gl_ModelViewMatrix * gl_Vertex;
	N = normalize(gl_NormalMatrix * gl_Normal);
	V = -normalize(vVertexPos.xyz);
	
	Texcoord    = gl_MultiTexCoord0.xy;
	
	gl_Position = ftransform();
}

#else

varying vec4 vVertexPos;
varying vec3 N;
varying vec3 V;
varying vec2 Texcoord;

attribute vec4 vVertexWeight;
attribute vec4 vWeightedVertexID;

uniform mat4 matBones[10];

void main()
{

	mat4 matWeight = mat4(1.);

	for ( int iBone = 0; iBone < 4; iBone++ )
	{
		float fBoneID = vWeightedVertexID[ iBone ];
		if ( fBoneID != -1. )
		{
			float fWeightedVertexValue = vVertexWeight[ iBone ];
			matWeight += fWeightedVertexValue * matBones[ fBoneID ];
		}
	}

	
	
	vVertexPos = gl_ModelViewMatrix * gl_Vertex;
	N = normalize(gl_NormalMatrix * gl_Normal);
	V = -normalize(vVertexPos.xyz);
	
	Texcoord    = gl_MultiTexCoord0.xy;
	
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * matWeight * gl_Vertex;



}

#endif //PIXEL_LIGHTING