#include "language.h"
using namespace std;


namespace ocr {

Language::Language(string path)
{
    string line, meaning, filename, vert_offset;
    ifstream file;
    string variable, value;

    file.open((path+"/lang.txt").c_str());
    if(!file.is_open()) throw; // Add exception..

    getline(file, variable, '='); if(variable != "name") throw;
    getline(file, value, '\n'); name = value;

    getline(file, variable, '='); if(variable != "min_space") throw;
    getline(file, value, '\n'); min_space = atoi(value.c_str());

    getline(file, variable, '='); if(variable != "min_accuracy") throw;
    getline(file, value, '\n'); min_accuracy = atof(value.c_str())/100.00;

    getline(file, variable, '='); if(variable != "max_kerning") throw;
    getline(file, value, '\n'); max_kerning = atoi(value.c_str());

    QDirIterator dir(QString::fromStdString(path), QDir::Dirs, QDirIterator::Subdirectories);
    path = dir.path().toStdString();
    do {
        if(path[path.size()-1] != '.') {
            file.open((path+"/chars.txt").c_str());
            if(file.is_open()) {
                getline(file, line); // skip first line
                while(file.good()) {
                    getline(file, meaning, '\t');
                    getline(file, filename, '\t');
                    getline(file, vert_offset, '\n');
                    if(filename == "") filename = meaning + ".png";
                    if(vert_offset == "") vert_offset = "0";
                    chars.push_back(new Image(path+"/"+filename,Image::LANG_COLORS));
                    chars.back()->text = meaning;
                    chars.back()->vert_offset = atoi(vert_offset.c_str());
                }
                file.close();
            }
        }
        path = dir.next().toStdString();
    } while (dir.hasNext());

    chars.sort(Image::compare_width);
}
Language::~Language()
{
}

void Language::display()
{
    for(list<Image*>::iterator it=chars.begin(); it!=chars.end(); ++it) {
        (*it)->display();
        cout << "<[ " << (*it)->text << " "
             << (*it)->width << "x" << (*it)->height << " "
             << (*it)->vert_offset << " ]>" << endl << endl;
    }
    cout << "lang_name=" << name << endl;
    cout << "min_space=" << min_space << endl;
    cout << "min_accuracy=" << min_accuracy << endl;
    cout << "chars_count=" << chars.size() << endl;
}

Result Language::read(string file, std::set<uint>& text_colors)
{
    QImage image(QString::fromStdString(file));
    Image img(image, text_colors);
    return read(&img);
}

Result Language::read(Image* image)
{
    Result result;
    Image* matched_char = NULL;
    int space, left_offset = 0;
    float accuracy, matched_accuracy = 0;

    for(Image* ch : chars) {
        if ((matched_char != NULL) && (matched_char->width > ch->width)) break;
        image->vert_offset = ch->vert_offset + ch->edge - image->edge;
        accuracy = merge(ch, image, left_offset);
        if (accuracy >= min_accuracy) {
            if ((matched_char == NULL) || (matched_accuracy < accuracy)) {
                matched_char = ch;
                matched_accuracy = accuracy;
            }
        }
    }
    if (matched_char == NULL) return result;
    image->vert_offset = matched_char->vert_offset + matched_char->edge - image->edge;
    image->text = matched_char->text; //cout << matched_char->text;
    left_offset = matched_char->width;

    while (left_offset < image->width) {
        space = 0;
        while (image->is_empty_column(left_offset)) {
            left_offset++; space++;
            if (left_offset == image->width) return result;
        }
        if (space >= min_space) image->text += ' ';

        matched_char = NULL;
        matched_accuracy = 0;
        for(Image* ch : chars) {
            if ((matched_char != NULL) && (matched_char->width > ch->width)) break;
            accuracy = merge(ch, image, left_offset);
            if (accuracy >= min_accuracy) {
                if ((matched_char == NULL) || (matched_accuracy < accuracy)) {
                    matched_char = ch;
                    matched_accuracy = accuracy;
                }
            }
        }
        if((max_kerning > 0) && (matched_char == NULL)) {
            left_offset--;
            for(Image* ch : chars) {
                if ((matched_char != NULL) && (matched_char->width > ch->width)) break;
                accuracy = merge(ch, image, left_offset);
                if (accuracy >= min_accuracy) {
                    if ((matched_char == NULL) || (matched_accuracy < accuracy)) {
                        matched_char = ch;
                        matched_accuracy = accuracy;
                    }
                }
            }
        }
        if (matched_char == NULL) return result;
        image->text += matched_char->text; //cout << matched_char->text;
        left_offset += matched_char->width;
    }

    result.success = true;
    result.text = image->text;
    return result;
}

float Language::merge(Image *ch, Image *img, int left_offset)
{
    int ch_x, ch_y;
    int img_x, img_y;
    int wrong_pixels = 0;
    int vert_offset = ch->vert_offset - img->vert_offset;

    if ((img->height + img->vert_offset < ch->height + ch->vert_offset) ||
        (img->vert_offset > ch->vert_offset)) {
        return 0;
    }

    for (ch_x = 0; (ch_x < ch->width) && (ch_x < img->width - left_offset); ++ch_x) {
        img_x = ch_x + left_offset;
        img_y = 0;
        ch_y  = ch->height + vert_offset - img->height;
        for (; ch_y < 0; img_y++, ch_y++)
            if (img->pixel[img_x][img_y] != 0) wrong_pixels++;
        for (; ch_y < ch->height; img_y++, ch_y++)
            if (ch->pixel[ch_x][ch_y] != img->pixel[img_x][img_y]) wrong_pixels++;
        for (; img_y < img->height; img_y++)
            if (img->pixel[img_x][img_y] != 0) wrong_pixels++;
    }

    if (img->width - left_offset < ch->width) {
        for (; ch_x < ch->width; ++ch_x)
            for (img_y = 0; img_y < ch->height; ++img_y)
                if (ch->pixel[ch_x][img_y] != 0) wrong_pixels++;
    }

    //float result = ((float)(ch->Area() - wrong_pixels) / (float)ch->Area());
    //if(result>0.8) cout << ch->text << " " << result << endl;
    return ((float)(ch->area() - wrong_pixels) / (float)ch->area());
}

} // namespace OCR
