#version 330

in vec4 ex_Color; // 버텍스 세이더에게서 전달 받음
in vec3 frag_pos;
in vec3 normal;

out vec4 FragColor; // 색상 출력


uniform float ambientLight;
uniform vec3 lightPos;
uniform vec3 lightColor;

uniform vec3 view_pos;

void main(void)
{
	vec3 ambient = ambientLight * lightColor;

	vec3 light_dir = normalize(lightPos - frag_pos);
	float diffuseLight = max(dot(normal, light_dir), 0.0);
	vec3 diffuse = diffuseLight * lightColor;

	int shininess = 128;
	vec3 view_dir = normalize(view_pos - frag_pos);
	vec3 reflect_dir = reflect(light_dir, normal);
	float specular_effect = max(dot(view_dir, reflect_dir),0.0);
	vec3 specular = specular_effect * lightColor;

	vec3 sum = ambient + diffuse + specular;
	vec4 result = vec4(sum, 1.0) * ex_Color;
	FragColor = result;
}

