#ifdef VERTEX

position_layout in vec3 position;
uv_layout in vec2 uv;
out vec2 UV;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform vec4 texrect;
uniform int texmask;

void main() {
    UV.x = (texmask & T_WRAP_X) == 0 ? (uv.x * texrect.z + texrect.x) : uv.x;
    UV.y = (texmask & T_WRAP_Y) == 0 ? (uv.y * texrect.w + texrect.y) : uv.y;
    gl_Position = projection * view * model * vec4(position, 1.0);
}

#endif



#ifdef FRAGMENT

precision mediump float;

uniform ivec4 color;
in vec2 UV;
out vec4 fragment;
uniform mat4 model;
uniform sampler2D texture0;
uniform vec4 texrect;
uniform int texmask;
const vec3 erase_color = vec3(215, 0, 255) / 255.0;

void main() {
    vec4 c = color / 255.0;
    vec2 uv = UV;
    if ((texmask & T_WRAP_X) != 0) uv.x = fract((UV.x - texrect.x) / texrect.z) * texrect.z + texrect.x;
    if ((texmask & T_WRAP_Y) != 0) uv.y = fract((UV.y - texrect.y) / texrect.w) * texrect.w + texrect.y;
    if ((texmask & T_SDF) != 0) {
        float dist = texture(texture0, uv).r;
        float width = fwidth(dist);
        float alpha = smoothstep(0.5 - width, 0.5 + width, dist);
        fragment = vec4(c.rgb, alpha * c.a);
    }
    else {
        if (texmask > 0) {
            vec4 sampled = texture(texture0, uv);
            if ((texmask & T_R) != 0) fragment = vec4(1.0, 1.0, 1.0, sampled.r) * c;
            else if ((texmask & T_RGB) != 0) {
                if (sampled.rgb == erase_color) discard;
                fragment = vec4(sampled.rgb, 1.0) * c;
            }
            else fragment = sampled * c;
        }
        else fragment = c;
    }
    if (fragment.a == 0) discard;
}

#endif
