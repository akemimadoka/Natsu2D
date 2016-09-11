#version 430 core

// Interpolated values from the vertex shaders
in vec2 UV;

// Output data
out vec4 color;

// Values that stay constant for the whole mesh.
layout(location = 1) uniform sampler2D text;
layout(location = 2) uniform sampler2D textTexture;
layout(location = 3) uniform bool useTexture;
layout(location = 4) uniform vec3 textColor;

void main()
{
	// Output color = color of the texture at the specified UV
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, UV).r);
	if (useTexture)
	{
		color = texture2D(textTexture, UV).rgba * sampled;
	}
	else
	{
		color = vec4(textColor, 1.0) * sampled;
	}
}
