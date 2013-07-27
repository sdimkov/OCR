#include "ocr.h"
using namespace std;

namespace ocr {

OCR::OCR()
{
}

void OCR::add_language(string path)
{
    languages.push_back(new Language(path));
}

string OCR::read_text(int x, int y, int w, int h)
{
    QImage qimage = QPixmap::grabWindow(wid, x, y, w, h).toImage();
    //qimage.save("/home/sdimkov/ocr.png");
    return read(qimage).text;
}

string OCR::read_image(string path)
{
    Image image(path, text_colors);
    return read(&image).text;
}

string OCR::Exec(char* cmd)
{
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    string result = "";
    while(!feof(pipe)) {
        if(fgets(buffer, 128, pipe) != NULL)
                result += buffer;
    }
    pclose(pipe);
    return result;
}

Result OCR::read(QImage qimage)
{
    Image image(qimage, text_colors);
    return read(&image);
}
Result OCR::read(Image *image)
{
    Image* t_image = image->trim();
    Result result;

    for(auto language : languages) {
        result = language->read(t_image);
        if(result.success) break;
    }
    if(!result.success) {
        Image* r_image = t_image->invert();
        for(auto language : languages) {
            result = language->read(r_image);
            if(result.success) break;
        }
        delete r_image;
    }

    delete t_image;
    return result;
}

} // namespace OCR
