#version 430 core

/*uniform float     u_time;

const float step_w = 0.0015625;
const float step_h = 0.0027778;
const float maxW   = 0.01;*/

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;

// Output data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D TextureSampler;
uniform vec3 LightPosition_worldspace;

void main()
{
    // Output color = color of the texture at the specified UV

    // Light emission properties
	// You probably want to put them as uniforms
	vec4 LightColor = vec4(1,1,1,1);
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
		MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance*distance);

	//color = texture2D(myTextureSampler, UV).rgba;
	//color = vec4(color.r, color.r, color.r, 1);

    //float x = UV.x - maxW*sin((UV.y*80.0 - u_time*7.50));
    //x = fract(x);
    //color = texture2D(myTextureSampler, vec2(x, UV.y)).rgba;
}