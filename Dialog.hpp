#pragma once
#include <string>
#include <glm/glm.hpp>

namespace gps {
    class Shader;

    class Dialog {
    public:
        Dialog() = default;

        // init cu dimensiunea ferestrei
        void init(int winW, int winH);

        // cand se schimba rezolutia
        void resize(int winW, int winH);

        // arata/ascunde + schimba textura
        void show(const std::string& texturePath);
        void hide();
        bool isVisible() const { return visible; }

        // render (apelat la final, dupa scena 3D)
        void render(gps::Shader& shader);

    private:
        bool visible = false;

        unsigned int vao = 0, vbo = 0, ebo = 0;
        unsigned int tex = 0;

        glm::mat4 ortho = glm::mat4(1.0f);

        int winW = 1024, winH = 768;

        // pozitia/size in pixeli
        float x = 40.0f;
        float y = 40.0f;
        float w = 520.0f;
        float h = 160.0f;

        unsigned int loadTextureRGBA(const std::string& path);
        void createQuad();
        void updateOrtho();
        void destroyTexture();
    };
}
