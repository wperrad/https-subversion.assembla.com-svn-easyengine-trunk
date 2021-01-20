varying vec2 Texcoord;

void main()
{
	vec4 vModelVertexPos = gl_ModelViewMatrix * gl_Vertex;
	vec4 vViewVertexPos = gl_ProjectionMatrix * vModelVertexPos;
	Texcoord    = gl_MultiTexCoord0.xy;
	gl_Position = vViewVertexPos;
}