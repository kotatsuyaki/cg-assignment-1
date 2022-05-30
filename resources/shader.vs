#version 330 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_color;
layout (location = 2) in vec3 in_normal;
layout (location = 3) in vec2 in_tex_coord;

out vec3 vertex_color;
out vec4 world_pos;
out vec3 n;
out vec3 l;
out vec3 h;
out vec2 tex_coord;

uniform mat4 vp;
uniform mat4 m;

uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;

uniform int light_mode;
uniform int is_vertex_light;

uniform vec3 light_pos;
uniform float diffuse;
uniform float cutoff;
uniform float shine;

uniform int texture_offset_index;

const int DIR_MODE = 0;
const int POINT_MODE = 1;
const int SPOT_MODE = 2;

const float SPOT_EXP = 50.0f;

const vec3 POINT_ATTEN_COEF = vec3(0.01f, 0.8f, 0.1f);
const vec3 SPOT_ATTEN_COEF = vec3(0.05, 0.3f, 0.6f);

const vec3 BAD_MOVE = vec3(1.0f, 0.0f, 0.5f);
const vec3 NOT_VERTEX_LIGHT = vec3(0.5f, 1.0f, 0.0f);

const vec2 TEXTURE_OFFSETS[7] = vec2[7](
    vec2(0.0f, 0.0f),
    vec2(0.0f, -0.25f),
    vec2(0.0f, -0.5f),
    vec2(0.0f, -0.75f),
    vec2(0.5f, 0.0f),
    vec2(0.5f, -0.25f),
    vec2(0.5f, -0.5f)
);

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
            return pow(max(0.0f, dot(l, vec3(0.0f, 0.0f, 1.0f))), SPOT_EXP);
        }
    } else {
        return 1.0f;
    }
}

void main() {
	gl_Position = vp * m * vec4(in_pos, 1.0f);
    tex_coord = in_tex_coord + TEXTURE_OFFSETS[texture_offset_index];

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
		vec3 diffu = kd * max(0.0f, dot(n, l)) * diffuse;
		vec3 spec = ks * pow(max(dot(normalize(n), normalize(h)), 0.0f), shine);

		vertex_color = ambient + spot * atten * (diffu + spec);
	} else {
		vertex_color = NOT_VERTEX_LIGHT;
    }
}

// vim: set ft=glsl:
