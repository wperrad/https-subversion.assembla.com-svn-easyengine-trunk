varying vec2 Texcoord;

uniform vec2 vImagePosition;

void main()
{
	vec4 vOffset = vec4(vImagePosition[0], vImagePosition[1],0,0);
	gl_Position = gl_Vertex + vOffset;
	Texcoord    = gl_MultiTexCoord0.xy;
}
