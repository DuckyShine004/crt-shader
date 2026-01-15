#version 330 core

struct Light {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 f_fragment_position;

in vec3 f_normal;

in vec3 f_colour;

uniform Light u_light;

uniform vec3 u_view_position;

out vec4 o_colour;

vec3 get_phong(Light light, vec3 normal, vec3 view_position, vec3 fragment_position, vec3 colour) {
    vec3 N = normalize(normal);
    vec3 L = normalize(-light.direction);

    float diffuse_strength = max(dot(N, L), 0.0f);
    vec3 diffuse = light.diffuse * diffuse_strength;

    vec3 V = normalize(view_position - fragment_position);
    vec3 R = reflect(-L, N);

    float specular_strength = pow(max(dot(V, R), 0.0f), 32.0f);
    vec3 specular = light.specular * specular_strength;

    return light.ambient * colour + diffuse * colour + specular;
}

void main() {
    vec3 phong = get_phong(u_light, f_normal, u_view_position, f_fragment_position, f_colour);

    o_colour = vec4(phong, 1.0f);
}
