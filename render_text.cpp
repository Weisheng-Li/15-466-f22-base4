#include "GL.hpp"
#include "gl_compile_program.hpp"

#include <string>
#include <vector>

#include <hb.h>
#include <hb-ft.h>

#include "render_text.hpp"

void TextRenderer::render_text(std::string text, float x, float y, float scale, glm::vec3 color) {
    // activate corresponding render state	
    glUseProgram(program);

    glUniform3f(glGetUniformLocation(program, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // do the shaping through harfbuzz
    hb_buffer_t *buf = hb_buffer_create();
    hb_buffer_add_utf8(buf, text.c_str(), -1, 0, -1);

    // hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
    // hb_buffer_set_script(buf, HB_SCRIPT_LATIN);
    // hb_buffer_set_language(buf, hb_language_from_string("en", -1));
    hb_buffer_guess_segment_properties (buf);

    hb_font_t *font = hb_ft_font_create(face, nullptr);
    hb_shape(font, buf, NULL, 0);

    // iterate through all glyphs
    unsigned int glyph_count;
    hb_glyph_info_t *glyph_info = hb_buffer_get_glyph_infos(buf, &glyph_count);
    hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(buf, &glyph_count);

    // if x set to 0, choose the center position
    if (x == 0) {
        // get the total length of line
        for (unsigned int i = 0; i < glyph_count; i++) {
            x += (glyph_pos[i].x_advance >> 6) * scale;
        }
        // choose the center position
        x = (SCR_WIDTH - x) / 2;
    }

    for (unsigned int i = 0; i < glyph_count; i++) 
    {
        hb_codepoint_t glyphid = glyph_info[i].codepoint;
        if (glyph_tex.find(glyphid) == glyph_tex.end()) {
            load_glyph(glyphid);
        }
        assert(glyph_tex.find(glyphid) != glyph_tex.end());
        FT_Load_Glyph(face, glyphid, FT_LOAD_DEFAULT);
        FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
        Character ch = glyph_tex.find(glyphid)->second;

        // float xpos = x + glyph_pos[i].x_offset * scale;
        // float ypos = y - glyph_pos[i].y_offset * scale;

        float xpos = x + face->glyph->bitmap_left * scale;
        float ypos = y - (face->glyph->bitmap.rows - face->glyph->bitmap_top) * scale;

        // float w = ch.Size.x * scale;
        // float h = ch.Size.y * scale;

        float w = (float) face->glyph->bitmap.width;
        float h = (float) face->glyph->bitmap.rows;

        // update VBO for each character
        // std::cout << std::to_string(glyph_pos[i].x_offset) + " " << std::to_string(glyph_pos[i].y_offset) + " " << std::endl;
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (glyph_pos[i].x_advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        y += (glyph_pos[i].y_advance >> 6) * scale;

        // std::cout << "x: " << glyph_pos[i].x_advance << "y: " << glyph_pos[i].y_advance << std::endl;

        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    glBindVertexArray(0);
    glUseProgram(0);

    // clean up harfbuzz
    hb_buffer_destroy(buf);
    hb_font_destroy(font);

    GL_ERRORS();
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
    );

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
    glUseProgram(program);
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUseProgram(0);

    // allow pixel color on textures to be only 1-byte aligned
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // set up font table
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (FT_New_Face(ft, "../Reem_Kufi_Ink/ReemKufiInk-Regular.ttf", 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;  
        exit(EXIT_FAILURE);
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    line_space = face->size->metrics.height >> 6;

    // preload ASCII characters, since they are most likely
    // to appear in text
    for (unsigned char c = 0; c < 128; c++) {
        FT_UInt glyph_index = FT_Get_Char_Index(face, c);
        load_glyph(glyph_index);
    }

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

    GL_ERRORS();
}

void TextRenderer::load_glyph (FT_UInt glyph_index) {
    // load character glyph 
    if (FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER))
    {
        std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
        return;
    }
    // generate texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        face->glyph->bitmap.width,
        face->glyph->bitmap.rows,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        face->glyph->bitmap.buffer
    );
    // set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Character ch = {
        texture, 
        glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows)
    };
    glyph_tex.insert(std::pair<FT_UInt, Character>(glyph_index, ch));
    glBindTexture(GL_TEXTURE_2D, 0);
}


TextRenderer::~TextRenderer() {
    if (program != 0) {
        glDeleteProgram(program);
        program = 0;
    }

    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

