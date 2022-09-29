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
        glm::ivec2   Offset;    // offset from baseline to left/top of glyph
    };

    TextRenderer();
    ~TextRenderer();

    // mapping from glyph index to textures
    std::map<FT_UInt, Character> glyph_tex;

    FT_Face face;
    FT_Library ft;

    // distance between two lines, determined by font and size
    unsigned int line_space;

    void load_glyph(FT_UInt glyph_index);
    void render_text(std::string text, float x, float y, float scale, glm::vec3 color);

    GLuint program = 0;
    GLuint tofu_texture;
    GLuint VAO, VBO;
};