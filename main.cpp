#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <exception>
#include <vector>
#include <map>
#include <algorithm>
#include <windows.h>

using namespace std;

wstring u8_to_u16(const string &u8)
{
    int buffer_size = MultiByteToWideChar(CP_UTF8, 0, u8.c_str(), -1, NULL, 0);
    wchar_t buffer[buffer_size];
    MultiByteToWideChar(CP_UTF8, 0, u8.c_str(), -1, buffer, buffer_size);
    wstring u16(buffer);
    return u16;
}

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
            "By default, using a metadata option without an argument instructs program to\n"
            "leave it empty and not using a metadata option results in copying it from the\n"
            "source music file.\n"
            "By setting a special option:\n"
            "     -x\n"
            "you can reverse this behavior: using a metadata option without an argument will\n"
            "make program copy it from the source music file and not using a metadata\n"
            "option will leave it empty.\n\n"
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
    map<string, string> global;
    bool copy_by_default = true;
    for (int i = 0; i < argc; i++) {
        if (argv[i][0] == '-' && argv[i][1] == 'x' && strlen(argv[i]) == 2) {
            copy_by_default = false;
        }
    }
    for (unsigned i = 0; i < metadata.size() - 3; i++) {
        if (copy_by_default) {
            global.insert(make_pair(metadata[i], "\032"));
        } else {
            global.insert(make_pair(metadata[i], ""));
        }
    }
    for (unsigned i = 0; i < options.size() - 1; i++) {
        global.insert(make_pair(options[i], ""));
    }
    global.at("output") = "k. n - a";
    global.at("desc-format") = "k. n - i";

    //TODO: more exceptions
    if (argc > 2) {
        if (system("ffmpeg -version >NUL 2>&1") != 0) {
            cerr << "You have to put ffmpeg.exe in your environmental variable or where the program\nis located!";
            return 0;
        }
        {
            ifstream mus_file;
            mus_file.open(argv[argc - 2]);
            if (!mus_file.good()) {
                cerr << "Error opening audio file!";
                return 0;
            }
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
                        if (argv[i][1] != 'x') {
                            if (metadata.back().find(argv[i][1]) != string::npos) {
                                if (copy_by_default) {
                                    global.at(meta_token.at(argv[i][1]).s) = "";
                                } else {
                                    global.at(meta_token.at(argv[i][1]).s) = "\032";
                                }
                                open_token = meta_token.at(argv[i][1]).i;
                            } else {
                                string::size_type opt_pos = options.back().find(argv[i][1]);
                                if (opt_pos != string::npos) {
                                    if (i < argc - 3) {
                                        global.at(options[opt_pos]) = argv[++i];
                                    } else {
                                        throw invalid_argument(argv[i]);
                                    }
                                } else {
                                    if (open_token != -1) {
                                        global.at(metadata[open_token]) = argv[i];
                                    } else {
                                        throw invalid_argument(argv[i]);
                                    }
                                }
                                open_token = -1;
                            }
                        }
                    } else if (argv[i][1] == '-') {
                        string long_option(argv[i] + 2);
                        string::size_type eq_mark = long_option.find('=');
                        bool string_hit = false;
                        for (unsigned i = 0; i < metadata.size() - 1; i++) {
                            if (long_option.rfind(metadata[i], 0) == 0) {
                                string_hit = true;
                                if (eq_mark != string::npos) {
                                    global.at(metadata[i]) = long_option.substr(eq_mark + 1);
                                } else {
                                    if (copy_by_default) {
                                        global.at(metadata[i]) = "";
                                    } else {
                                        global.at(metadata[i]) = "\032";
                                    }
                                }
                                open_token = -1;
                            }
                        }
                        if (!string_hit) {
                            for (unsigned i = 0; i < options.size() - 1; i++) {
                                if (long_option.rfind(options[i], 0) == 0) {
                                    string_hit = true;
                                    if (eq_mark != string::npos) {
                                        global.at(options[i]) = long_option.substr(eq_mark + 1);
                                    } else {
                                        if (copy_by_default) {
                                            global.at(options[i]) = "";
                                        } else {
                                            global.at(options[i]) = "\032";
                                        }
                                    }
                                    open_token = -1;
                                }
                            }
                            if (!string_hit) {
                                if (open_token != -1) {
                                    global.at(metadata[open_token]) = argv[i];
                                } else {
                                    throw invalid_argument(argv[i]);
                                }
                                open_token = -1;
                            }
                        }
                    } else {
                        if (open_token != -1) {
                            global.at(metadata[open_token]) = argv[i];
                        } else {
                            throw invalid_argument(argv[i]);
                        }
                        open_token = -1;
                    }
                } else {
                    if (open_token != -1) {
                        global.at(metadata[open_token]) = argv[i];
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

        /*if (global.at("desc-format") != "") {
            unsigned i = global.at("desc-format").find('\0');
            for (; i != string::npos; i = global.at("desc-format").find('\0', i + 1)) {
                global.at("desc-format").erase(i);
            }
        }*/

        /*cout << "global:\n"
            "\ttitle=" << global.at("title") << "\n"
            "\tartist=" << global.at("artist") << "\n"
            "\talbum-artist=" << global.at("album-artist") << "\n"
            "\talbum=" << global.at("album") << "\n"
            "\tgrouping=" << global.at("grouping") << "\n"
            "\tcomposer=" << global.at("composer") << "\n"
            "\tyear=" << global.at("year") << "\n"
            "\ttrack-number=" << global.at("track-number") << "\n"
            "\tcomment=" << global.at("comment") << "\n"
            "\tgenre=" << global.at("genre") << "\n"
            "\tcopyright=" << global.at("copyright") << "\n"
            "\tdescription=" << global.at("description") << "\n"
            "\tsynopsis=" << global.at("synopsis") << "\n"
            "\tlyrics=" << global.at("lyrics") << "\n"
            "\toutput=" << global.at("output") << "\n"
            "\tdesc-format=" << global.at("desc-format") << "\n"
            "\toffset-pre=" << global.at("offset-pre") << "\n"
            "\toffset-post=" << global.at("offset-post") << "\n";

        cout << "\nTest #1 zakonczony\n\n";*/

        for (unsigned i = 0; i < metadata.back().size() - 1; i++) {
            string::difference_type cnt = count(global.at("desc-format").begin(), global.at("desc-format").end(), metadata.back().at(i));
            if (cnt > 1) {
                cerr << "Invalid argument: " << global.at("desc-format") << endl;
                return 0;
            }
        }

        vector<char> desc_order;
        vector<string> desc_pre;
        desc_order.reserve(15);
        desc_pre.reserve(16);
        {
            unsigned i = 0;
            string::size_type token_pos;
            while ((token_pos = global.at("desc-format").find_first_of(metadata.back(), i)) != string::npos) {
                desc_order.push_back(global.at("desc-format")[token_pos]);
                desc_pre.push_back(global.at("desc-format").substr(i, token_pos - i));
                i = token_pos + 1;
            }
            desc_pre.push_back(global.at("desc-format").substr(i));
            desc_pre.back() += '\n';
        }

        {
            string::difference_type cnt = count(desc_pre.begin() + 1, desc_pre.end() - 1, "");
            if (cnt > 0) {
                cerr << "Invalid argument: " << global.at("desc-format") << endl;
                cerr << "Please just separate the escape characters and let me sleep tonight ;-;" << endl;
                return 0;
            }
        }

        /*cout << "desc-format includes:" << endl;
        for (unsigned i = 0; i < desc_order.size(); i++) {
            cout << "\t" << meta_token.at(desc_order[i]).s << " preceded by: \"" << desc_pre[i] << "\"" << endl;
        }

        cout << "\nTest #2 zakonczony\n\n";*/

        map<string, string> prev, curr;
        for (unsigned i = 0; i < metadata.size() - 3; i++) {
            curr.insert(make_pair(metadata[i], global.at(metadata[i])));
        }
        for (unsigned i = metadata.size() - 3; i < metadata.size() - 1; i++) {
            curr.insert(make_pair(metadata[i], ""));
        }

        ///OUTPUT FILENAME FORMAT
        vector<char> fout_order;
        vector<string> fout_pre;
        fout_order.reserve(15);
        fout_pre.reserve(16);
        {
            unsigned i = 0;
            string::size_type token_pos;
            while ((token_pos = global.at("output").find_first_of(metadata.back(), i)) != string::npos) {
                fout_order.push_back(global.at("output")[token_pos]);
                fout_pre.push_back(global.at("output").substr(i, token_pos - i));
                i = token_pos + 1;
            }
            fout_pre.push_back(global.at("output").substr(i));
            fout_pre.back() += '\n';
            i = token_pos + 1;
        }

        {
            string::difference_type cnt = count(fout_pre.begin() + 1, fout_pre.end() - 1, "");
            if (cnt > 0) {
                cerr << "Invalid argument: " << global.at("output") << endl;
                cerr << "Please just separate the escape characters and let me sleep tonight ;-;" << endl;
                return 0;
            }
        }
        /*cout << "output includes:" << endl;
        for (unsigned i = 0; i < fout_order.size(); i++) {
            cout << "\t" << meta_token.at(fout_order[i]).s << " preceded by: \"" << fout_pre[i] << "\"" << endl;
        }*/

        //UTF-8 BOM skip
        char possible_BOM[3];
        const char BOM[3] = {'\xEF', '\xBB', '\xBF'};
        desc_file.read(possible_BOM, 3);
        for (int i = 0; i < 3; i++) {
            if (possible_BOM[i] != BOM[i]) {
                desc_file.seekg(0);
            }
        }

        ///MAIN LOOP
        vector<char>::iterator number_pos = find(desc_order.begin(), desc_order.end(), 'k');
        unsigned int track_number = 1;
        if (number_pos == desc_order.end() && global.at("track-number") != "\032" && global.at("track-number") != "") {
            track_number = stoul(global.at("track-number"));
        }
        for (; desc_file.good(); track_number++/*bool i = true; i; i = false*/) {
            //TODO: support for disgusting spaces
            prev = curr;
            curr.at("track-number") = global.at("track-number");
            for (unsigned i = 0; i < desc_order.size(); i++) {
                if (desc_pre[i].size() > 1) {
                    desc_file.ignore(desc_pre[i].size() - 1, '\n');
                }
                getline(desc_file, curr.at(meta_token.at(desc_order[i]).s), desc_pre[i + 1][0]);
                switch (desc_order[i]) {
                    case 'k':
                    case 'y': {
                        try {
                            curr[meta_token.at(desc_order[i]).s] = to_string(stoi(curr.at(meta_token.at(desc_order[i]).s)));
                        } catch (invalid_argument exc) {
                            cout << "Invalid argument: " << curr.at(meta_token.at(desc_order[i]).s) << endl;
                            return 0;
                        }
                        break;
                    }
                    case 'i': {
                        string::difference_type cnt = count(curr[meta_token.at(desc_order[i]).s].begin(),
                                                            curr[meta_token.at(desc_order[i]).s].end(),
                                                            ':');
                        if (cnt == 2) {
                            string::size_type colon_fst = curr[meta_token.at(desc_order[i]).s].find(':');
                            string::size_type colon_snd = curr[meta_token.at(desc_order[i]).s].find(':', colon_fst + 1);
                            curr[meta_token.at(desc_order[i]).s] =
                                to_string(stoul(curr[meta_token.at(desc_order[i]).s].substr(0, colon_fst)) * 3600
                                        + stoul(curr[meta_token.at(desc_order[i]).s].substr(colon_fst + 1, colon_snd - colon_fst)) * 60
                                        + stoul(curr[meta_token.at(desc_order[i]).s].substr(colon_snd + 1)));
                        } else if (cnt == 1) {
                            string::size_type colon = curr[meta_token.at(desc_order[i]).s].find(':');
                            curr[meta_token.at(desc_order[i]).s] =
                                to_string(stoul(curr[meta_token.at(desc_order[i]).s].substr(0, colon)) * 60
                                        + stoul(curr[meta_token.at(desc_order[i]).s].substr(colon + 1)));
                        } else {
                            curr[meta_token.at(desc_order[i]).s] = to_string(stoul(curr[meta_token.at(desc_order[i]).s]));
                        }
                        break;
                    }
                    default: {
                        break;
                    }
                }
            }
            if (curr["track-number"] == "" || curr["track-number"] == "\032") {
                curr["track-number"] = to_string(track_number);
            }
            ///COMMAND
            if (prev.at("start-time") != "") {
                string input_name = argv[argc - 2];
                prev.at("length") = to_string(stoul(curr.at("start-time")) - stoul(prev.at("start-time")) - 1);
                if (global.at("offset-pre") != "") {
                    prev.at("start-time") = to_string(stoi(prev.at("start-time")) + stoi(global.at("offset-pre")));
                }
                if (global.at("offset-post") != "") {
                    prev.at("length") = to_string(stoi(prev.at("length")) - stoi(global.at("offset-pre"))
                                        - stoi(global.at("offset-post")));
                }
                string command = "ffmpeg -loglevel warning -i \"" + input_name + "\" -c copy -ss " + prev.at("start-time")
                                 + " -t " + prev.at("length");
                for (unsigned i = 0; i < metadata.size() - 3; i++) {
                    if (prev.at(metadata[i]) != "\032") {
                        command += " -metadata ";
                        if (i == 1) {
                            command += "album_artist";
                        } else if (i == 6) {
                            command += "track";
                        } else {
                            command += metadata[i];
                        }
                        command += "=\"" + prev.at(metadata[i]) + "\"";
                    }
                }
                ///OUTPUT FILENAME
                string foutname = "";
                for (unsigned i = 0; i < fout_order.size(); i++) {
                    if (prev.at(meta_token.at(fout_order[i]).s) != "\032") {
                        foutname += fout_pre[i];
                        foutname += prev.at(meta_token.at(fout_order[i]).s);
                    }
                }
                {
                    unsigned i;
                    while ((i = foutname.find_first_of("<>:\"/\\|?*", i + 1)) != string::npos) {
                        foutname.erase(i, 1);
                    }
                }
                command += " \"" + foutname + input_name.substr(input_name.rfind('.')) + "\"";
                //cout << command << endl;
                //
                //cout << "\nTest #4 zakonczony" << endl;
                int i;
                try {
                    if ((i = _wsystem(u8_to_u16(command).c_str())) != 0) {
                        throw runtime_error(to_string(i));
                    }
                    //cout << "Successfully created \"" << foutname << input_name.substr(input_name.rfind('.')) << "\" file.\n";
                    //windows
                    _wsystem((L"echo Successfully created \"" + u8_to_u16(foutname)
                             + u8_to_u16(input_name.substr(input_name.rfind('.'))) + L"\" file.\n").c_str());
                } catch (runtime_error exc) {
                    cout << "Skipping \"" << foutname << input_name.substr(input_name.rfind('.')) << "\" file...\n";
                }
            }
        }
        if (curr.at("start-time") != "") {
            string input_name = argv[argc - 2];
            curr.at("length") = to_string(stoul(curr.at("start-time")) - stoul(curr.at("start-time")) - 1);
            if (global.at("offset-pre") != "") {
                curr.at("start-time") = to_string(stoi(curr.at("start-time")) + stoi(global.at("offset-pre")));
            }
            if (global.at("offset-post") != "") {
                curr.at("length") = to_string(stoi(curr.at("length")) - stoi(global.at("offset-pre"))
                                    - stoi(global.at("offset-post")));
            }
            string command = "ffmpeg -loglevel warning -i \"" + input_name + "\" -c copy -ss " + curr.at("start-time");
            for (unsigned i = 0; i < metadata.size() - 3; i++) {
                if (curr.at(metadata[i]) != "\032") {
                    command += " -metadata ";
                    if (i == 1) {
                        command += "album_artist";
                    } else if (i == 6) {
                        command += "track";
                    } else {
                        command += metadata[i];
                    }
                    command += "=\"" + curr.at(metadata[i]) + "\"";
                }
            }
            ///OUTPUT FILENAME
            string foutname = "";
            for (unsigned i = 0; i < fout_order.size(); i++) {
                if (curr.at(meta_token.at(fout_order[i]).s) != "\032") {
                    foutname += fout_pre[i];
                    foutname += curr.at(meta_token.at(fout_order[i]).s);
                }
            }
            {
                unsigned i;
                while ((i = foutname.find_first_of("<>:\"/\\|?*", i + 1)) != string::npos) {
                    foutname.erase(i, 1);
                }
            }
            command += " \"" + foutname + input_name.substr(input_name.rfind('.')) + "\"";
            //cout << command << endl;
            //
            //cout << "\nTest #4 zakonczony" << endl;
            int i;
            try {
                if ((i = _wsystem(u8_to_u16(command).c_str())) != 0) {
                    throw runtime_error(to_string(i));
                }
                cout << "Successfully created \"" << foutname << input_name.substr(input_name.rfind('.')) << "\" file.\n";
                //windows
                _wsystem((L"echo Successfully created \"" + u8_to_u16(foutname)
                         + u8_to_u16(input_name.substr(input_name.rfind('.'))) + L"\" file.\n").c_str());
            } catch (runtime_error exc) {
                cout << "Skipping \"" << foutname << input_name.substr(input_name.rfind('.')) << "\" file...\n";
            }
        }
        //cout << "\nTest #3 zakonczony" << endl;
    } else {
        print_help();
    }
    return 0;
}
