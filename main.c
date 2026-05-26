#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <windows.h>
#include <conio.h>

// ============================================================================
// 1. SYSTEM DEFINITIONS & METRIC FLAGS (100% ASCII Safe)
// ============================================================================
#define COLOR_GOLD    "\033[1;33m"
#define COLOR_CYAN    "\033[1;36m"
#define COLOR_RED     "\033[1;31m"
#define COLOR_RESET   "\033[0m"
#define COLOR_WHITE   "\033[1;37m"
#define COLOR_DARK    "\033[1;30m"

typedef enum {
    LANG_EN = 1,
    LANG_TR
} Language;

typedef enum {
    STORY_UNASSIGNED = -1,
    STORY_TRADITIONAL,
    STORY_UNCROWNED,
    STORY_EXILED
} StoryState;

typedef struct {
    char player_name[50];
    char god_alignment[30];
    char archetype_alignment[50];
    StoryState story;

    // Advanced 7-Parametric Vectors
    int ms; // Logic & Strategy
    int kh; // Charisma & Rhetoric
    int fk; // Physical Strength
    int hc; // Precision & Agility
    int mb; // Mystic Bond & Prophecy
    int sm; // Political Legitimacy
    int kd; // Chaotic Resistance
} CharacterProfile;

typedef struct {
    char god[30];
    char archetype[50];
    int ms, kh, fk, hc, mb, sm, kd;
} ArchetypeMatrix;

// Global matrix of 30 exact mythological profiles
ArchetypeMatrix database[30] = {
    {"Zeus", "The Sovereign", 1, 2, 1, 0, 1, 2, 0},
    {"Zeus", "The Stormbringer", 1, 0, 2, 0, 2, 1, 1},
    {"Zeus", "The Arbiter", 2, 1, 0, 0, 0, 2, 1},
    {"Poseidon", "The Earth-Shaker", 1, 0, 2, 0, 1, 0, 2},
    {"Poseidon", "The Abyssal Master", 2, 1, 1, 1, 2, 0, 0},
    {"Poseidon", "The Wild Tamer", 0, 0, 2, 2, 1, 0, 2},
    {"Athena", "The Tactician", 2, 1, 1, 2, 0, 1, 0},
    {"Athena", "The Polias", 2, 2, 0, 1, 0, 2, 0},
    {"Ares", "The Berserker", 0, 0, 2, 1, 0, 0, 2},
    {"Ares", "The Rebel Vanguard", 0, 2, 1, 0, 1, 0, 2},
    {"Apollo", "The Oracle", 2, 1, 0, 0, 2, 1, 0},
    {"Apollo", "The Plague-Bringer", 1, 0, 1, 2, 2, 0, 0},
    {"Artemis", "The Lone Stalker", 1, 0, 0, 2, 1, 0, 2},
    {"Artemis", "The Beast Master", 1, 0, 1, 1, 2, 0, 1},
    {"Hephaestus", "The Grand Forgemaster", 2, 0, 1, 2, 0, 0, 1},
    {"Hephaestus", "The Iron Wall", 1, 0, 2, 1, 0, 1, 2},
    {"Hermes", "The Trickster", 1, 2, 0, 2, 0, 0, 1},
    {"Hermes", "The Herald", 2, 2, 0, 1, 0, 1, 0},
    {"Dionysus", "The Liberator", 0, 2, 0, 0, 2, 0, 2},
    {"Dionysus", "The Hedonist", 1, 1, 0, 2, 1, 0, 2},
    {"Hera", "The Iron Queen", 1, 2, 1, 0, 0, 2, 1},
    {"Hera", "The Sovereign Avenger", 2, 2, 0, 1, 0, 1, 1},
    {"Demeter", "The Harvester", 1, 0, 1, 1, 2, 1, 0},
    {"Demeter", "The Winter Scourge", 0, 0, 2, 0, 1, 1, 2},
    {"Aphrodite", "The Siren", 1, 2, 0, 1, 2, 0, 0},
    {"Aphrodite", "The Golden Presence", 1, 2, 0, 0, 1, 2, 0},
    {"Hades", "The Lord of Wealth", 2, 1, 1, 1, 1, 2, 0},
    {"Hades", "The Soul Collector", 1, 0, 1, 0, 2, 2, 1},
    {"Hestia", "The Hearth Keeper", 1, 1, 0, 1, 2, 1, 1},
    {"Hestia", "The Eternal Flame", 2, 0, 0, 0, 2, 2, 1}
};

// ============================================================================
// 2. GLOBAL SYSTEM STATE
// ============================================================================
Language current_language = LANG_EN;

// ============================================================================
// 3. FUNCTION PROTOTYPES
// ============================================================================
void set_cursor_visibility(bool visible);
void clear_screen(void);
bool render_lightning_storm(int frame);
void render_menu_options(bool is_flashing);

// Core Modules
bool scene_start_journey(CharacterProfile* profile);
void execute_parametric_test(CharacterProfile* profile);
void evaluate_cosmic_alignment(CharacterProfile* profile);
void scene_continue_journey(void);
void scene_language_options(void);
void scene_system_status(CharacterProfile* profile);

// ============================================================================
// 4. MAIN GAME LOOP
// ============================================================================
int main(void) {
    bool running = true;
    int frame_counter = 0;

    CharacterProfile player = {
        .player_name = "Wastrel",
        .god_alignment = "UNASSIGNED",
        .archetype_alignment = "UNASSIGNED",
        .story = STORY_UNASSIGNED,
        .ms = 0, .kh = 0, .fk = 0, .hc = 0, .mb = 0, .sm = 0, .kd = 0
    };

    clear_screen();
    set_cursor_visibility(false);

    while (running) {
        printf("\033[H");

        bool is_flashing = render_lightning_storm(frame_counter);
        render_menu_options(is_flashing);

        if (_kbhit()) {
            char input_char = _getch();

            switch (input_char) {
                case '1':
                    set_cursor_visibility(true);
                    if (!scene_start_journey(&player)) {
                        running = false;
                    }
                    set_cursor_visibility(false);
                    break;
                case '2':
                    scene_continue_journey();
                    break;
                case '3':
                    scene_language_options();
                    break;
                case '4':
                    clear_screen();
                    printf(COLOR_WHITE "\n \"You cannot escape your destiny; you can only delay it.\"\n" COLOR_RESET);
                    running = false;
                    break;
                case '0':
                    scene_system_status(&player);
                    break;
                default:
                    break;
            }
        }

        frame_counter++;
        Sleep(80);
    }

    set_cursor_visibility(true);
    return 0;
}

// ============================================================================
// 5. ENGINE MODULE DEFINITIONS
// ============================================================================

void set_cursor_visibility(bool visible) {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursor_info;
    GetConsoleCursorInfo(hConsole, &cursor_info);
    cursor_info.bVisible = visible;
    SetConsoleCursorInfo(hConsole, &cursor_info);
#endif
}

void clear_screen(void) {
    printf("\033[H\033[J");
}

bool render_lightning_storm(int frame) {
    int storm_tick = frame % 60;
    printf(COLOR_WHITE "       .---.   .--.  .-'   '-.  .--.   .---.  .--.   .--.\033[K\n");
    printf("    _ (     ) (    )(         )(    ) (     )(    ) (    )_\033[K\n" COLOR_RESET);

    if (storm_tick >= 0 && storm_tick <= 2) {
        printf(COLOR_CYAN  "       _\\_                                                      \033[K\n" COLOR_RESET);
        printf(COLOR_WHITE "      / __|                                                     \033[K\n" COLOR_RESET);
        printf(COLOR_CYAN  "        \\_\\                                                     \033[K\n" COLOR_RESET);
        return true;
    }
    else if (storm_tick == 3) {
        printf(COLOR_DARK  "       _\\_                                                      \033[K\n" COLOR_RESET);
        printf(COLOR_DARK  "      / __|                                                     \033[K\n" COLOR_RESET);
        printf(COLOR_DARK  "        \\_\\                                                     \033[K\n" COLOR_RESET);
        return false;
    }
    else if (storm_tick >= 12 && storm_tick <= 14) {
        printf(COLOR_CYAN  "                                            /\\_   _/\\           \033[K\n" COLOR_RESET);
        printf(COLOR_WHITE "                                           /  /_ /_  \\          \033[K\n" COLOR_RESET);
        printf(COLOR_CYAN  "                                           \\/_/\\_\\/\\_\\          \033[K\n" COLOR_RESET);
        return true;
    }
    else if (storm_tick == 15) {
        printf(COLOR_DARK  "                                            /\\_   _/\\           \033[K\n" COLOR_RESET);
        printf(COLOR_DARK  "                                           /  /_ /_  \\          \033[K\n" COLOR_RESET);
        printf(COLOR_DARK  "                                           \\/_/\\_\\/\\_\\          \033[K\n" COLOR_RESET);
        return false;
    }
    else if (storm_tick >= 26 && storm_tick <= 28) {
        printf(COLOR_CYAN  "                               _\\_                              \033[K\n" COLOR_RESET);
        printf(COLOR_WHITE "                              / __|                             \033[K\n" COLOR_RESET);
        printf(COLOR_CYAN  "                                \\_\\                             \033[K\n" COLOR_RESET);
        return true;
    }
    else if (storm_tick == 29) {
        printf(COLOR_DARK  "                               _\\_                              \033[K\n" COLOR_RESET);
        printf(COLOR_DARK  "                              / __|                             \033[K\n" COLOR_RESET);
        printf(COLOR_DARK  "                                \\_\\                             \033[K\n" COLOR_RESET);
        return false;
    }
    else if (storm_tick >= 42 && storm_tick <= 45) {
        printf(COLOR_CYAN  "       _\\_                                     _/_/_/           \033[K\n" COLOR_RESET);
        printf(COLOR_WHITE "      / __|                                      \\_\\            \033[K\n" COLOR_RESET);
        printf(COLOR_CYAN  "        \\_\\                                      /_/            \033[K\n" COLOR_RESET);
        return true;
    }
    else if (storm_tick == 46) {
        printf(COLOR_DARK  "       _\\_                                     _/_/_/           \033[K\n" COLOR_RESET);
        printf(COLOR_DARK  "      / __|                                      \\_\\            \033[K\n" COLOR_RESET);
        printf(COLOR_DARK  "        \\_\\                                      /_/            \033[K\n" COLOR_RESET);
        return false;
    }
    else {
        printf("\033[K\n");
        printf("\033[K\n");
        printf("\033[K\n");
        return false;
    }
}

void render_menu_options(bool is_flashing) {
    char* title_color  = is_flashing ? COLOR_WHITE : COLOR_GOLD;
    char* option_color = is_flashing ? COLOR_GOLD  : COLOR_CYAN;
    char* border_color = is_flashing ? COLOR_WHITE : COLOR_RESET;

    printf("\033[K\n\033[K\n\033[K\n");
    printf("%s   === MYTHOLOGICAL ODYSSEY OPERATING SYSTEM ===\033[K\n\n%s", title_color, border_color);
    printf("   [%s1%s] START JOURNEY\033[K\n", option_color, border_color);
    printf("   [%s2%s] CONTINUE JOURNEY\033[K\n", option_color, border_color);
    printf("   [%s3%s] LANGUAGE OPTIONS\033[K\n", option_color, border_color);
    printf("   [%s4%s] ESCAPE DESTINY (Exit Game)\033[K\n", option_color, border_color);
    printf("   -------------------------------------------------------------\033[K\n");
    printf("   [%s0%s] System Status & Matrix Verification\033[K\n", title_color, border_color);
    printf("   -------------------------------------------------------------\033[K\n\n");
    printf("%s Target Action (0-4): \033[K" COLOR_RESET, option_color);
}

// ============================================================================
// 6. CINEMATIC NARRATIVE AND MATRIX TESTING
// ============================================================================

bool scene_start_journey(CharacterProfile* profile) {
    clear_screen();
    char choice = '0';
    bool valid_input = false;

    printf(COLOR_RED "\n\n\n\n\n\n                  I   D   L   E   R   ! ! !\n\n\n\n\n\n" COLOR_RESET);
    printf(COLOR_DARK " [Press ANY KEY to open your eyes] " COLOR_RESET);
    _getch(); clear_screen();

    printf(COLOR_WHITE "\n\n  AWAKE... " COLOR_RESET); printf(COLOR_RED "You have no tomorrow.\n\n" COLOR_RESET);
    printf(COLOR_GOLD "  Rise now. Or blind your ears to my call and never set foot upon this path.\n\n\n" COLOR_RESET);
    printf(COLOR_DARK " [Press ANY KEY to confront the ultimatum] " COLOR_RESET);
    _getch(); clear_screen();

    printf(COLOR_RED "\n  #############################################################################\n");
    printf("  #                               F  L  E  E  ! ! !                           #\n");
    printf("  #############################################################################\n\n\n" COLOR_RESET);
    printf(COLOR_WHITE "  The cosmic frequencies demand an instant reaction:\n\n" COLOR_RESET);
    printf("  [" COLOR_CYAN "1" COLOR_RESET "] Escape silently into the shadows (Run away).\n");
    printf("  [" COLOR_CYAN "2" COLOR_RESET "] Stand frozen and endure the tempest silently (Calm observation).\n");
    printf("  [" COLOR_CYAN "3" COLOR_RESET "] Roar back at the heavens and strike the ground (Fierce defiance).\n\n");
    printf(COLOR_CYAN " Execute Instinct (1-3): " COLOR_RESET);

    while (!valid_input) {
        if (_kbhit()) {
            choice = _getch();
            if (choice >= '1' && choice <= '3') valid_input = true;
        }
        Sleep(20);
    }

    if (choice == '1') {
        clear_screen();
        printf(COLOR_RED "\n\n  You chose obscurity. The cosmic void reclaims your meaningless thread.\n\n" COLOR_RESET);
        printf(COLOR_DARK " [Press ANY KEY to exit game] " COLOR_RESET);
        _getch();
        return false;
    }
    else if (choice == '2') {
        profile->story = STORY_UNCROWNED;
    }
    else if (choice == '3') {
        profile->story = STORY_TRADITIONAL;
    }

    execute_parametric_test(profile);
    return true;
}

void execute_parametric_test(CharacterProfile* profile) {
    clear_screen();
    char ch;

    printf(COLOR_GOLD " =============================================================\n");
    printf("                THE TRIAL OF THE COSMIC ENTITY                \n");
    printf(" =============================================================\n\n" COLOR_RESET);
    printf(COLOR_WHITE "  A massive shadow splits the cloud lining. It looks down at you.\n");
    printf("  'Prove your essence, wastrel. Resolve my paradoxes.'\n\n" COLOR_RESET);
    printf(COLOR_DARK " [Press ANY KEY to begin the trial] " COLOR_RESET);
    _getch();

    // QUESTION 1: The Roadblock (Tests FK, HC, KD)
    clear_screen();
    printf(COLOR_GOLD "\n [TRIAL 1] A colossus rock blocks the path to your destiny. How do you clear it?\n\n" COLOR_RESET);
    printf("  [1] Smash it with raw physical force.\n");
    printf("  [2] Analyze its weaknesses and dismantle it precisely.\n");
    printf("  [3] Climb over it blindly, enduring the bruises.\n\n");
    printf(COLOR_CYAN " Choice (1-3): " COLOR_RESET);
    while(1) { if(_kbhit()) { ch = _getch(); if(ch >= '1' && ch <= '3') break; } Sleep(2); }
    if(ch == '1') profile->fk += 2; if(ch == '2') profile->hc += 2; if(ch == '3') profile->kd += 2;

    // QUESTION 2: The Gateway Extortion (Tests SM, KH, HC)
    clear_screen();
    printf(COLOR_GOLD "\n [TRIAL 2] A corrupt guard demands your pride and coins to enter the gates. Your move?\n\n" COLOR_RESET);
    printf("  [1] Cite the supreme laws of the high kings to command him.\n");
    printf("  [2] Convince him with magnetic charm and smooth lies.\n");
    printf("  [3] Slip past his sight and steal his key from his belt.\n\n");
    printf(COLOR_CYAN " Choice (1-3): " COLOR_RESET);
    while(1) { if(_kbhit()) { ch = _getch(); if(ch >= '1' && ch <= '3') break; } Sleep(2); }
    if(ch == '1') profile->sm += 2; if(ch == '2') profile->kh += 2; if(ch == '3') profile->hc += 2;

    // QUESTION 3: The Cryptic Archive (Tests MS, MB, FK)
    clear_screen();
    printf(COLOR_GOLD "\n [TRIAL 3] A glowing seal blocks a chamber of ancient power. How do you bypass it?\n\n" COLOR_RESET);
    printf("  [1] Solve the mathematical riddle inscribed on the stone.\n");
    printf("  [2] Channel the raw divine energy, offering your faith.\n");
    printf("  [3] Kick the door down, ignoring the magical recoil.\n\n");
    printf(COLOR_CYAN " Choice (1-3): " COLOR_RESET);
    while(1) { if(_kbhit()) { ch = _getch(); if(ch >= '1' && ch <= '3') break; } Sleep(2); }
    if(ch == '1') profile->ms += 2; if(ch == '2') profile->mb += 2; if(ch == '3') { profile->fk += 1; profile->kd += 1; }

    // QUESTION 4: The Riot Encirclement (Tests KH, MS, MB)
    clear_screen();
    printf(COLOR_GOLD "\n [TRIAL 4] An angry mob surrounds you, screaming for blood. How do you handle them?\n\n" COLOR_RESET);
    printf("  [1] Tame their rage with an electrifying public speech.\n");
    printf("  [2] Outmaneuver them using immediate tactical positioning.\n");
    printf("  [3] Pray for a sudden terrifying omen from the sky.\n\n");
    printf(COLOR_CYAN " Choice (1-3): " COLOR_RESET);
    while(1) { if(_kbhit()) { ch = _getch(); if(ch >= '1' && ch <= '3') break; } Sleep(2); }
    if(ch == '1') profile->kh += 2; if(ch == '2') { profile->ms += 1; profile->sm += 1; } if(ch == '3') profile->mb += 2;

    evaluate_cosmic_alignment(profile);
}

void evaluate_cosmic_alignment(CharacterProfile* profile) {
    clear_screen();
    int min_distance = 999;
    int distances[30];
    int match_count = 0;
    int tied_indices[30];

    for (int i = 0; i < 30; i++) {
        int d = abs(profile->ms - database[i].ms) +
                abs(profile->kh - database[i].kh) +
                abs(profile->fk - database[i].fk) +
                abs(profile->hc - database[i].hc) +
                abs(profile->mb - database[i].mb) +
                abs(profile->sm - database[i].sm) +
                abs(profile->kd - database[i].kd);
        distances[i] = d;
        if (d < min_distance) {
            min_distance = d;
        }
    }

    for (int i = 0; i < 30; i++) {
        if (distances[i] == min_distance) {
            tied_indices[match_count] = i;
            match_count++;
        }
    }

    printf(COLOR_GOLD " =============================================================\n");
    printf("                    THE COSMIC VERDICT                        \n");
    printf(" =============================================================\n\n" COLOR_RESET);

    if (min_distance == 0) {
        int idx = tied_indices[0];
        strcpy(profile->god_alignment, database[idx].god);
        strcpy(profile->archetype_alignment, database[idx].archetype);

        printf(COLOR_WHITE "  The Cosmic Entity narrows its massive eyes, laughing sarcastically:\n\n" COLOR_RESET);
        printf(COLOR_CYAN "  \"Aaaa a child of %s [ %s ]... How predictable.\"\n\n" COLOR_RESET,
               profile->god_alignment, profile->archetype_alignment);
    }
    else {
        int primary_idx = tied_indices[0];
        strcpy(profile->god_alignment, database[primary_idx].god);
        strcpy(profile->archetype_alignment, database[primary_idx].archetype);

        printf(COLOR_WHITE "  The Cosmic Entity shifts through the dimension matrix:\n\n" COLOR_RESET);
        printf("  \"You look a lot like a child of " COLOR_CYAN "%s [ %s ]" COLOR_RESET " to me.\"\n\n",
               profile->god_alignment, profile->archetype_alignment);

        if (match_count > 1) {
            printf(COLOR_WHITE "  Yet, the cosmic frequencies are warped. Choose your true bloodline alternative:\n\n" COLOR_RESET);
            printf("  [1] Accept the Entity's vision (%s - %s)\n", database[primary_idx].god, database[primary_idx].archetype);

            for (int i = 1; i < match_count; i++) {
                int idx = tied_indices[i];
                printf("  [%d] Actually, I am the child of %s [ %s ]\n", i + 1, database[idx].god, database[idx].archetype);
            }

            printf("\n" COLOR_CYAN " Select your real bloodline resonance (1-%d): " COLOR_RESET, match_count);
            char ch;
            while(1) {
                if(_kbhit()) {
                    ch = _getch();
                    int sel = ch - '0';
                    if(sel >= 1 && sel <= match_count) {
                        int final_idx = tied_indices[sel - 1];
                        strcpy(profile->god_alignment, database[final_idx].god);
                        strcpy(profile->archetype_alignment, database[final_idx].archetype);
                        break;
                    }
                }
                Sleep(2);
            }
            printf(COLOR_GOLD "\n\n Matrix re-calibrated to your final choice!\n" COLOR_RESET);
        }
    }

    printf("\n\n" COLOR_WHITE " [Press ANY KEY to return to the Storm Menu and check your Status] " COLOR_RESET);
    _getch();
    clear_screen();
}

void scene_continue_journey(void) {
    clear_screen();
    printf(COLOR_RED " =============================================================\n");
    printf("                     SCENE: CONTINUE JOURNEY                  \n");
    printf(" =============================================================\n\n" COLOR_RESET);
    printf("  No saved progression matrix found within local memory cells.\n");
    printf(COLOR_WHITE " [Press ANY KEY to return safely to the Storm Menu] " COLOR_RESET);
    _getch();
    clear_screen();
}

void scene_language_options(void) {
    clear_screen();
    printf(COLOR_GOLD " =============================================================\n");
    printf("                    SCENE: LANGUAGE OPTIONS                   \n");
    printf(" =============================================================\n" COLOR_RESET);
    printf("  Active Configuration: English (ASCII Native Standard)\n\n");
    printf(COLOR_WHITE " [Press ANY KEY to return safely to the Storm Menu] " COLOR_RESET);
    _getch();
    clear_screen();
}

void scene_system_status(CharacterProfile* profile) {
    clear_screen();
    char* story_map[] = { "TRADITIONAL", "UNCROWNED", "EXILED" };

    printf(COLOR_GOLD " =============================================================\n");
    printf("                   SCENE: MATRIX VERIFICATION                 \n");
    printf(" =============================================================\n" COLOR_RESET);
    printf("  --- HERO MEMORY REGISTER STATUS ---\n");
    printf("  * Character State    : %s\n", profile->player_name);
    printf("  * Story Background   : %s\n", (profile->story == STORY_UNASSIGNED) ? "UNASSIGNED" : story_map[profile->story]);

    printf("\n  --- ASSIGNED GOD & ARCHETYPE MATRICES ---\n");
    printf("  * God Alignment      : " COLOR_GOLD "%s\n" COLOR_RESET, profile->god_alignment);
    printf("  * Situational Class  : " COLOR_CYAN "%s\n" COLOR_RESET, profile->archetype_alignment);

    printf("\n  --- ACTIVE 7-PARAMETRIC ATTRIBUTES ---\n");
    printf("  * [MS] Logic & Strategy       : %d points\n", profile->ms);
    printf("  * [KH] Charisma & Rhetoric    : %d points\n", profile->kh);
    printf("  * [FK] Physical Strength      : %d points\n", profile->fk);
    printf("  * [HÇ] Precision & Agility     : %d points\n", profile->hc);
    printf("  * [MB] Mystic Bond & Prophecy : %d points\n", profile->mb);
    printf("  * [SM] Political Legitimacy   : %d points\n", profile->sm);
    printf("  * [KD] Chaotic Resistance     : %d points\n", profile->kd);
    printf("  -------------------------------------------------------------\n\n");
    printf(COLOR_WHITE " [Press ANY KEY to return safely to the Storm Menu] " COLOR_RESET);
    _getch();
    clear_screen();
}