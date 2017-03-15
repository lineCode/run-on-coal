#include "stdafx.h"
#include "Elements/Font.h"

const float g_AtlasPixelOffset = 1.f / static_cast<float>(FONT_ATLAS_SIZE);

ROC::Font::Font()
{
    m_elementType = ElementType::FontElement;

    m_loaded = false;
    m_library = FT_Library();
    m_face = FT_Face();

    m_atlasTexture = 0U;
    m_atlasPack = NULL;

    m_charIter = m_charMap.begin();
    m_charMapEnd = m_charMap.end();

    m_vertices = NULL;
    m_uv = NULL;

    m_filteringType = 0;
}
ROC::Font::~Font()
{
    if(m_loaded)
    {
        glDeleteTextures(1, &m_atlasTexture);
        glDeleteBuffers(1, &m_vertexVBO);
        glDeleteBuffers(1, &m_uvVBO);
        glDeleteVertexArrays(1, &m_VAO);

        delete m_vertices;
        delete m_uv;

        delete m_atlasPack;

        FT_Done_Face(m_face);
        FT_Done_FreeType(m_library);
    }
}

bool ROC::Font::LoadTTF(const std::string &f_path, int f_size, int f_filter)
{
    if(!m_loaded)
    {
        if(!FT_Init_FreeType(&m_library))
        {
            if(!FT_New_Face(m_library, f_path.c_str(), 0, &m_face))
            {
                FT_Select_Charmap(m_face, ft_encoding_unicode);
                FT_Set_Pixel_Sizes(m_face, 0, f_size);
                m_filteringType = f_filter;
                btClamp(m_filteringType, FONT_FILTER_NEAREST, FONT_FILTER_LINEAR);

                // Generate atlas texture
                glGenTextures(1, &m_atlasTexture);
                glBindTexture(GL_TEXTURE_2D, m_atlasTexture);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST + m_filteringType);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST + m_filteringType);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, FONT_ATLAS_SIZE, FONT_ATLAS_SIZE, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);

                // Generate atlas
                m_atlasPack = new rbp::MaxRectsBinPack();
                m_atlasPack->Init(FONT_ATLAS_SIZE, FONT_ATLAS_SIZE, false);

                // Generate buffers
                glGenVertexArrays(1, &m_VAO);
                glGenBuffers(1, &m_vertexVBO);
                glGenBuffers(1, &m_uvVBO);

                glBindVertexArray(m_VAO);

                glEnableVertexAttribArray(0);
                glBindBuffer(GL_ARRAY_BUFFER, m_vertexVBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 6 * FONT_MAX_TEXT_LENGTH, NULL, GL_DYNAMIC_DRAW);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
                m_vertices = new std::vector<glm::vec3>;
                m_vertices->assign(6 * FONT_MAX_TEXT_LENGTH, glm::vec3(0.f, 0.f, 1.f));

                glEnableVertexAttribArray(1);
                glBindBuffer(GL_ARRAY_BUFFER, m_uvVBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 6 * FONT_MAX_TEXT_LENGTH, NULL, GL_DYNAMIC_DRAW);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);
                m_uv = new std::vector<glm::vec2>(6 * FONT_MAX_TEXT_LENGTH);

                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(0);

                m_loaded = true;
            }
            else FT_Done_FreeType(m_library);
        }
    }
    return m_loaded;
}
bool ROC::Font::LoadChar(unsigned int f_char)
{
    bool l_result = false;
    if(!FT_Load_Char(m_face, f_char, FT_LOAD_RENDER))
    {
        charData *l_charData = new charData();
        l_charData->m_size.x = m_face->glyph->bitmap.width;
        l_charData->m_size.y = m_face->glyph->bitmap.rows;
        l_charData->m_advance = static_cast<float>(m_face->glyph->advance.x >> 6);
        l_charData->m_bearing.x = m_face->glyph->bitmap_left;
        l_charData->m_bearing.y = m_face->glyph->bitmap_top;

        if(l_charData->m_size.x > 0 && l_charData->m_size.y > 0)
        {
            rbp::Rect l_rectangle = m_atlasPack->Insert(l_charData->m_size.x, l_charData->m_size.y, rbp::MaxRectsBinPack::RectBestLongSideFit);
            if(l_rectangle.height > 0)
            {
                l_charData->m_atlasPosition.x = static_cast<float>(l_rectangle.x) * g_AtlasPixelOffset;
                l_charData->m_atlasPosition.y = static_cast<float>(l_rectangle.y) * g_AtlasPixelOffset;
                l_charData->m_atlasPosition.z = l_charData->m_atlasPosition.x + static_cast<float>(l_charData->m_size.x) * g_AtlasPixelOffset;
                l_charData->m_atlasPosition.w = l_charData->m_atlasPosition.y + static_cast<float>(l_charData->m_size.y) * g_AtlasPixelOffset;
                glTexSubImage2D(GL_TEXTURE_2D, 0, l_rectangle.x, l_rectangle.y, l_rectangle.width, l_rectangle.height, GL_RED, GL_UNSIGNED_BYTE, m_face->glyph->bitmap.buffer);
            }
        }

        m_charIter = m_charMap.insert(std::pair<unsigned int, charData*>(f_char, l_charData)).first;
        m_charMapEnd = m_charMap.end();

        l_result = true;
    }
    return l_result;
}

void ROC::Font::Draw(const sf::String &f_text, const glm::vec2 &f_pos, bool f_bind)
{
    if(m_loaded)
    {
        if(f_bind)
        {
            glBindVertexArray(m_VAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_atlasTexture);
        }
        float l_displacement = f_pos.x;
        glm::vec2 l_result;
        int l_charIncrement = 0;
        for(auto iter : f_text)
        {
            if(l_charIncrement >= FONT_MAX_TEXT_LENGTH) break;
            int l_charQueue = l_charIncrement * 6;

            m_charIter = m_charMap.find(iter);
            if(m_charIter == m_charMapEnd)
            {
                if(!LoadChar(iter)) continue;
            }

            if(m_charIter->second->m_size.x > 0 && m_charIter->second->m_size.y > 0)
            {
                l_result.x = l_displacement + m_charIter->second->m_bearing.x;
                l_result.y = f_pos.y - (m_charIter->second->m_size.y - m_charIter->second->m_bearing.y);

                m_vertices->operator[](l_charQueue).x = m_vertices->operator[](l_charQueue + 1).x = m_vertices->operator[](l_charQueue + 3).x = l_result.x;
                m_vertices->operator[](l_charQueue + 1).y = m_vertices->operator[](l_charQueue + 2).y = m_vertices->operator[](l_charQueue + 4).y = l_result.y;
                m_vertices->operator[](l_charQueue + 2).x = m_vertices->operator[](l_charQueue + 4).x = m_vertices->operator[](l_charQueue + 5).x = l_result.x + m_charIter->second->m_size.x;
                m_vertices->operator[](l_charQueue).y = m_vertices->operator[](l_charQueue + 3).y = m_vertices->operator[](l_charQueue + 5).y = l_result.y + m_charIter->second->m_size.y;

                m_uv->operator[](l_charQueue).x = m_uv->operator[](l_charQueue + 1).x = m_uv->operator[](l_charQueue + 3).x = m_charIter->second->m_atlasPosition.x;
                m_uv->operator[](l_charQueue).y = m_uv->operator[](l_charQueue + 3).y = m_uv->operator[](l_charQueue + 5).y = m_charIter->second->m_atlasPosition.y;
                m_uv->operator[](l_charQueue + 2).x = m_uv->operator[](l_charQueue + 4).x = m_uv->operator[](l_charQueue + 5).x = m_charIter->second->m_atlasPosition.z;
                m_uv->operator[](l_charQueue + 1).y = m_uv->operator[](l_charQueue + 2).y = m_uv->operator[](l_charQueue + 4).y = m_charIter->second->m_atlasPosition.w;

                l_charIncrement++;
            }

            l_displacement += m_charIter->second->m_advance;
        }
        if(l_charIncrement > 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, m_vertexVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 3 * 6 * l_charIncrement, m_vertices->data());
            glBindBuffer(GL_ARRAY_BUFFER, m_uvVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 2 * 6 * l_charIncrement, m_uv->data());
            glDrawArrays(GL_TRIANGLES, 0, 6 * l_charIncrement);
        }
    }
}
