#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;

// Output data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
/*uniform float     u_time;

const float step_w = 0.0015625;
const float step_h = 0.0027778;
const float maxW   = 0.01;*/

void main(){
    // Output color = color of the texture at the specified UV
    color = texture2D( myTextureSampler, UV ).rgba;

    //float x = UV.x - maxW*sin((UV.y*80.0 - u_time*7.50));
    //x = fract(x);
    //color = texture2D(myTextureSampler, vec2(x, UV.y)).rgba;
}
