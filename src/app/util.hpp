#pragma once

#include <fx.hpp>


#define COUNT_OF(x) (sizeof(x) / sizeof(0[x]))


struct Rect {
    int x, y, w, h;
    int left() const { return x; }
    int right() const { return x + w; }
    int top() const { return y; }
    int bottom() const { return y + h; }
    bool contains(int px, int py) const {
        return px >= x && px < right() && py >= y && py < bottom();
    }
    int center_x() const { return x + w / 2; }
    int center_y() const { return y + h / 2; }
    bool overlap(Rect const& r) const {
        return !(x >= r.right() || y >= r.bottom() || right() <= r.x || bottom() <= r.y);
    }
    int overlap_x(Rect const& r) const {
        if (!overlap(r)) return 0;
        int s = r.right() - x;
        int t = r.x - right();
        return abs(s) < abs(t) ? s : t;
    }
    int overlap_y(Rect const& r) const {
        if (!overlap(r)) return 0;
        int s = r.bottom() - y;
        int t = r.y - bottom();
        return abs(s) < abs(t) ? s : t;
    }
};


extern int8_t const SIN_TABLE[256] PROGMEM;
inline int8_t my_sin(uint8_t x) { return pgm_read_byte(SIN_TABLE + x); }
inline int8_t my_cos(uint8_t x) { return my_sin(x + 64); }

void itos(char* s, uint16_t n);


extern uint8_t prev_button_bits;
inline bool button_down(int b) {
    return (fx::button_bits >> b) & 1;
}
inline bool button_just_pressed(int b) {
    return button_down(b) & ~(prev_button_bits >> b) & 1;
}


struct Random {
    uint16_t seed;

    uint16_t hash(uint32_t input, uint32_t key) {
        uint32_t h = input * key;
        return (h >> 16) ^ h;
    }

    uint16_t rand() {
        seed += 0xfc15;
        return hash(seed, 0x2ab);
    }
};



struct HighScores {
    enum { LEN = 10, CHARS = 3 };
    struct Entry {
        uint16_t score;
        char     name[4];
    };
    Entry entries[LEN];

    void reset() {
        memset(entries, 0, sizeof(entries));
    }
    void add(char const* name, uint16_t score) {
        for (int i = 0; i < LEN; ++i) {
            Entry& e = entries[i];
            if (score > e.score) {
                for (int j = LEN - 1; j > i; --j) entries[j] = entries[j - 1];
                e.score = score;
                int l = strlen(name);
                e.name[0] = name[0];
                e.name[1] = l < 1 ? 0 : name[1];
                e.name[2] = l < 2 ? 0 : name[2];
                return;
            }
        }
    }

    bool load(char const* filename) {
        return fx::try_load(filename, &entries, sizeof(entries));
    }
    void save(char const* filename) {
        fx::store(filename, &entries, sizeof(entries));
    }
};

