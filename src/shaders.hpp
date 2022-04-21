#include <string>

namespace resources {
const std::string shader_vs = R"(
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vertex_color;
uniform mat4 mvp;

void main()
{
	// [TODO]
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	vertex_color = aColor;
}

)";

const std::string shader_fs = R"(
// vim: set ft=glsl:

#version 330 core

out vec4 FragColor;
in vec3 vertex_color;

void main() {
	FragColor = vec4(vertex_color, 1.0f);
}
)";

} // namespace resources
