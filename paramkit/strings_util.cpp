#include "strings_util.h"

#include <algorithm>
#include <cstring>

std::string paramkit::util::to_lowercase(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), tolower);
    return str;
}

#define MIN(x,y) ((x) < (y) ? (x) : (y))

size_t paramkit::util::levenshtein_distance(const char s1[], const char s2[])
{
    const size_t MAX_LEN = 100;
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);

    if (len1 >= MAX_LEN || len2 >= MAX_LEN) return(-1);

    //init the distance matrix
    int dist[MAX_LEN][MAX_LEN] = { 0 };
    for (int i = 0;i <= len1;i++) {
        dist[0][i] = i;
    }
    for (int j = 0;j <= len2; j++) {
        dist[j][0] = j;
    }
    // calculate
    for (int j = 1;j <= len1; j++) {
        for (int i = 1;i <= len2; i++) {
            int track = 1;
            if (s1[i - 1] == s2[j - 1]) {
                track = 0;
            }
            int t = MIN((dist[i - 1][j] + 1), (dist[i][j - 1] + 1));
            dist[i][j] = MIN(t, (dist[i - 1][j - 1] + track));
        }
    }
    return dist[len2][len1];
}

size_t paramkit::util::str_hist_diffrence(const char s1[], const char s2[])
{
    const size_t MAX_LEN = 255;
    size_t hist1[MAX_LEN] = { 0 };
    size_t hist2[MAX_LEN] = { 0 };

    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);

    for (size_t i = 0; i < strlen(s1); i++) {
        char c = tolower(s1[i]);
        hist1[c]++;
    }

    for (size_t i = 0; i < strlen(s2); i++) {
        char c = tolower(s2[i]);
        hist2[c]++;
    }

    size_t diffs = 0;
    for (size_t i = 0; i < MAX_LEN; i++) {
        if (hist2[i] == hist1[i]) continue;
        diffs++;
    }
    return diffs;
}

paramkit::util::stringsim_type paramkit::util::has_keyword( std::string param, std::string filter)
{
    param = to_lowercase(param);
    filter = to_lowercase(filter);
    const bool sim_found = (param.find(filter) != std::string::npos) || (filter.find(param) != std::string::npos);
    if (sim_found) return SIM_SUBSTR;
    return SIM_NONE;
}

paramkit::util::stringsim_type paramkit::util::is_string_similar(const std::string &param, const std::string &filter)
{
    bool sim_found = false;
    if (has_keyword(param, filter) != SIM_NONE) {
        return SIM_SUBSTR;
    }
    size_t dist = util::levenshtein_distance(filter.c_str(), param.c_str());
    if (dist <= (param.length() / 2)) {
        sim_found = true;
    }
    if (dist >= param.length() || dist >= filter.length()) {
        sim_found = false;
    }
    if (sim_found) return SIM_LAV_DIST;

    size_t diff = util::str_hist_diffrence(filter.c_str(), param.c_str());
    if (diff <= (param.length() / 2) || diff <= (filter.length() / 2)) {
        sim_found = true;
    }
    if (diff >= param.length() || diff >= filter.length()) {
        sim_found = false;
    }
    if (sim_found) return SIM_HIST;
    return SIM_NONE;
}
