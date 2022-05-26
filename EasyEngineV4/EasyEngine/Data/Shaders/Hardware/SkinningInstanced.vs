#define MULTIMATERIAL

varying vec4 vVertexPos;
varying vec3 N;
varying vec3 V;
varying vec2 Texcoord;

#ifdef MULTIMATERIAL
varying float nPSMatID;
#endif // MULTIMATERIAL

attribute vec4 vVertexWeight;
attribute vec4 vWeightedVertexID;
#ifdef MULTIMATERIAL
attribute float nMatID;
#endif // MULTIMATERIAL

uniform 
uniform mat4 matBones0[70];
uniform mat4 matBones1[70];
uniform mat4 matBones2[70];
uniform mat4 vEntityMatrix[20];

void main()
{
#ifdef MULTIMATERIAL
	nPSMatID = nMatID;
#endif // MULTIMATERIAL

	mat4 matBones[70];
	if(gl_InstanceID == 0)
		matBones = matBones0;
	if(gl_InstanceID == 1)
		matBones = matBones1;
	if(gl_InstanceID == 2)
		matBones = matBones2;
	mat4 matWeight = mat4(0.);

	for ( int iBone = 0; iBone < 4; iBone++ )
	{
		float fBoneID = vWeightedVertexID[ iBone ];
		if ( fBoneID != -1. )
		{
			float fWeightedVertexValue = vVertexWeight[ iBone ];
			matWeight += fWeightedVertexValue * matBones[ int(fBoneID) ];
		}
	}
	
	vVertexPos = gl_ModelViewMatrix * gl_Vertex;
	N = normalize(gl_NormalMatrix * gl_Normal);
	N = mat3(matWeight) * N;
	V = -normalize(vVertexPos.xyz);
	
	Texcoord    = gl_MultiTexCoord0.xy;
	
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vEntityMatrix[gl_InstanceID] * matWeight * gl_Vertex;



}