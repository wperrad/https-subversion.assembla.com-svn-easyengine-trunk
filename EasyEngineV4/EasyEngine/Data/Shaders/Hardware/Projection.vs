
void main()
{
	gl_FrontColor = vec4(1,1,1,1);
	
	mat4 oProjectionMatrix = mat4(1.);
	oProjectionMatrix[ 0 ][ 0 ] = 1.4323944;
	oProjectionMatrix[ 0 ][ 1 ] = 0.;
	oProjectionMatrix[ 0 ][ 2 ] = 0.;
	oProjectionMatrix[ 0 ][ 3 ] = 0.;
	
	oProjectionMatrix[ 1 ][ 0 ] = 0.;
	oProjectionMatrix[ 1 ][ 1 ] = 1.7904929;
	oProjectionMatrix[ 1 ][ 2 ] = 0.;
	oProjectionMatrix[ 1 ][ 3 ] = 0.;
	
	oProjectionMatrix[ 2 ][ 0 ] = 0.;
	oProjectionMatrix[ 2 ][ 1 ] = 0.;
	oProjectionMatrix[ 2 ][ 2 ] = -1.0000200;
	oProjectionMatrix[ 3 ][ 2 ] = -2.0000200;
	
	oProjectionMatrix[ 3 ][ 0 ] = 0.;
	oProjectionMatrix[ 3 ][ 1 ] = 0.;
	oProjectionMatrix[ 2 ][ 3 ] = -1.0000000;
	oProjectionMatrix[ 3 ][ 3 ] = 0.;
	
	gl_Position = oProjectionMatrix * gl_Vertex;
	
	
	float x = gl_Position.x;
	float y = gl_Position.y;
	float z = gl_Position.z;
	/*
	if ( x > -500 && x < -495 )
		gl_FrontColor += vec4( 1, 0, 0, 0 );
	if ( y > 100 && y < 300 )
		gl_FrontColor += vec4( 0, 1, 0, 0 );
		*/
		/*
	float w = gl_Position.w;
	if ( w > 725 && w < 750 )
		gl_FrontColor += vec4( 0, 0, 1, 0 );
		*/

	
}