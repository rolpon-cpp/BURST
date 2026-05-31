#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

out vec4 finalColor;

uniform float Transparency;

void main()
{
    vec4 normalColor = texture(texture0, fragTexCoord)*colDiffuse*fragColor;
    float dist = distance(fragTexCoord, vec2(0.5, 0.5));
    vec4 vignetteColor = vec4(1.0, 0.1, 0.1, 1.0);

    finalColor = vec4(
        normalColor.r + (vignetteColor.r - normalColor.r) * (dist * Transparency),
        normalColor.g + (vignetteColor.g - normalColor.g) * (dist * Transparency),
        normalColor.b + (vignetteColor.b - normalColor.b) * (dist * Transparency),
        normalColor.a + (vignetteColor.a - normalColor.a) * (dist * Transparency)
    );
}