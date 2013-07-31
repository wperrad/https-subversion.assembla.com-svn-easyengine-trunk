
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