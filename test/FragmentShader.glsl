#version 450 core

/*uniform float     u_time;

const float step_w = 0.0015625;
const float step_h = 0.0027778;
const float maxW   = 0.01;*/

struct LightProperties
{
	bool isEnabled;
	bool isLocal;
	bool isSpot;
	vec3 ambient;
	vec3 color;
	vec3 position;
	vec3 halfVector;
	vec3 coneDirection;
	float spotCosCutoff;
	float spotExponent;
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
};

struct MaterialProperties
{
	vec4 Diffuse;
	vec4 Specular;
	vec4 Ambient;
	vec4 Emission;
	float Shininess;
	float Strength;
};

const int MaxLights = 10;

const float Shininess = 1.0f;
const float Strength = 1.0f;

// Values that stay constant for the whole mesh.

layout(std140, binding = 1) uniform Light
{
	LightProperties Lights[MaxLights];
};

layout(std140, binding = 2) uniform Mater
{
	MaterialProperties Material;
};

uniform sampler2D TextureSampler;
uniform vec3 LightPosition_worldspace;

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;

// Output data
out vec4 color;

void main()
{
	vec3 scatteredLight = vec3(0.0f);
	vec3 reflectedLight = vec3(0.0f);

	for (int i = 0; i < MaxLights; ++i)
	{
		if (!Lights[i].isEnabled)
		{
			continue;
		}

		vec3 halfVector;
		vec3 lightDirection = Lights[i].position;
		float attenuation = 1.0f;

		if (Lights[i].isLocal)
		{
			lightDirection -= vec3(Position_worldspace);
			float lightDistance = length(lightDirection);
			lightDirection /= lightDistance;

			attenuation /=
				Lights[i].constantAttenuation
				+ Lights[i].linearAttenuation 	* lightDistance
				+ Lights[i].quadraticAttenuation* lightDistance * lightDistance;

			if (Lights[i].isSpot)
			{
				float spotCos = dot(lightDirection, -Lights[i].coneDirection);

				if (spotCos < Lights[i].spotCosCutoff)
				{
					attenuation = 0.0f;
				}
				else
				{
					attenuation *= pow(spotCos, Lights[i].spotExponent);
				}
			}

			halfVector = normalize(lightDirection + EyeDirection_cameraspace);
		}
		else
		{
			halfVector = Lights[i].halfVector;
		}

		float diffuse = max(0.0f, dot(Normal_cameraspace, lightDirection));
		float specular = max(0.0f, dot(Normal_cameraspace, halfVector));

		if (diffuse == 0.0f)
		{
			specular = 0.0f;
		}
		else
		{
			specular = pow(specular, Shininess) * Strength;
		}

		scatteredLight += 	Lights[i].ambient * attenuation +
							Lights[i].color * diffuse * attenuation;

		reflectedLight +=	Lights[i].color * specular * attenuation;
	}

	vec4 TexColor = texture2D( TextureSampler, UV ).rgba;
	color = vec4(min(TexColor.rgb * scatteredLight + reflectedLight, vec3(1.0f)), TexColor.a);

    // Output color = color of the texture at the specified UV

    // Light emission properties
	// You probably want to put them as uniforms
	/*vec4 LightColor = vec4(1,1,1,1);
	float LightPower = 2000.0f;
	
	// Material properties
	vec4 MaterialDiffuseColor = texture2D( TextureSampler, UV ).rgba;

	if (MaterialDiffuseColor.a <= 0.1)
	{
		discard;
	}

	vec4 MaterialAmbientColor = vec4(0.1,0.1,0.1,1) * MaterialDiffuseColor;
	vec4 MaterialSpecularColor = vec4(0.3,0.3,0.3,1);

	// Distance to the light
	float distance = length( LightPosition_worldspace - Position_worldspace );

	// Normal of the computed fragment, in camera space
	vec3 n = normalize( Normal_cameraspace );
	// Direction of the light (from the fragment to the light)
	vec3 l = normalize( LightDirection_cameraspace );
	// Cosine of the angle between the normal and the light direction, 
	// clamped above 0
	//  - light is at the vertical of the triangle -> 1
	//  - light is perpendicular to the triangle -> 0
	//  - light is behind the triangle -> 0
	float cosTheta = clamp( dot(n, l), 0,1 );
	//float cosTheta = 1.0f;
	
	// Eye vector (towards the camera)
	vec3 E = normalize(EyeDirection_cameraspace);
	// Direction in which the triangle reflects the light
	vec3 R = reflect(-l,n);
	// Cosine of the angle between the Eye vector and the Reflect vector,
	// clamped to 0
	//  - Looking into the reflection -> 1
	//  - Looking elsewhere -> < 1
	float cosAlpha = clamp( dot(E, R), 0,1 );
	//float cosAlpha = 1.0f;
	
	color = 
		// Ambient : simulates indirect lighting
		MaterialAmbientColor +
		// Diffuse : "color" of the object
		MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance) +
		// Specular : reflective highlight, like a mirror
		MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance*distance);*/

	//color = texture2D(myTextureSampler, UV).rgba;
	//color = vec4(color.r, color.r, color.r, 1);

    //float x = UV.x - maxW*sin((UV.y*80.0 - u_time*7.50));
    //x = fract(x);
    //color = texture2D(myTextureSampler, vec2(x, UV.y)).rgba;
}