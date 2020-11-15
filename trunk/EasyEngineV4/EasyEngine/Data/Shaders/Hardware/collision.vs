varying vec4 vColor;
uniform float zMin, scale, isGround, isGrid;
uniform mat4 modelMatrix;

uniform float a; // test

mat4 MaxToOpenglMatrix = mat4(  -1,			0,			 0,			0,
								 0, 		0,		 	 1,			0,
								 0,		   -1,	 	 	 0,			0,
								 0,			0,			 0,			1);

float PI = 3.1415927;
float cosa = cos(PI*a/180.);
float sina = sin(a);


mat4 xRot = mat4(1,		   0,		    0,				0,
				 0, 	cosa,		-sina,				0,
				 0,		sina,		 cosa,				0,
				 0,			0,			0,				1);
				 
mat4 xRot180 = mat4( 1,		   	0,		    0,				0,
					 0, 	  	-1,		    0,				0,
					 0,		 	0,		   -1,				0,
					 0,			0,			0,				1);
					 
					 
mat4 zRot180 = mat4( 	-1,			0,		    0,				0,
					 0, 	 	-1,		    0,				0,
					 0,		 	0,		   -1,				0,
					 0,			0,			0,				1);


void main()
{
	//vec4 vVertex = gl_ProjectionMatrix * MaxToOpenglMatrix * xRot180 * modelMatrix * gl_Vertex;
	vec4 vVertex = gl_ProjectionMatrix * MaxToOpenglMatrix * zRot180 * modelMatrix * gl_Vertex;
	//vec4 vVertex = gl_ProjectionMatrix * MaxToOpenglMatrix * modelMatrix * gl_Vertex;
	gl_Position = vVertex / scale;
	gl_Position.w = 1.;
	
	if( isGround == 1. )
	{
		vColor = vec4(0, 1, 0, 1);
	}
	else
	{
		if(isGrid == 1.)
			vColor = vec4(1, 1, 1, 1);
		else
			vColor = vec4(1, 0, 0, 1);
	}
}