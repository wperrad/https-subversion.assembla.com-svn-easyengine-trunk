varying vec4 vColor;
uniform float h;
uniform float zMin, scale;
uniform int nPrecision;

void main()
{	
	vec4 vVertex = gl_ProjectionMatrix * gl_Vertex;
	gl_Position = vVertex / scale;
	gl_Position.w = 1.;	
	
	
	if( nPrecision == 1 )
	{
		float color = ( vVertex.z - zMin ) / h;
		vColor = vec4(color, color, color, 1);
	}
	else if( nPrecision == 3 )
	{

		double height = ( double(vVertex.z) - double(zMin ) ) / double(h);
		double dColor = height * double(pow(2., 24.));
		int nColor = int( dColor );
		int rest = nColor;
		int nb = nColor / int(pow( 2., 16. ));
		rest = rest - nb * int(pow( 2., 16. ));
		int ng = rest / int(pow( 2., 8. ));
		rest = rest - ng * int(pow(2., 8.));
		int nr = rest;
		vColor = vec4( double(nr) / 256.f, double(ng) / 256.f, double(nb) / 256.f, 1.f );
	}
}