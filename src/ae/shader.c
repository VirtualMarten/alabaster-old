#include "ae/shader.h"
#include "ae/str.h"
#include "ae/array.h"
#include "ae/texture_defs_str.h"
#include "ae/utility.h"
#include <stdio.h>

static vertex_buffer_t *polygon_vertex_buffer = NULL;
static vertex_buffer_t *mesh_vertex_buffer = NULL;

vertex_buffer_t* get_default_polygon_vertex_buffer() {
    return polygon_vertex_buffer
        ? polygon_vertex_buffer
        : (polygon_vertex_buffer = new_vertex_buffer(POLYGON_VERTEX_FORMAT));
}

vertex_buffer_t* get_default_mesh_vertex_buffer() {
    return mesh_vertex_buffer
        ? mesh_vertex_buffer
        : (mesh_vertex_buffer = new_vertex_buffer(MESH_VERTEX_FORMAT));
}

shader_t* new_shader() {
    shader_t *s = calloc(1, sizeof(shader_t));
    s->polygon_vertex_buffer = get_default_polygon_vertex_buffer();
    s->mesh_vertex_buffer = get_default_mesh_vertex_buffer();
    return s;
}

shader_t* new_shader_ext(vertex_buffer_t *polygon_vertex_buffer, vertex_buffer_t *mesh_vertex_buffer) {
    if (!(polygon_vertex_buffer && mesh_vertex_buffer)) return NULL;
    shader_t *s = calloc(1, sizeof(shader_t));
    s->polygon_vertex_buffer = polygon_vertex_buffer;
    s->mesh_vertex_buffer = mesh_vertex_buffer;
    return s;
}

void delete_shader(shader_t **shader) {
    uint32_t i;
    for (i = 0; i < arr_len((*shader)->polygon_vertex_buffer->shader_bindings); i++) {
        if ((*shader)->polygon_vertex_buffer->shader_bindings[i].shader_program_id == (*shader)->program_id) {
            glDeleteVertexArrays(1, (GLuint*)&(*shader)->polygon_vertex_buffer->shader_bindings[i]);
            arr_remove((*shader)->polygon_vertex_buffer->shader_bindings, i);
        }
    }
    for (i = 0; i < arr_len((*shader)->mesh_vertex_buffer->shader_bindings); i++) {
        if ((*shader)->mesh_vertex_buffer->shader_bindings[i].shader_program_id == (*shader)->program_id) {
            glDeleteVertexArrays(1, (GLuint*)&(*shader)->mesh_vertex_buffer->shader_bindings[i]);
            arr_remove((*shader)->mesh_vertex_buffer->shader_bindings, i);
        }
    }
    if ((*shader)->program_id)
        glDeleteProgram((*shader)->program_id);
    free(*shader);
    *shader = NULL;
}

static uint32_t compile_shader(shader_t *shader, uint32_t type, const char *source) {
    {
        str_t ifdef = str_new("#ifdef ");
        switch (type) {
            case GL_VERTEX_SHADER:
                str_append(ifdef, "VERTEX", 6);
                break;
            case GL_FRAGMENT_SHADER:
                str_append(ifdef, "FRAGMENT", 6);
                break;
            case GL_GEOMETRY_SHADER:
                str_append(ifdef, "GEOMETRY", 6);
                break;
        }
        int16_t ifdef_i = 0;
        for (size_t i = 0; i < str_len(source); i++) {
            if (ifdef_i == (int16_t)str_len(ifdef) - 1) {
                ifdef_i = -1;
                break;
            }
            else {
                if (source[i] == ifdef[ifdef_i]) ++ifdef_i;
                else ifdef_i = 0;
            }
        }
        str_del(ifdef);
        if (ifdef_i != -1) return 0;
    }
    #ifndef GLSL_VERSION
    #define GLSL_VERSION "330 core"
    #endif
    char *shader_log = NULL;
    int log_length, status;
    char *sources[4] = { "#version " GLSL_VERSION "\n", NULL, NULL, (char*)source };
    sources[1] = str_new((const char*)include_ae_texture_defs_h);
    // TODO This needs to be compatible with the mesh vertex array, somehow.
    for (unsigned int i = 0; i < shader->polygon_vertex_buffer->attribute_count; i++) {
        char s[256];
        sprintf(s, "#define %s_layout layout(location=%u)\n", shader->polygon_vertex_buffer->attributes[i].name, i);
        str_append(sources[1], s, 0);
    }
    str_append(sources[1], "\0", 1);
    switch (type) {
        case GL_VERTEX_SHADER: sources[2] = "#define VERTEX\n"; break;
        case GL_FRAGMENT_SHADER: sources[2] = "#define FRAGMENT\n"; break;
        case GL_GEOMETRY_SHADER: sources[2] = "#define GEOMETRY\n"; break;
    }
    uint32_t sid = glCreateShader(type);
    glShaderSource(sid, 4, (const char* const*)sources, NULL);
    glCompileShader(sid);
    glGetShaderiv(sid, GL_INFO_LOG_LENGTH, &log_length);
    if (log_length) {
        shader_log = realloc(shader_log, log_length);
        glGetShaderInfoLog(sid, log_length, NULL, shader_log);
        puterr("GLSL", "Failed to compile shader.", 0);
        puterr("GLSL", "Log:\n%s\n", 0, shader_log);
    }
    glGetShaderiv(sid, GL_COMPILE_STATUS, &status);
    if (!status) return 0;
    str_del(sources[1]);
    return sid;
}

bool update_shader(shader_t *shader, const char *source) {
    if (!(shader && source)) return false;
    uint32_t program = glCreateProgram();
    uint32_t vertex = compile_shader(shader, GL_VERTEX_SHADER, source);
    if (!vertex) {
        glDeleteProgram(program);
        return false;
    }
    uint32_t fragment = compile_shader(shader, GL_FRAGMENT_SHADER, source);
    if (!fragment) {
        glDeleteShader(vertex);
        glDeleteProgram(program);
        return false;
    }
    uint32_t geometry = compile_shader(shader, GL_GEOMETRY_SHADER, source);
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    if (geometry) glAttachShader(program, geometry);
    glLinkProgram(program);
    glValidateProgram(program);
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometry) glDeleteShader(geometry);
    char *shader_log = NULL;
    int log_length, link_status, validation_status;
    glGetProgramiv(program, GL_VALIDATE_STATUS, &validation_status);
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
    if (!(link_status && validation_status) && log_length) {
        shader_log = realloc(shader_log, log_length);
        glGetProgramInfoLog(program, log_length, NULL, shader_log);
        puterr("GLSL", "Failed to link shader program.", 0);
        puterr("GLSL", "Log:\n%s\n", 0, shader_log);
    }
    bool valid = (link_status && validation_status);
    if (!valid) return false;
    if ((shader->uniform_color      = glGetUniformLocation(program, "color"))      == -1) valid = false;
    if ((shader->uniform_texmask    = glGetUniformLocation(program, "texmask"))    == -1) valid = false;
    if ((shader->uniform_texrect    = glGetUniformLocation(program, "texrect"))    == -1) valid = false;
    if ((shader->uniform_size       = glGetUniformLocation(program, "size"))   == -1)     valid = valid; // skip
    if ((shader->uniform_view       = glGetUniformLocation(program, "view"))       == -1) valid = false;
    if ((shader->uniform_model      = glGetUniformLocation(program, "model"))      == -1) valid = false;
    if ((shader->uniform_projection = glGetUniformLocation(program, "projection")) == -1) valid = false;
    if (!valid) return false;
    if (shader->program_id) 
        glDeleteProgram(shader->program_id);
    shader->program_id = program;
    vertex_shader_binding_t binding = {0};
    // TODO Somehow the mesh VAO needs to be incorperated into this
    for (uint32_t i = 0; i < arr_len(shader->polygon_vertex_buffer->shader_bindings); i++) {
        if (shader->polygon_vertex_buffer->shader_bindings[i].shader_program_id == shader->program_id)
            binding = shader->polygon_vertex_buffer->shader_bindings[i];
    }
    if (!binding.shader_program_id) {
        binding.shader_program_id = shader->program_id;
        glGenVertexArrays(1, (GLuint*)&binding.vao_id);
        shader->vao_id = binding.vao_id;
        arr_add(shader->polygon_vertex_buffer->shader_bindings, binding);
    }
    unsigned int i, offset;
    vertex_attribute_t *a;
    glBindVertexArray(binding.vao_id);
    // TODO Binding for Polygon VBO, we'll need to append this code with Mesh VBO when we add mesh support
    glBindBuffer(GL_ARRAY_BUFFER, shader->polygon_vertex_buffer->buffer_id);
    for (offset = i = 0; i < shader->polygon_vertex_buffer->attribute_count; i++) {
        a = &shader->polygon_vertex_buffer->attributes[i];
        glVertexAttribPointer(i, a->length, a->type, GL_FALSE, shader->polygon_vertex_buffer->vertex_size, (void*)(uintptr_t)offset);
        glEnableVertexAttribArray(i);
        offset += a->size;
    }
    return valid;
}

bool load_shader(shader_t *shader, const char *path) {
    if (!path) return false;
    str_t source = read_file(path);
    if (!source) return false;
    return update_shader(shader, source);
}