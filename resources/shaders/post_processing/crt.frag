#version 330 core

const float PI = acos(-1.0f);

const int PIXEL_SIZE = 8;

in vec2 f_uv;

uniform float u_brightness;
uniform float u_vignette_opacity;
uniform float u_vignette_roundness;

uniform vec2 u_curvature;
uniform vec2 u_resolution;
uniform vec2 u_scanline_opacity;

uniform sampler2D u_scene;

out vec4 o_colour;

vec2 curve_uv(vec2 uv, vec2 curvature) {
    uv = uv * 2.0f - 1.0f;

    vec2 offset = abs(uv.yx) / vec2(curvature.x, curvature.y);

    uv = uv + uv * offset * offset;
    uv = uv * 0.5f + 0.5f;

    return uv;
}

vec4 scanline_intensity(float uv, float resolution, float opacity) {
    float intensity = sin(uv * resolution * PI * 2.0f);

    intensity = ((0.5f * intensity) + 0.5f) * 0.9f + 0.1f;

    return vec4(vec3(pow(intensity, opacity)), 1.0f);
}

vec4 vignette_intensity(vec2 uv, vec2 resolution, float opacity, float roundness) {
    float intensity = uv.x * uv.y * (1.0f - uv.x) * (1.0f - uv.y);

    return vec4(vec3(clamp(pow((resolution.x / roundness) * intensity, opacity), 0.0f, 1.0f)), 1.0f);
}

vec2 pixelate_uv(vec2 uv, vec2 resolution) {
    vec2 fragment_position = uv * resolution;

    float x = float(int(fragment_position.x) % PIXEL_SIZE);
    float y = float(int(fragment_position.y) % PIXEL_SIZE);

    x = fragment_position.x + floor(float(PIXEL_SIZE) / 2.0f) - x;
    y = fragment_position.y + floor(float(PIXEL_SIZE) / 2.0f) - y;

    return vec2(x, y) / u_resolution;
}

void main() {
    vec2 uv_curve = curve_uv(f_uv, u_curvature);

    vec2 uv_pixelate = pixelate_uv(uv_curve, u_resolution);

    vec4 base_colour = texture(u_scene, uv_pixelate);

    base_colour *= vignette_intensity(uv_curve, u_resolution, u_vignette_opacity, u_vignette_roundness);

    base_colour *= scanline_intensity(uv_curve.x, u_resolution.y, u_scanline_opacity.x);
    base_colour *= scanline_intensity(uv_curve.y, u_resolution.x, u_scanline_opacity.y);

    base_colour *= vec4(vec3(u_brightness), 1.0f);

    if (uv_curve.x < 0.0f || uv_curve.y < 0.0f || uv_curve.x > 1.0f || uv_curve.y > 1.0f) {
        o_colour = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    } else {
        o_colour = base_colour;
    }
}
