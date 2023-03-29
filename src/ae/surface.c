#include "ae/surface.h"
#include "ae/array.h"
#include <GL/glew.h>

polygon_t *quad_polygon = NULL;
static uint16_t surface_count = 0;

surface_t* new_surface(shader_t *shader, uint16_t width, uint16_t height, color_t color, uint16_t mask) {
    if (!(shader && shader->program_id)) return NULL;
    surface_t *surface = calloc(1, sizeof(surface_t));
    glGenFramebuffers(1, (GLuint*)&surface->framebuffer_id);
    glBindFramebuffer(GL_FRAMEBUFFER, surface->framebuffer_id);
    texture_t *texture = new_texture(width, height, NULL, 0, mask);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->id, 0);
    glGenRenderbuffers(1, (GLuint*)&surface->renderbuffer_id);
    glBindRenderbuffer(GL_RENDERBUFFER, surface->renderbuffer_id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, surface->renderbuffer_id);
    surface->color = color;
    surface->projection = mat4_ortho(0, texture->width, 0, texture->height, PROJECTION_DEPTH_RANGE, -PROJECTION_DEPTH_RANGE);
    surface->texture_id = texture->id;
    surface->width = texture->width;
    surface->height = texture->height;
    surface->mask = texture->mask;
    surface->shader = shader;
    surface->view = (view_t){vec2(0, 0), 0, texture->width, texture->height};
    if (!opaque_render_instances) opaque_render_instances = arr_new(sizeof(render_instance_t), 100, 100);
    if (!transparent_render_instances) transparent_render_instances = arr_new(sizeof(render_instance_t), 100, 50);
    free(texture);
    if (!quad_polygon) quad_polygon = new_polygon(4, (float[]){
        0, 0,
        0, 1,
        1, 1,
        1, 0
    });
    ++surface_count;
    return surface;
}

static void on_window_resize(void *userdata, window_t *window, uint32_t width, uint32_t height) {
    surface_t *surface = userdata;
    surface->projection = mat4_ortho(0, width, height, 0, PROJECTION_DEPTH_RANGE, -PROJECTION_DEPTH_RANGE);
    // if (surface->view.width == surface->width)
    //     surface->view.width = width;
    // if (surface->view.height == surface->height)
    //     surface->view.height = height;
    surface->width = width;
    surface->height = height;
}

surface_t* new_window_surface(window_t *window, shader_t *shader, color_t color) {
    if (!(shader && shader->program_id)) return NULL;
    surface_t *surface = calloc(1, sizeof(surface_t));
    surface->color = color;
    surface->projection = mat4_ortho(0, window->width, window->height, 0, PROJECTION_DEPTH_RANGE, -PROJECTION_DEPTH_RANGE);
    surface->width = window->width;
    surface->height = window->height;
    surface->mask = T_RGB | T_OPAQUE;
    surface->shader = shader;
    surface->view = (view_t){vec2(0, 0), 0, window->width, window->height};
    if (!opaque_render_instances) opaque_render_instances = arr_new(sizeof(render_instance_t), 100, 100);
    if (!transparent_render_instances) transparent_render_instances = arr_new(sizeof(render_instance_t), 100, 50);
    if (!quad_polygon) quad_polygon = new_polygon(4, (float[]){
        0, 0,
        0, 1,
        1, 1,
        1, 0
    });
    handle_event(window, EV_WINDOW_RESIZE, on_window_resize, surface);
    ++surface_count;
    return surface;
}

void delete_surface(surface_t **surface) {
    if (--surface_count == 0) {
        arr_delete(opaque_render_instances);
        arr_delete(transparent_render_instances);
        delete_polygon(&quad_polygon);
    }
    glDeleteTextures(1, (GLuint*)&(*surface)->texture_id);
    glDeleteFramebuffers(1, (GLuint*)&(*surface)->framebuffer_id);
    glDeleteRenderbuffers(1, (GLuint*)&(*surface)->renderbuffer_id);
    free(*surface);
    *surface = NULL;
}

static int opaque_render_instance_compare(const void *_a, const void *_b) {
    const render_instance_t *a = _a, *b = _b;
    return (b->texture ? b->texture->id : 0) - (a->texture ? a->texture->id : 0);
}

static int transparent_render_instance_compare(const void *_a, const void *_b) {
    const render_instance_t *a = _a, *b = _b;
    const float a_depth = (a->type == INST_DRAW_2D) ? a->draw_2d.depth : (a->type == INST_DRAW_3D) ? a->draw_3d.transform.m33 : 0;
    const float b_depth = (b->type == INST_DRAW_2D) ? b->draw_2d.depth : (b->type == INST_DRAW_3D) ? b->draw_3d.transform.m33 : 0;
    int diff = b_depth - a_depth;
    if (!diff) return (b->texture ? b->texture->id : 0) - (a->texture ? a->texture->id : 0);
    return diff;
}

static void _update_uniforms(surface_t *surface, bool post, uniform_update_t *updates) {
    for (uint8_t i = 0; i < (uint8_t)arr_len(updates); i++) {
        if (post != updates[i].post) continue;
        int location = glGetUniformLocation(surface->shader->program_id, updates[i].name);
        if (location < 0) continue;
        switch (updates[i].type) {
            case UNIFORM_1F:
                glUniform1fv(location, 1, (float*)updates[i].data);
                break;
            case UNIFORM_2F:
                glUniform2fv(location, 1, (float*)updates[i].data);
                break;
            case UNIFORM_3F:
                glUniform3fv(location, 1, (float*)updates[i].data);
                break;
            case UNIFORM_4F:
                glUniform4fv(location, 1, (float*)updates[i].data);
                break;
            case UNIFORM_1I:
                glUniform1iv(location, 1, (int*)updates[i].data);
                break;
            case UNIFORM_2I:
                glUniform2iv(location, 1, (int*)updates[i].data);
                break;
            case UNIFORM_3I:
                glUniform3iv(location, 1, (int*)updates[i].data);
                break;
            case UNIFORM_4I:
                glUniform4iv(location, 1, (int*)updates[i].data);
                break;
            default: continue;
        }
    }
}

static void _render_instance(surface_t *surface, render_instance_t *inst) {
    static uint8_t last_texture_id = 0;

    if (inst->uniform_updates)
        _update_uniforms(surface, false, inst->uniform_updates);

    if (!inst->texture) {
        if (last_texture_id != 0) {
            last_texture_id = 0;
            glBindTexture(GL_TEXTURE_2D, 0);
            glUniform1i(surface->shader->uniform_texmask, 0);
        }
    }
    else if (last_texture_id != inst->texture->id) {
        last_texture_id = inst->texture->id;
        glBindTexture(GL_TEXTURE_2D, inst->texture->id);
    }

    glUniform4i(surface->shader->uniform_color, unpack_rgba8(inst->color)); 

    switch (inst->type) {
        case INST_DRAW_2D:
            if (inst->draw_2d.polygon->vertex_buffer != surface->shader->polygon_vertex_buffer) return;
            if (inst->texture) {
                glUniform1i(surface->shader->uniform_texmask, inst->draw_2d.texrect.wrap | (inst->texture->mask & ~T_WRAP));
                glUniform4f(surface->shader->uniform_texrect,
                    inst->draw_2d.texrect.x ? inst->draw_2d.texrect.x / (float)inst->texture->width : 0,
                    inst->draw_2d.texrect.y ? inst->draw_2d.texrect.y / (float)inst->texture->height : 0,
                    inst->draw_2d.texrect.width ? (inst->draw_2d.texrect.width / (float)inst->texture->width) : 1,
                    inst->draw_2d.texrect.height ? (inst->draw_2d.texrect.height / (float)inst->texture->height) : 1
                );
                // glUniform2f(surface->shader->uniform_texscale,
                //     inst->draw_2d.width / (float)inst->texture->width,
                //     inst->draw_2d.height / (float)inst->texture->height
                // );
            }
            glUniform2f(surface->shader->uniform_size, inst->draw_2d.width, inst->draw_2d.height);
            {
                mat4_t transform = mat4_from_mat3(inst->draw_2d.transform);
                transform.m43 = inst->draw_2d.depth;
                glUniformMatrix4fv(surface->shader->uniform_model, 1, GL_FALSE, transform.data);
            }
            glBindBuffer(GL_ARRAY_BUFFER, inst->draw_2d.polygon->vertex_buffer->buffer_id);
            glDrawArrays(GL_TRIANGLE_FAN, inst->draw_2d.polygon->vertex_buffer_offset, inst->draw_2d.polygon->vertex_count);
            break;
        default: return;
    }

    if (inst->uniform_updates) {
        _update_uniforms(surface, true, inst->uniform_updates);
        arr_delete(inst->uniform_updates);
    }
}

void render_surface(surface_t *surface) {
    static surface_t *last_surface = NULL;
    static uint8_t last_program_id = 0;
    static uint32_t last_surface_size = 0;

    if (last_surface != surface) {
        last_surface = surface;
        glBindFramebuffer(GL_FRAMEBUFFER, surface->framebuffer_id);
        glClearColor(unpack_rgba(surface->color));
        glActiveTexture(GL_TEXTURE0);
    }

    if (last_program_id != surface->shader->program_id) {
        last_program_id = surface->shader->program_id;
        glUseProgram(surface->shader->program_id);
        glBindVertexArray(surface->shader->vao_id);
    }

    if (last_surface_size != surface->width * surface->height) {
        glUniformMatrix4fv(surface->shader->uniform_projection, 1, GL_FALSE, surface->projection.data);
        last_surface_size = surface->width * surface->height;
    }  

    glViewport(0, 0, surface->view.width, surface->view.height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    vec2_t origin = vec2(surface->view.width / 2.0f, surface->view.height / 2.0f);
    mat3_t view_transform = mat3_from_translation(surface->view.position);
    view_transform = mat3_rotate(view_transform, surface->view.rotation);
    view_transform = mat3_scale(view_transform, vec2(surface->view.width / (float)surface->width, surface->view.height / (float)surface->height));
    view_transform = mat3_translate(view_transform, vec2(-origin.x, -origin.y));
    glUniformMatrix4fv(surface->shader->uniform_view, 1, GL_FALSE, mat4_from_mat3(mat3_invert(view_transform)).data);
    //glUniformMatrix4fv(surface->shader->uniform_view, 1, GL_FALSE, identity_mat4.data);

    const uint32_t opaque_inst_count = arr_len(opaque_render_instances);
    const uint32_t transparent_inst_count = arr_len(transparent_render_instances);

    if (opaque_inst_count > 1)
        qsort(opaque_render_instances, opaque_inst_count, sizeof(render_instance_t), opaque_render_instance_compare);
    if (transparent_inst_count > 1)
        qsort(transparent_render_instances, transparent_inst_count, sizeof(render_instance_t), transparent_render_instance_compare);

    uint32_t i;
    render_instance_t *inst;

    for (i = 0; i < opaque_inst_count; i++) {
        inst = &opaque_render_instances[i];
        if (!inst->type) continue;
        _render_instance(surface, inst);
        inst->type = 0;
    }

    for (i = 0; i < transparent_inst_count; i++) {
        inst = &transparent_render_instances[i];
        if (!inst->type) continue;
        _render_instance(surface, inst);
        inst->type = 0;
    }
}