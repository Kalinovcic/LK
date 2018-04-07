#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <windows.h>
#include <Shlwapi.h>

#include <vector>
#include <set>
#include <map>

char* copy_string(char* string)
{
    int length = strlen(string);
    char* new_string = (char*) malloc(length + 1);
    memcpy(new_string, string, length);
    new_string[length] = 0;
    return new_string;
}

char* copy_string(char* from, char* to)
{
    int length = to - from;
    char* new_string = (char*) malloc(length + 1);
    memcpy(new_string, from, length);
    new_string[length] = 0;
    return new_string;
}

char* concatenate(char* left, char* right)
{
    int length_left = strlen(left);
    int length_right = strlen(right);
    int length = length_left + length_right;
    char* new_string = (char*) malloc(length + 1);
    memcpy(new_string, left, length_left);
    memcpy(new_string + length_left, right, length_right);
    new_string[length] = 0;
    return new_string;
}

char* concatenate(char* left, char* middle, char* right)
{
    int length_left = strlen(left);
    int length_middle = strlen(middle);
    int length_right = strlen(right);
    int length = length_left + length_middle + length_right;
    char* new_string = (char*) malloc(length + 1);
    memcpy(new_string, left, length_left);
    memcpy(new_string + length_left, middle, length_middle);
    memcpy(new_string + length_left + length_middle, right, length_right);
    new_string[length] = 0;
    return new_string;
}

void eat_whitespace(char** string)
{
    while (**string && isspace(**string))
        (*string)++;
}

char* eat_token(char** string)
{
    eat_whitespace(string);
    if (!**string)
        return NULL;

    char* start = *string;
    while (**string && !isspace(**string))
        (*string)++;

    return copy_string(start, *string);
}

char* trim(char** string)
{
    eat_whitespace(string);
    if (!**string)
        return NULL;

    char* start = *string;
    while (true)
    {
        while (**string && !isspace(**string))
            (*string)++;

        char* maybe_end = *string;
        eat_whitespace(string);
        if (**string) continue;

        return copy_string(start, maybe_end);
    }
}

bool match(char* string1, char* string2)
{
    return !strcmp(string1, string2);
}

bool ends_with(char* string, char* suffix)
{
    int length = strlen(string);
    int suffix_length = strlen(suffix);
    if (suffix_length > length)
        return false;

    return !memcmp(string + length - suffix_length, suffix, suffix_length);
}

bool starts_with(char* string, char* prefix)
{
    int length = strlen(string);
    int prefix_length = strlen(prefix);
    if (prefix_length > length)
        return false;

    return !memcmp(string, prefix, prefix_length);
}

bool is_source_file(char* string)
{
    return ends_with(string, ".c") ||
           ends_with(string, ".cc") ||
           ends_with(string, ".cpp") ||
           ends_with(string, ".cxx");
}

bool is_header_file(char* string)
{
    return ends_with(string, ".h") ||
           ends_with(string, ".hpp");
}

struct Tracked_File
{
    char* path;
    bool is_source;

    bool dirty = false;

    bool found_dependencies = false;
    std::set<Tracked_File*> depends;
    std::set<Tracked_File*> depend_on_me;

    FILETIME write_time;
    FILETIME db_time = {};
};

struct Configuration
{
    char* obj_output;
    char* cl_options = "";
    char* link_options = "";
    char* libraries = "";

    std::vector<char*> includes;
    std::vector<char*> excludes;
    std::vector<char*> source_files;

    std::map<std::string, Tracked_File*> tracked_files;
};

char* sanitize_path(char* path)
{
    char* clone = copy_string(path);
    int length = strlen(clone);
    std::vector<char*> names;

    char* start = clone;
    for (int i = 0; i < length; i++)
    {
        if (clone[i] == '/' || clone[i] == '\\')
        {
            clone[i] = 0;
            names.push_back(start);
            start = clone + i + 1;
        }
    }

    names.push_back(start);

    for (int i = 0; i < names.size(); i++)
    {
        char* name = names[i];
        if (match(name, "."))
        {
            names.erase(names.begin() + i);
            i--;
        }
        else if (match(name, ".."))
        {
            if (i)
            {
                names.erase(names.begin() + i);
                names.erase(names.begin() + i - 1);
                i -= 2;
            }
            else
            {
                printf("Encountered a path that escapes the root folder.\nPath is %s\nNote that lk_build.exe must be in the source tree root.\n", path);
                exit(0);
            }
        }

        if (!*name)
        {
            printf("Encountered an invalid path with a zero-width name component.\nPath is %s\n", path);
            exit(0);
        }
    }

    int sanitized_length = names.size();
    for (char* name : names)
        sanitized_length += strlen(name);

    char* sanitized = (char*) malloc(sanitized_length + 1);
    sanitized[sanitized_length] = 0;

    char* write = sanitized;
    for (char* name : names)
    {
        int length = strlen(name);
        memcpy(write, name, length);
        write += length;
        write[0] = '\\';
        write++;
    }
    write[0] = 0;

    return sanitized;
}

char* sanitize_file(char* path)
{
    char* sanitized = sanitize_path(path);
    sanitized[strlen(sanitized) - 1] = 0; // remove the trailing slash
    return sanitized;
}

char* file_to_folder(char* path)
{
    return sanitize_path(concatenate(path, "/.."));
}

void list_folder(std::vector<char*>& files, char* folder, bool directories)
{
    char* wildcard = concatenate(folder, "*");

    WIN32_FIND_DATAA file_data;
    HANDLE handle = FindFirstFileA(wildcard, &file_data);
    if (handle == INVALID_HANDLE_VALUE)
    {
        printf("FindFirstFile() returned an invalid handle while searching for files in folder %s!\n", folder);
        exit(0);
    }

    do
    {
        bool directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
        if (directory != directories) continue;

        char* name = file_data.cFileName;
        if (match(name, ".")) continue;
        if (match(name, "..")) continue;

        char* path;
        if (directory)
            path = concatenate(folder, name, "\\");
        else
            path = concatenate(folder, name);
        files.push_back(path);
    }
    while (FindNextFileA(handle, &file_data) != 0);
}

void recursive_file_search(Configuration* config, char* folder)
{
    for (char* excluded : config->excludes)
        if (match(folder, excluded))
            return;

    std::vector<char*> files;
    list_folder(files, folder, false);
    for (char* file : files)
        if (is_source_file(file))
            config->source_files.push_back(file);

    std::vector<char*> folders;
    list_folder(folders, folder, true);
    for (char* folder : folders)
        recursive_file_search(config, folder);
}

void find_all_source_files(Configuration* config)
{
    for (char* folder : config->includes)
        recursive_file_search(config, folder);
}

static char line_buffer[16 * 1024 * 1024];

void scan_file_for_dependencies(std::vector<char*>& dependencies, char* path)
{
    FILE* in = fopen(path, "rt");
    if (!in)
    {
        printf("Failed to open file \"%s\" for reading!\n", path);
        exit(0);
    }

    int line_number = 0;
    while (fgets(line_buffer, sizeof(line_buffer), in))
    {
        line_number++;
        char* cursor = line_buffer;
        char* token = eat_token(&cursor);
        if (!token) continue;

        if (match(token, "#"))
        {
            token = eat_token(&cursor);
            if (!token) continue;
            if (!match(token, "include"))
                continue;
        }
        else
        {
            if (!match(token, "#include"))
                continue;
        }

        char* relative = trim(&cursor);
        int length = strlen(relative);
        if (length < 3) continue;
        if (relative[0] != '"' || relative[length - 1] != '"') continue;

        relative[length - 1] = 0; // remove trailing "
        relative++; // skip first "

        char* dependency = sanitize_file(concatenate(file_to_folder(path), relative));
        dependencies.push_back(dependency);
    }

    fclose(in);
}

void find_dependencies(Configuration* config)
{
    for (char* source : config->source_files)
    {
        Tracked_File* file = new Tracked_File;
        file->path = source;
        file->is_source = true;
        config->tracked_files[source] = file;
    }

    bool found_new = true;
    while (found_new)
    {
        found_new = false;

        std::vector<Tracked_File*> pass;
        for (auto& it : config->tracked_files)
            pass.push_back(it.second);

        for (Tracked_File* file : pass)
        {
            if (file->found_dependencies)
                continue;

            if (is_source_file(file->path) || is_header_file(file->path))
            {
                std::vector<char*> dependencies;
                scan_file_for_dependencies(dependencies, file->path);
                for (char* dependency : dependencies)
                {
                    for (char* excluded : config->excludes)
                        if (starts_with(dependency, excluded))
                            goto skip_dependency;

                    Tracked_File* existing = config->tracked_files[dependency];
                    if (existing)
                    {
                        existing->depend_on_me.insert(file);
                        continue;
                    }

                    Tracked_File* new_file = new Tracked_File;
                    new_file->path = dependency;
                    new_file->is_source = false;
                    config->tracked_files[dependency] = new_file;
                    found_new = true;

                    file->depends.insert(new_file);
                    new_file->depend_on_me.insert(file);

                    skip_dependency:;
                }
            }

            file->found_dependencies = true;
        }
    }
}

void find_file_times(Configuration* config)
{
    for (auto& it : config->tracked_files)
    {
        Tracked_File* file = it.second;

        HANDLE handle = CreateFileA(file->path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (handle == INVALID_HANDLE_VALUE)
        {
            printf("CreateFileA() failed for file %s\n", file->path);
            exit(0);
        }

        FILETIME create_time, access_time, write_time;
        GetFileTime(handle, &create_time, &access_time, &write_time);

        CloseHandle(handle);

        file->write_time = write_time;
    }
}

void read_config_file(Configuration* config, char* configuration_path)
{
    FILE* in = fopen(configuration_path, "rt");
    if (!in)
    {
        printf("Couldn't find lk_build.txt in the current directory!\n");
        exit(0);
    }

    int line_number = 0;
    while (fgets(line_buffer, sizeof(line_buffer), in))
    {
        line_number++;
        char* cursor = line_buffer;
        char* command = eat_token(&cursor);
        if (!command) continue;

        if (match(command, "include"))
        {
            char* path = trim(&cursor);
            if (!path)
            {
                printf("Expected a path to a directory after \"include\" on line %d in lk_build.txt\n", line_number);
                exit(0);
            }

            path = sanitize_path(path);
            config->includes.push_back(path);
        }
        else if (match(command, "exclude"))
        {
            char* path = trim(&cursor);
            if (!path)
            {
                printf("Expected a path to a directory after \"exclude\" on line %d in lk_build.txt\n", line_number);
                exit(0);
            }

            path = sanitize_path(path);
            config->excludes.push_back(path);
        }
        else if (match(command, "obj_output"))
        {
            char* path = trim(&cursor);
            if (!path)
            {
                printf("Expected a path to a directory after \"obj_output\" on line %d in lk_build.txt\n", line_number);
                exit(0);
            }

            path = sanitize_path(path);
            config->obj_output = path;
        }
        else if (match(command, "cl_options"))
        {
            char* string = trim(&cursor);
            if (!string) string = "";
            config->cl_options = string;
        }
        else if (match(command, "link_options"))
        {
            char* string = trim(&cursor);
            if (!string) string = "";
            config->link_options = string;
        }
        else if (match(command, "libraries"))
        {
            char* string = trim(&cursor);
            if (!string) string = "";
            config->libraries = string;
        }
        else
        {
            printf("Unrecognized configuration command \"%s\" on line %d in lk_build.txt\n", command, line_number);
            exit(0);
        }
    }

    fclose(in);
}

void read_database(Configuration* config)
{
    char* db_file = concatenate(config->obj_output, "db.lk_build");

    FILE* in = fopen(db_file, "rt");
    if (!in)
    {
        printf("WARNING! Failed to read database!\nDB path: %s\nNOTE: If this is your first time running this build, this is normal.\n", db_file);
        return;
    }

    bool success = false;
    int line_number = 0;
    while (fgets(line_buffer, sizeof(line_buffer), in))
    {
        line_number++;
        char* cursor = line_buffer;
        char* token1 = eat_token(&cursor);
        if (!token1) continue;

        if (match(token1, "#COMPLETE"))
        {
            success = true;
            break;
        }

        if (strlen(token1) != 8) break;

        char* token2 = eat_token(&cursor);
        if (!token2 || strlen(token2) != 8) break;

        char* path = trim(&cursor);
        if (!path) break;

        char filetime_string[16];
        memcpy(filetime_string, token1, 8);
        memcpy(filetime_string + 8, token2, 8);

        unsigned long long filetime = 0;
        for (int i = 0; i < 16; i++)
        {
            filetime *= 16;
            if (filetime_string[i] >= '0' && filetime_string[i] <= '9')
                filetime += filetime_string[i] - '0';
            else if (filetime_string[i] >= 'A' && filetime_string[i] <= 'F')
                filetime += 10 + filetime_string[i] - 'A';
            else if (filetime_string[i] >= 'a' && filetime_string[i] <= 'f')
                filetime += 10 + filetime_string[i] - 'a';
            else goto corrupt;
        }

        FILETIME db_time;
        db_time.dwHighDateTime = filetime >> 32;
        db_time.dwLowDateTime = filetime & 0xFFFFFFFFull;

        if (config->tracked_files.find(path) == config->tracked_files.end())
        {
            printf("WARNING! Database lists a file that wasn't found by the source tree scan.\nFile: \"%s\"\n", path);
            continue;
        }

        Tracked_File* file = config->tracked_files[path];
        file->db_time = db_time;
    }

    corrupt:;
    if (!success)
    {
        printf("WARNING! CORRUPT DATABASE! Treating all files as modified...\nDB path: %s\n", db_file);
    }

    fclose(in);
}

void write_database(Configuration* config)
{
    char* db_file = concatenate(config->obj_output, "db.lk_build");

    FILE* out = fopen(db_file, "wt");
    if (!out)
    {
        printf("Failed to write out to database!\nDB path: %s\n", db_file);
        exit(0);
    }

    for (auto& it : config->tracked_files)
    {
        Tracked_File* file = it.second;
        FILETIME time = file->db_time;
        fprintf(out, "%08X %08X %s\n", time.dwHighDateTime, time.dwLowDateTime, file->path);
    }

    fprintf(out, "#COMPLETE\n");
    fclose(out);
}

void propagate_dirty_flag(Configuration* config, Tracked_File* file)
{
    if (file->dirty) return;
    file->dirty = true;

    for (Tracked_File* child : file->depend_on_me)
    {
        propagate_dirty_flag(config, child);
    }
}

void mark_dirty_files(Configuration* config)
{
    for (auto& it : config->tracked_files)
    {
        Tracked_File* file = it.second;
        if (CompareFileTime(&file->db_time, &file->write_time) != 0)
        {
            propagate_dirty_flag(config, file);
        }
    }
}

void write_to_temporary_batch(char* bat_file, char* string)
{
    FILE* out = fopen(bat_file, "wt");
    if (!out)
    {
        printf("Can't write to temporary batch file!\nBatch path: %s\n", bat_file);
        exit(0);
    }

    fprintf(out, "%s", string);
    fclose(out);
}

void clear_build_failed(char* failure_file)
{
    DeleteFileA(failure_file);
}

bool check_build_failed(char* failure_file)
{
    if (PathFileExistsA(failure_file))
    {
        clear_build_failed(failure_file);
        return true;
    }

    return false;
}

void do_incremental_compilation(Configuration* config, int* success_count, int* failure_count)
{
    read_database(config);
    mark_dirty_files(config);

    char* bat_file = concatenate(config->obj_output, "lk_build_temp.bat");
    char* failure_file = concatenate(config->obj_output, "lk_build_failure");
    clear_build_failed(failure_file);

    for (auto& it : config->tracked_files)
    {
        Tracked_File* file = it.second;
        if (file->dirty && file->is_source)
        {
            sprintf(line_buffer,
                "@echo off\n"
                "cl -nologo \"%s\" -c -Fo\"%s\\\" %s\n"
                "if not \"%%ERRORLEVEL%%\"==\"0\" echo biatch > \"%s\"\n",
                file->path, config->obj_output, config->cl_options, failure_file);

            write_to_temporary_batch(bat_file, line_buffer);
            system(bat_file);

            if (check_build_failed(failure_file))
            {
                printf("FAILURE: %s\n", file->path);
                (*failure_count)++;
                goto failed;
            }
            else
            {
                (*success_count)++;
            }
        }

        file->db_time = file->write_time;
        failed:;
    }

    write_database(config);
}

void do_linking(Configuration* config)
{
    #define Append (line_buffer + strlen(line_buffer))

    line_buffer[0] = 0;
    sprintf(Append, "link %s %s", config->link_options, config->libraries);

    for (auto& it : config->tracked_files)
    {
        Tracked_File* file = it.second;
        if (file->is_source)
        {
            char* file_name = file->path + strlen(file->path);
            while (file_name >= file->path && *file_name != '\\')
                file_name--;
            file_name++;

            file_name = copy_string(file_name);
                 if (ends_with(file_name, ".c"  )) file_name[strlen(file_name) - 2] = 0;
            else if (ends_with(file_name, ".cc" )) file_name[strlen(file_name) - 3] = 0;
            else if (ends_with(file_name, ".cpp")) file_name[strlen(file_name) - 4] = 0;
            else if (ends_with(file_name, ".cxx")) file_name[strlen(file_name) - 4] = 0;

            char* obj_path = concatenate(config->obj_output, file_name, ".obj");
            sprintf(Append, " %s", obj_path);
        }
    }

    system(line_buffer);
}

double time_stamp()
{
    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);
    LARGE_INTEGER time_frequency;
    QueryPerformanceFrequency(&time_frequency);
    return (double) time.QuadPart / (double) time_frequency.QuadPart;
}

#define EnglishPlural(number) (((number) == 1) ? "" : "s")

int main(int argc, char** argv)
{
    double start = time_stamp();

    if (argc > 2)
    {
        printf("Usage: lk_build.exe config.txt\n");
        exit(0);
    }

    char* configuration_path = "lk_build.txt";
    if (argc == 2)
    {
        configuration_path = argv[1];
    }

    Configuration config;
    read_config_file(&config, configuration_path);

    find_all_source_files(&config);
    find_dependencies(&config);
    find_file_times(&config);

    double finish = time_stamp();
    double tree_scan_time = finish - start;


    int success_count = 0;
    int failure_count = 0;

    start = time_stamp();
    do_incremental_compilation(&config, &success_count, &failure_count);
    finish = time_stamp();
    double compilation_time = finish - start;


    start = time_stamp();
    do_linking(&config);
    finish = time_stamp();
    double linking_time = finish - start;


    /*
    printf("Source tree scan finished in %.2lf ms\n", tree_scan_time * 1000);
    printf("Compilation took %.2f seconds\n", compilation_time);
    printf("%d compilation unit%s succeeded\n", success_count, EnglishPlural(success_count));
    printf("%d compilation unit%s failed\n", failure_count, EnglishPlural(failure_count));
    printf("Linking took %.2f seconds\n", linking_time);
    */

    printf("%d/%d success, %d/%d fail (scan %.2fs, cl %.2fs, link %.2fs)\n",
        success_count, (int) config.source_files.size(),
        failure_count, (int) config.source_files.size(),
        tree_scan_time, compilation_time, linking_time);


    return EXIT_SUCCESS;
}