#include <string>

namespace resources {
const std::string SHADER_VS = R"(
#version 330 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_color;
layout (location = 2) in vec3 in_normal;

out vec3 vertex_color;
out vec4 world_pos;
out vec3 n;
out vec3 l;
out vec3 h;

uniform mat4 vp;
uniform mat4 m;

uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;

uniform int light_mode;
uniform int is_vertex_light;

const vec3 light_pos = vec3(1.0f, 1.0f, 1.0f);
const float diffuse = 1.0f;
const float cutoff = radians(30);
const float shine = 64.0f;

const int DIR_MODE = 0;
const int POINT_MODE = 1;
const int SPOT_MODE = 2;

const vec3 POINT_ATTEN_COEF = vec3(0.01f, 0.8f, 0.1f);
const vec3 SPOT_ATTEN_COEF = vec3(0.05, 0.3f, 0.6f);

const vec3 BAD_MOVE = vec3(1.0f, 0.0f, 0.5f);
const vec3 NOT_VERTEX_LIGHT = vec3(0.5f, 1.0f, 0.0f);

// Apply polynomial coefficients specified in `coef` to variable `d`
float poly(float d, vec3 coef) {
    return coef.x + coef.y * d + coef.z * d * d;
}

float compute_atten(int light_mode, vec3 light_pos, vec3 world_pos) {
    float d = length(light_pos - world_pos);

    if (light_mode == POINT_MODE) {
        return min(1.0f / poly(d, POINT_ATTEN_COEF), 1.0f);
    } else if (light_mode == SPOT_MODE) {
        return 1.0f / poly(d, SPOT_ATTEN_COEF);
    } else {
        return 1.0f;
    }
}

float compute_spot(int light_mode, vec3 l) {
    if (light_mode == SPOT_MODE) {
        if (dot(l, vec3(0.0f, 0.0f, 1.0f)) < cos(cutoff)) {
            return 0.0f;
        } else {
            return pow(max(dot(l, vec3(0.0f, 0.0f, 1.0f)), 0.0f), 50.0f);
        }
    } else {
        return 1.0f;
    }
}

void main() {
	gl_Position = vp * m * vec4(in_pos, 1.0f);

	world_pos = m * vec4(in_pos, 1.0f);
    world_pos /= world_pos.w;

    vec3 world_pos_p = world_pos.xyz;

    n = normalize((m * vec4(in_normal, 0.0f)).xyz);

    if (light_mode == DIR_MODE) {
        l = normalize(light_pos);
    } else if (light_mode == POINT_MODE) {
        l = normalize(light_pos - world_pos_p);
    } else if (light_mode == SPOT_MODE) {
        l = normalize(light_pos - world_pos_p);
    } else {
        l = BAD_MOVE;
    }
    h = normalize(normalize(vec3(0.0f, 0.0f, 2.0f) - world_pos_p) + l);

	if (is_vertex_light == 1) {
        float atten = compute_atten(light_mode, light_pos, world_pos.xyz);
        float spot = compute_spot(light_mode, l);

		vec3 ambient = ka * 0.15f;
		vec3 diffu =  kd * max(dot(n, l), 0.0f) * diffuse;
		vec3 spec = ks * pow(max(dot(normalize(n), normalize(h)), 0.0f), shine);

		vertex_color = ambient + spot * atten * (diffu + spec);
	} else {
		vertex_color = NOT_VERTEX_LIGHT;
    }
}

// vim: set ft=glsl:
)";

const std::string SHADER_FS = R"(
#version 330 core

in vec3 vertex_color;
in vec4 world_pos;
in vec3 n;
in vec3 l;
in vec3 h;

out vec4 out_color;

uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;

uniform int light_mode;
uniform int is_vertex_light;

const vec3 light_pos = vec3(1.0f, 1.0f, 1.0f);
const float diffuse = 1.0f;
const float cutoff = radians(30);
const float shine = 64.0f;

const int DIR_MODE = 0;
const int POINT_MODE = 1;
const int SPOT_MODE = 2;

const vec3 POINT_ATTEN_COEF = vec3(0.01f, 0.8f, 0.1f);
const vec3 SPOT_ATTEN_COEF = vec3(0.05, 0.3f, 0.6f);

// Apply polynomial coefficients specified in `coef` to variable `d`
float poly(float d, vec3 coef) {
    return coef.x + coef.y * d + coef.z * d * d;
}

float compute_atten(int light_mode, vec3 light_pos, vec3 world_pos) {
    float d = length(light_pos - world_pos);

    if (light_mode == POINT_MODE) {
        return min(1.0f / poly(d, POINT_ATTEN_COEF), 1.0f);
    } else if (light_mode == SPOT_MODE) {
        return 1.0f / poly(d, SPOT_ATTEN_COEF);
    } else {
        return 1.0f;
    }
}

float compute_spot(int light_mode, vec3 l) {
    if (light_mode == SPOT_MODE) {
        if (dot(l, vec3(0.0f, 0.0f, 1.0f)) < cos(cutoff)) {
            return 0.0f;
        } else {
            return pow(max(dot(l, vec3(0.0f, 0.0f, 1.0f)), 0.0f), 50.0f);
        }
    } else {
        return 1.0f;
    }
}

void main() {
	if (is_vertex_light == 1) {
		out_color = vec4(vertex_color, 1.0f);
	} else {
        float atten = compute_atten(light_mode, light_pos, world_pos.xyz);
        float spot = compute_spot(light_mode, l);

		vec3 amb = ka * 0.15f;
		vec3 diffu =  kd * max(dot(normalize(n), normalize(l)), 0.0f) * diffuse;
		vec3 spec = ks * pow(max(dot(normalize(n), normalize(h)), 0.0f), shine);

		vec3 color = amb + spot * atten * (diffu + spec);
		out_color = vec4(color, 1.0f);
	}
}

// vim: set ft=glsl:
)";

}
