#version 330
// in_Position was bound to attribute index 0 and
// in_Color was bound to attribute index 1
in vec3 in_Position; // ��ġ �Ӽ�
in vec4 in_Color; // ���� �Ӽ�
uniform mat4 modelTransform; // ��ȯ ��Ʈ����
uniform mat4 viewTransform; // view��ȯ ��Ʈ����
uniform mat4 projectionTransform;
uniform vec3 uniform_color; //  ����
out vec4 ex_Color; // �����׸�Ʈ ���̴����� ����

void main(void)
{
	gl_Position = projectionTransform * viewTransform * modelTransform * vec4(in_Position, 1.0);
	ex_Color = vec4(uniform_color, 1.0);
}
