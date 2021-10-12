#define MULTIMATERIAL

varying vec4 vModelVertexPos;
varying vec3 N;
varying vec3 V;
varying vec2 Texcoord;

#ifdef MULTIMATERIAL
attribute float nMatID;
varying float nPSMatID;
#endif // MULTIMATERIAL

uniform mat4 vEntityMatrix[20];

void main()
{
#ifdef MULTIMATERIAL
	nPSMatID = nMatID;
#endif // MULTIMATERIAL
	vModelVertexPos = gl_ModelViewMatrix * vEntityMatrix[gl_InstanceID] * gl_Vertex;
	vec4 vViewVertexPos = gl_ProjectionMatrix * vModelVertexPos;
	N = normalize(gl_NormalMatrix * gl_Normal);
	V = -normalize(vModelVertexPos.xyz);
	Texcoord    = gl_MultiTexCoord0.xy;
	gl_Position = vViewVertexPos;
}