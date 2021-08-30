#include "game.hpp"
#include "../render.hpp"


#define HIGH_SCORES_FILE "/tetris"


int8_t const STONE_DATA[TetrisGame::STONE_COUNT * 3 * 4] PROGMEM = {
    // I
    -1, 1, 2,
    -11, 11, 22,
    -1, 1, 2,
    -11, 11, 22,
    // J
    -11, 10, 11,
    -1, 1, 12,
    -11, -10, 11,
    -12, -1, 1,
    // L
    -11, 11, 12,
    -10, -1, 1,
    -12, -11, 11,
    -1, 1, 10,
    // O
    1, 11, 12,
    1, 11, 12,
    1, 11, 12,
    1, 11, 12,
    // S
    -11, -10, -1,
    -12, -1, 11,
    -11, -10, -1,
    -12, -1, 11,
    // Z
    -12, -11, 1,
    -11, -1, 10,
    -12, -11, 1,
    -11, -1, 10,
    // T
    -11, -1, 1,
    -11, -1, 11,
    -1, 1, 11,
    -11, 1, 11,
};



void TetrisGame::init() {

    if (!high_scores.load(HIGH_SCORES_FILE)) {
        high_scores.reset();
        for (int i = HighScores::LEN; i > 0; --i) high_scores.add("...", i * 10);
        high_scores.save(HIGH_SCORES_FILE);
    }
    name[0] = 'A';
    name[1] = 'A';
    name[2] = 'A';

    title_screen();
}

void TetrisGame::title_screen() {
    new_game();
//    state = S_HIGH_SCORES;
}

void TetrisGame::new_game() {
    state = S_NORMAL;

    memset(grid, 0, sizeof(grid));
    for (int i = 0; i < sizeof(grid); i += GRID_W) grid[i] = 0xff;
    for (int i = 231; i < sizeof(grid); ++i) grid[i] = 0xff;

    current_letter = 0;
    new_high_score = false;
    stone_perm_pos = 0;
    drop = false;
    new_stone();
}

void TetrisGame::new_stone() {
    tick  = 0;
    pos   = 16;
    first_stone_frame = true;

	if (--stone_perm_pos < 0) {
		stone_perm_pos = STONE_COUNT - 1;
        for (int i = 0; i < STONE_COUNT; ++i) stone_permutation[i] = i;
		for (int i = 0; i < STONE_COUNT; ++i) {
			int j = random.rand() % STONE_COUNT;
			int a = stone_permutation[i];
			stone_permutation[i] = stone_permutation[j];
			stone_permutation[j] = a;
		}
	}

    stone = stone_permutation[stone_perm_pos];
    rot   = random.rand() & 3;
}

bool TetrisGame::check_collision() {
    int8_t const* d = STONE_DATA + stone * 12 + rot * 3;
    int ps[] = {
        pos,
        pos + (int8_t) pgm_read_byte(d + 0),
        pos + (int8_t) pgm_read_byte(d + 1),
        pos + (int8_t) pgm_read_byte(d + 2),
    };
    for (int p : ps) {
        if (p >= 0 && grid[p] > 0) return true;
    }
    return false;
}

void TetrisGame::paint_stone(uint8_t color) {
    int8_t const* d = STONE_DATA + stone * 12 + rot * 3;
    int ps[] = {
        pos,
        pos + (int8_t) pgm_read_byte(d + 0),
        pos + (int8_t) pgm_read_byte(d + 1),
        pos + (int8_t) pgm_read_byte(d + 2),
    };
    for (int p : ps) {
        if (p >= 0) grid[p] = color;
    }
}

void TetrisGame::update() {


    if (button_just_pressed(fx::BTN_LEFT))  { button_tick = -1; button = fx::BTN_LEFT;  }
    if (button_just_pressed(fx::BTN_RIGHT)) { button_tick = -1; button = fx::BTN_RIGHT; }
    if (button_just_pressed(fx::BTN_DOWN))  { button_tick = -1; button = fx::BTN_DOWN;  }
    if (button_just_pressed(fx::BTN_UP))    { button_tick = -1; button = fx::BTN_UP;    }
    if (button_just_pressed(fx::BTN_A))     { button_tick = -1; button = fx::BTN_A;     }
    if (button_just_pressed(fx::BTN_B))     { button_tick = -1; button = fx::BTN_B;     }
    if (fx::button_bits & (1 << button)) ++button_tick;
    else button = 0xff;

    int dx = 0;
    int dy = 0;
    int dr = 0;
    if (drop) dy = 1;
    else if (button != 0xff && (button_tick & 3) == 0 && button_tick != 4 && button_tick != 8) {
        dx = (button == fx::BTN_RIGHT) - (button == fx::BTN_LEFT);
        dy = button == fx::BTN_DOWN;
        dr = (button == fx::BTN_A) - (button == fx::BTN_UP);
        if (button == fx::BTN_B) {
            drop = true;
            dy = 1;
        }
    }
    else if (++tick > 60) dy = 1;
    if (dy) tick = 0;


    // remove stone from grid
    if (!first_stone_frame) paint_stone(0);
    first_stone_frame = false;

    // find new stone position
    int old_pos = pos;
    int old_rot = rot;
    pos += dx;
    pos += dy * GRID_W;
    rot = (rot + 4 + dr) & 0x3;
    bool collision = check_collision();
    if (collision) {
        drop = false;
        pos = old_pos;
        rot = old_rot;
    }


    // paint stone
    paint_stone(stone + 1);

    if (collision && dy) {
        // TODO: check for complete lines

        new_stone();
    }


    // draw
    render::clear(0);

    render::fill_rect({0, 0, 5, 128}, 5);
    render::fill_rect({66, 0, 5, 128}, 5);
    render::fill_rect({0, 0, 66, 3}, 5);
    render::fill_rect({0, 124, 66, 4}, 5);

    uint8_t const COLORS[]      = { 12, 13, 9, 10, 11, 8, 14, };
    uint8_t const COLORS_DARK[] = { 1, 1, 4, 9, 3, 2, 2, };

    int i = 11;
    for (int y = 0; y < 20; ++y) {
        for (int x = 0; x < 11; ++x) {
            uint8_t b = grid[i++];
            if (b == 0 || b == 0xff) continue;
            int xx = x * 6;
            int yy = 4 + y * 6;
            render::fill_rect({xx, yy, 5, 5}, COLORS[b - 1]);
            render::pixel(xx, yy, 7);
            render::pixel(xx + 4, yy + 4, COLORS_DARK[b - 1]);
        }
    }
}
