#version 330 core
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

out vec4 color;

uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;


struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;

struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

void main()
{
	//环境
    vec3 ambient = material.ambient * light.ambient;
	
	//漫反射
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * (light.diffuse * material.diffuse);	
	
	//镜面反射
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = material.specular * (spec * light.specular);

	vec3 in_result = ambient + diffuse + specular;
	
    color = mix(texture(ourTexture1, TexCoord), texture(ourTexture2, TexCoord), 0.5) * vec4(in_result,1.0f);
}
