#version 330 core
in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightColor;
//uniform vec3 lightPos;
uniform vec3 viewPos;

out vec4 color;

//uniform sampler2D ourTexture1;
//uniform sampler2D ourTexture2;


struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
uniform Material material;

struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	float constant;
    float linear;
    float quadratic;
};
uniform Light light;


struct Torch
{
	vec3 position;
    vec3 direction;
    float cutOff;
	float outerCutOff;
};
uniform Torch torch;

void main()
{
	//手电筒	


	vec3 torchDir = normalize(torch.position - FragPos);
	vec3 torchresult = vec3(0.0f,0.0f,0.0f);

	float theta = dot(torchDir, normalize(-torch.direction));
	float epsilon = torch.cutOff - torch.outerCutOff;
	float intensity = clamp((theta - torch.outerCutOff) / epsilon, 0.0f, 1.0f);

	// 执行光照计算
	//环境
	vec3 ambient_t = vec3(texture(material.diffuse, TexCoords)) * light.ambient;

	//漫反射
	vec3 norm_t = normalize(Normal);
	//vec3 lightDir = normalize(light.position - FragPos);
	float diff_t = max(dot(norm_t, torchDir), 0.0);
	vec3 diffuse_t = diff_t * (light.diffuse * vec3(texture(material.diffuse, TexCoords)));

	//镜面反射
	vec3 viewDir_t = normalize(viewPos - FragPos);
	vec3 reflectDir_t = reflect(-torchDir, norm_t);
	float spec_t = pow(max(dot(viewDir_t, reflectDir_t), 0.0), material.shininess);
	vec3 specular_t = vec3(texture(material.specular, TexCoords)) * (spec_t * light.specular);

	//距离
	float distance_t = length(torch.position - FragPos);
	float attenuation_t = 1.0f / (light.constant + light.linear *distance_t + light.quadratic * (distance_t*distance_t));
	diffuse_t *= intensity;
	specular_t *= intensity;

	ambient_t *= attenuation_t;
	diffuse_t *= attenuation_t;
	specular_t *= attenuation_t;
	torchresult = ambient_t + diffuse_t + specular_t;
	
	vec3 in_result = vec3(0.0f, 0.0f, 0.0f);
	//环境
    vec3 ambient = vec3(texture(material.diffuse, TexCoords)) * light.ambient;
	
	//漫反射
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * (light.diffuse * vec3(texture(material.diffuse, TexCoords)));	
	
	//镜面反射
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = vec3(texture(material.specular, TexCoords)) * (spec * light.specular);
	
	//距离
	float distance = length(light.position - FragPos);
	float attenuation = 1.0f / (light.constant + light.linear *distance + light.quadratic * (distance*distance));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	in_result = ambient + diffuse + specular;
	
    //color = mix(texture(ourTexture1, TexCoord), texture(ourTexture2, TexCoord), 0.5) * vec4(in_result,1.0f);
	color = vec4(in_result + torchresult,1.0f);
}
