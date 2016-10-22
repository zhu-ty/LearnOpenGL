#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

void main()
{
    // 注意从右向左读
    gl_Position = projection * view * model * vec4(position, 1.0f);
	FragPos = vec3(model * vec4(position, 1.0f));
	//TexCoord = vec2(texCoord.x, 1.0f - texCoord.y);
	TexCoords = texCoord;
	Normal = mat3(transpose(inverse(model))) * normal;
}
