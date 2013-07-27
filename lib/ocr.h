#ifndef OCR_H
#define OCR_H

#include "image.h"
#include "language.h"
#include <iostream>
#include <string>
#include <list>
#include <QPixmap>

namespace ocr {

class OCR
{
public:
    OCR();

    void add_language(std::string path);
    std::string read_text(int x, int y, int w, int h);
    std::string read_image(std::string image);

    std::list<ocr::Language*> languages;
    std::set<uint> text_colors;
    WId wid;

private:
    Result read(QImage image);
    Result read(Image* image);
    std::string Exec(char* cmd);
};

} // namespace OCR

#endif // OCR_H
