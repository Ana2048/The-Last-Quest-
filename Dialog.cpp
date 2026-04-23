#include "Dialog.hpp"
#include "Shader.hpp"

#include <GL/glew.h>
#include <iostream>
#include "stb_image.h"

namespace gps {

    void Dialog::init(int wW, int wH) {
        winW = wW; winH = wH;
        stbi_set_flip_vertically_on_load(false);
        createQuad();
    }

    void Dialog::resize(int wW, int wH) {
        winW = wW; winH = wH;
    }

    void Dialog::show(const std::string& texturePath) {
        destroyTexture();
        tex = loadTextureRGBA(texturePath);
        visible = (tex != 0);
    }

    void Dialog::hide() {
        visible = false;
    }

    void Dialog::destroyTexture() {
        if (tex != 0) {
            glDeleteTextures(1, &tex);
            tex = 0;
        }
    }

    void Dialog::createQuad() {

        float verts[] = {
            // posX posY   u    v
            -1.f, -1.f,  0.f, 0.f,
             1.f, -1.f,  1.f, 0.f,
             1.f,  1.f,  1.f, 1.f,

            -1.f, -1.f,  0.f, 0.f,
             1.f,  1.f,  1.f, 1.f,
            -1.f,  1.f,  0.f, 1.f
        };

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        glBindVertexArray(0);
    }

    unsigned int Dialog::loadTextureRGBA(const std::string& path) {
        int w, h, ch;
        unsigned char* data = stbi_load(path.c_str(), &w, &h, &ch, 4);
        if (!data) {
            std::cerr << "Dialog: cannot load texture: " << path << "\n";
            return 0;
        }

        unsigned int id = 0;
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        stbi_image_free(data);
        glBindTexture(GL_TEXTURE_2D, 0);
        return id;
    }

    void Dialog::render(gps::Shader& shader) {
        if (!visible || tex == 0) return;

        shader.useShaderProgram();

        // sampler uniform
        GLint texLoc = glGetUniformLocation(shader.shaderProgram, "uTex");
        glUniform1i(texLoc, 0);

        // UI state: peste tot
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glBindTexture(GL_TEXTURE_2D, 0);

        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
    }

} 
