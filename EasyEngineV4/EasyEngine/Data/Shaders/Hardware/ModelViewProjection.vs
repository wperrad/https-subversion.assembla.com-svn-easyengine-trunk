
void main()
{
	gl_FrontColor = vec4(1,1,1,1);
	
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
}