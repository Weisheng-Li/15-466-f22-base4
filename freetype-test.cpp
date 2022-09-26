#include <ft2build.h>
#include FT_FREETYPE_H

#include <hb.h>
#include <hb-ft.h>

#include <iostream>

//This file exists to check that programs that use freetype / harfbuzz link properly in this base code.
//You probably shouldn't be looking here to learn to use either library.

int main(int argc, char **argv) {
	// create buffer
	hb_buffer_t *buf = hb_buffer_create();
	char* text = "hello world";
	// these arguments work as long as text is NULL terminated
	// and you want to shape everything in text
	hb_buffer_add_utf8(buf, text, -1, 0, -1);

	// load font for freetype
	FT_Library library;
	FT_ERROR status;
	FT_Init_FreeType( &library );

	hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
    hb_buffer_set_script(buf, HB_SCRIPT_LATIN);
    hb_buffer_set_language(buf, hb_language_from_string("en", -1));

	FT_face ft_face;
	// create face object (contain list of glyphs and size object)
	status = FT_New_Face( library,
				"Reem_Kufi_Ink/ReemKufiInk-Regular.ttf",
				0,
				&ft_face );
	assert(status == 0);

	// set the value for size object (initialize to 0)
	// point is a physical distance unit smaller than inch
	status = FT_Set_Char_Size(
          ft_face,    /* handle to face object           */
          0,       /* char_width in 1/64th of points  */
          16*64,   /* char_height in 1/64th of points */
          96,      /* horizontal device resolution (dpi) */
          0 );     /* vertical device resolution (dpi)   */
	assert(status == 0);

	// hb_font is an instance of hb_face with size specified
	hb_font_t *font = hb_ft_font_create(ft_face);

	hb_shape(font, buf, NULL, 0);

	// retrieve array of glyphs and their corresponding position
	unsigned int glyph_count;
    // fhb_glyph_info_t *glyph_info    = hb_buffer_get_glyph_infos(buf, &glyph_count);
    hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(buf, &glyph_count);

	hb_position_t cursor_x = 0;
	hb_position_t cursor_y = 0;
	for (unsigned int i = 0; i < glyph_count; i++) {
		// although it's called codepoint, it's actually glyph index
		hb_codepoint_t glyphid  = glyph_info[i].codepoint;

		status = FT_Load_Glyph (ft_face, glyphid, FT_LOAD_DEFAULT);

		// convert from vector graphic (default to fft format) to bitmap
		status = FT_Render_Glyph(ft_face->glyph, FT_RENDER_MODE_NORMAL);

		hb_position_t x_offset  = glyph_pos[i].x_offset;
		hb_position_t y_offset  = glyph_pos[i].y_offset;
		hb_position_t x_advance = glyph_pos[i].x_advance;
		hb_position_t y_advance = glyph_pos[i].y_advance;
		draw_glyph(face->glyph->bitmap, cursor_x + x_offset, cursor_y + y_offset);
		std::cout << "x: " << cursor_x + x_offset << "y: " << cursor_y + y_offset << std::endl;
		cursor_x += x_advance;
		cursor_y += y_advance;
	}


	// clean up
	hb_buffer_destroy(buf);
    hb_font_destroy(font);
    hb_face_destroy(face);
    hb_blob_destroy(blob);
}
