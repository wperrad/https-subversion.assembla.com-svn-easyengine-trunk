attribute float Index;

varying vec4 vColor;

void main()
{
	gl_FrontColor = vec4(1,1,1,1);
	/*
	if ( Index == 0. )
		gl_FrontColor = vec4( 1,0,0,1);
	if ( Index == 1. )
		gl_FrontColor = vec4( 0,1,0,1);
	if ( Index == 2. )
		gl_FrontColor = vec4( 0,0,1,1);
	//gl_FrontColor = vec4(0,1,0,1);
	*/
	
	if ( Index == 0. )
		vColor = vec4( 1,0,0,1);
	if ( Index == 1. )
		vColor = vec4( 0,1,0,1);
	if ( Index == 2. )
		vColor = vec4( 0,0,1,1);

	gl_Position = ftransform();
}