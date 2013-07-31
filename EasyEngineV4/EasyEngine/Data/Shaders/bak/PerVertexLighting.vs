/*
attribute vec4 vVertexWeight;
attribute vec4 vWeightedVertexID;
uniform int bIsWeighted;
uniform mat4 matBones[10];
*/

float GetAttenuationFactor( vec4 vertexPos, gl_LightSourceParameters light );
vec4 GetLightInfluence( gl_LightSourceParameters light, vec4 vVertexPos, vec3 N, vec3 V );

varying vec2 Texcoord;

void main()
{
	vec4 vVertexPos = gl_ModelViewMatrix * gl_Vertex;
	vec3 N = normalize(gl_NormalMatrix * gl_Normal);
	vec3 V = -normalize(vVertexPos.xyz);
	
	gl_FrontColor = vec4(0,0,0,0);
	for ( int i = 0; i < 8; i++ )
		gl_FrontColor += GetLightInfluence( gl_LightSource[i], vVertexPos, N, V );
	
	Texcoord    = gl_MultiTexCoord0.xy;
	
	/*
	mat4 matWeight = mat4(1.);
	if ( bIsWeighted == 1 )
	{
		for ( int iBone = 0; iBone < 4; iBone++ )
		{
			int nBoneID = int(vWeightedVertexID[ iBone ]);
			if ( nBoneID != -1 )
			{
				float fWeightedVertexValue = vVertexWeight[ iBone ];
				matWeight += fWeightedVertexValue * matBones[ nBoneID ];
	
				//if ( nBoneID == 0 )
				//	gl_FrontColor += vec4(fWeightedVertexValue,0,0,1);
				//if ( nBoneID == 1 )
				//	gl_FrontColor += vec4(0,fWeightedVertexValue,0,1);
			}
		}
	}
	*/
	
	//gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * matWeight * gl_Vertex;
	gl_Position = ftransform();
}

float GetAttenuationFactor( vec4 vertexPos, gl_LightSourceParameters light )
{
	float fDistance = distance(light.position, vertexPos);
	return 1. / ( light.constantAttenuation + light.linearAttenuation * fDistance + light.quadraticAttenuation*pow(fDistance,2.) );
}

vec4 GetLightInfluence( gl_LightSourceParameters light, vec4 vVertexPos, vec3 N, vec3 V )
{
	vec4 vAmbient = gl_FrontMaterial.ambient * light.ambient;
	vec3 L = normalize(light.position - vVertexPos).xyz;
	vec4 vDiffuse = max(dot(L,N), 0.) * gl_FrontMaterial.diffuse * light.diffuse;
	vec3 R = reflect( -L, N );
	vec4 vSpecular = pow(max(dot(R,V),0.), gl_FrontMaterial.shininess) * gl_FrontMaterial.specular * light.specular;
	float fAttenuationFactor = GetAttenuationFactor( vVertexPos, light );
	return fAttenuationFactor * ( vAmbient + vDiffuse + vSpecular );
}
