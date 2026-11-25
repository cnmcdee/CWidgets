// common/ipc.h — how the palette and layout windows share state.
// Palette -> layout: the palette writes the selected widget name to the
// selection file on every selection; the layout polls the file's mtime.
// Layout -> tests: the layout writes its full placed-widget state to the
// state file after every mutation, so the driver can verify bounds
// without guessing from pixels.
#pragma once

#include <cstdio>
#include <string>
#include <sys/stat.h>

namespace cw {

inline std::string selection_path() {
    const char* e = getenv("CWIDGETS_SELECTION");
    if (e && *e) return e;
    return "/tmp/cwidgets-selection.txt";
}

inline std::string layout_state_path() {
    const char* e = getenv("CWIDGETS_LAYOUT_STATE");
    if (e && *e) return e;
    return "/tmp/cwidgets-layout-state.txt";
}

inline std::string icons_dir() {
    const char* e = getenv("CWIDGETS_ICONS");
    if (e && *e) return e;
    return "/opt/cwidgets/widgets/icons";
}

inline bool write_selection(const std::string& name, int index) {
    FILE* f = fopen(selection_path().c_str(), "w");
    if (!f) return false;
    fprintf(f, "%s\n%d\n", name.c_str(), index);
    fclose(f);
    return true;
}

inline bool read_selection(std::string& name, int& index) {
    FILE* f = fopen(selection_path().c_str(), "r");
    if (!f) return false;
    char buf[256];
    bool ok = false;
    if (fgets(buf, sizeof(buf), f)) {
        buf[strcspn(buf, "\r\n")] = 0;
        name = buf;
        ok = true;
    }
    index = -1;
    if (fscanf(f, "%d", &index) != 1) index = -1;
    fclose(f);
    return ok;
}

inline long file_mtime(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) != 0) return -1;
    return (long)st.st_mtime * 1000000000L + (long)st.st_mtim.tv_nsec;
}

}  // namespace cw
