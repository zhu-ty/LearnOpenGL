#version 330 core
in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;

out vec4 color;


struct Material
{
    sampler2D diffuse1;
    sampler2D specular1;
    float shininess;
};
uniform Material material;


//平行光
struct DirLight 
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform DirLight dirLight;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);


//点光源
struct PointLight 
{
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
#define NR_POINT_LIGHTS 4  
uniform PointLight pointLights[NR_POINT_LIGHTS];

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

//手电筒
struct TorchLight
{
	vec3 position;
	vec3 direction;
	float cutoff;
	float outercutoff;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform TorchLight torchLight;

vec3 CalcTorchLight(TorchLight light, vec3 normal, vec3 fragPos);

void main()
{
	// 一些属性
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	// 第一步，计算平行光照
	vec3 result = CalcDirLight(dirLight, norm, viewDir);
	// 第二步，计算顶点光照
	for (int i = 0; i < NR_POINT_LIGHTS; i++)
		result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
	// 第三部，计算 Spot light
	//result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
	result += CalcTorchLight(torchLight, norm, FragPos);
	color = vec4(result, 1.0);
}


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	// 计算漫反射强度
	float diff = max(dot(normal, lightDir), 0.0);
	// 计算镜面反射强度
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// 合并各个光照分量
	vec3 ambient = light.ambient  * vec3(texture(material.diffuse1, TexCoords));
	vec3 diffuse = light.diffuse  * diff * vec3(texture(material.diffuse1, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular1, TexCoords));
	return (ambient + diffuse + specular);
}

// 计算定点光在确定位置的光照颜色
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// 计算漫反射强度
	float diff = max(dot(normal, lightDir), 0.0);
	// 计算镜面反射
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// 计算衰减
	float distance = length(light.position - fragPos);
	float attenuation = 1.0f / (light.constant + light.linear * distance +
		light.quadratic * (distance * distance));
	// 将各个分量合并
	vec3 ambient = light.ambient  * vec3(texture(material.diffuse1, TexCoords));
	vec3 diffuse = light.diffuse  * diff * vec3(texture(material.diffuse1, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular1, TexCoords));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
}

vec3 CalcTorchLight(TorchLight light, vec3 normal, vec3 fragPos)
{
	vec3 torchDir = normalize(light.position - fragPos);
	vec3 torchresult = vec3(0.0f, 0.0f, 0.0f);

	float theta = dot(torchDir, normalize(-light.direction));
	float epsilon = light.cutoff - light.outercutoff;
	float intensity = clamp((theta - light.outercutoff) / epsilon, 0.0f, 1.0f);

	// 执行光照计算
	//环境
	vec3 ambient_t = vec3(texture(material.diffuse1, TexCoords)) * light.ambient;

	//漫反射
	vec3 norm_t = normalize(normal);
	//vec3 lightDir = normalize(light.position - FragPos);
	float diff_t = max(dot(norm_t, torchDir), 0.0);
	vec3 diffuse_t = diff_t * (light.diffuse * vec3(texture(material.diffuse1, TexCoords)));

	//镜面反射
	vec3 viewDir_t = normalize(viewPos - fragPos);
	vec3 reflectDir_t = reflect(-torchDir, norm_t);
	float spec_t = pow(max(dot(viewDir_t, reflectDir_t), 0.0), material.shininess);
	vec3 specular_t = vec3(texture(material.specular1, TexCoords)) * (spec_t * light.specular);

	//距离
	float distance_t = length(light.position - FragPos);
	float attenuation_t = 1.0f / (light.constant + light.linear *distance_t + light.quadratic * (distance_t*distance_t));
	diffuse_t *= intensity;
	specular_t *= intensity;

	ambient_t *= attenuation_t;
	diffuse_t *= attenuation_t;
	specular_t *= attenuation_t;
	return (ambient_t + diffuse_t + specular_t);
}
