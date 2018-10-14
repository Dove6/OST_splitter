#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <exception>

using namespace std;

struct description {
    int start; //in seconds
    int length; //in seconds
    string title;
    string artist;
    string album_artist;
    string album;
    string grouping;
    string composer;
    short year;
    int number;
    string comment;
    string genre;
    string copyright;
    string description;
    string synopsis;
    string lyrics;
};

struct {
    string outname_format;
    string description_format;
    unsigned int start_offset;
    unsigned int end_offset;
} details;

void print_help()
{
    cout << "Usage: OST_splitter [options] music_file instruction_file\n\n"
            "OPTIONS\n"
            "Metadata options:\n"
            "     -t\t[artist]\t\tspecify global artist metadata\n"
            "    --artist[=artist]\n"
            "     -b [album-artist]\t\tspecify global album artist metadata\n"
            "    --album-artist[=album-artist]\n"
            "     -a [album]\t\t\tspecify global album metadata\n"
            "    --album[=album]\n"
            "     -G [grouping]\t\tspecify global grouping metadata\n"
            "    --grouping[=grouping]\n"
            "     -c [composer]\t\tspecify global composer metadata\n"
            "    --composer[=composer]\n"
            "     -y [year]\t\t\tspecify global year metadata\n"
            "    --year[=year]\n"
            "     -k number\t\tspecify the first track number\n"
            "    --track-number=number\n"
            "     -m [comment]\t\tspecify global comment metadata\n"
            "    --comment[=comment]\n"
            "     -g [genre]\t\t\tspecify global genre metadata\n"
            "    --genre[=genre]\n"
            "     -r [copyright]\t\tspecify global copyright metadata\n"
            "    --copyright[=copyright]\n"
            "     -d [description]\t\tspecify global description metadata\n"
            "    --description[=description]\n"
            "     -s [synopsis]\t\tspecify global synopsis metadata\n"
            "    --synopsis[=synopsis]\n"
            "     -l [lyrics]\t\tspecify global lyrics metadata\n"
            "    --lyrics[=lyrics]\n"
            "Using a metadata option without an argument instructs program to copy certain\nmetadata from the input music file.\n\n"
            "Format options:\n"
            "     -o\tformat\t\t\toutput files names format (default: \"k. n - a\")\n"
            "    --output=format\n"
            "     -f\tformat\t\t\tdescription file format (default: \"k. n - i\")\n"
            "    --desc-format=format\n"
            "Available output file format escape character:\n"
            "    t    (artist)\n"
            "    b    (album artist)\n"
            "    a    (album)\n"
            "    G    (grouping)\n"
            "    c    (composer)\n"
            "    y    (year)\n"
            "    g    (genre)\n"
            "    k    (track number)\n"
            "    n    (track name)\n"
            "In a description file you can additionally use:\n"
            "    m    (comment)\n"
            "    r    (copyright)\n"
            "    d    (description)\n"
            "    s    (synopsis)\n"
            "    l    (lyrics)\n"
            "    i    (start time)\n\n"
            "Offset options:\n"
            "     -p offset\t\t\toffset in seconds after the start\n"
            "    --offset-pre=offset\n"
            "     -e\toffset\t\t\toffset in second before the end\n"
            "    --offset-post[=offset]\n";
}

int main(int argc, char **argv)
{
    if (argc > 2) {
        if (system("ffmpeg -version") != 0) {
            cerr << "You have to put ffmpeg.exe in your environmental variable or where the program\nis located!";
            return 0;
        }
        ifstream desc_file;
        desc_file.open(argv[argc - 1]);
        if (!desc_file.good()) {
            cerr << "Error opening description file!";
            return 0;
        }
        description global;
        int field_pointer = -1;
        bool inherited;
        try {
            for (int i = 1; i < argc - 2; i++) {
                if (argv[i][0] == '-') {
                    if (strlen(argv[i]) == 2) {
                        switch (argv[i][1]) {
                            case 't': {
                                global.artist = "\032";
                                field_pointer = 0;
                                break;
                            }
                            case 'b': {
                                global.album_artist = "\032";
                                field_pointer = 1;
                                break;
                            }
                            case 'a': {
                                global.album = "\032";
                                field_pointer = 2;
                                break;
                            }
                            case 'G': {
                                global.grouping = "\032";
                                field_pointer = 3;
                                break;
                            }
                            case 'c': {
                                global.composer = "\032";
                                field_pointer = 4;
                                break;
                            }
                            case 'y': {
                                global.year = -032;
                                field_pointer = 5;
                                break;
                            }
                            case 'k': {
                                //TRACK NUMBER
                                global.number = -032;
                                field_pointer = 6;
                                break;
                            }
                            case 'm': {
                                global.comment = "\032";
                                field_pointer = 7;
                                break;
                            }
                            case 'g': {
                                global.genre = "\032";
                                field_pointer = 8;
                                break;
                            }
                            case 'r': {
                                global.copyright = "\032";
                                field_pointer = 9;
                                break;
                            }
                            case 'd': {
                                global.description = "\032";
                                field_pointer = 10;
                                break;
                            }
                            case 's': {
                                global.synopsis = "\032";
                                field_pointer = 11;
                                break;
                            }
                            case 'l': {
                                global.lyrics = "\032";
                                field_pointer = 12;
                                break;
                            }
                            case 'o': {
                                //OUTPUT FORMAT
                                if (i < argc - 3) {
                                    i++;
                                    details.outname_format = argv[i];
                                } else {
                                    throw invalid_argument(argv[i]);
                                }
                                field_pointer = -1;
                                break;
                            }
                            case 'f': {
                                //DESCRIPTION FORMAT
                                if (i < argc - 3) {
                                    i++;
                                    details.description_format = argv[i];
                                } else {
                                    throw invalid_argument(argv[i]);
                                }
                                field_pointer = -1;
                                break;
                            }
                            case 'p': {
                                //START-OFFSET
                                if (i < argc - 3) {
                                    i++;
                                    details.start_offset = stoul(argv[i]);
                                } else {
                                    throw invalid_argument(argv[i]);
                                }
                                field_pointer = -1;
                                break;
                            }
                            case 'e': {
                                //END-OFFSET
                                if (i < argc - 3) {
                                    i++;
                                    details.end_offset = stoul(argv[i]);
                                } else {
                                    throw invalid_argument(argv[i]);
                                }
                                field_pointer = -1;
                                break;
                            }
                            default: {
                                switch (field_pointer) {
                                    case 0: {
                                        global.artist = argv[i];
                                        break;
                                    }
                                    case 1: {
                                        global.album_artist = argv[i];
                                        break;
                                    }
                                    case 2: {
                                        global.album = argv[i];
                                        break;
                                    }
                                    case 3: {
                                        global.grouping = argv[i];
                                        break;
                                    }
                                    case 4: {
                                        global.composer = argv[i];
                                        break;
                                    }
                                    case 5: {
                                        global.year = stoi(argv[i]);
                                        break;
                                    }
                                    case 6: {
                                        //TRACK NUMBER
                                        global.number = stoi(argv[i]);
                                        break;
                                    }
                                    case 7: {
                                        global.comment = argv[i];
                                        break;
                                    }
                                    case 8: {
                                        global.genre = argv[i];
                                        break;
                                    }
                                    case 9: {
                                        global.copyright = argv[i];
                                        break;
                                    }
                                    case 10: {
                                        global.description = argv[i];
                                        break;
                                    }
                                    case 11: {
                                        global.synopsis = argv[i];
                                        break;
                                    }
                                    case 12: {
                                        global.lyrics = argv[i];
                                        break;
                                    }
                                    default: {
                                        throw invalid_argument(argv[i]);
                                    }
                                }
                                field_pointer = -1;
                                break;
                            }
                        }
                    } else if (argv[i][1] == '-') {
                        string long_option(argv[i] + 2);
                        string::size_type eq_mark = long_option.find('=');
                        if (long_option.rfind("artist", 0) == 0) {
                            if (eq_mark != string::npos) {
                                global.artist = long_option.substr(eq_mark + 1);
                            } else {
                                global.artist = "\x1A";
                            }
                            field_pointer = -1;
                        } else if (long_option.rfind("album-artist", 0) == 0) {
                            if (eq_mark != string::npos) {
                                global.album_artist = long_option.substr(eq_mark + 1);
                            } else {
                                global.album_artist = "\x1A";
                            }
                            field_pointer = -1;
                        } else if (long_option.rfind("album", 0) == 0) {
                            if (eq_mark != string::npos) {
                                global.album = long_option.substr(eq_mark + 1);
                            } else {
                                global.album = "\x1A";
                            }
                            field_pointer = -1;
                        } else if (long_option.rfind("grouping", 0) == 0) {
                            if (eq_mark != string::npos) {
                                global.grouping = long_option.substr(eq_mark + 1);
                            } else {
                                global.grouping = "\x1A";
                            }
                            field_pointer = -1;
                        } else if (long_option.rfind("composer", 0) == 0) {
                            if (eq_mark != string::npos) {
                                global.composer = long_option.substr(eq_mark + 1);
                            } else {
                                global.composer = "\x1A";
                            }
                            field_pointer = -1;
                        } else if (long_option.rfind("year", 0) == 0) {
                            if (eq_mark != string::npos) {
                                global.year = stoi(long_option.substr(eq_mark + 1));
                            } else {
                                global.year = 0x1A;
                            }
                            field_pointer = -1;
                        } else if (long_option.rfind("track-number", 0) == 0) {
                            //TRACK NUMBER
                            if (eq_mark != string::npos) {
                                global.number = stoi(long_option.substr(eq_mark + 1));
                            } else {
                                global.number = 1;
                            }
                            field_pointer = -1;
                        } else if (long_option.rfind("comment", 0) == 0) {
                            if (eq_mark != string::npos) {
                                global.comment = long_option.substr(eq_mark + 1);
                            } else {
                                global.comment = "\x1A";
                            }
                            field_pointer = -1;
                        } else if (long_option.rfind("genre", 0) == 0) {
                            if (eq_mark != string::npos) {
                                global.genre = long_option.substr(eq_mark + 1);
                            } else {
                                global.genre = "\x1A";
                            }
                            field_pointer = -1;
                        } else if (long_option.rfind("copyright", 0) == 0) {
                            if (eq_mark != string::npos) {
                                global.copyright = long_option.substr(eq_mark + 1);
                            } else {
                                global.copyright = "\x1A";
                            }
                            field_pointer = -1;
                        } else if (long_option.rfind("description", 0) == 0) {
                            if (eq_mark != string::npos) {
                                global.description = long_option.substr(eq_mark + 1);
                            } else {
                                global.description = "\x1A";
                            }
                            field_pointer = -1;
                        } else if (long_option.rfind("synopsis", 0) == 0) {
                            if (eq_mark != string::npos) {
                                global.synopsis = long_option.substr(eq_mark + 1);
                            } else {
                                global.synopsis = "\x1A";
                            }
                            field_pointer = -1;
                        } else if (long_option.rfind("lyrics", 0) == 0) {
                            if (eq_mark != string::npos) {
                                global.lyrics = long_option.substr(eq_mark + 1);
                            } else {
                                global.lyrics = "\x1A";
                            }
                            field_pointer = -1;
                        } else if (long_option.rfind("output", 0) == 0) {
                            //OUTPUT FORMAT
                            field_pointer = -1;
                        } else if (long_option.rfind("desc-format", 0) == 0) {
                            //DESCRIPTION FORMAT
                            field_pointer = -1;
                        } else if (long_option.rfind("offset-pre", 0) == 0) {
                            //START-OFFSET
                            field_pointer = -1;
                        } else if (long_option.rfind("offset-post", 0) == 0) {
                            //END-OFFSET
                            field_pointer = -1;
                        } else {
                            switch (field_pointer) {
                                case 0: {
                                    global.artist = argv[i];
                                    break;
                                }
                                case 1: {
                                    global.album_artist = argv[i];
                                    break;
                                }
                                case 2: {
                                    global.album = argv[i];
                                    break;
                                }
                                case 3: {
                                    global.grouping = argv[i];
                                    break;
                                }
                                case 4: {
                                    global.composer = argv[i];
                                    break;
                                }
                                case 5: {
                                    global.year = stoi(argv[i]);
                                    break;
                                }
                                case 6: {
                                    //TRACK NUMBER
                                    global.number = stoi(argv[i]);
                                    break;
                                }
                                case 7: {
                                    global.comment = argv[i];
                                    break;
                                }
                                case 8: {
                                    global.genre = argv[i];
                                    break;
                                }
                                case 9: {
                                    global.copyright = argv[i];
                                    break;
                                }
                                case 10: {
                                    global.description = argv[i];
                                    break;
                                }
                                case 11: {
                                    global.synopsis = argv[i];
                                    break;
                                }
                                case 12: {
                                    global.lyrics = argv[i];
                                    break;
                                }
                                case 13: {
                                    //OUTPUT FORMAT
                                    break;
                                }
                                case 14: {
                                    //DESCRIPTION FORMAT
                                    break;
                                }
                                case 15: {
                                    //START-
                                    break;
                                }
                                case 16: {
                                    //END-OFFSET
                                    break;
                                }
                                default: {
                                    cerr << "Wrong argument: " << argv[i];
                                    return 0;
                                }
                            }
                            field_pointer = -1;
                        }
                    }
                }
            }
        } catch (invalid_argument exc) {
            cerr << "Wrong argument: " << exc.what();
            return 0;
        }
        for (int i = 1; i < argc; i += 2) {
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
    return 0;
}
