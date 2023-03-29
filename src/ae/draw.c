#include "ae/draw.h"
#include "ae/array.h"

render_instance_t *opaque_render_instances = NULL;
render_instance_t *transparent_render_instances = NULL;

render_instance_t* push_render_instance(render_instance_t inst) {
    render_instance_t **target;
    if (inst.color.a == 255 && (inst.texture ? (inst.texture->mask & T_OPAQUE) : 1))
        target = &opaque_render_instances;
    else
        target = &transparent_render_instances;
    for (uint32_t i = 0; i < arr_len(*target); i++) {
        if ((*target)[i].type == 0) {
            (*target)[i] = inst;
            return &((*target)[i]);
        }
    }
    arr_add(*target, inst);
    return &((*target)[arr_len(*target) - 1]);
}

void _set_uniform_array(render_instance_t *inst, const char *name, bool post, uniform_type_t type, size_t count, va_list args) {
    //if (!shader) return false;
    //int location = glGetUniformLocation(shader->program, name);
    //if (location < 0) return false;
    uniform_update_t up = {
        .name = name,
        .type = type,
        .post = post
    };
    switch (type) {
        case UNIFORM_VEC2: {
                up.type = UNIFORM_2F;
                up.data = malloc(sizeof(float) * 2);
                vec2_t v = va_arg(args, vec2_t);
                memcpy(up.data, v.data, sizeof(float) * 2);
                //glUniform2fv(location, count, (float*)&v);
            }
            break;
        case UNIFORM_VEC3: {
                up.type = UNIFORM_3F;
                up.data = malloc(sizeof(float) * 3);
                vec3_t v = va_arg(args, vec3_t);
                memcpy(up.data, v.data, sizeof(float) * 3);
            }
            break;
            //glUniform3fv(location, count, va_arg(args, vec3_t).data); break;
        case UNIFORM_VEC4: {
                up.type = UNIFORM_4F;
                up.data = malloc(sizeof(float) * 4);
                vec4_t v = va_arg(args, vec4_t);
                memcpy(up.data, v.data, sizeof(float) * 4);
            }
            break;
            //glUniform4fv(location, count, va_arg(args, vec4_t).data); break;
        case UNIFORM_MAT3: {
                up.type = UNIFORM_MATRIX_3F;
                up.data = malloc(sizeof(mat3_t));
                mat3_t m = va_arg(args, mat3_t);
                memcpy(up.data, m.data, sizeof(mat3_t));
            }
            break;
            //glUniformMatrix3fv(location, count, false, va_arg(args, mat3_t).data); break;
        case UNIFORM_MAT4: {
                up.type = UNIFORM_MATRIX_4F;
                up.data = malloc(sizeof(mat4_t));
                mat4_t m = va_arg(args, mat4_t);
                memcpy(up.data, m.data, sizeof(mat4_t));
            }
            break;
            //glUniformMatrix4fv(location, count, false, va_arg(args, mat4_t).data); break;
        case UNIFORM_1F: {
                up.data = malloc(sizeof(float));
                float f = va_arg(args, double);
                memcpy(up.data, &f, sizeof(float));
            }
            break;
            //glUniform1f(location, va_arg(args, double)); break;
        case UNIFORM_2F: {
                up.data = malloc(sizeof(float) * 2);
                float x = va_arg(args, double);
                float y = va_arg(args, double);
                memcpy(up.data, &x, sizeof(float));
                memcpy(up.data + sizeof(float), &y, sizeof(float));
            }
            break;
            // {
            //     const float x = va_arg(args, double);
            //     const float y = va_arg(args, double);
            //     glUniform2f(location, x, y);
            // }
            // break;
        case UNIFORM_3F: {
                up.data = malloc(sizeof(float) * 3);
                float x = va_arg(args, double);
                float y = va_arg(args, double);
                float z = va_arg(args, double);
                memcpy(up.data, &x, sizeof(float));
                memcpy(up.data + sizeof(float), &y, sizeof(float));
                memcpy(up.data + sizeof(float) * 2, &z, sizeof(float));
            }
            break;
            // {
            //     const float x = va_arg(args, double);
            //     const float y = va_arg(args, double);
            //     const float z = va_arg(args, double);
            //     glUniform3f(location, x, y, z);
            // }
            // break;
        case UNIFORM_4F: {
                up.data = malloc(sizeof(float) * 4);
                float x = va_arg(args, double);
                float y = va_arg(args, double);
                float z = va_arg(args, double);
                float w = va_arg(args, double);
                memcpy(up.data, &x, sizeof(float));
                memcpy(up.data + sizeof(float), &y, sizeof(float));
                memcpy(up.data + sizeof(float) * 2, &z, sizeof(float));
                memcpy(up.data + sizeof(float) * 3, &w, sizeof(float));
            }
            break;
            // {
            //     const float x = va_arg(args, double);
            //     const float y = va_arg(args, double);
            //     const float z = va_arg(args, double);
            //     const float w = va_arg(args, double);
            //     glUniform4f(location, x, y, z, w);
            // }
            // break;
        case UNIFORM_1I: {
                up.data = malloc(sizeof(int));
                int i = va_arg(args, int);
                memcpy(up.data, &i, sizeof(int));
            }
            break;
            //glUniform1i(location, va_arg(args, int)); break;
        case UNIFORM_2I: {
                up.data = malloc(sizeof(int) * 2);
                int x = va_arg(args, int);
                int y = va_arg(args, int);
                memcpy(up.data, &x, sizeof(int));
                memcpy(up.data + sizeof(int), &y, sizeof(int));
            }
            break;
            // {
            //     const int x = va_arg(args, int);
            //     const int y = va_arg(args, int);
            //     glUniform2i(location, x, y);
            // }
            // break;
        case UNIFORM_3I: {
                up.data = malloc(sizeof(int) * 3);
                int x = va_arg(args, int);
                int y = va_arg(args, int);
                int z = va_arg(args, int);
                memcpy(up.data, &x, sizeof(int));
                memcpy(up.data + sizeof(int), &y, sizeof(int));
                memcpy(up.data + sizeof(int) * 2, &z, sizeof(int));
            }
            break; 
            // {
            //     const int x = va_arg(args, int);
            //     const int y = va_arg(args, int);
            //     const int z = va_arg(args, int);
            //     glUniform3i(location, x, y, z);
            // }
            // break;
        case UNIFORM_4I: {
                up.data = malloc(sizeof(int) * 4);
                int x = va_arg(args, int);
                int y = va_arg(args, int);
                int z = va_arg(args, int);
                int w = va_arg(args, int);
                memcpy(up.data, &x, sizeof(int));
                memcpy(up.data + sizeof(int), &y, sizeof(int));
                memcpy(up.data + sizeof(int) * 2, &z, sizeof(int));
                memcpy(up.data + sizeof(int) * 3, &w, sizeof(int));
            }
            break; 
            // {
            //     const int x = va_arg(args, int);
            //     const int y = va_arg(args, int);
            //     const int z = va_arg(args, int);
            //     const int w = va_arg(args, int);
            //     glUniform4i(location, x, y, z, w);
            // }
            // break;
        // case UNIFORM_1FV:
        // case UNIFORM_2FV:
        // case UNIFORM_3FV:
        // case UNIFORM_4FV:
        // case UNIFORM_MATRIX_2FV:
        // case UNIFORM_MATRIX_3FV:
        // case UNIFORM_MATRIX_4FV: {
        //         up.data = malloc(sizeof(float*));
        //         float *fp = va_arg(args, double*);
        //         memcpy(up.data, &fp, sizeof(float*));
        //     }
        //     break;
        // case UNIFORM_1IV:
        // case UNIFORM_2IV:
        // case UNIFORM_3IV:
        // case UNIFORM_4IV: {
        //         up.data = malloc(sizeof(int*));
        //         int *ip = va_arg(args, int*);
        //         memcpy(up.data, &ip, sizeof(int*));
        //     }
        //     break;
            //glUniform1fv(location, count, (float*)va_arg(args, double*)); break;
        // case UNIFORM_2FV: glUniform2fv(location, count, (float*)va_arg(args, double*)); break;
        // case UNIFORM_3FV: glUniform3fv(location, count, (float*)va_arg(args, double*)); break;
        // case UNIFORM_4FV: glUniform4fv(location, count, (float*)va_arg(args, double*)); break;
        // case UNIFORM_1IV: glUniform1iv(location, count, va_arg(args, int*)); break;
        // case UNIFORM_2IV: glUniform2iv(location, count, va_arg(args, int*)); break;
        // case UNIFORM_3IV: glUniform3iv(location, count, va_arg(args, int*)); break;
        // case UNIFORM_4IV: glUniform4iv(location, count, va_arg(args, int*)); break;
        // case UNIFORM_MATRIX_2FV: return false;
        // case UNIFORM_MATRIX_3FV: glUniformMatrix3fv(location, count, false, (float*)va_arg(args, double*)); break;
        // case UNIFORM_MATRIX_4FV: glUniformMatrix4fv(location, count, false, (float*)va_arg(args, double*)); break;
        default: return;
    }
    if (!inst->uniform_updates)
        inst->uniform_updates = arr_new(sizeof(uniform_update_t), 1, 1);
    arr_add(inst->uniform_updates, up);
}

void set_uniform(render_instance_t *inst, const char *name, bool post, uniform_type_t type, ...) {
    va_list args;
    va_start(args, type);
    _set_uniform_array(inst, name, post, type, 1, args);
    va_end(args);
}

void set_uniform_array(render_instance_t *inst, const char *name, bool post, uniform_type_t type, uint32_t count, ...) {
    va_list args;
    va_start(args, count);
    _set_uniform_array(inst, name, post, type, count, args);
    va_end(args);
}

render_instance_t* draw_2d(polygon_t *polygon, mat3_t transform, float depth, uint16_t width, uint16_t height, texture_t *texture, texrect_t texrect, color_t color) {
    render_instance_t inst = {
        .type = INST_DRAW_2D,
        .texture = texture,
        .color = color,
        .uniform_updates = NULL,
        { .draw_2d = {
            .polygon = polygon,
            .width = width,
            .height = height,
            .transform = transform,
            .depth = depth,
            .texrect = texrect,
        }}
    };
    return push_render_instance(inst);
}

render_instance_t* draw_polygon(polygon_t *polygon, vec2_t origin, vec2_t position, float depth, uint16_t width, uint16_t height, color_t color) {
    mat3_t m = mat3_from_translation(position);
    m = mat3_scale(m, vec2(width, height));
    m = mat3_translate(m, vec2(-origin.x, -origin.y));
    return draw_2d(polygon, m, depth, width, height, NULL, (texrect_t){0}, color);
}

render_instance_t* draw_polygon_ext(polygon_t *polygon, vec2_t origin, vec2_t position, float depth, uint16_t width, uint16_t height, vec2_t scale, float rotation, color_t color) {
    mat3_t m = mat3_from_translation(position);
    if (rotation) m = mat3_rotate(m, torad(rotation));
    m = mat3_scale(m, vec2(width * scale.x, height * scale.y));
    m = mat3_translate(m, vec2(-origin.x, -origin.y));
    return draw_2d(polygon, m, depth, width, height, NULL, (texrect_t){0}, color);
}

render_instance_t* draw_textured_polygon(polygon_t *polygon, vec2_t origin, vec2_t position, float depth, uint16_t width, uint16_t height, color_t color, texture_t *texture, texrect_t texrect) {
    mat3_t m = mat3_from_translation(position);
    m = mat3_scale(m, vec2(width, height));
    m = mat3_translate(m, vec2(-origin.x, -origin.y));
    return draw_2d(polygon, m, depth, width, height, texture, texrect, color);
}

render_instance_t* draw_textured_polygon_ext(polygon_t *polygon, vec2_t origin, vec2_t position, float depth, uint16_t width, uint16_t height, vec2_t scale, float rotation, color_t color, texture_t *texture, texrect_t texrect) {
    mat3_t m = mat3_from_translation(position);
    if (rotation) m = mat3_rotate(m, torad(rotation));
    m = mat3_scale(m, vec2(width * scale.x, height * scale.y));
    m = mat3_translate(m, vec2(-origin.x, -origin.y));
    return draw_2d(polygon, m, depth, width, height, texture, texrect, color);
}