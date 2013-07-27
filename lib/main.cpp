#pragma once

#include <iostream>
#include "ocr.h"
#include <QtGui/QApplication>
#include <QPixmap>
#include <QDesktopWidget>
#include <QTime>
#include <iomanip>
#include <set>
#include <stdio.h>

using namespace std;
using namespace ocr;


//  C LIB ..

int a1=0;
char** a2;
QApplication app(a1,a2);

extern "C"
{
    OCR*        ocr_create();
    void        ocr_delete(OCR* obj);
    int         ocr_get_window(OCR* obj);
    void        ocr_set_window(OCR* obj, int wid);
    void        ocr_add_language(OCR* obj, char* path);
    void        ocr_add_color(OCR* obj, int r, int g, int b);
    void        ocr_remove_color(OCR* obj, int r, int g, int b);
    void        ocr_remove_all_colors(OCR* obj);
    const char* ocr_read_text(OCR* obj, int x, int y, int w, int h);
    const char* ocr_read_image(OCR* obj, char* path);

    void img_screenshot(char* file, int wid);
    void img_screenshot_area(char* file, int wid, int x, int y, int w, int h);
    void img_draw_rect(char *file, int x, int y, int w, int h, int r, int g, int b);
}

OCR*        ocr_create()                                        { return new OCR(); }
void        ocr_delete(OCR* obj)                                { delete obj; }
int         ocr_get_window(OCR* obj)                            { return obj->wid; }
void        ocr_set_window(OCR* obj, int wid)                   { obj->wid=wid; }
void        ocr_add_language(OCR* obj, char* path)              { obj->add_language(path); }
void        ocr_add_color(OCR* obj, int r, int g, int b)        { obj->text_colors.insert(qRgb(r,g,b)); }
void        ocr_remove_color(OCR* obj, int r, int g, int b)     { obj->text_colors.erase(qRgb(r,g,b)); }
void        ocr_remove_all_colors(OCR* obj)                     { obj->text_colors.clear(); }
const char* ocr_read_text(OCR* obj, int x, int y, int w, int h) { return obj->read_text(x,y,w,h).c_str(); }
const char* ocr_read_image(OCR* obj, char* path)                { return obj->read_image(path).c_str(); }

void img_screenshot(char* file, int wid)
{
    QPixmap::grabWindow(wid).toImage().save(file);
}
void img_screenshot_area(char* file, int wid, int x, int y, int w, int h)
{
    QPixmap::grabWindow(wid, x, y, w, h).toImage().save(file);
}
void img_draw_rect(char* file, int x, int y, int w, int h, int r, int g, int b)
{
    QImage image(QString::fromStdString(file));
    QRgb color = qRgb(r,g,b);
    for(int i=x+w, y1=y, y2=y+h, y3=y+1, y4=y+h-1;  i>=x;  i--)
    {
        image.setPixel(i, y1, color);
        image.setPixel(i, y2, color);
        image.setPixel(i, y3, color);
        image.setPixel(i, y4, color);
    }
    for(int j=y+h, x1=x, x2=x+w, x3=x+1, x4=x+w-1;  j>=y;  j--)
    {
        image.setPixel(x1, j, color);
        image.setPixel(x2, j, color);
        image.setPixel(x3, j, color);
        image.setPixel(x4, j, color);
    }
    image.save(QString::fromStdString(file));
}











/*
  CODE FOR EXTRACTING CHARS
  !!!

int main(int argc, char *argv[])
{
    ocr::Image* img;
    ocr::Image* t_img;
    ocr::Image* r_img;
    list<ocr::Image*> chars, t_chars;

    for(int i=1; i<argc; i++) {

        img = new ocr::Image((string)argv[i]);
        t_img = img->Trim();

        chars = t_img->Split(1);
        for(auto img : chars) t_chars.push_back(img->Trim());

        delete img;
        delete t_img;
    }

    //t_chars.sort(ocr::Image::compare_width);
    //t_chars.unique();

    for(auto it1 = t_chars.begin(); it1 != t_chars.end(); it1++)
        for(auto it2 = t_chars.begin(); it2 != t_chars.end(); it2++)
            if( (it1 != it2) && (*it1 != NULL) && (*it2 != NULL) )
                if( (**it1) == (**it2) ) {
                    delete *it2;
                    *it2 = NULL;
                }

    int i=1;
    for(auto img : t_chars) if(img) img->Save("chars/"+to_string(i++)+".png");

    cout << "Extracted " << i-1 << " unique char(s)." << endl;

    return 0;
}
*/










/* CODE FOR Testing OCR */

/*
int main(int argc, char *argv[])
{
    int a1=0;
    char **a2;
    QApplication app(a1,a2);
    string text, lang_path = "/home/sdimkov/Projects/ApplicationDriver/lang/";
    OCR driver;

    for (int i=1; i<argc; i++) driver.AddOCRLang(argv[i]);

    //driver.AddOCRLang(lang_path+"pos1/");
    //driver.AddOCRLang(lang_path+"pos2/");
    //driver.AddOCRLang(lang_path+"pos3/");
    //driver.AddOCRLang(lang_path+"pos4/");
    driver.text_colors = { qRgb(0,0,0),      // BLACK
                      //   qRgb(255,255,0),  // YELLOW
                      //   qRgb(254,254,0),  // YELLOW
                      //   qRgb(253,253,0),  // YELLOW
                           qRgb(8,8,8),      // BLACK
                           qRgb(10,10,10),   // BLACK
                           qRgb(0,0,82),     // BLUE ?
                           qRgb(0,0,123),    // BLUE
                           qRgb(0,0,127) };  // BLUE


//    for(int i=1; i<argc; i++)  {
//        timer.restart();
//        text = driver.ReadImage(argv[i]);
//        time = timer.elapsed();
//        cout << setw(4) << time << "  " << text << endl;
//    }

    int x,y,w,h;
    cin >> driver.wid;

    while (true)
    {
        cin >> x >> y >> w >> h;
        cout << driver.GetText(x,y,w,h) << endl;
    }

    return 0;
}

*/
