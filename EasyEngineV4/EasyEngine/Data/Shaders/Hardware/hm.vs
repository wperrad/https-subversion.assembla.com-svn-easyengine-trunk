varying vec4 vColor;
uniform float h;
uniform float zMin, scale;

mat4 trans = mat4( -1,			0,			 0,			0,
					0, 		 	0,		 	 1,			0,
					0,		   -1,	 	 	 0,			0,
					0,			0,			 0,			1);

void main()
{
	vec4 vVertex = gl_ProjectionMatrix * trans * gl_Vertex;
	gl_Position = vVertex / scale;
	gl_Position.w = 1.;	
	
	float color = 1. - ( vVertex.z - zMin ) / h;
	vColor = vec4(color, color, color, 1);
}