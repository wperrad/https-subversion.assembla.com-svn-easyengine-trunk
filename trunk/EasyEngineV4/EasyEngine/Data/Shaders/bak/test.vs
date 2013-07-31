//uniform vec3 vLightPosition;


void main()
{
	vec3 vLightPosition = vec3( 2000.f, 2000.f, -2000.f );	
	float fAtenuation = dot(gl_Normal, normalize(vLightPosition - gl_Position ) );	
	gl_FrontColor = fAtenuation * gl_Color;
//	gl_FrontColor = gl_Color;
	gl_Position = ftransform();
}
