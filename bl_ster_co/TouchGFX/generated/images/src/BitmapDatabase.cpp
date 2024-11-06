// 4.24.1 0x899a45f2
// Generated by imageconverter. Please, do not edit!

#include <images/BitmapDatabase.hpp>
#include <touchgfx/Bitmap.hpp>

extern const unsigned char image_bg[]; // BITMAP_BG_ID = 0, Size: 480x272 pixels
extern const unsigned char image_black_arrow[]; // BITMAP_BLACK_ARROW_ID = 1, Size: 30x17 pixels
extern const unsigned char image_black_arrow_up[]; // BITMAP_BLACK_ARROW_UP_ID = 2, Size: 30x17 pixels
extern const unsigned char image_btn_long[]; // BITMAP_BTN_LONG_ID = 3, Size: 200x56 pixels
extern const unsigned char image_btn_long_pressed[]; // BITMAP_BTN_LONG_PRESSED_ID = 4, Size: 200x56 pixels
extern const unsigned char image_counter_box[]; // BITMAP_COUNTER_BOX_ID = 5, Size: 152x154 pixels
extern const unsigned char image_orange_arrow[]; // BITMAP_ORANGE_ARROW_ID = 6, Size: 30x17 pixels
extern const unsigned char image_orange_arrow_up[]; // BITMAP_ORANGE_ARROW_UP_ID = 7, Size: 30x17 pixels
extern const unsigned char image_small_btn[]; // BITMAP_SMALL_BTN_ID = 8, Size: 100x56 pixels
extern const unsigned char image_small_btn_disable[]; // BITMAP_SMALL_BTN_DISABLE_ID = 9, Size: 100x56 pixels
extern const unsigned char image_small_btn_pressed[]; // BITMAP_SMALL_BTN_PRESSED_ID = 10, Size: 100x56 pixels

const touchgfx::Bitmap::BitmapData bitmap_database[] = {
    { image_bg, 0, 480, 272, 0, 0, 480, ((uint8_t)touchgfx::Bitmap::RGB565) >> 3, 272, ((uint8_t)touchgfx::Bitmap::RGB565) & 0x7 },
    { image_black_arrow, 0, 30, 17, 11, 10, 8, ((uint8_t)touchgfx::Bitmap::ARGB8888) >> 3, 4, ((uint8_t)touchgfx::Bitmap::ARGB8888) & 0x7 },
    { image_black_arrow_up, 0, 30, 17, 11, 3, 8, ((uint8_t)touchgfx::Bitmap::ARGB8888) >> 3, 4, ((uint8_t)touchgfx::Bitmap::ARGB8888) & 0x7 },
    { image_btn_long, 0, 200, 56, 3, 0, 194, ((uint8_t)touchgfx::Bitmap::ARGB8888) >> 3, 55, ((uint8_t)touchgfx::Bitmap::ARGB8888) & 0x7 },
    { image_btn_long_pressed, 0, 200, 56, 3, 0, 194, ((uint8_t)touchgfx::Bitmap::ARGB8888) >> 3, 55, ((uint8_t)touchgfx::Bitmap::ARGB8888) & 0x7 },
    { image_counter_box, 0, 152, 154, 0, 0, 152, ((uint8_t)touchgfx::Bitmap::RGB565) >> 3, 154, ((uint8_t)touchgfx::Bitmap::RGB565) & 0x7 },
    { image_orange_arrow, 0, 30, 17, 11, 10, 8, ((uint8_t)touchgfx::Bitmap::ARGB8888) >> 3, 4, ((uint8_t)touchgfx::Bitmap::ARGB8888) & 0x7 },
    { image_orange_arrow_up, 0, 30, 17, 11, 3, 8, ((uint8_t)touchgfx::Bitmap::ARGB8888) >> 3, 4, ((uint8_t)touchgfx::Bitmap::ARGB8888) & 0x7 },
    { image_small_btn, 0, 100, 56, 3, 0, 95, ((uint8_t)touchgfx::Bitmap::ARGB8888) >> 3, 55, ((uint8_t)touchgfx::Bitmap::ARGB8888) & 0x7 },
    { image_small_btn_disable, 0, 100, 56, 3, 0, 94, ((uint8_t)touchgfx::Bitmap::ARGB8888) >> 3, 55, ((uint8_t)touchgfx::Bitmap::ARGB8888) & 0x7 },
    { image_small_btn_pressed, 0, 100, 56, 3, 0, 94, ((uint8_t)touchgfx::Bitmap::ARGB8888) >> 3, 55, ((uint8_t)touchgfx::Bitmap::ARGB8888) & 0x7 }
};

namespace BitmapDatabase
{
const touchgfx::Bitmap::BitmapData* getInstance()
{
    return bitmap_database;
}

uint16_t getInstanceSize()
{
    return (uint16_t)(sizeof(bitmap_database) / sizeof(touchgfx::Bitmap::BitmapData));
}
} // namespace BitmapDatabase
