#include "GL.hpp"
#include <glm/glm.hpp>

#include <string.h>
#include <vector>

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

    GLuint program = 0;
    GLuint tofu_texture;
    GLuint VAO, VBO;

    void render_text(std::string text, float x, float y, float scale, glm::vec3 color) {
         // activate corresponding render state	
        glUseProgram(program);

        glUniform3f(glGetUniformLocation(program, "textColor"), color.x, color.y, color.z);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);

        // iterate through all characters
        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++) 
        {
            // Character ch = Characters[*c];
            Character tofu = {
                tofu_texture,       // texture
                glm::ivec2(25, 25), // size
                glm::ivec2(0, 0),   // offset
                30                  // Advance
            }

            float xpos = x + tofu.offset.x * scale;
            float ypos = y - (tofu.Size.y - tofu.offset.y) * scale;

            float w = tofu.Size.x * scale;
            float h = tofu.Size.y * scale;
            // update VBO for each character
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },            
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }           
            };
            // render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, tofu.TextureID);
            // update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            x += (tofu.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        }

        glUseProgram(0);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

TextRenderer::TextRenderer() {
    // text shaders
    program = gl_compile_program(
        // vertex shader
        "#version 330\n"
        "layout (location = 0) in vec4 vertex;\n" // <vec2 pos, vec2 tex>
        "out vec2 TexCoords;\n"
        "\n"
        "uniform mat4 projection;\n"
        "\n"
        "void main() {\n"
        "    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"
        "    TexCoords = vertex.zw;\n"
        "}\n",

        // fragment shader
        "#version 330\n"
        "in vec2 TexCoords;\n"
        "out vec4 color;\n"
        "\n"
        "uniform sampler2D text;\n"
        "uniform vec3 textColor;\n"
        "\n"
        "void main() {\n"
        "    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
        "    color = vec4(textColor, 1.0) * sampled;\n"
        "}\n"
    )

    // set up tofu texture
    glGenTextures(1, &tofu_texture);
    glBindTexture(GL_TEXTURE_2D, tofu_texture);
    std::vector<uint8_t> tofu_image(25 * 25, 0xff);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        25
        25,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        &tofu_image[0]
    );

    // set up VAO and VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);   
}


TextRenderer::~TextRenderer() {
    if (program != 0) {
        glDeleteProgram(program);
        program = 0;
    }
}

