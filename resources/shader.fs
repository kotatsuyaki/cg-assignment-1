#version 330 core

in vec3 vertex_color;
in vec4 world_pos;
in vec3 n;
in vec3 l;
in vec3 h;
in vec2 tex_coord;

out vec4 out_color;

uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;

uniform int light_mode;
uniform int is_vertex_light;

uniform vec3 light_pos;
uniform float diffuse;
uniform float cutoff;
uniform float shine;

uniform int tex_loaded;
uniform sampler2D tex;

const int DIR_MODE = 0;
const int POINT_MODE = 1;
const int SPOT_MODE = 2;

const float SPOT_EXP = 50.0f;

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
            return pow(max(dot(l, vec3(0.0f, 0.0f, 1.0f)), 0.0f), SPOT_EXP);
        }
    } else {
        return 1.0f;
    }
}

void main() {
    vec3 tex_color;
    if (tex_loaded == 1) {
        tex_color = texture(tex, tex_coord).rgb;
    } else {
        tex_color = vec3(1.0f);
    }

	if (is_vertex_light == 1) {
		out_color = vec4(tex_color * vertex_color, 1.0f);
	} else {
        float atten = compute_atten(light_mode, light_pos, world_pos.xyz);
        float spot = compute_spot(light_mode, l);

		vec3 amb = ka * 0.15f;
		vec3 diffu = kd * max(dot(normalize(n), normalize(l)), 0.0f) * diffuse;
		vec3 spec = ks * pow(max(dot(normalize(n), normalize(h)), 0.0f), shine);

		vec3 color = amb + spot * atten * (diffu + spec);
		out_color = vec4(tex_color * color, 1.0f);
	}
}

// vim: set ft=glsl:
