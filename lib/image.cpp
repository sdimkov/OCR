#include "image.h"
using namespace std;


namespace ocr {

// Public members:

bool Image::compare_width(Image* a, Image* b)
{
    return ( (a->width > b->width) || ((a->width == b->width) && (a->height > b->height)) );
}

Image::Image()
{
    width = 0; height = 0;
    empty = true; pixel = NULL;
}
Image::Image(string file, const set<uint>& text_colors)
{
    QImage image(QString::fromStdString(file));
    read_image(image, text_colors);
}
Image::Image(QString file, const set<uint>& text_colors)
{
    QImage image(file);
    read_image(image, text_colors);
}
Image::Image(const QImage& image, const set<uint>& text_colors)
{
    read_image(image, text_colors);
}
Image::~Image()
{
    for (int x = 0; x < width; ++x)
        delete[] pixel[x];
    delete[] pixel;
}
bool Image::save(string path)
{
    QRgb color;
    QRgb black = qRgb(0,0,0);
    QRgb white = qRgb(255,255,255);
    QImage qimage(width, height, QImage::Format_ARGB32);
    for(int x=0; x<width; ++x)
        for(int y=0; y<height; ++y) {
            color = pixel[x][y] ? black : white;
            qimage.setPixel(x,y,color);
        }
    return qimage.save(QString::fromStdString(path));
}
Image* Image::invert()
{
    Image* copy = this->copy();
    for(int x=0; x<width; ++x)
        for(int y=0; y<height; ++y)
            copy->pixel[x][y] ^= 1;
    copy->update_edge();
    return copy;
}
Image* Image::trim()
{
    int x = 0;
    int y = 0;
    int w = width - 1;
    int h = height - 1;

    while (x < width && is_empty_column(x)) ++x;
    if (x == width) return copy(0,0,0,0); // return empty image
    while (y < height && is_empty_row(y))    ++y;
    while (w > -1     && is_empty_column(w)) --w;
    while (h > -1     && is_empty_row(h))    --h;

    w = w - x + 1;
    h = h - y + 1;

    return copy(x, y, w, h);
}
Image* Image::copy()
{
    return this->copy(0,0,-1,-1);
}
Image* Image::copy(int x, int y, int w, int h)
{
    w = (w == -1) ? width : w;
    h = (h == -1) ? height : h;
    Image* copy = new Image();
    if((w == 0)||(h == 0)) return copy;
    copy->width = w;
    copy->height = h;
    copy->empty = false;
    copy->pixel = new int*[w];
    for(int i=0; i<w; ++i, ++x) {
        copy->pixel[i] = new int[h];
        memcpy(copy->pixel[i], &pixel[x][y], sizeof(int)*h);
    }
    copy->update_edge();
    return copy;
}
list<Image*> Image::split(int separator_width)
{
    int start = 0;
    int current = 0;
    int empty_space = 0;
    bool found_word = false;
    list<Image*> words;

    if (empty) return words; // If image is empty return an empty list object.

    while (current < width) {
        if (!found_word) {
            if (!is_empty_column(current)) {
                start = current;
                found_word = true;
                empty_space = 0;
            }
        }
        else if (is_empty_column(current)) {
            empty_space++;
            if (empty_space == separator_width) {
                words.push_back(copy(start, 0, current - start - separator_width + 1, height));
                found_word = false;
            }
        }
        else empty_space = 0;
        current++;
    }

    if (found_word) {
        words.push_back(copy(start, 0, current - start, height));
        found_word = false;
    }

    return words;
}
bool Image::is_empty_row(int row)
{
    for (int x = 0; x < width; ++x)
        if (pixel[x][row] == 1) return false;
    return true;
}
bool Image::is_empty_column(int column)
{
    for (int y = 0; y < height; ++y)
        if (pixel[column][y] == 1) return false;
    return true;
}
bool Image::operator ==(Image& img)
{
    if((width != img.width) || (height != img.height)) return false;
    for(int x=0; x<width; ++x)
        for(int y=0; y<height; ++y)
            if(pixel[x][y] != img.pixel[x][y]) return false;
    return true;
}
void Image::display()
{
    string line;
    for(int y=0; y<height; ++y) {
        line = "";
        for(int x=0; x<width; ++x)
            line += (pixel[x][y] == 1) ? "X" : ".";
        cout << line << endl;
    }
}
int Image::area()
{
    return width * height;
}

                                           //  BLACK        YELLOW            values= 4278190080, 4294967040
//const unsigned int Image::TEXT_COLORS[6] = { qRgb(0,0,0), qRgb(255,255,0), qRgb(254,254,0), qRgb(253,253,0), qRgb(10,10,10), qRgb(0,0,127) };
//const unsigned int Image::TEXT_COLORS_COUNT = 6;
const std::set<uint> Image::LANG_COLORS = { qRgb(0,0,0) };


// Private members:

void Image::read_image(const QImage& image, const set<uint>& text_colors)
{
    // get size
    width = image.size().width();
    height = image.size().height();
    empty = false;
    // copy all pixels
    pixel = new int*[width];
    for(int x=0; x<width; ++x) {
        pixel[x] = new int[height];
        for(int y=0; y<height; ++y)
            pixel[x][y] = text_colors.count(image.pixel(x,y)) ? 1 : 0;
    }
    update_edge();
}

void Image::update_edge()
{
    edge = -1;
    for (int y = height - 1; y > -1; y--) {
        if (pixel[0][y] == 1) {
            edge = height - y - 1;
            break;
        }
    }
}

} // namespace OCR
