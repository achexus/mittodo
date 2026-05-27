#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <windows.h>
#include <conio.h>

// ============================================================================
// 1. SYSTEM DEFINITIONS & METRIC FLAGS
// ============================================================================
#define COLOR_GOLD    "\033[1;33m"
#define COLOR_CYAN    "\033[1;36m"
#define COLOR_RED     "\033[1;31m"
#define COLOR_RESET   "\033[0m"
#define COLOR_WHITE   "\033[1;37m"
#define COLOR_DARK    "\033[1;30m"

typedef enum {
    STORY_UNASSIGNED = -1,
    STORY_TRADITIONAL, // Aggressive Defiance
    STORY_UNCROWNED,   // Calm Observation
    STORY_EXILED
} StoryState;

typedef enum {
    NATURE_UNASSIGNED = -1,
    NATURE_CALM,
    NATURE_AGGRESSIVE
} Nature;

typedef enum {
    CLASS_UNASSIGNED = 0,
    CLASS_RULER,
    CLASS_SOLDIER,
    CLASS_ARTIST,
    CLASS_DIPLOMAT,
    CLASS_MERCHANT
} ClassType;

typedef struct {
    char player_name[50];
    char god_alignment[30];
    char archetype_alignment[50];
    StoryState story;

    // Mindset & Anomaly System
    Nature player_nature;
    Nature god_nature;
    ClassType chosen_class;
    char class_name[30];
    int affinity;
    bool is_pure;
    char final_title[100];

    // The 5-Parametric Vector System
    int intel; // Intelligence / Logic
    int might; // Might / Power
    int honor; // Honor / Sacrifice
    int skill; // Dexterity / Personal Skills
    int faith; // Faith / Celestial Connection
} CharacterProfile;

typedef struct {
    char god[30];
    char archetype[50];
    int intel, might, honor, skill, faith; // UPDATED
} ArchetypeMatrix;

// Global matrix of 48 calibrated unique historical profiles
ArchetypeMatrix database[48] = {
    {"Zeus", "The Absolute Sky Sovereign", 0, 5, 5, 0, 0},
    {"Zeus", "The Cloud Arbiter", 4, 1, 5, 0, 0},
    {"Zeus", "The Thunder Wrath", 0, 7, 0, 0, 3},
    {"Zeus", "The Hospitality Oathkeeper", 2, 0, 6, 0, 2},
    {"Poseidon", "The Earth-Shaker", 0, 10, 0, 0, 0},
    {"Poseidon", "The Abyssal Master", 1, 3, 0, 0, 6},
    {"Poseidon", "The Wild Equine Tamer", 0, 5, 0, 5, 0},
    {"Poseidon", "The Tidal Ravager", 2, 6, 0, 0, 1},
    {"Athena", "The Supreme Tactician", 8, 0, 2, 0, 0},
    {"Athena", "The Citadel Protector", 3, 2, 5, 0, 0},
    {"Athena", "The Divine Weaver", 5, 0, 0, 5, 0},
    {"Athena", "The Counselor of Heroes", 6, 0, 3, 1, 0},
    {"Athena", "The Just Judgment", 5, 0, 4, 0, 1},
    {"Hades", "The Underworld Autocrat", 3, 2, 5, 0, 0},
    {"Hades", "The Lord of Wealth", 4, 0, 2, 4, 0},
    {"Hades", "The Soul Collector", 0, 3, 0, 0, 7},
    {"Hades", "The Inexorable Judge", 4, 1, 4, 0, 1},
    {"Ares", "The Blind Berserker", 0, 9, 0, 1, 0},
    {"Ares", "The Bloodlust Vanguard", 0, 6, 0, 4, 0},
    {"Ares", "The Rebel Warlord", 0, 7, 0, 0, 0},
    {"Ares", "The Dread Champion", 0, 5, 1, 0, 4},
    {"Apollo", "The Radiant Prophet", 0, 0, 0, 0, 10},
    {"Apollo", "The Sun Bowman", 1, 0, 0, 8, 1},
    {"Apollo", "The Golden Maestro", 3, 0, 5, 2, 0},
    {"Apollo", "The Plague Bringer", 4, 0, 0, 3, 3},
    {"Artemis", "The Midnight Stalker", 0, 0, 0, 9, 1},
    {"Artemis", "The Forest Matriarch", 1, 0, 0, 2, 7},
    {"Artemis", "The Silver Arrow", 0, 2, 0, 6, 0},
    {"Hephaestus", "The Forgemaster Architect", 2, 0, 0, 8, 0},
    {"Hephaestus", "The Volcanic Juggernaut", 0, 6, 0, 4, 0},
    {"Hephaestus", "The Outcast Innovator", 4, 0, 0, 2, 0},
    {"Hephaestus", "The Iron Trapmaster", 3, 1, 0, 6, 0},
    {"Hermes", "The Phantom Thief", 0, 0, 0, 10, 0},
    {"Hermes", "The Golden Herald", 5, 0, 3, 2, 0},
    {"Hermes", "The Crossroads Guide", 2, 0, 0, 3, 5},
    {"Hermes", "The Divine Trickster", 6, 0, 0, 4, 0},
    {"Dionysus", "The Madness Bringer", 0, 0, 0, 0, 9},
    {"Dionysus", "The Hedonist Vagabond", 3, 0, 0, 2, 3},
    {"Dionysus", "The Cult Liberator", 0, 0, 0, 0, 5},
    {"Hera", "The Zenith Matriarch", 0, 0, 10, 0, 0},
    {"Hera", "The Palace Intriguer", 6, 0, 4, 0, 0},
    {"Hera", "The Royal Avenger", 2, 2, 5, 1, 0},
    {"Demeter", "The Eternal Harvester", 0, 0, 0, 5, 5},
    {"Demeter", "The Winter Famine", 0, 5, 0, 0, 5},
    {"Demeter", "The Earth Mother", 0, 0, 2, 0, 6},
    {"Aphrodite", "The Siren Puppetmaster", 1, 0, 0, 1, 8},
    {"Aphrodite", "The Golden Presence", 0, 0, 8, 0, 2},
    {"Aphrodite", "The Loom of Desire", 4, 0, 0, 4, 1}
};

// ============================================================================
// 2. FUNCTION PROTOTYPES
// ============================================================================
void set_cursor_visibility(bool visible);
void clear_screen(void);
bool render_lightning_storm(int frame);
void render_menu_options(bool is_flashing);

bool scene_start_journey(CharacterProfile* profile);
void execute_parametric_test(CharacterProfile* profile);
void evaluate_cosmic_alignment(CharacterProfile* profile);
void scene_class_selection(CharacterProfile* profile);
void get_god_affinity_data(const char* god_name, Nature* nature, int* r, int* s, int* a, int* d, int* m);
void calculate_final_title(CharacterProfile* profile);

int get_parametric_input(void);
void print_permanent_choices(void);
void scene_continue_journey(void);
void scene_language_options(void);
void scene_system_status(CharacterProfile* profile);
void display_character_sheet(CharacterProfile* profile);

// ============================================================================
// 3. MAIN GAME LOOP
// ============================================================================
int main(void) {
#ifdef _WIN32
    SetConsoleOutputCP(65001); // Enable UTF-8 Output for Turkish Characters
#endif

    bool running = true;
    int frame_counter = 0;

    CharacterProfile player = {
        .player_name = "Wastrel",
        .god_alignment = "UNASSIGNED",
        .archetype_alignment = "UNASSIGNED",
        .story = STORY_UNASSIGNED,
        .player_nature = NATURE_UNASSIGNED,
        .god_nature = NATURE_UNASSIGNED,
        .chosen_class = CLASS_UNASSIGNED,
        .affinity = 0,
        .is_pure = false,
        .class_name = "UNASSIGNED",
        .final_title = "UNASSIGNED",
        .intel = 0, .might = 0, .honor = 0, .skill = 0, .faith = 0
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

                    // --- YENİ EKLENEN KISIM: HER YENİ OYUNDA STATLARI SIFIRLA ---
                    player.intel = 0;
                    player.might = 0;
                    player.honor = 0;
                    player.skill = 0;
                    player.faith = 0;
                    player.affinity = 0;
                    strcpy(player.god_alignment, "UNASSIGNED");
                    strcpy(player.class_name, "UNASSIGNED");
                    // ------------------------------------------------------------

                    if (!scene_start_journey(&player)) running = false;
                    set_cursor_visibility(false);
                    break;
                case '2': scene_continue_journey(); break;
                case '3': scene_language_options(); break;
                case '4':
                    clear_screen();
                    printf(COLOR_WHITE "\n \"You cannot escape your destiny; you can only delay it.\"\n" COLOR_RESET);
                    running = false;
                    break;
                case '0': scene_system_status(&player); break;
                default: break;
            }
        }
        frame_counter++;
        Sleep(80);
    }
    set_cursor_visibility(true);
    return 0;
}

// ============================================================================
// 4. CORE ENGINE MODULES
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
        printf("\033[K\n"); printf("\033[K\n"); printf("\033[K\n");
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
// 5. NARRATIVE TRIAL ENGINE
// ============================================================================
bool scene_start_journey(CharacterProfile* profile) {
    clear_screen();
    char choice = '0';
    bool valid_input = false;

    printf(COLOR_RED "\n\n\n");
    printf("                 _____   _____    _        ______  _____   _  _  _ \n");
    printf("                |_   _| |  __ \\  | |      |  ____||  __ \\ | || || |\n");
    printf("                  | |   | |  | | | |      | |__   | |__) || || || |\n");
    printf("                  | |   | |  | | | |      |  __|  |  _  / | || || |\n");
    printf("                 _| |_  | |__| | | |____  | |____ | | \\ \\ |_||_||_|\n");
    printf("                |_____| |_____/  |______| |______||_|  \\_\\(_)(_)(_)\n");
    printf("\n\n\n\n" COLOR_RESET);

    printf(COLOR_DARK "                     [Press ANY KEY to open your eyes] " COLOR_RESET);
    _getch(); clear_screen();

    // --- İSİM ALMA İŞLEMİ BURAYA TAŞINDI ---
    printf(COLOR_WHITE "\n\n  AWAKE... " COLOR_RESET); printf(COLOR_RED "You have no tomorrow.\n\n" COLOR_RESET);
    printf(COLOR_CYAN "  What was your name before the fall? : " COLOR_RESET);

    set_cursor_visibility(true);
    scanf(" %[^\n]", profile->player_name);
    set_cursor_visibility(false);
    clear_screen();

    // İsim alındıktan sonra hikaye oyuncunun ismiyle devam ediyor
    printf(COLOR_GOLD "\n\n  Rise now, %s. Or blind your ears to my call and never set foot upon this path.\n\n\n" COLOR_RESET, profile->player_name);
    printf(COLOR_DARK " [Press ANY KEY to confront the ultimatum] " COLOR_RESET);
    _getch(); clear_screen();
    // ----------------------------------------

    printf(COLOR_RED "\n  #############################################################################\n");
    printf("  #                               F  L  E  E  ! ! !                           #\n");
    printf("  #############################################################################\n\n\n" COLOR_RESET);
    // ... fonksiyonun geri kalanı aynı (Escape, Stand frozen, Roar back) ...    printf(COLOR_WHITE "  The cosmic frequencies demand an instant reaction:\n\n" COLOR_RESET);
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
        profile->player_nature = NATURE_CALM; // Logged as Calm Mindset
    }
    else if (choice == '3') {
        profile->story = STORY_TRADITIONAL;
        profile->player_nature = NATURE_AGGRESSIVE; // Logged as Aggressive Mindset
    }

    execute_parametric_test(profile);
    return true;
}

int get_parametric_input(void) {
    while (1) {
        if (_kbhit()) {
            char ch = _getch();
            if (ch >= '1' && ch <= '9') return ch - '0';
            if (ch == '0') return 10;
        }
        Sleep(20);
    }
}

void print_permanent_choices(void) {
    printf("  [" COLOR_CYAN "1" COLOR_RESET "] I resolve this directly through my intellect.\n");
    printf("  [" COLOR_CYAN "2" COLOR_RESET "] I rely purely on my own might.\n");
    printf("  [" COLOR_CYAN "3" COLOR_RESET "] I sacrifice everything for my honor.\n");
    printf("  [" COLOR_CYAN "4" COLOR_RESET "] I trust in my own skills.\n");
    printf("  [" COLOR_CYAN "5" COLOR_RESET "] I overcome this through absolute faith in celestial justice.\n");
    printf("  [" COLOR_CYAN "6" COLOR_RESET "] I resolve this cunningly and through stealth.\n");
    printf("  [" COLOR_CYAN "7" COLOR_RESET "] I strike savagely and without restraint.\n");
    printf("  [" COLOR_CYAN "8" COLOR_RESET "] I break the rules and play dirty.\n");
    printf("  [" COLOR_CYAN "9" COLOR_RESET "] I surrender entirely to the will of the cosmic order.\n");
    printf("  [" COLOR_CYAN "0" COLOR_RESET "] I submit and negotiate out of fear.\n\n");
}

void execute_parametric_test(CharacterProfile* profile) {
    clear_screen();
    int choice;

    printf(COLOR_GOLD " =============================================================\n");
    printf("                THE TRIAL OF THE COSMIC ENTITY                \n");
    printf(" =============================================================\n\n" COLOR_RESET);
    printf(COLOR_WHITE "  A massive shadow splits the cloud lining. It looks down at you.\n");
    printf("  'Prove your essence, wastrel. Resolve my paradoxes.'\n\n" COLOR_RESET);
    printf(COLOR_DARK " [Press ANY KEY to begin the trial] " COLOR_RESET);
    _getch();

    // TRIAL 1
    clear_screen();
    printf(COLOR_GOLD "\n [TRIAL 1] The First Uprising (General of the Armies): You are in a great\n");
    printf(" war against a new, arrogant, and ruthless council seeking to seize the\n");
    printf(" heavens. You have been chosen as the military leader and supreme general\n");
    printf(" of your lineage. The enemy's celestial wrath is about to shatter your\n");
    printf(" frontlines, and your army wants to retreat. How do you handle this military\n");
    printf(" crisis and test of leadership?\n\n" COLOR_RESET);
    print_permanent_choices();
    printf(COLOR_CYAN " Instinct Vector (1-0): " COLOR_RESET);
    // ÖRNEK OLARAK TRIAL 1'İN PUANLAMA KISMI (Diğer 4 TRIAL'ı da buna göre değiştir)
    choice = get_parametric_input();
    if(choice == 1)  profile->intel += 2;
    else if(choice == 2)  profile->might += 2;
    else if(choice == 3)  profile->honor += 2;
    else if(choice == 4)  profile->skill += 2;
    else if(choice == 5)  profile->faith += 2;
    else if(choice == 6)  { profile->intel += 2; profile->honor -= 1; }
    else if(choice == 7)  { profile->might += 2; profile->intel -= 1; }
    else if(choice == 8)  { profile->skill += 2; profile->honor -= 2; }
    else if(choice == 9)  { profile->faith += 1; profile->might -= 1; }
    else if(choice == 10) { profile->honor += 1; profile->might -= 2; }

    // TRIAL 2
    clear_screen();
    printf(COLOR_GOLD "\n [TRIAL 2] The Condemnation Session (Stance Against the Tyrant): The war is\n");
    printf(" lost. The new tyrant of the apex has chained you and your entire lineage.\n");
    printf(" Before the eyes of all, he sentences you to a severe, eternal, and exemplary\n");
    printf(" exile. As you are banished to the darkest edge of the world, what will be\n");
    printf(" your stance against this new authority that condemns you?\n\n" COLOR_RESET);
    print_permanent_choices();
    printf(COLOR_CYAN " Instinct Vector (1-0): " COLOR_RESET);
    // ÖRNEK OLARAK TRIAL 1'İN PUANLAMA KISMI (Diğer 4 TRIAL'ı da buna göre değiştir)
    choice = get_parametric_input();
    if(choice == 1)  profile->intel += 2;
    else if(choice == 2)  profile->might += 2;
    else if(choice == 3)  profile->honor += 2;
    else if(choice == 4)  profile->skill += 2;
    else if(choice == 5)  profile->faith += 2;
    else if(choice == 6)  { profile->intel += 2; profile->honor -= 1; }
    else if(choice == 7)  { profile->might += 2; profile->intel -= 1; }
    else if(choice == 8)  { profile->skill += 2; profile->honor -= 2; }
    else if(choice == 9)  { profile->faith += 1; profile->might -= 1; }
    else if(choice == 10) { profile->honor += 1; profile->might -= 2; }

    // TRIAL 3
    clear_screen();
    printf(COLOR_GOLD "\n [TRIAL 3] The Borrowed Burden (Trust and Deception): A cunning mortal\n");
    printf(" stands before you, offering to briefly take over the eternal burden on your\n");
    printf(" shoulders. However, your instincts whisper that the moment you turn your back,\n");
    printf(" he will break his promise and flee, locking you in this dungeon forever.\n");
    printf(" How do you resolve this crisis of deception?\n\n" COLOR_RESET);
    print_permanent_choices();
    printf(COLOR_CYAN " Instinct Vector (1-0): " COLOR_RESET);
    // ÖRNEK OLARAK TRIAL 1'İN PUANLAMA KISMI (Diğer 4 TRIAL'ı da buna göre değiştir)
    choice = get_parametric_input();
    if(choice == 1)  profile->intel += 2;
    else if(choice == 2)  profile->might += 2;
    else if(choice == 3)  profile->honor += 2;
    else if(choice == 4)  profile->skill += 2;
    else if(choice == 5)  profile->faith += 2;
    else if(choice == 6)  { profile->intel += 2; profile->honor -= 1; }
    else if(choice == 7)  { profile->might += 2; profile->intel -= 1; }
    else if(choice == 8)  { profile->skill += 2; profile->honor -= 2; }
    else if(choice == 9)  { profile->faith += 1; profile->might -= 1; }
    else if(choice == 10) { profile->honor += 1; profile->might -= 2; }
    // TRIAL 4
    clear_screen();
    printf(COLOR_GOLD "\n [TRIAL 4] The Cursed Stranger (Theft of Destiny): An arrogant stranger\n");
    printf(" arrives at your door, demanding sanctuary. However, ancient prophecies have\n");
    printf(" whispered that this man will steal a priceless legacy from your bloodline.\n");
    printf(" Moreover, in his satchel, he carries the head of a cursed entity whose gaze\n");
    printf(" turns all living things to cold stone. How do you confront this danger?\n\n" COLOR_RESET);
    print_permanent_choices();
    printf(COLOR_CYAN " Instinct Vector (1-0): " COLOR_RESET);
    // ÖRNEK OLARAK TRIAL 1'İN PUANLAMA KISMI (Diğer 4 TRIAL'ı da buna göre değiştir)
    choice = get_parametric_input();
    if(choice == 1)  profile->intel += 2;
    else if(choice == 2)  profile->might += 2;
    else if(choice == 3)  profile->honor += 2;
    else if(choice == 4)  profile->skill += 2;
    else if(choice == 5)  profile->faith += 2;
    else if(choice == 6)  { profile->intel += 2; profile->honor -= 1; }
    else if(choice == 7)  { profile->might += 2; profile->intel -= 1; }
    else if(choice == 8)  { profile->skill += 2; profile->honor -= 2; }
    else if(choice == 9)  { profile->faith += 1; profile->might -= 1; }
    else if(choice == 10) { profile->honor += 1; profile->might -= 2; }
    // TRIAL 5
    clear_screen();
    printf(COLOR_GOLD "\n [TRIAL 5] Siege of the Sacred Garden (Defense of the Legacy): Your lineage's\n");
    printf(" greatest sanctuary, your most heavily guarded divine garden and the ancient\n");
    printf(" relics within, have been besieged by ambitious, foreign raiders. How do\n");
    printf(" you make your final move to protect your family and this deep-rooted legacy?\n\n" COLOR_RESET);
    print_permanent_choices();
    printf(COLOR_CYAN " Instinct Vector (1-0): " COLOR_RESET);
    // ÖRNEK OLARAK TRIAL 1'İN PUANLAMA KISMI (Diğer 4 TRIAL'ı da buna göre değiştir)
    choice = get_parametric_input();
    if(choice == 1)  profile->intel += 2;
    else if(choice == 2)  profile->might += 2;
    else if(choice == 3)  profile->honor += 2;
    else if(choice == 4)  profile->skill += 2;
    else if(choice == 5)  profile->faith += 2;
    else if(choice == 6)  { profile->intel += 2; profile->honor -= 1; }
    else if(choice == 7)  { profile->might += 2; profile->intel -= 1; }
    else if(choice == 8)  { profile->skill += 2; profile->honor -= 2; }
    else if(choice == 9)  { profile->faith += 1; profile->might -= 1; }
    else if(choice == 10) { profile->honor += 1; profile->might -= 2; }
    evaluate_cosmic_alignment(profile);

    // YENI MEKANİK BURADA DEVREYE GİRİYOR
    scene_class_selection(profile);
}

void evaluate_cosmic_alignment(CharacterProfile* profile) {
    clear_screen();
    int min_distance = 999;
    int distances[48];
    int match_count = 0;
    int tied_indices[48];

    // evaluate_cosmic_alignment içindeki for döngüsü:
    for (int i = 0; i < 48; i++) {
        int d = abs(profile->intel - database[i].intel) +
                abs(profile->might - database[i].might) +
                abs(profile->honor - database[i].honor) +
                abs(profile->skill - database[i].skill) +
                abs(profile->faith - database[i].faith);
        distances[i] = d;
        if (d < min_distance) {
            min_distance = d;
        }
    }

    for (int i = 0; i < 48; i++) {
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
        printf(COLOR_CYAN "  \"Aaaa, a child of %s [ %s ]... How shockingly predictable.\"\n\n" COLOR_RESET,
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

    printf("\n\n" COLOR_WHITE " [Press ANY KEY to continue to the Path of Destiny] " COLOR_RESET);
    _getch();
}

// ============================================================================
// 6. CLASS SELECTION & ANOMALY SYSTEM
// ============================================================================

void get_god_affinity_data(const char* god_name, Nature* nature, int* r, int* s, int* a, int* d, int* m) {
    if (strcmp(god_name, "Zeus") == 0)      { *nature = NATURE_AGGRESSIVE; *r=100; *s=70; *a=10; *d=60; *m=20; }
    else if (strcmp(god_name, "Hera") == 0) { *nature = NATURE_AGGRESSIVE; *r=95;  *s=40; *a=20; *d=80; *m=30; }
    else if (strcmp(god_name, "Hades") == 0){ *nature = NATURE_CALM;       *r=95;  *s=40; *a=10; *d=50; *m=80; }
    else if (strcmp(god_name, "Athena")== 0){ *nature = NATURE_CALM;       *r=85;  *s=95; *a=60; *d=70; *m=40; }
    else if (strcmp(god_name, "Ares") == 0) { *nature = NATURE_AGGRESSIVE; *r=30;  *s=100;*a=5;  *d=5;  *m=10; }
    else if (strcmp(god_name, "Poseidon")==0){*nature = NATURE_AGGRESSIVE; *r=80;  *s=85; *a=15; *d=30; *m=50; }
    else if (strcmp(god_name, "Apollo")== 0){ *nature = NATURE_CALM;       *r=60;  *s=70; *a=100;*d=60; *m=30; }
    else if (strcmp(god_name, "Hephaestus")==0){*nature = NATURE_CALM;     *r=20;  *s=50; *a=95; *d=10; *m=80; }
    else if (strcmp(god_name, "Aphrodite")==0){*nature= NATURE_AGGRESSIVE; *r=50;  *s=10; *a=90; *d=85; *m=50; }
    else if (strcmp(god_name, "Dionysus")== 0){*nature= NATURE_AGGRESSIVE; *r=30;  *s=20; *a=95; *d=60; *m=50; }
    else if (strcmp(god_name, "Hermes") == 0){*nature = NATURE_CALM;       *r=20;  *s=20; *a=40; *d=95; *m=100;}
    else if (strcmp(god_name, "Artemis") ==0){*nature = NATURE_CALM;       *r=30;  *s=85; *a=20; *d=10; *m=10; }
    else if (strcmp(god_name, "Demeter") ==0){*nature = NATURE_CALM;       *r=60;  *s=10; *a=30; *d=40; *m=80; }
    else { *nature = NATURE_UNASSIGNED; *r=50; *s=50; *a=50; *d=50; *m=50; } // Fallback
}

void scene_class_selection(CharacterProfile* profile) {
    clear_screen();
    int r, s, a, d, m;
    get_god_affinity_data(profile->god_alignment, &profile->god_nature, &r, &s, &a, &d, &m);

    // Determine if Pure or Cross
    profile->is_pure = (profile->player_nature == profile->god_nature);

    printf(COLOR_GOLD " =============================================================\n");
    printf("                    THE PATH OF DESTINY                       \n");
    printf(" =============================================================\n\n" COLOR_RESET);
    printf(COLOR_WHITE "  You bear the blood of %s. Now, claim your role in this world.\n\n" COLOR_RESET, profile->god_alignment);

    printf("  [" COLOR_CYAN "1" COLOR_RESET "] RULER    (Authority & Leadership)\n");
    printf("  [" COLOR_CYAN "2" COLOR_RESET "] SOLDIER  (War & Discipline)\n");
    printf("  [" COLOR_CYAN "3" COLOR_RESET "] ARTIST   (Inspiration & Craft)\n");
    printf("  [" COLOR_CYAN "4" COLOR_RESET "] DIPLOMAT (Communication & Negotiation)\n");
    printf("  [" COLOR_CYAN "5" COLOR_RESET "] MERCHANT (Economy & Resources)\n\n");
    printf(COLOR_CYAN " Select your societal class (1-5): " COLOR_RESET);

    char choice;
    bool valid = false;
    while (!valid) {
        if (_kbhit()) {
            choice = _getch();
            if (choice >= '1' && choice <= '5') {
                valid = true;
                switch(choice) {
                    case '1': profile->chosen_class = CLASS_RULER; strcpy(profile->class_name, "Ruler"); profile->affinity = r; break;
                    case '2': profile->chosen_class = CLASS_SOLDIER; strcpy(profile->class_name, "Soldier"); profile->affinity = s; break;
                    case '3': profile->chosen_class = CLASS_ARTIST; strcpy(profile->class_name, "Artist"); profile->affinity = a; break;
                    case '4': profile->chosen_class = CLASS_DIPLOMAT; strcpy(profile->class_name, "Diplomat"); profile->affinity = d; break;
                    case '5': profile->chosen_class = CLASS_MERCHANT; strcpy(profile->class_name, "Merchant"); profile->affinity = m; break;
                }
            }
        }
        Sleep(20);
    }

    calculate_final_title(profile);
}

void calculate_final_title(CharacterProfile* profile) {
    int aff = profile->affinity;
    bool pure = profile->is_pure;

    if (profile->chosen_class == CLASS_RULER) {
        if(aff >= 90) strcpy(profile->final_title, pure ? "Divine Sovereign" : "Dread Tyrant");
        else if(aff >= 50) strcpy(profile->final_title, pure ? "Just Monarch" : "Pragmatic Autocrat");
        else if(aff >= 10) strcpy(profile->final_title, pure ? "Visionary Guide" : "Chaotic Demagogue");
        else strcpy(profile->final_title, pure ? "Transcendent Overlord" : "Lord of Anarchy");
    }
    else if (profile->chosen_class == CLASS_SOLDIER) {
        if(aff >= 90) strcpy(profile->final_title, pure ? "Sword of War" : "Rider of the Apocalypse");
        else if(aff >= 50) strcpy(profile->final_title, pure ? "Honorable Commander" : "Ruthless Mercenary");
        else if(aff >= 10) strcpy(profile->final_title, pure ? "Unorthodox Tactician" : "Phantom Guerrilla");
        else strcpy(profile->final_title, pure ? "Peaceful Blademaster" : "Walking Calamity");
    }
    else if (profile->chosen_class == CLASS_ARTIST) {
        if(aff >= 90) strcpy(profile->final_title, pure ? "Cosmic Creator" : "Mind-Bending Genius");
        else if(aff >= 50) strcpy(profile->final_title, pure ? "Master Artisan" : "Avant-Garde Rebel");
        else if(aff >= 10) strcpy(profile->final_title, pure ? "Eccentric Surrealist" : "Provocative Illusionist");
        else strcpy(profile->final_title, pure ? "Bard of the Void" : "Reality Weaver");
    }
    else if (profile->chosen_class == CLASS_DIPLOMAT) {
        if(aff >= 90) strcpy(profile->final_title, pure ? "Divine Mediator" : "Mind Puppeteer");
        else if(aff >= 50) strcpy(profile->final_title, pure ? "Chief Negotiator" : "Shadow Broker");
        else if(aff >= 10) strcpy(profile->final_title, pure ? "Unpredictable Envoy" : "Chaos Whisperer");
        else strcpy(profile->final_title, pure ? "Silent Oracle" : "Concept Distorter");
    }
    else if (profile->chosen_class == CLASS_MERCHANT) {
        if(aff >= 90) strcpy(profile->final_title, pure ? "Golden Sovereign" : "Underworld Emperor");
        else if(aff >= 50) strcpy(profile->final_title, pure ? "Trade Baron" : "Black Market Cartel");
        else if(aff >= 10) strcpy(profile->final_title, pure ? "Enigmatic Collector" : "Fortune Smuggler");
        else strcpy(profile->final_title, pure ? "Weaver of Fates" : "Calamity Speculator");
    }

    clear_screen();
    printf(COLOR_RED " =============================================================\n");
    printf("                      YOUR TRUE FORM AWAKENS                  \n");
    printf(" =============================================================\n\n" COLOR_RESET);

    printf(COLOR_WHITE "  Bloodline Affinity : " COLOR_CYAN "%d%%\n" COLOR_RESET, aff);
    printf(COLOR_WHITE "  Mindset Alignment  : %s\n", pure ? COLOR_GOLD "PURE (Harmonious)" COLOR_RESET : COLOR_RED "CROSS (Anomalous)" COLOR_RESET);
    printf(COLOR_WHITE "  Destined Title     : " COLOR_GOLD "[ %s ]\n\n" COLOR_RESET, profile->final_title);

    printf(COLOR_DARK " [Press ANY KEY to finalize your destiny] " COLOR_RESET);
    _getch();

    // YENİ: Unvanı aldıktan sonra ismini sor ve karakter kağıdını bas!
    display_character_sheet(profile);
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

    printf("\n  --- ASSIGNED GOD & DESTINY MATRICES ---\n");
    printf("  * God Alignment      : " COLOR_GOLD "%s\n" COLOR_RESET, profile->god_alignment);
    printf("  * Final Class        : " COLOR_CYAN "%s\n" COLOR_RESET, profile->class_name);
    printf("  * Blood Affinity     : %d%%\n", profile->affinity);
    printf("  * Ultimate Title     : " COLOR_GOLD "%s\n" COLOR_RESET, profile->final_title);

    printf("\n  --- ACTIVE 5-PARAMETRIC ATTRIBUTES ---\n");
    printf("  * [INT] Intelligence (Logic) : %d points\n", profile->intel);
    printf("  * [MGT] Might (Power)        : %d points\n", profile->might);
    printf("  * [HNR] Honor (Sacrifice)    : %d points\n", profile->honor);
    printf("  * [SKL] Personal Skills      : %d points\n", profile->skill);
    printf("  * [FTH] Celestial Faith      : %d points\n", profile->faith);
    printf("  -------------------------------------------------------------\n\n");
    printf("  -------------------------------------------------------------\n\n");
    printf(COLOR_WHITE " [Press ANY KEY to return safely to the Storm Menu] " COLOR_RESET);
    _getch();
    clear_screen();
}

// ============================================================================
// 7. AETHERIAL DOSSIER (CHARACTER SHEET)
// ============================================================================

void print_stat_bar(int value, int max_val, const char* color) {
    printf("%s", color);
    for(int i = 0; i < max_val; i++) {
        if(i < value) printf("█"); // Dolu blok (UTF-8)
        else printf(COLOR_DARK "░"); // Boş blok (UTF-8)
        printf("%s", color); // Rengi sıfırla/koru
    }
    printf(COLOR_RESET);
}

void display_character_sheet(CharacterProfile* profile) {
    clear_screen();

    char buffer[256];
    char buf1[10], buf2[10];
    int vis_len;
    int bar1, bar2;

    // Dinamik Tag Sistemi (Array ve Uzunluk Takibi ile)
    const char* active_tags[20];
    int active_tags_len[20];
    int tag_count = 0;

    if (profile->affinity >= 80) { active_tags[tag_count] = COLOR_CYAN "< DIVINE_RESONANCE >" COLOR_RESET; active_tags_len[tag_count++] = 20; }
    else if (profile->affinity <= 30) { active_tags[tag_count] = COLOR_RED "< SEVERE_ANOMALY >" COLOR_RESET; active_tags_len[tag_count++] = 18; }

    if (profile->is_pure) { active_tags[tag_count] = COLOR_GOLD "< HARMONIC_SOUL >" COLOR_RESET; active_tags_len[tag_count++] = 17; }
    else { active_tags[tag_count] = COLOR_RED "< FRACTURED_MIND >" COLOR_RESET; active_tags_len[tag_count++] = 18; }

    if (profile->intel >= 6) { active_tags[tag_count] = COLOR_WHITE "< TACTICAL_GENIUS >" COLOR_RESET; active_tags_len[tag_count++] = 19; }
    if (profile->might >= 6) { active_tags[tag_count] = COLOR_WHITE "< OVERWHELMING_FORCE >" COLOR_RESET; active_tags_len[tag_count++] = 22; }
    if (profile->skill >= 6) { active_tags[tag_count] = COLOR_WHITE "< MASTER_OF_CRAFTS >" COLOR_RESET; active_tags_len[tag_count++] = 20; }
    if (profile->honor >= 6) { active_tags[tag_count] = COLOR_WHITE "< UNBREAKABLE_WILL >" COLOR_RESET; active_tags_len[tag_count++] = 20; }
    if (profile->faith == 0) { active_tags[tag_count] = COLOR_DARK "< GODLESS_ENTITY >" COLOR_RESET; active_tags_len[tag_count++] = 18; }

    printf(COLOR_DARK "\n   ╒════════════════════════════════════════════════════════════════════════╕\n");

    // Header Satırı
    sprintf(buffer, " SYS-INIT :: AETHERIAL DOSSIER");
    vis_len = strlen(buffer) + 13; // 12 karakter [ v. 1.0.4 ] için
    printf(COLOR_DARK "   │" COLOR_WHITE "%s", buffer);
    for(int i = 0; i < 72 - vis_len; i++) printf(" ");
    printf(COLOR_DARK "[ v. 1.0.4 ] │\n");

    printf("   ╞════════════════════════════════════════════════════════════════════════╡\n");
    printf("   │                                                                        │\n" COLOR_RESET);

    // ID Satırı
    sprintf(buffer, "%s (The %s)", profile->player_name, profile->final_title);
    vis_len = 23 + strlen(buffer);
    printf(COLOR_DARK "   │  " COLOR_CYAN "[ID]" COLOR_RESET " DESIGNATION   : " COLOR_WHITE "%s" COLOR_DARK, buffer);
    for(int i = 0; i < 72 - vis_len; i++) printf(" ");
    printf("│\n");

    // Bloodline Satırı
    sprintf(buffer, "%s - %s", profile->god_alignment, profile->archetype_alignment);
    vis_len = 23 + strlen(buffer);
    printf(COLOR_DARK "   │  " COLOR_CYAN "[BL]" COLOR_RESET " BLOODLINE     : " COLOR_GOLD "%s" COLOR_DARK, buffer);
    for(int i = 0; i < 72 - vis_len; i++) printf(" ");
    printf("│\n");

    // Caste Satırı
    vis_len = 23 + strlen(profile->class_name);
    printf(COLOR_DARK "   │  " COLOR_CYAN "[CL]" COLOR_RESET " CASTE         : " COLOR_WHITE "%s" COLOR_DARK, profile->class_name);
    for(int i = 0; i < 72 - vis_len; i++) printf(" ");
    printf("│\n");

    printf("   │                                                                        │\n");
    printf("   │  ====================== [ " COLOR_CYAN "SYNCHRONIZATION" COLOR_DARK " ] =========================  │\n");

    // Affinity Satırı
    sprintf(buffer, "%d%%", profile->affinity);
    vis_len = 20 + strlen(buffer);
    printf("   │  " COLOR_RESET "AFFINITY LEVEL :  %s" COLOR_DARK, buffer);
    for(int i = 0; i < 72 - vis_len; i++) printf(" ");
    printf("│\n");

    // Mindset Satırı
    const char* m_str = profile->is_pure ? "PURE (Harmonious)" : "CROSS (Anomalous)";
    const char* m_col = profile->is_pure ? COLOR_GOLD : COLOR_RED;
    vis_len = 20 + strlen(m_str);
    printf("   │  " COLOR_RESET "MINDSET STATE  :  %s%s" COLOR_DARK, m_col, m_str);
    for(int i = 0; i < 72 - vis_len; i++) printf(" ");
    printf("│\n");

    printf("   │                                                                        │\n");
    printf("   │  ====================== [ " COLOR_CYAN "PARAMETRIC VECTORS" COLOR_DARK " ] ======================  │\n");

    // Stat Satırı 1: INTEL & HONOR
    sprintf(buf1, "%2d", profile->intel);
    sprintf(buf2, "%2d", profile->honor);
    bar1 = profile->intel < 0 ? 0 : (profile->intel > 10 ? 10 : profile->intel);
    bar2 = profile->honor < 0 ? 0 : (profile->honor > 10 ? 10 : profile->honor);
    printf("   │  " COLOR_RESET "[INT] INTEL : %s  ", buf1);
    print_stat_bar(bar1, 10, COLOR_CYAN);
    printf(COLOR_DARK "  │   " COLOR_RESET "[HNR] HONOR : %s  ", buf2);
    print_stat_bar(bar2, 10, COLOR_GOLD);
    vis_len = 60 + strlen(buf1) + strlen(buf2);
    for(int i = 0; i < 72 - vis_len; i++) printf(" ");
    printf(COLOR_DARK "│\n");

    // Stat Satırı 2: MIGHT & SKILL
    sprintf(buf1, "%2d", profile->might);
    sprintf(buf2, "%2d", profile->skill);
    bar1 = profile->might < 0 ? 0 : (profile->might > 10 ? 10 : profile->might);
    bar2 = profile->skill < 0 ? 0 : (profile->skill > 10 ? 10 : profile->skill);
    printf(COLOR_DARK "   │  " COLOR_RESET "[MGT] MIGHT : %s  ", buf1);
    print_stat_bar(bar1, 10, COLOR_RED);
    printf(COLOR_DARK "  │   " COLOR_RESET "[SKL] SKILL : %s  ", buf2);
    print_stat_bar(bar2, 10, COLOR_WHITE);
    vis_len = 60 + strlen(buf1) + strlen(buf2);
    for(int i = 0; i < 72 - vis_len; i++) printf(" ");
    printf(COLOR_DARK "│\n");

    // Stat Satırı 3: FAITH
    sprintf(buf1, "%2d", profile->faith);
    bar1 = profile->faith < 0 ? 0 : (profile->faith > 10 ? 10 : profile->faith);
    printf(COLOR_DARK "   │  " COLOR_RESET "[FTH] FAITH : %s  ", buf1);
    print_stat_bar(bar1, 10, COLOR_CYAN);
    printf(COLOR_DARK "  │");
    vis_len = 31 + strlen(buf1);
    for(int i = 0; i < 72 - vis_len; i++) printf(" ");
    printf("│\n");

    printf("   │                                                                        │\n");
    printf("   │  ====================== [ " COLOR_CYAN "ACTIVE ANOMALIES" COLOR_DARK " ] ========================  │\n");

    // Tag Çizimi (Taşmaları önleyen otomatik alt satıra geçme algoritması)
    int current_line_len = 0;
    printf("   │  " COLOR_RESET);
    current_line_len = 2;

    if (tag_count == 0) {
        printf(COLOR_DARK "NO ANOMALIES DETECTED");
        current_line_len += 21;
    } else {
        for (int i = 0; i < tag_count; i++) {
            if (current_line_len + active_tags_len[i] + 2 > 70 && current_line_len > 2) {
                for (int p = 0; p < 72 - current_line_len; p++) printf(" ");
                printf(COLOR_DARK "│\n   │  " COLOR_RESET);
                current_line_len = 2;
            }
            printf("%s  ", active_tags[i]);
            current_line_len += active_tags_len[i] + 2;
        }
    }
    // Son satırı kapat
    for (int p = 0; p < 72 - current_line_len; p++) printf(" ");
    printf(COLOR_DARK "│\n");

    printf("   ╘════════════════════════════════════════════════════════════════════════╛\n\n" COLOR_RESET);

    printf(COLOR_DARK "   [System Sealed. Press ANY KEY to enter the Overworld Menu] " COLOR_RESET);
    _getch();
    clear_screen();
}
