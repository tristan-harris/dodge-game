#version 330

#define PI 3.1415926538

#define PULSE_AMPLITUDE 0.15

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform vec2 frameBufferSize;
uniform float time;
uniform float bpm;

out vec4 finalColor;

const int BLUR_RADIUS = 50;
const float SIGMA = float(BLUR_RADIUS) * 0.25;

float gaussian(float x, float sigma)
{
    return exp(-(x * x) / (2.0 * sigma * sigma));
}

void main()
{
    vec2 texel = 1.0 / frameBufferSize;

    vec4 sum = vec4(0.0);
    float weightSum = 0.0;

    for (int x = -BLUR_RADIUS; x <= BLUR_RADIUS; x++)
    {
        float pulse = 0.75 + cos(time * 2 * PI * (bpm / 60)) * PULSE_AMPLITUDE;
        float w = gaussian(float(x), SIGMA * pulse);
        vec2 offset = vec2(float(x), 0.0) * texel;

        sum += texture(texture0, fragTexCoord + offset) * w;
        weightSum += w;
    }

    finalColor = (sum / weightSum) * colDiffuse * fragColor;
}
