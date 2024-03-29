//x,y,z coordinates
int globe_cnt = 190;
int globe_scale = 2;
int globe_vertices[190][3] = {
  
{5, 11, 62},
{8, 2, 63},
{21, 8, 59},
{22, 22, 55},
{24, 9, 58},
{25, 5, 58},
{27, 8, 57},
{33, 2, 54},
{32, 1, 54},
{33, 2, 54},
{33, 1, 54},
{35, 1, 53},
{38, 0, 51},
{39, 1, 50},
{40, 0, 49},
{40, 1, 49},
{41, 3, 48},
{40, 1, 49},
{39, 3, 50},
{38, 2, 51},
{38, 2, 51},
{35, 3, 53},
{36, 5, 52},
{37, 3, 51},
{39, 4, 50},
{39, 6, 49},
{37, 6, 51},
{46, 6, 43},
{46, 1, 43},
{42, 3, 47},
{41, -1, 49},
{37, -5, 51},
{33, -5, 54},
{36, -7, 51},
{34, -12, 52},
{24, -11, 58},
{35, -8, 52},
{29, -2, 56},
{19, -8, 60},
{18, -16, 58},
{10, -20, 59},
{-10, -15, 61},
{-25, -2, 58},
{-36, -15, 50},
{-29, -11, 55},
{-25, -25, 53},
{-31, -34, 43},
{-30, -38, 41},
{-32, -40, 36},
{-23, -44, 39},
{-29, -47, 31},
{-17, -57, 21},
{-21, -58, 13},
{-16, -61, 8},
{-10, -61, 14},
{-16, -61, 5},
{-4, -61, 16},
{0, -59, 23},
{10, -61, 15},
{13, -61, 8},
{18, -57, 21},
{32, -54, 2},
{44, -44, 14},
{39, -48, 11},
{49, -40, -6},
{46, -40, -17},
{46, -24, -35},
{50, -16, -35},
{60, -11, -18},
{63, -9, 3},
{63, 8, 4},
{59, 19, 15},
{56, 15, 27},
{52, 4, 36},
{50, -8, 37},
{51, -18, 32},
{45, -30, 32},
{41, -30, 38},
{47, -19, 38},
{43, -10, 45},
{47, -14, 40},
{46, -6, 43},
{49, 0, 41},
{51, 1, 38},
{50, 7, 38},
{11, 34, -52},
{15, 50, -36},
{20, 56, -20},
{20, 60, -6},
{13, 62, 6},
{-14, 58, 20},
{-24, 46, 35},
{-24, 36, 46},
{-25, 21, 54},
{-33, 15, 52},
{-26, -4, 58},
{-18, 8, 60},
{-14, 17, 59},
{0, 22, 59},
{-1, 33, 54},
{6, 39, 49},
{6, 29, 56},
{22, 33, 49},
{17, 42, 44},
{15, 45, 41},
{12, 50, 36},
{8, 54, 32},
{10, 57, 27},
{5, 55, 31},
{-6, 56, 29},
{-6, 60, 20},
{2, 59, 22},
{15, 61, 8},
{29, 55, 10},
{52, 36, -5},
{45, 38, -23},
{30, 42, -36},
{17, 40, -46},
{-50, -37, -12},
{-50, -25, -29},
{-43, -25, -39},
{-34, -41, -33},
{-21, -47, -36},
{-24, -54, -23},
{-41, -47, -13},
{-46, -39, -18},
{41, -47, -13},
{39, -41, -28},
{44, -42, -18},
{-37, -26, 44},
{-41, -33, 35},
{-34, -41, 33},
{-36, -36, 37},
{-38, -33, 38},
{-20, -60, 1},
{-27, -57, -4},
{-29, -56, 7},
{-52, 6, -36},
{-50, 0, -39},
{-42, -9, -46},
{-49, 5, -40},
{14, 24, -57},
{4, 20, -60},
{-11, 16, -60},
{-14, -2, -62},
{-20, -3, -60},
{-19, -17, -58},
{-4, -26, -58},
{7, -21, -59},
{15, -20, -58},
{21, -4, -60},
{18, 5, -60},
{8, 12, -62},
{11, -62, 6},
{9, 58, 25},
{16, 57, 21},
{12, 58, 21},
{18, 57, 21},
{22, 56, 20},
{19, 57, 19},
{-54, 25, 20},
{-54, -31, -10},
{-49, -38, -10},
{-43, -46, -2},
{-50, -38, -4},
{-30, -50, 25},
{-31, -52, 18},
{-35, -51, 13},
{-36, -52, 7},

{0, 0, 0},  // 169 ISS

{0, 0, 0},  // 170 ISS Orbital Path
{0, 0, 0},
{0, 0, 0},
{0, 0, 0},
{0, 0, 0},
{0, 0, 0},
{0, 0, 0},
{0, 0, 0},
{0, 0, 0},
{0, 0, 0},

{0, 0, 0},
{0, 0, 0},
{0, 0, 0},
{0, 0, 0},
{0, 0, 0},
{0, 0, 0},
{0, 0, 0},
{0, 0, 0},
{0, 0, 0},
{0, 0, 0},


};
