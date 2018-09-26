#version 330

in vec3 Color;

uniform vec3 SolidColor = vec3(-1,-1,-1);
uniform float ComplementaryCoeff = 0.0;

out vec4 Fragment;

void main()
{
	
	Fragment = vec4(((1.0 * ComplementaryCoeff) - Color.r) + 2 * (Color.r * (1.0 - ComplementaryCoeff)), ((1.0 * ComplementaryCoeff) - Color.g) + 2 * (Color.g * (1.0 - ComplementaryCoeff)), ((1.0 * ComplementaryCoeff) - Color.b) + 2 * (Color.b * (1.0 - ComplementaryCoeff)), 1);
	
	if(SolidColor.r != -1.0 && SolidColor.g != -1.0 && SolidColor.b != -1.0)
		Fragment = vec4(((1.0 * ComplementaryCoeff) - SolidColor.r) + 2 * (SolidColor.r * (1.0 - ComplementaryCoeff)), ((1.0 * ComplementaryCoeff) - SolidColor.g) + 2 * (SolidColor.g * (1.0 - ComplementaryCoeff)), ((1.0 * ComplementaryCoeff) - SolidColor.b) + 2 * (SolidColor.b * (1.0 - ComplementaryCoeff)), 1);

	return;
}