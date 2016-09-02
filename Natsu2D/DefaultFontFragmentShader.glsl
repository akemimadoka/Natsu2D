#version 420 core

// Interpolated values from the vertex shaders
in vec2 UV;

// Output data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D text;
uniform sampler2D textTexture;
uniform bool useTexture;
uniform vec3 textColor;

void main()
{
    // Output color = color of the texture at the specified UV
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, UV).r);
    if (useTexture)
    {
    	color = texture2D(text, UV).rgba * sampled;
    }
    else
    {
    	color = vec4(textColor, 1.0) * sampled;
    }
}
