#ifndef OCR_LANGUAGE_H
#define OCR_LANGUAGE_H


#include "image.h"
#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <QDirIterator>


namespace ocr {

struct Result
{
    Result() { success = false; text = ""; accuracy = 0; }
    bool success;
    std::string text;
    float accuracy;
};

class Language
{
public:
    Language(std::string path);
    ~Language();

    Result read(Image* image);
    Result read(std::string file, std::set<uint>& text_colors);
    void display();

    std::string name;

private:
    float merge(Image* ch, Image* word, int left_offset);

    std::list<Image*> chars;
    int min_space;
    int max_kerning;
    float min_accuracy;

};

} // namespace OCR

#endif // OCR_LANGUAGE_H
