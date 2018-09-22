#version 330 core

in vec4 coloredCubeColor;
out vec4 FragColor;

void main()
{
	FragColor = coloredCubeColor;
}