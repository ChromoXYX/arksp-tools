#pragma once

//  a tool for Arknights Script Parser & Video Maker
//  thanks to https://mingplusplus.com/tech/2014/09/13/freetype/

#include <opencv2/core.hpp>
#include <string>
#include <utility>
#include <vector>
#include <functional>
extern "C" {
#include <ft2build.h>
#include <freetype/freetype.h>
}

#define ARKSP_FT(_Statement) \
	{ \
	FT_Error ec=_Statement; \
	if(ec!=0){ \
		throw std::string("Error: Line "+std::to_string(__LINE__)+" freetype failed\nerror code "+std::to_string(ec)); \
	}\
	}

namespace arksp {
	class DialogGen {
	public:
		DialogGen() {
		}

		void gen(const std::wstring& str,
			unsigned int interval,
			unsigned int rows,
			unsigned int cols,
			const char* font_path,
			std::function<void(int)> onProgress = [](int) {},
			FT_UInt pixel_height = 16,
			FT_UInt pixel_width = 0,
			FT_Long face_index = 0,
			unsigned int y_offset = 20,
			unsigned int line_spacing = 20,
			unsigned int x_offset = 0) {

			m_mat.clear(); m_mat.shrink_to_fit();
			m_mat.push_back(std::make_shared<cv::Mat>(cv::Mat(rows, cols, CV_8UC3, cv::Scalar(0, 0, 0))));
			auto tstr = str;
			unsigned int x = x_offset, y = y_offset, line_width = 0;
			m_interval = interval;

			FT_Library lib;
			ARKSP_FT(FT_Init_FreeType(&lib));
			FT_Face face;
			ARKSP_FT(FT_New_Face(lib, font_path, face_index, &face));
			ARKSP_FT(FT_Set_Pixel_Sizes(face, pixel_width, pixel_height));

			int _index = 0;
			for (int i = 0; i < tstr.length(); ++i) {
				++_index;
				ARKSP_FT(FT_Load_Char(face, tstr.c_str()[i], FT_LOAD_RENDER));

				unsigned int face_rows = face->glyph->bitmap.rows;
				unsigned int face_cols = face->glyph->bitmap.width;
				line_width += face->glyph->advance.x >> 6;
				if (line_width > cols - x_offset) {
					line_width = 0; x = x_offset; y += line_spacing;
					tstr.assign(tstr.begin() + i, tstr.end());
					i = -1;
					continue;
				}

				cv::Mat temp = (*(m_mat.end() - 1))->clone();
				for (unsigned int j = 0; j < face_rows; ++j) {
					for (unsigned int m = 0; m < face_cols; ++m) {
						unsigned int _y = y - face->glyph->bitmap_top + j;
						unsigned int _x = face->glyph->bitmap_left + x + m;
						if (_y >= rows || _x >= cols ||
							_y < 0 || _x < 0) {
							continue;
						}
						temp.at<cv::Vec3b>(_y, _x) = {
							face->glyph->bitmap.buffer[j * face_cols + m],
							face->glyph->bitmap.buffer[j * face_cols + m],
							face->glyph->bitmap.buffer[j * face_cols + m]
						};
					}
				}
				m_mat.push_back(std::make_shared<cv::Mat>(temp));
				for (unsigned int i = 1; i < interval - 1; ++i) {
					m_mat.push_back(*(m_mat.end() - 1));
				}
				x += face->glyph->advance.x >> 6;

				onProgress(_index);
			}

			ARKSP_FT(FT_Done_Face(face));
		}

		static std::shared_ptr<DialogGen> create(
			const std::wstring& str,
			unsigned int interval,
			unsigned int rows,
			unsigned int cols,
			const char* font_path,
			std::function<void(int)> onProgress = [](int) {},
			FT_UInt pixel_height = 16,
			FT_UInt pixel_width = 0,
			FT_Long face_index = 0,
			unsigned int y_offset = 20,
			unsigned int line_spacing = 20,
			unsigned int x_offset = 0) {
			auto p = std::make_shared<DialogGen>();
			p->gen(str, interval, rows, cols, font_path, onProgress, pixel_height, pixel_width, face_index, y_offset, line_spacing, x_offset);
			return p;
		}

		auto begin() {
			return m_mat.begin();
		}
		auto end() {
			return m_mat.end();
		}
		auto size() {
			return m_mat.size();
		}
		auto get() {
			return m_mat;
		}

	private:
		DialogGen(const DialogGen& _) {}
		std::vector<std::shared_ptr<cv::Mat>> m_mat;
		unsigned int m_interval = 0;
	};
}