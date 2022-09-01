#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 800
#define HEIGHT 600
#define SEEDS_COUNT 10
#define SEED_RADIUS 5
// #define PALETTE_COUNT 7

#define OUTPUT "output.ppm"

#define BLACK_COLOR         0xFF000000
#define WHITE_COLOR         0xFFFFFFFF
#define RED_COLOR           0xFF0000FF
#define GREEN_COLOR         0xFF00FF00
#define BLUE_COLOR          0xFFFF0000
#define BRIGHT_RED 			0xFF3449FB
#define BRIGHT_GREEN   		0xFF26BBB8
#define BRIGHT_YELLOW  		0xFF2FBDFA
#define BRIGHT_BLUE    		0xFF98A583
#define BRIGHT_PURPLE  		0xFF9B86D3
#define BRIGHT_AQUA    		0xFF7CC08E
#define BRIGHT_ORANGE  		0xFF1980FE
// ========================================
#define BACKGROUND_COLOR    0xFF121212
#define SEED_COLOR          WHITE_COLOR

#define max(a, b) (a > b ? a : b)
#define min(a, b) (a < b ? a : b)

typedef struct {
    uint16_t x, y;
} Point;

static uint32_t field[HEIGHT][WIDTH];
static Point seeds[SEEDS_COUNT];

// static uint32_t palette[] = {
//     BRIGHT_RED,
//     BRIGHT_GREEN,
//     BRIGHT_YELLOW,
//     BRIGHT_BLUE,
//     BRIGHT_PURPLE,
//     BRIGHT_AQUA,
//     BRIGHT_ORANGE
// };

uint32_t sqr_dist(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    uint16_t dx = max(x0, x1) - min(x0, x1), dy = max(y0, y1) - min(y0, y1);
    return dx * dx + dy * dy;
}

uint32_t point_to_color(uint16_t x, uint16_t y) {
    return (y << 16) | x;
}

Point color_to_point(uint32_t color) {
    Point p = {color & 0x0000FFFF, (color & 0xFFFF0000) >> 16};
    return p;
}

void fill_image(uint32_t color) {
    for (uint16_t y = 0; y < HEIGHT; y++) {
        for (uint16_t x = 0; x < WIDTH; x++) {
            field[y][x] = color;
        }
    }
}

void generate_seeds(void) {
    for (size_t i = 0; i < SEEDS_COUNT; i++) {
        seeds[i].x = rand() % WIDTH; 
        seeds[i].y = rand() % HEIGHT;
    }
}

void paint_segments(void) {
    for (uint16_t y = 0; y < HEIGHT; y++) {
        for (uint16_t x = 0; x < WIDTH; x++) {
            size_t j = 0;
            for (size_t i = 1; i < SEEDS_COUNT; i++) {
                if (sqr_dist(seeds[i].x, seeds[i].y, x, y) 
                        < sqr_dist(seeds[j].x, seeds[j].y, x, y)) {
                    j = i;
                }
            }
            field[y][x] = point_to_color(seeds[j].x, seeds[j].y);
            // field[y][x] = palette[j % PALETTE_COUNT];
        }
    }
}

void apply_next_seed(size_t index) {
    for (uint16_t y = 0; y < HEIGHT; y++) {
        for (uint16_t x = 0; x < WIDTH; x++) {
            Point curr_seed = color_to_point(field[y][x]);
            if (sqr_dist(x, y, seeds[index].x, seeds[index].y) 
                < sqr_dist(x, y, curr_seed.x, curr_seed.y)) {
                field[y][x] = point_to_color(seeds[index].x, seeds[index].y);
            }
        }
    }
}

void paint_segments2(void) {
    fill_image(point_to_color(seeds[0].x, seeds[0].y));

    for (size_t i = 1; i < SEEDS_COUNT; i++) {
        apply_next_seed(i);
    }

}

void draw_circle(uint16_t x, uint16_t y, uint16_t radius, uint32_t color) {
    for (uint16_t y0 = y - min(y, radius); y0 < y + radius && y0 < HEIGHT; y0++) {
        for (uint16_t x0 = x - min(x, radius); x0 < x + radius && x0 < WIDTH; x0++) {
            if (sqr_dist(x0, y0, x, y) <= radius * radius) {
                field[y0][x0] = color;
            }        
        }
    }
}

// drawing picture
void draw(void) {

    // painting segments
    paint_segments2();

    // drawing seeds
    for (size_t i = 0; i < SEEDS_COUNT; i++) {
        draw_circle(seeds[i].x, seeds[i].y, SEED_RADIUS, SEED_COLOR);
    }
}

void dump_image(void) {
    FILE *f = fopen(OUTPUT, "wb");

    if (!f) {
        fprintf(stderr, "Couldn't open output file\n");
    }

    fprintf(f, "P6\n%d %d\n255\n", WIDTH, HEIGHT);

    for (uint16_t y = 0; y < HEIGHT; y++) {
        for (uint16_t x = 0; x < WIDTH; x++) {
            unsigned char channels[3] = {
                (field[y][x] & 0xFF0000FF),
                (field[y][x] & 0xFF00FF00) >> 8,
                (field[y][x] & 0xFFFF0000) >> 16
            };
            fwrite(channels, sizeof(channels), 1, f);
        }
    }

    fclose(f);
}

int main(void) {
    srand(time(NULL));
    generate_seeds();
    draw();
    dump_image();
    return 0;
}
