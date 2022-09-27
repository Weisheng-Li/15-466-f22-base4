# pragma once
#include "GL.hpp"
#include "gl_errors.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

// as defined in main.cpp
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

struct TextRenderer {
    struct Character {
        unsigned int TextureID; // ID handle of the glyph texture
        glm::ivec2   Size;      // Size of glyph
        glm::ivec2   Offset;    // Offset from baseline to left/top of glyph
        unsigned int Advance;   // Horizontal offset to advance to next glyph
    };

    TextRenderer();
    ~TextRenderer();

    std::map<char, Character> Characters;

    void render_text(std::string text, float x, float y, float scale, glm::vec3 color);

    GLuint program = 0;
    GLuint tofu_texture;
    GLuint VAO, VBO;
};