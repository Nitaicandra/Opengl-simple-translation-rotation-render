#shader vertex
#version 440 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 color;


out vec3 vs_color;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

uniform mat4 ReverseMatrix;


void main()
{
	//position = vec4(ModelMatrix * vec4(position)).xyz;
	gl_Position=  ProjectionMatrix * ViewMatrix *ModelMatrix * position;
	//gl_Position = ProjectionMatrix* ReverseMatrix * ViewMatrix *  ModelMatrix  * position;
	vs_color = color;
};



#shader fragment
#version 440 core

//layout(location = 0) out vec4 color;
in vec3 vs_color;
out vec4 fs_color;

uniform vec4 u_Color;

void main()
{
	fs_color = vec4(vs_color, 1.f);
	//color = vec4(1.0,0.0,0.0,1.0);
};