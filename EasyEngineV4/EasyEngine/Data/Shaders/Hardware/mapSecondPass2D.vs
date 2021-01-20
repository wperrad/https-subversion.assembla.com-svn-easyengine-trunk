varying vec2 Texcoord;

void main()
{
	gl_Position = gl_Vertex;
	Texcoord = gl_MultiTexCoord0.xy;
}  