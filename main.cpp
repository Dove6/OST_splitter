#include <iostream>
#include <fstream>
#include <string>

using namespace std;

struct description {
    int start; //in seconds
    int length; //in seconds
    string title;
    string artist;
    string album;
    int number;
};

void print_help()
{
    cout << "Usage: OST_splitter music_file description_file [music_file2 description_file2...]\n"
            "\tdescription file should contain specifications of demanded output files set in following format:\n"
            "\tstart_time\\title\\artist\\album\n"
            "\t(start time format: MM:SS\n"
            "\tOh, and place correct ffmpeg version in program's location or include it in the env/path variable\n";
}

int main(int argc, char **argv)
{
    if (argc > 1) {
        if (argc % 2 != 0) {
            for (int i = 1; i < argc; i += 2) {
                ifstream desc_file;
                desc_file.open(argv[i + 1]);
                description prev, curr;
                int mins, secs;
                curr.number = 1;
                desc_file >> mins;
                desc_file.ignore(1, ':');
                desc_file >> secs;
                curr.start = mins * 60 + secs;
                desc_file.ignore(1, '\\');
                getline(desc_file, curr.title, '\\');
                getline(desc_file, curr.artist, '\\');
                getline(desc_file, curr.album);

                for (int track = 2; ; track++) {
                    prev = curr;
                    curr.number = track;
                    desc_file >> mins;
                    desc_file.ignore(1, ':');
                    desc_file >> secs;
                    curr.start = mins * 60 + secs;
                    prev.length = curr.start - prev.start - 1;
                    desc_file.ignore(1, '\\');
                    getline(desc_file, curr.title, '\\');
                    getline(desc_file, curr.artist, '\\');
                    getline(desc_file, curr.album);
                    if (!desc_file.eof()) {
                        system(string("ffmpeg -i \"" + string(argv[i]) + "\" -c copy -ss " + to_string(prev.start) + " -t " + \
                                to_string(prev.length) + " -metadata title=\"" + prev.title + "\" -metadata author=\"" + \
                                prev.artist + "\" -metadata album=\"" + prev.album + "\" -metadata track=\"" + \
                                to_string(prev.number) + "\" \"" + prev.title + ".m4a\"").c_str());
                    } else {
                        system(string("ffmpeg -i \"" + string(argv[i]) + "\" -c copy -ss " + to_string(prev.start) + " -t " + \
                                to_string(prev.length) + " -metadata title=\"" + prev.title + "\" -metadata author=\"" + \
                                prev.artist + "\" -metadata album=\"" + prev.album + "\" -metadata track=\"" + \
                                to_string(prev.number) + "\" \"" + prev.title + ".m4a\"").c_str());
                        system(string("ffmpeg -i \"" + string(argv[i]) + "\" -c copy -ss " + to_string(curr.start) + " -t " + \
                                to_string(curr.length) + " -metadata title=\"" + curr.title + "\" -metadata author=\"" + \
                                curr.artist + "\" -metadata album=\"" + curr.album + "\" -metadata track=\"" + \
                                to_string(curr.number) + "\" \"" + curr.title + ".m4a\"").c_str());
                        break;
                    }
                }
                desc_file.close();
            }
        } else {
            print_help();
        }
    } else {
        print_help();
    }
    return 0;
}
