#ifndef IMAGE_H
#define IMAGE_H

#include <iostream>
#include <string>
#include <list>
#include <set>
#include <QImage>


namespace ocr {

class Image
{
public:
    Image();                                                       // Create empty Image object
    Image(std::string file, const std::set<uint>& text_colors);    // Create from image file (PNG/JPG)
    Image(QString file, const std::set<uint>& text_colors);
    Image(const QImage& image, const std::set<uint>& text_colors); // Create from QImage object
    ~Image();

    int get_edge();
    Image* invert();                              // Returns new inverted image (uses Copy())
    Image* trim();                                // Returns new trimmed image (uses Copy())
    Image* copy();                                // Returns a full copy of the image (dynamically allocated)
    Image* copy(int x, int y, int w, int h);      // Returns a partial copy of the image (dynamically allocated)
    std::list<Image*> split(int separator_width); // Split image based on space between chars
    void display();                               // Display the image in readable way
    bool save(std::string path);
    bool is_empty_row(int row);
    bool is_empty_column(int column);
    bool operator==(Image& img);
    int area();

    int width, height;
    int** pixel;
    int vert_offset;
    std::string text;
    bool empty;

    //static const unsigned int TEXT_COLORS[];
    //static const unsigned int TEXT_COLORS_COUNT;
    static const std::set<uint> LANG_COLORS;  // just for getting lang chars (in black and white) TEMP

    static bool compare_width(Image* a, Image* b);
private:
    void read_image(const QImage& image, const std::set<uint>& text_colors);
};

} // namespace OCR

#endif // IMAGE_H
