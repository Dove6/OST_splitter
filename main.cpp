#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <exception>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

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
            "     -k number\t\tspecify the first track number (default: 1)\n"
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

struct char_int {
    char c;
    int i;
    char_int(char c, int i)
        : c(c), i(i) {}
};

struct string_int {
    string s;
    int i;
    string_int(string s, int i)
        : s(s), i(i) {}
};

struct {
    const map<const string, const char_int> meta_long(const vector<string> &metadata, const vector<string> &options)
    {
        const vector<string>::size_type md_size = metadata.size();
        const vector<string>::size_type opt_size = options.size();
        map<const string, const char_int> m;
        for (unsigned i = 0; i < md_size - 1; i++) {
            m.insert(make_pair(metadata[i], char_int(metadata.back()[i], i)));
        }
        for (unsigned i = 0; i < opt_size - 1; i++) {
            m.insert(make_pair(options[i], char_int(options.back()[i], i)));
        }
        return m;
    }
    const map<const char, const string_int> meta_token(const vector<string> &metadata, const vector<string> &options)
    {
        const vector<string>::size_type md_size = metadata.size();
        const vector<string>::size_type opt_size = options.size();
        map<const char, const string_int> m;
        for (unsigned i = 0; i < md_size - 2; i++) {
            m.insert(make_pair(metadata.back()[i], string_int(metadata[i], i)));
        }
        for (unsigned i = 0; i < opt_size - 1; i++) {
            m.insert(make_pair(options.back()[i], string_int(options[i], i)));
        }
        return m;
    }
} init;

int main(int argc, char **argv)
{
    const vector<string> metadata = {"artist",
                                     "album-artist",
                                     "album",
                                     "grouping",
                                     "composer",
                                     "year",
                                     "track-number",
                                     "comment",
                                     "genre",
                                     "copyright",
                                     "description",
                                     "synopsis",
                                     "lyrics",
                                     "title",
                                     "start-time",
                                     "length",
                                     "tbaGcykmgrdslni\0"}; //one-letter command-line options
    const vector<string> options = {"output", //output file name format
                                    "desc-format", //description format
                                    "offset-pre", //offset before each track (in seconds)
                                    "offset-post", //offset after the end of each track (in seconds)}
                                    "ofpe"};
    const map<const string, const char_int> meta_long = init.meta_long(metadata, options);
    const map<const char, const string_int> meta_token = init.meta_token(metadata, options);
    map<string, string> global, prev, curr;
    for (unsigned i = 0; i < metadata.size() - 3; i++) {
        global.insert(make_pair(metadata[i], ""));
        prev.insert(make_pair(metadata[i], ""));
        curr.insert(make_pair(metadata[i], ""));
    }
    for (unsigned i = metadata.size() - 3; i < metadata.size() - 1; i++) {
        prev.insert(make_pair(metadata[i], ""));
        curr.insert(make_pair(metadata[i], ""));
    }
    for (unsigned i = 0; i < options.size() - 1; i++) {
        global.insert(make_pair(options[i], ""));
    }
    unsigned int track_number = 1;
    global["output"] = "k. n - a";
    global["desc-format"] = "k. n - i";

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
        int open_token = -1;
        try {
            for (int i = 1; i < argc - 2; i++) {
                if (argv[i][0] == '-') {
                    if (strlen(argv[i]) == 2) {
                        if (metadata.back().find(argv[i][1]) != string::npos) {
                            global[string(1, argv[i][1])] = "\032";
                            open_token = meta_token.at(argv[i][1]).i;
                        } else {
                            string::size_type opt_pos = options.back().find(argv[i][1]);
                            if (opt_pos != string::npos) {
                                if (i < argc - 3) {
                                    global[options[opt_pos]] = argv[++i];
                                } else {
                                    throw invalid_argument(argv[i]);
                                }
                            } else {
                                if (open_token != -1) {
                                    global[metadata[open_token]] = argv[i];
                                } else {
                                    throw invalid_argument(argv[i]);
                                }
                            }
                            open_token = -1;
                        }
                    } else if (argv[i][1] == '-') {
                        string long_option(argv[i] + 2);
                        string::size_type eq_mark = long_option.find('=');
                        bool string_hit = false;
                        for (unsigned i = 0; i < metadata.size() - 1; i++) {
                            if (long_option.rfind(metadata[i], 0) == 0) {
                                string_hit = true;
                                if (eq_mark != string::npos) {
                                    global[metadata[i]] = long_option.substr(eq_mark + 1);
                                } else {
                                    global[metadata[i]] = "\032";
                                }
                                open_token = -1;
                            }
                        }
                        if (!string_hit) {
                            for (unsigned i = 0; i < options.size() - 1; i++) {
                                if (long_option.rfind(options[i], 0) == 0) {
                                    string_hit = true;
                                    if (eq_mark != string::npos) {
                                        global[options[i]] = long_option.substr(eq_mark + 1);
                                    } else {
                                        global[options[i]] = "\032";
                                    }
                                    open_token = -1;
                                }
                            }
                            if (!string_hit) {
                                if (open_token != -1) {
                                    global[metadata[open_token]] = argv[i];
                                } else {
                                    throw invalid_argument(argv[i]);
                                }
                                open_token = -1;
                            }
                        }
                    } else {
                        if (open_token != -1) {
                            global[metadata[open_token]] = argv[i];
                        } else {
                            throw invalid_argument(argv[i]);
                        }
                        open_token = -1;
                    }
                } else {
                    if (open_token != -1) {
                        global[metadata[open_token]] = argv[i];
                    } else {
                        throw invalid_argument(argv[i]);
                    }
                    open_token = -1;
                }
            }
        } catch (invalid_argument exc) {
            cerr << "Wrong argument: " << exc.what();
            return 0;
        }

        cout << "global:\n"
            "\ttitle=" << global["title"] << "\n"
            "\tartist=" << global["artist"] << "\n"
            "\talbum-artist=" << global["album-artist"] << "\n"
            "\talbum=" << global["album"] << "\n"
            "\tgrouping=" << global["grouping"] << "\n"
            "\tcomposer=" << global["composer"] << "\n"
            "\tyear=" << global["year"] << "\n"
            "\tnumber=" << global["track-number"] << "\n"
            "\tcomment=" << global["comment"] << "\n"
            "\tgenre=" << global["genre"] << "\n"
            "\tcopyright=" << global["copyright"] << "\n"
            "\tdescription=" << global["description"] << "\n"
            "\tsynopsis=" << global["synopsis"] << "\n"
            "\tlyrics=" << global["lyrics"] << "\n"
            "\toutput=" << global["output"] << "\n"
            "\tdesc-format=" << global["desc-format"] << "\n"
            "\toffset-pre=" << global["offset-pre"] << "\n"
            "\toffset-post=" << global["offset-post"] << "\n";

        cout << "\nTest #1 zakonczony\n\n";

        for (unsigned i = 0; i < metadata.back().size() - 1; i++) {
            string::difference_type cnt = count(global["desc-format"].begin(), global["desc-format"].end(), metadata.back().at(i));
            if (cnt > 1) {
                cerr << "Invalid argument: " << global["desc-format"] << endl;
                return 0;
            }
        }
        vector<char> desc_order;
        vector<string> desc_pre;
        desc_order.reserve(15);
        for (unsigned i = 0; i < global["desc-format"].size();) {
            string::size_type token_pos = global["desc-format"].find_first_of(metadata.back(), i);
            if (token_pos != string::npos) {
                desc_order.push_back(global["desc-format"][token_pos]);
                desc_pre.push_back(global["desc-format"].substr(i, token_pos - i));
                i = token_pos + 1;
            } else {
                i += global["desc-format"].size();
            }
        }
        cout << "desc-format includes:" << endl;
        for (unsigned i = 0; i < desc_order.size(); i++) {
            cout << "\t" << meta_token.at(desc_order[i]).s << " preceded by: \"" << desc_pre[i] << "\"" << endl;
        }

        cout << "\nTest #2 zakonczony" << endl;
        return 0;

        /*for (int i = 1; i < argc; i += 2) {
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
        }*/
    } else {
        print_help();
    }
    return 0;
}
