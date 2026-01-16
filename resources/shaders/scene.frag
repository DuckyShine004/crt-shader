#version 330 core

struct Fog {
    float start;
    float end;

    vec3 colour;
};

struct Light {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

const vec2 poisson[4] = vec2[](
        vec2(-0.94201624, -0.39906216),
        vec2(0.94558609, -0.76890725),
        vec2(-0.094184101, -0.92938870),
        vec2(0.34495938, 0.29387760)
    );

in vec3 f_fragment_position;

in vec3 f_normal;

in vec3 f_colour;

in vec4 f_shadow_position;
in vec4 f_light_space_position;

uniform Fog u_fog;
uniform Light u_light;

uniform vec3 u_view_position;

uniform sampler2D u_shadow_map;

out vec4 o_colour;

float get_shadow(sampler2D shadow_map, vec4 light_space_position, vec4 shadow_position) {
    vec3 position = shadow_position.xyz / shadow_position.w;

    if (position.x < 0.0f || position.x > 1.0f || position.y < 0.0f || position.y > 1.0f) {
        return 1.0f;
    }

    float shadow = 1.0f;

    float bias = 0.005f;

    for (int i = 0; i < 4; ++i) {
        float depth = texture(u_shadow_map, position.xy + poisson[i] / 700.0f).r;

        if (depth < (position.z - bias)) {
            shadow -= 0.2f;
        }
    }

    return shadow;
}

vec3 get_phong(Light light, vec3 normal, vec3 view_position, vec3 fragment_position, vec3 colour, float shadow) {
    vec3 N = normalize(normal);
    vec3 L = normalize(-light.direction);

    float diffuse_strength = max(dot(N, L), 0.0f);
    vec3 diffuse = light.diffuse * diffuse_strength;

    vec3 V = normalize(view_position - fragment_position);
    vec3 R = reflect(-L, N);

    float specular_strength = pow(max(dot(V, R), 0.0f), 32.0f);
    vec3 specular = light.specular * specular_strength;

    vec3 ambient_light = light.ambient * colour;
    vec3 diffuse_light = shadow * diffuse * colour;
    vec3 specular_light = shadow * specular;

    return ambient_light + diffuse_light + specular_light;
}

float get_fog(Fog fog, float distance) {
    float density = 0.05f;

    float power = -density * density * distance * distance;

    // float fog_factor = exp(power);
    float fog_factor = (fog.end - distance) / (fog.end - fog.start);

    return clamp(fog_factor, 0.0f, 1.0f);
}

void main() {
    float distance_to_fragment = length(u_view_position - f_fragment_position);

    float shadow = get_shadow(u_shadow_map, f_light_space_position, f_shadow_position);

    float fog = get_fog(u_fog, distance_to_fragment);

    vec3 phong = get_phong(u_light, f_normal, u_view_position, f_fragment_position, f_colour, shadow);

    vec3 colour = mix(u_fog.colour, phong, fog);

    o_colour = vec4(colour, 1.0f);
}
