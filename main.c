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

// Global Language Flag (0: English, 1: Turkish)
int current_lang = 0;

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
    char archetype_alignment_tr[50]; // Turkish archetype
    StoryState story;

    // Mindset & Anomaly System
    Nature player_nature;
    Nature god_nature;
    ClassType chosen_class;
    char class_name[30];
    char class_name_tr[30]; // Turkish class name
    int affinity;
    bool is_pure;
    char final_title[100];
    char final_title_tr[100]; // Turkish title

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
    char archetype_tr[50]; // Turkish Translation added
    int intel, might, honor, skill, faith;
} ArchetypeMatrix;

// Global matrix of 48 calibrated unique historical profiles (Now with Turkish Translations)
ArchetypeMatrix database[48] = {
    {"Zeus", "The Absolute Sky Sovereign", "Mutlak Gökyüzü Hükümdarı", 0, 5, 5, 0, 0},
    {"Zeus", "The Cloud Arbiter", "Bulutların Hakemi", 4, 1, 5, 0, 0},
    {"Zeus", "The Thunder Wrath", "Yıldırım Öfkesi", 0, 7, 0, 0, 3},
    {"Zeus", "The Hospitality Oathkeeper", "Misafirperverlik Yeminlisi", 2, 0, 6, 0, 2},
    {"Poseidon", "The Earth-Shaker", "Sarsılmaz Yeryüzü Titreten", 0, 10, 0, 0, 0},
    {"Poseidon", "The Abyssal Master", "Uçurumun Efendisi", 1, 3, 0, 0, 6},
    {"Poseidon", "The Wild Equine Tamer", "Vahşi At Terbiyecisi", 0, 5, 0, 5, 0},
    {"Poseidon", "The Tidal Ravager", "Gelgit Yıkıcısı", 2, 6, 0, 0, 1},
    {"Athena", "The Supreme Tactician", "Yüce Taktisyen", 8, 0, 2, 0, 0},
    {"Athena", "The Citadel Protector", "Hisar Koruyucusu", 3, 2, 5, 0, 0},
    {"Athena", "The Divine Weaver", "İlahi Dokumacı", 5, 0, 0, 5, 0},
    {"Athena", "The Counselor of Heroes", "Kahramanların Danışmanı", 6, 0, 3, 1, 0},
    {"Athena", "The Just Judgment", "Adil Yargı", 5, 0, 4, 0, 1},
    {"Hades", "The Underworld Autocrat", "Yeraltı Otokratı", 3, 2, 5, 0, 0},
    {"Hades", "The Lord of Wealth", "Zenginlik Lordu", 4, 0, 2, 4, 0},
    {"Hades", "The Soul Collector", "Ruh Toplayıcı", 0, 3, 0, 0, 7},
    {"Hades", "The Inexorable Judge", "Tavizsiz Yargıç", 4, 1, 4, 0, 1},
    {"Ares", "The Blind Berserker", "Kör Gözü Dönmüş Savaşçı", 0, 9, 0, 1, 0},
    {"Ares", "The Bloodlust Vanguard", "Kana Susamış Öncü", 0, 6, 0, 4, 0},
    {"Ares", "The Rebel Warlord", "İsyankar Savaş Lordu", 0, 7, 0, 0, 0},
    {"Ares", "The Dread Champion", "Dehşet Şampiyonu", 0, 5, 1, 0, 4},
    {"Apollo", "The Radiant Prophet", "Parlak Kahin", 0, 0, 0, 0, 10},
    {"Apollo", "The Sun Bowman", "Güneş Okçusu", 1, 0, 0, 8, 1},
    {"Apollo", "The Golden Maestro", "Altın Maestro", 3, 0, 5, 2, 0},
    {"Apollo", "The Plague Bringer", "Veba Getiren", 4, 0, 0, 3, 3},
    {"Artemis", "The Midnight Stalker", "Gece Yarısı Avcısı", 0, 0, 0, 9, 1},
    {"Artemis", "The Forest Matriarch", "Orman Anaerkili", 1, 0, 0, 2, 7},
    {"Artemis", "The Silver Arrow", "Gümüş Ok", 0, 2, 0, 6, 0},
    {"Hephaestus", "The Forgemaster Architect", "Usta Demirci Mimar", 2, 0, 0, 8, 0},
    {"Hephaestus", "The Volcanic Juggernaut", "Volkanik Dev", 0, 6, 0, 4, 0},
    {"Hephaestus", "The Outcast Innovator", "Dışlanmış Yenilikçi", 4, 0, 0, 2, 0},
    {"Hephaestus", "The Iron Trapmaster", "Demir Tuzak Ustası", 3, 1, 0, 6, 0},
    {"Hermes", "The Phantom Thief", "Hayalet Hırsız", 0, 0, 0, 10, 0},
    {"Hermes", "The Golden Herald", "Altın Haberci", 5, 0, 3, 2, 0},
    {"Hermes", "The Crossroads Guide", "Kavşak Rehberi", 2, 0, 0, 3, 5},
    {"Hermes", "The Divine Trickster", "İlahi Düzenbaz", 6, 0, 0, 4, 0},
    {"Dionysus", "The Madness Bringer", "Delilik Getiren", 0, 0, 0, 0, 9},
    {"Dionysus", "The Hedonist Vagabond", "Hedonist Serseri", 3, 0, 0, 2, 3},
    {"Dionysus", "The Cult Liberator", "Kült Kurtarıcısı", 0, 0, 0, 0, 5},
    {"Hera", "The Zenith Matriarch", "Zirve Kraliçesi", 0, 0, 10, 0, 0},
    {"Hera", "The Palace Intriguer", "Saray Entrikacısı", 6, 0, 4, 0, 0},
    {"Hera", "The Royal Avenger", "Kraliyet İntikamcısı", 2, 2, 5, 1, 0},
    {"Demeter", "The Eternal Harvester", "Ebedi Hasatçı", 0, 0, 0, 5, 5},
    {"Demeter", "The Winter Famine", "Kış Kıtlığı", 0, 5, 0, 0, 5},
    {"Demeter", "The Earth Mother", "Toprak Ana", 0, 0, 2, 0, 6},
    {"Aphrodite", "The Siren Puppetmaster", "Siren Kukla Ustası", 1, 0, 0, 1, 8},
    {"Aphrodite", "The Golden Presence", "Altın Varlık", 0, 0, 8, 0, 2},
    {"Aphrodite", "The Loom of Desire", "Arzunun Dokuma Tezgahı", 4, 0, 0, 4, 1}
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
void scene_continue_journey(CharacterProfile* profile);
void scene_language_options(void);
void scene_system_status(CharacterProfile* profile);
void display_character_sheet(CharacterProfile* profile);
void scene_continue_journey(CharacterProfile* profile); // Parametre eklendi
void scene_map(CharacterProfile* profile);              // Yeni harita fonksiyonu eklendi
void scene_inner_shrine(CharacterProfile* profile);


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
        .archetype_alignment_tr = "ATANMADI",
        .story = STORY_UNASSIGNED,
        .player_nature = NATURE_UNASSIGNED,
        .god_nature = NATURE_UNASSIGNED,
        .chosen_class = CLASS_UNASSIGNED,
        .affinity = 0,
        .is_pure = false,
        .class_name = "UNASSIGNED",
        .class_name_tr = "ATANMADI",
        .final_title = "UNASSIGNED",
        .final_title_tr = "ATANMADI",
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
                    player.intel = 0; player.might = 0; player.honor = 0; player.skill = 0; player.faith = 0; player.affinity = 0;
                    strcpy(player.god_alignment, "UNASSIGNED");
                    strcpy(player.class_name, "UNASSIGNED");
                    strcpy(player.class_name_tr, "ATANMADI");

                    if (!scene_start_journey(&player)) running = false;
                    set_cursor_visibility(false);
                    break;
                case '2':
                    set_cursor_visibility(true);
                    scene_continue_journey(&player);
                    set_cursor_visibility(false);
                    break;
                case '3': scene_language_options(); break;
                case '4':
                    clear_screen();
                    if (current_lang == 1) printf(COLOR_WHITE "\n \"Kaderinden kaçamazsın; sadece onu geciktirebilirsin.\"\n" COLOR_RESET);
                    else printf(COLOR_WHITE "\n \"You cannot escape your destiny; you can only delay it.\"\n" COLOR_RESET);
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

    if (current_lang == 1) { // Turkish Menu
        printf("   [%s1%s] YOLCULUĞA BAŞLA\033[K\n", option_color, border_color);
        printf("   [%s2%s] YOLCULUĞA DEVAM ET\033[K\n", option_color, border_color);
        printf("   [%s3%s] DİL SEÇENEKLERİ\033[K\n", option_color, border_color);
        printf("   [%s4%s] KADERDEN KAÇ (Çıkış)\033[K\n", option_color, border_color);
        printf("   -------------------------------------------------------------\033[K\n");
        printf("   [%s0%s] Sistem Durumu & Matris Doğrulama\033[K\n", title_color, border_color);
        printf("   -------------------------------------------------------------\033[K\n\n");
        printf("%s Hedef Eylem (0-4): \033[K" COLOR_RESET, option_color);
    } else { // English Menu
        printf("   [%s1%s] START JOURNEY\033[K\n", option_color, border_color);
        printf("   [%s2%s] CONTINUE JOURNEY\033[K\n", option_color, border_color);
        printf("   [%s3%s] LANGUAGE OPTIONS\033[K\n", option_color, border_color);
        printf("   [%s4%s] ESCAPE DESTINY (Exit Game)\033[K\n", option_color, border_color);
        printf("   -------------------------------------------------------------\033[K\n");
        printf("   [%s0%s] System Status & Matrix Verification\033[K\n", title_color, border_color);
        printf("   -------------------------------------------------------------\033[K\n\n");
        printf("%s Target Action (0-4): \033[K" COLOR_RESET, option_color);
    }
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

    if (current_lang == 1) printf(COLOR_DARK "                     [Gözlerini açmak için HERHANGİ BİR TUŞA bas] " COLOR_RESET);
    else printf(COLOR_DARK "                     [Press ANY KEY to open your eyes] " COLOR_RESET);
    _getch(); clear_screen();

    if (current_lang == 1) {
        printf(COLOR_WHITE "\n\n  UYAN... " COLOR_RESET); printf(COLOR_RED "Yarının yok.\n\n" COLOR_RESET);
        printf(COLOR_CYAN "  Düşüşten önceki adın neydi? : " COLOR_RESET);
    } else {
        printf(COLOR_WHITE "\n\n  AWAKE... " COLOR_RESET); printf(COLOR_RED "You have no tomorrow.\n\n" COLOR_RESET);
        printf(COLOR_CYAN "  What was your name before the fall? : " COLOR_RESET);
    }

    set_cursor_visibility(true);
    scanf(" %[^\n]", profile->player_name);
    set_cursor_visibility(false);
    clear_screen();

    if (current_lang == 1) {
        printf(COLOR_GOLD "\n\n  Şimdi ayağa kalk, %s. Ya da çağrıma kulaklarını tıka ve bu yola hiç adım atma.\n\n\n" COLOR_RESET, profile->player_name);
        printf(COLOR_DARK " [Ültimatomla yüzleşmek için HERHANGİ BİR TUŞA bas] " COLOR_RESET);
    } else {
        printf(COLOR_GOLD "\n\n  Rise now, %s. Or blind your ears to my call and never set foot upon this path.\n\n\n" COLOR_RESET, profile->player_name);
        printf(COLOR_DARK " [Press ANY KEY to confront the ultimatum] " COLOR_RESET);
    }
    _getch(); clear_screen();

    printf(COLOR_RED "\n  #############################################################################\n");
    if (current_lang == 1) printf("  #                               K  A  Ç  ! ! !                              #\n");
    else printf("  #                               F  L  E  E  ! ! !                           #\n");
    printf("  #############################################################################\n\n\n" COLOR_RESET);

    if (current_lang == 1) {
        printf("  [" COLOR_CYAN "1" COLOR_RESET "] Sessizce gölgelere kaç.\n");
        printf("  [" COLOR_CYAN "2" COLOR_RESET "] Donakal ve fırtınaya sessizce dayan (Sakin gözlem).\n");
        printf("  [" COLOR_CYAN "3" COLOR_RESET "] Gökklere kükre ve yere vur (Şiddetli meydan okuma).\n\n");
        printf(COLOR_CYAN " İçgüdünü Uygula (1-3): " COLOR_RESET);
    } else {
        printf("  [" COLOR_CYAN "1" COLOR_RESET "] Escape silently into the shadows (Run away).\n");
        printf("  [" COLOR_CYAN "2" COLOR_RESET "] Stand frozen and endure the tempest silently (Calm observation).\n");
        printf("  [" COLOR_CYAN "3" COLOR_RESET "] Roar back at the heavens and strike the ground (Fierce defiance).\n\n");
        printf(COLOR_CYAN " Execute Instinct (1-3): " COLOR_RESET);
    }

    while (!valid_input) {
        if (_kbhit()) {
            choice = _getch();
            if (choice >= '1' && choice <= '3') valid_input = true;
        }
        Sleep(20);
    }

    if (choice == '1') {
        clear_screen();
        if (current_lang == 1) {
            printf(COLOR_RED "\n\n  Bilinmezliği seçtin. Kozmik boşluk anlamsız bağını geri alıyor.\n\n" COLOR_RESET);
            printf(COLOR_DARK " [Çıkmak için HERHANGİ BİR TUŞA bas] " COLOR_RESET);
        } else {
            printf(COLOR_RED "\n\n  You chose obscurity. The cosmic void reclaims your meaningless thread.\n\n" COLOR_RESET);
            printf(COLOR_DARK " [Press ANY KEY to exit game] " COLOR_RESET);
        }
        _getch();
        return false;
    }
    else if (choice == '2') {
        profile->story = STORY_UNCROWNED; profile->player_nature = NATURE_CALM;
    }
    else if (choice == '3') {
        profile->story = STORY_TRADITIONAL; profile->player_nature = NATURE_AGGRESSIVE;
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
    if (current_lang == 1) {
        printf("  [" COLOR_CYAN "1" COLOR_RESET "] Bunu doğrudan zekamla çözerim.\n");
        printf("  [" COLOR_CYAN "2" COLOR_RESET "] Sadece kendi gücüme güvenirim.\n");
        printf("  [" COLOR_CYAN "3" COLOR_RESET "] Onurum için her şeyi feda ederim.\n");
        printf("  [" COLOR_CYAN "4" COLOR_RESET "] Kendi yeteneklerime güvenirim.\n");
        printf("  [" COLOR_CYAN "5" COLOR_RESET "] İlahi adalete olan sarsılmaz inancımla üstesinden gelirim.\n");
        printf("  [" COLOR_CYAN "6" COLOR_RESET "] Kurnazca ve gizlilikle çözerim.\n");
        printf("  [" COLOR_CYAN "7" COLOR_RESET "] Vahşice ve sınır tanımadan saldırırım.\n");
        printf("  [" COLOR_CYAN "8" COLOR_RESET "] Kuralları yıkar ve kirli oynarım.\n");
        printf("  [" COLOR_CYAN "9" COLOR_RESET "] Kendimi tamamen kozmik düzenin iradesine bırakırım.\n");
        printf("  [" COLOR_CYAN "0" COLOR_RESET "] Korkudan boyun eğer ve müzakere ederim.\n\n");
    } else {
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
}

void print_trial_header(int trial_num, const char* title_en, const char* title_tr, const char* desc_en, const char* desc_tr) {
    clear_screen();
    if (current_lang == 1) {
        printf(COLOR_GOLD "\n [SINAV %d] %s: %s\n\n" COLOR_RESET, trial_num, title_tr, desc_tr);
    } else {
        printf(COLOR_GOLD "\n [TRIAL %d] %s: %s\n\n" COLOR_RESET, trial_num, title_en, desc_en);
    }
    print_permanent_choices();
    if (current_lang == 1) printf(COLOR_CYAN " İçgüdü Vektörü (1-0): " COLOR_RESET);
    else printf(COLOR_CYAN " Instinct Vector (1-0): " COLOR_RESET);
}

void apply_trial_points(CharacterProfile* profile, int choice) {
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
}

void execute_parametric_test(CharacterProfile* profile) {
    clear_screen();
    int choice;

    if (current_lang == 1) {
        printf(COLOR_GOLD " =============================================================\n");
        printf("                    KOZMİK VARLIĞIN SINAVI                    \n");
        printf(" =============================================================\n\n" COLOR_RESET);
        printf(COLOR_WHITE "  Büyük bir gölge bulutların arasından ayrılıp sana bakıyor.\n");
        printf("  'Özünü kanıtla, sefil. Paradokslarımı çöz.'\n\n" COLOR_RESET);
        printf(COLOR_DARK " [Sınava başlamak için HERHANGİ BİR TUŞA bas] " COLOR_RESET);
    } else {
        printf(COLOR_GOLD " =============================================================\n");
        printf("                THE TRIAL OF THE COSMIC ENTITY                \n");
        printf(" =============================================================\n\n" COLOR_RESET);
        printf(COLOR_WHITE "  A massive shadow splits the cloud lining. It looks down at you.\n");
        printf("  'Prove your essence, wastrel. Resolve my paradoxes.'\n\n" COLOR_RESET);
        printf(COLOR_DARK " [Press ANY KEY to begin the trial] " COLOR_RESET);
    }
    _getch();

    // TRIAL 1
    print_trial_header(1,
        "The First Uprising", "İlk Ayaklanma",
        "You are in a great war against a new, arrogant, and ruthless council seeking to seize the heavens. You have been chosen as the military leader and supreme general of your lineage. The enemy's celestial wrath is about to shatter your frontlines, and your army wants to retreat. How do you handle this military crisis and test of leadership?",
        "Gökyüzünü ele geçirmek isteyen yeni, kibirli ve acımasız bir konseye karşı büyük bir savaştasınız. Soyunun askeri lideri ve başkomutanı seçildin. Düşmanın ilahi gazabı cepheni dağıtmak üzere ve ordun geri çekilmek istiyor. Bu askeri krizi ve liderlik sınavını nasıl yönetirsin?");
    choice = get_parametric_input(); apply_trial_points(profile, choice);

    // TRIAL 2
    print_trial_header(2,
        "The Condemnation Session", "Kınama Celsesi",
        "The war is lost. The new tyrant of the apex has chained you and your entire lineage. Before the eyes of all, he sentences you to a severe, eternal, and exemplary exile. As you are banished to the darkest edge of the world, what will be your stance against this new authority that condemns you?",
        "Savaş kaybedildi. Zirvenin yeni tiranı seni ve tüm soyunu zincirledi. Herkesin gözü önünde seni ağır, sonsuz ve ibretlik bir sürgüne mahkum ediyor. Dünyanın en karanlık köşesine sürülürken, seni mahkum eden bu yeni otoriteye karşı duruşun ne olacak?");
    choice = get_parametric_input(); apply_trial_points(profile, choice);

    // TRIAL 3
    print_trial_header(3,
        "The Borrowed Burden", "Ödünç Alınan Yük",
        "A cunning mortal stands before you, offering to briefly take over the eternal burden on your shoulders. However, your instincts whisper that the moment you turn your back, he will break his promise and flee, locking you in this dungeon forever. How do you resolve this crisis of deception?",
        "Kurnaz bir ölümlü karşında duruyor ve omuzlarındaki ebedi yükü kısa bir süreliğine devralmayı teklif ediyor. Ancak içgüdülerin sana, arkanı döndüğün an sözünden döneceğini ve kaçarak seni bu zindana sonsuza dek kilitleyeceğini fısıldıyor. Bu aldatmaca krizini nasıl çözersin?");
    choice = get_parametric_input(); apply_trial_points(profile, choice);

    // TRIAL 4
    print_trial_header(4,
        "The Cursed Stranger", "Lanetli Yabancı",
        "An arrogant stranger arrives at your door, demanding sanctuary. However, ancient prophecies have whispered that this man will steal a priceless legacy from your bloodline. Moreover, in his satchel, he carries the head of a cursed entity whose gaze turns all living things to cold stone. How do you confront this danger?",
        "Kibirli bir yabancı kapına geliyor ve sığınma talep ediyor. Ancak kadim kehanetler, bu adamın soyundan paha biçilmez bir mirası çalacağını fısıldadı. Dahası, çantasında bakışlarıyla tüm canlıları soğuk taşa çeviren lanetli bir varlığın başını taşıyor. Bu tehlikeyle nasıl yüzleşirsin?");
    choice = get_parametric_input(); apply_trial_points(profile, choice);

    // TRIAL 5
    print_trial_header(5,
        "Siege of the Sacred Garden", "Kutsal Bahçe Kuşatması",
        "Your lineage's greatest sanctuary, your most heavily guarded divine garden and the ancient relics within, have been besieged by ambitious, foreign raiders. How do you make your final move to protect your family and this deep-rooted legacy?",
        "Soyunun en büyük sığınağı, en sıkı korunan ilahi bahçen ve içindeki kadim emanetler hırslı, yabancı akıncılar tarafından kuşatıldı. Aileni ve bu köklü mirası korumak için son hamleni nasıl yaparsın?");
    choice = get_parametric_input(); apply_trial_points(profile, choice);

    evaluate_cosmic_alignment(profile);
    scene_class_selection(profile);
}

void evaluate_cosmic_alignment(CharacterProfile* profile) {
    clear_screen();
    int min_distance = 999;
    int distances[48];
    int match_count = 0;
    int tied_indices[48];

    for (int i = 0; i < 48; i++) {
        int d = abs(profile->intel - database[i].intel) +
                abs(profile->might - database[i].might) +
                abs(profile->honor - database[i].honor) +
                abs(profile->skill - database[i].skill) +
                abs(profile->faith - database[i].faith);
        distances[i] = d;
        if (d < min_distance) min_distance = d;
    }

    for (int i = 0; i < 48; i++) {
        if (distances[i] == min_distance) {
            tied_indices[match_count] = i;
            match_count++;
        }
    }

    if (current_lang == 1) {
        printf(COLOR_GOLD " =============================================================\n");
        printf("                     KOZMİK HÜKÜM                        \n");
        printf(" =============================================================\n\n" COLOR_RESET);
    } else {
        printf(COLOR_GOLD " =============================================================\n");
        printf("                    THE COSMIC VERDICT                        \n");
        printf(" =============================================================\n\n" COLOR_RESET);
    }

    if (min_distance == 0) {
        int idx = tied_indices[0];
        strcpy(profile->god_alignment, database[idx].god);
        strcpy(profile->archetype_alignment, database[idx].archetype);
        strcpy(profile->archetype_alignment_tr, database[idx].archetype_tr);

        if (current_lang == 1) {
            printf(COLOR_WHITE "  Kozmik Varlık devasa gözlerini kısıyor, alaycı bir şekilde gülüyor:\n\n" COLOR_RESET);
            printf(COLOR_CYAN "  \"Aaaa, %s'un bir çocuğu [ %s ]... Ne kadar şok edici derecede tahmin edilebilir.\"\n\n" COLOR_RESET,
                   profile->god_alignment, profile->archetype_alignment_tr);
        } else {
            printf(COLOR_WHITE "  The Cosmic Entity narrows its massive eyes, laughing sarcastically:\n\n" COLOR_RESET);
            printf(COLOR_CYAN "  \"Aaaa, a child of %s [ %s ]... How shockingly predictable.\"\n\n" COLOR_RESET,
                   profile->god_alignment, profile->archetype_alignment);
        }
    }
    else {
        int primary_idx = tied_indices[0];
        strcpy(profile->god_alignment, database[primary_idx].god);
        strcpy(profile->archetype_alignment, database[primary_idx].archetype);
        strcpy(profile->archetype_alignment_tr, database[primary_idx].archetype_tr);

        if (current_lang == 1) {
            printf(COLOR_WHITE "  Kozmik Varlık boyut matrisi üzerinden inceliyor:\n\n" COLOR_RESET);
            printf("  \"Bana daha çok " COLOR_CYAN "%s [ %s ]" COLOR_RESET " çocuğuna benziyorsun.\"\n\n",
                   profile->god_alignment, profile->archetype_alignment_tr);

            if (match_count > 1) {
                printf(COLOR_WHITE "  Yine de kozmik frekanslar çarpık. Gerçek soy alternatifini seç:\n\n" COLOR_RESET);
                printf("  [1] Varlığın görüşünü kabul et (%s - %s)\n", database[primary_idx].god, database[primary_idx].archetype_tr);

                for (int i = 1; i < match_count; i++) {
                    int idx = tied_indices[i];
                    printf("  [%d] Aslında ben %s [ %s ] çocuğuyum\n", i + 1, database[idx].god, database[idx].archetype_tr);
                }

                printf("\n" COLOR_CYAN " Gerçek soy rezonansını seç (1-%d): " COLOR_RESET, match_count);
            }
        } else {
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
            }
        }

        if (match_count > 1) {
            char ch;
            while(1) {
                if(_kbhit()) {
                    ch = _getch();
                    int sel = ch - '0';
                    if(sel >= 1 && sel <= match_count) {
                        int final_idx = tied_indices[sel - 1];
                        strcpy(profile->god_alignment, database[final_idx].god);
                        strcpy(profile->archetype_alignment, database[final_idx].archetype);
                        strcpy(profile->archetype_alignment_tr, database[final_idx].archetype_tr);
                        break;
                    }
                }
                Sleep(2);
            }
            if (current_lang == 1) printf(COLOR_GOLD "\n\n Matris son seçimine göre yeniden kalibre edildi!\n" COLOR_RESET);
            else printf(COLOR_GOLD "\n\n Matrix re-calibrated to your final choice!\n" COLOR_RESET);
        }
    }

    if (current_lang == 1) printf("\n\n" COLOR_WHITE " [Kader Yoluna devam etmek için HERHANGİ BİR TUŞA bas] " COLOR_RESET);
    else printf("\n\n" COLOR_WHITE " [Press ANY KEY to continue to the Path of Destiny] " COLOR_RESET);
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
    profile->is_pure = (profile->player_nature == profile->god_nature);

    if (current_lang == 1) {
        printf(COLOR_GOLD " =============================================================\n");
        printf("                       KADERİN YOLU                       \n");
        printf(" =============================================================\n\n" COLOR_RESET);
        printf(COLOR_WHITE "  %s kanını taşıyorsun. Şimdi, bu dünyadaki rolünü iddia et.\n\n" COLOR_RESET, profile->god_alignment);

        printf("  [" COLOR_CYAN "1" COLOR_RESET "] HÜKÜMDAR (Otorite & Liderlik)\n");
        printf("  [" COLOR_CYAN "2" COLOR_RESET "] ASKER    (Savaş & Disiplin)\n");
        printf("  [" COLOR_CYAN "3" COLOR_RESET "] SANATÇI  (İlham & Zanaat)\n");
        printf("  [" COLOR_CYAN "4" COLOR_RESET "] DİPLOMAT (İletişim & Müzakere)\n");
        printf("  [" COLOR_CYAN "5" COLOR_RESET "] TÜCCAR   (Ekonomi & Kaynaklar)\n\n");
        printf(COLOR_CYAN " Toplumsal sınıfını seç (1-5): " COLOR_RESET);
    } else {
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
    }

    char choice;
    bool valid = false;
    while (!valid) {
        if (_kbhit()) {
            choice = _getch();
            if (choice >= '1' && choice <= '5') {
                valid = true;
                switch(choice) {
                    case '1': profile->chosen_class = CLASS_RULER; strcpy(profile->class_name, "Ruler"); strcpy(profile->class_name_tr, "Hükümdar"); profile->affinity = r; break;
                    case '2': profile->chosen_class = CLASS_SOLDIER; strcpy(profile->class_name, "Soldier"); strcpy(profile->class_name_tr, "Asker"); profile->affinity = s; break;
                    case '3': profile->chosen_class = CLASS_ARTIST; strcpy(profile->class_name, "Artist"); strcpy(profile->class_name_tr, "Sanatçı"); profile->affinity = a; break;
                    case '4': profile->chosen_class = CLASS_DIPLOMAT; strcpy(profile->class_name, "Diplomat"); strcpy(profile->class_name_tr, "Diplomat"); profile->affinity = d; break;
                    case '5': profile->chosen_class = CLASS_MERCHANT; strcpy(profile->class_name, "Merchant"); strcpy(profile->class_name_tr, "Tüccar"); profile->affinity = m; break;
                }
            }
        }
        Sleep(20);
    }

    calculate_final_title(profile);
}

void set_titles(CharacterProfile* p, const char* en_title, const char* tr_title) {
    strcpy(p->final_title, en_title);
    strcpy(p->final_title_tr, tr_title);
}

void calculate_final_title(CharacterProfile* profile) {
    int aff = profile->affinity;
    bool pure = profile->is_pure;

    if (profile->chosen_class == CLASS_RULER) {
        if(aff >= 90) pure ? set_titles(profile, "Divine Sovereign", "İlahi Hükümdar") : set_titles(profile, "Dread Tyrant", "Korkunç Tiran");
        else if(aff >= 50) pure ? set_titles(profile, "Just Monarch", "Adil Hükümdar") : set_titles(profile, "Pragmatic Autocrat", "Pragmatik Otokrat");
        else if(aff >= 10) pure ? set_titles(profile, "Visionary Guide", "Vizyoner Rehber") : set_titles(profile, "Chaotic Demagogue", "Kaotik Demagog");
        else pure ? set_titles(profile, "Transcendent Overlord", "Yüce Derebeyi") : set_titles(profile, "Lord of Anarchy", "Anarşi Lordu");
    }
    else if (profile->chosen_class == CLASS_SOLDIER) {
        if(aff >= 90) pure ? set_titles(profile, "Sword of War", "Savaşın Kılıcı") : set_titles(profile, "Rider of the Apocalypse", "Kıyamet Binicisi");
        else if(aff >= 50) pure ? set_titles(profile, "Honorable Commander", "Onurlu Komutan") : set_titles(profile, "Ruthless Mercenary", "Acımasız Paralı Asker");
        else if(aff >= 10) pure ? set_titles(profile, "Unorthodox Tactician", "Sıradışı Taktisyen") : set_titles(profile, "Phantom Guerrilla", "Hayalet Gerilla");
        else pure ? set_titles(profile, "Peaceful Blademaster", "Barışçıl Kılıç Ustası") : set_titles(profile, "Walking Calamity", "Yürüyen Felaket");
    }
    else if (profile->chosen_class == CLASS_ARTIST) {
        if(aff >= 90) pure ? set_titles(profile, "Cosmic Creator", "Kozmik Yaratıcı") : set_titles(profile, "Mind-Bending Genius", "Zihin Büken Dahi");
        else if(aff >= 50) pure ? set_titles(profile, "Master Artisan", "Usta Zanaatkar") : set_titles(profile, "Avant-Garde Rebel", "Avangard İsyankar");
        else if(aff >= 10) pure ? set_titles(profile, "Eccentric Surrealist", "Eksantrik Sürrealist") : set_titles(profile, "Provocative Illusionist", "Kışkırtıcı İllüzyonist");
        else pure ? set_titles(profile, "Bard of the Void", "Hiçliğin Ozanı") : set_titles(profile, "Reality Weaver", "Gerçeklik Dokuyucusu");
    }
    else if (profile->chosen_class == CLASS_DIPLOMAT) {
        if(aff >= 90) pure ? set_titles(profile, "Divine Mediator", "İlahi Arabulucu") : set_titles(profile, "Mind Puppeteer", "Zihin Kuklacısı");
        else if(aff >= 50) pure ? set_titles(profile, "Chief Negotiator", "Baş Müzakereci") : set_titles(profile, "Shadow Broker", "Gölge Simsarı");
        else if(aff >= 10) pure ? set_titles(profile, "Unpredictable Envoy", "Öngörülemez Elçi") : set_titles(profile, "Chaos Whisperer", "Kaos Fısıldayan");
        else pure ? set_titles(profile, "Silent Oracle", "Sessiz Kahin") : set_titles(profile, "Concept Distorter", "Kavram Çarpıtıcı");
    }
    else if (profile->chosen_class == CLASS_MERCHANT) {
        if(aff >= 90) pure ? set_titles(profile, "Golden Sovereign", "Altın Hükümdar") : set_titles(profile, "Underworld Emperor", "Yeraltı İmparatoru");
        else if(aff >= 50) pure ? set_titles(profile, "Trade Baron", "Ticaret Baronu") : set_titles(profile, "Black Market Cartel", "Karaborsa Karteli");
        else if(aff >= 10) pure ? set_titles(profile, "Enigmatic Collector", "Esrarengiz Koleksiyoncu") : set_titles(profile, "Fortune Smuggler", "Servet Kaçakçısı");
        else pure ? set_titles(profile, "Weaver of Fates", "Kaderlerin Dokuyucusu") : set_titles(profile, "Calamity Speculator", "Felaket Spekülatörü");
    }

    clear_screen();
    if (current_lang == 1) {
        printf(COLOR_RED " =============================================================\n");
        printf("                      GERÇEK FORMUN UYANIYOR                  \n");
        printf(" =============================================================\n\n" COLOR_RESET);

        printf(COLOR_WHITE "  Kan Bağı Rezonansı : " COLOR_CYAN "%d%%\n" COLOR_RESET, aff);
        printf(COLOR_WHITE "  Zihniyet Uyumu     : %s\n", pure ? COLOR_GOLD "SAFKAN (Uyumlu)" COLOR_RESET : COLOR_RED "ÇAPRAZ (Anormal)" COLOR_RESET);
        printf(COLOR_WHITE "  Kader Unvanı       : " COLOR_GOLD "[ %s ]\n\n" COLOR_RESET, profile->final_title_tr);

        printf(COLOR_DARK " [Kaderini onaylamak için HERHANGİ BİR TUŞA bas] " COLOR_RESET);
    } else {
        printf(COLOR_RED " =============================================================\n");
        printf("                      YOUR TRUE FORM AWAKENS                  \n");
        printf(" =============================================================\n\n" COLOR_RESET);

        printf(COLOR_WHITE "  Bloodline Affinity : " COLOR_CYAN "%d%%\n" COLOR_RESET, aff);
        printf(COLOR_WHITE "  Mindset Alignment  : %s\n", pure ? COLOR_GOLD "PURE (Harmonious)" COLOR_RESET : COLOR_RED "CROSS (Anomalous)" COLOR_RESET);
        printf(COLOR_WHITE "  Destined Title     : " COLOR_GOLD "[ %s ]\n\n" COLOR_RESET, profile->final_title);

        printf(COLOR_DARK " [Press ANY KEY to finalize your destiny] " COLOR_RESET);
    }
    _getch();

    display_character_sheet(profile);
}



// ============================================================================
// YOLCULUĞA DEVAM ET (OTOMATİK POSEIDON PROFİLİ)
// ============================================================================
void scene_continue_journey(CharacterProfile* profile) {
    // 1. Hardcode Poseidon - The Earth-Shaker Profilini Doldur
    strcpy(profile->player_name, "Gezgin");
    strcpy(profile->god_alignment, "Poseidon");
    strcpy(profile->archetype_alignment, "The Earth-Shaker");
    strcpy(profile->archetype_alignment_tr, "Sarsılmaz Yeryüzü Titreten");

    // Temel Sınıf ve Uyum Ayarları
    profile->story = STORY_UNCROWNED;
    profile->player_nature = NATURE_AGGRESSIVE;
    profile->god_nature = NATURE_AGGRESSIVE;
    profile->chosen_class = CLASS_SOLDIER;
    strcpy(profile->class_name, "Soldier");
    strcpy(profile->class_name_tr, "Asker");
    strcpy(profile->final_title, "Sword of War");
    strcpy(profile->final_title_tr, "Savaşın Kılıcı");

    profile->affinity = 100;
    profile->is_pure = true;

    // 5-Parametreli Statlar (Sadece Might 10)
    profile->intel = 0;
    profile->might = 10;
    profile->honor = 0;
    profile->skill = 0;
    profile->faith = 0;

    // 2. Karakter Kağıdını Ekrana Bas (Sanki sistemden yüklenmiş gibi)
    display_character_sheet(profile);

    // 3. Karakter kağıdından sonra doğrudan haritaya geçiş yap
    scene_map(profile);
}

void scene_language_options(void) {
    clear_screen();
    printf(COLOR_GOLD " =============================================================\n");
    if (current_lang == 1) printf("                    SAHNE: DİL SEÇENEKLERİ                    \n");
    else printf("                    SCENE: LANGUAGE OPTIONS                   \n");
    printf(" =============================================================\n" COLOR_RESET);

    if (current_lang == 1) {
        printf("  Aktif Konfigürasyon: Türkçe\n\n");
        printf("  [" COLOR_CYAN "1" COLOR_RESET "] English\n");
        printf("  [" COLOR_CYAN "2" COLOR_RESET "] Türkçe\n\n");
        printf(COLOR_CYAN "  Dil seç (1-2): " COLOR_RESET);
    } else {
        printf("  Active Configuration: English\n\n");
        printf("  [" COLOR_CYAN "1" COLOR_RESET "] English\n");
        printf("  [" COLOR_CYAN "2" COLOR_RESET "] Türkçe\n\n");
        printf(COLOR_CYAN "  Select Language (1-2): " COLOR_RESET);
    }

    char choice;
    bool valid = false;
    while (!valid) {
        if (_kbhit()) {
            choice = _getch();
            if (choice == '1') { current_lang = 0; valid = true; }
            else if (choice == '2') { current_lang = 1; valid = true; }
        }
        Sleep(20);
    }

    clear_screen();
}

void scene_system_status(CharacterProfile* profile) {
    clear_screen();
    char* story_map[] = { "TRADITIONAL", "UNCROWNED", "EXILED" };
    char* story_map_tr[] = { "GELENEKSEL", "TAÇSIZ", "SÜRGÜN" };

    if (current_lang == 1) {
        printf(COLOR_GOLD " =============================================================\n");
        printf("                   SAHNE: MATRİS DOĞRULAMA                 \n");
        printf(" =============================================================\n" COLOR_RESET);
        printf("  --- KAHRAMAN HAFIZA KAYDI DURUMU ---\n");
        printf("  * Karakter Adı       : %s\n", profile->player_name);
        printf("  * Hikaye Geçmişi     : %s\n", (profile->story == STORY_UNASSIGNED) ? "ATANMADI" : story_map_tr[profile->story]);

        printf("\n  --- ATANAN TANRI & KADER MATRİSİ ---\n");
        printf("  * Tanrı Hizalaması   : " COLOR_GOLD "%s\n" COLOR_RESET, profile->god_alignment);
        printf("  * Nihai Sınıf        : " COLOR_CYAN "%s\n" COLOR_RESET, profile->class_name_tr);
        printf("  * Kan Rezonansı      : %d%%\n", profile->affinity);
        printf("  * Nihai Unvan        : " COLOR_GOLD "%s\n" COLOR_RESET, profile->final_title_tr);

        printf("\n  --- AKTİF 5-PARAMETRELİ NİTELİKLER ---\n");
        printf("  * [ZEK] Zeka (Mantık)        : %d puan\n", profile->intel);
        printf("  * [GÜÇ] Güç (Kudret)         : %d puan\n", profile->might);
        printf("  * [ONR] Onur (Fedakarlık)    : %d puan\n", profile->honor);
        printf("  * [YET] Kişisel Yetenekler   : %d puan\n", profile->skill);
        printf("  * [İNC] İlahi İnanç          : %d puan\n", profile->faith);
        printf("  -------------------------------------------------------------\n\n");
        printf(COLOR_WHITE " [Fırtına Menüsüne güvenle dönmek için HERHANGİ BİR TUŞA bas] " COLOR_RESET);
    } else {
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
        printf(COLOR_WHITE " [Press ANY KEY to return safely to the Storm Menu] " COLOR_RESET);
    }
    _getch();
    clear_screen();
}

// ============================================================================
// 7. AETHERIAL DOSSIER (CHARACTER SHEET)
// ============================================================================

void print_stat_bar(int value, int max_val, const char* color) {
    printf("%s", color);
    for(int i = 0; i < max_val; i++) {
        if(i < value) printf("█");
        else printf(COLOR_DARK "░");
        printf("%s", color);
    }
    printf(COLOR_RESET);
}

void display_character_sheet(CharacterProfile* profile) {
    clear_screen();

    char buffer[256];
    char buf1[10], buf2[10];
    int vis_len;
    int bar1, bar2;

    const char* active_tags[20];
    int active_tags_len[20];
    int tag_count = 0;

    if (current_lang == 1) { // Turkish Tags
        if (profile->affinity >= 80) { active_tags[tag_count] = COLOR_CYAN "< İLAHİ_REZONANS >" COLOR_RESET; active_tags_len[tag_count++] = 18; }
        else if (profile->affinity <= 30) { active_tags[tag_count] = COLOR_RED "< AĞIR_ANOMALİ >" COLOR_RESET; active_tags_len[tag_count++] = 16; }

        if (profile->is_pure) { active_tags[tag_count] = COLOR_GOLD "< UYUMLU_RUH >" COLOR_RESET; active_tags_len[tag_count++] = 14; }
        else { active_tags[tag_count] = COLOR_RED "< PARÇALANMIŞ_ZİHİN >" COLOR_RESET; active_tags_len[tag_count++] = 21; }

        if (profile->intel >= 6) { active_tags[tag_count] = COLOR_WHITE "< TAKTİKSEL_DAHİ >" COLOR_RESET; active_tags_len[tag_count++] = 18; }
        if (profile->might >= 6) { active_tags[tag_count] = COLOR_WHITE "< EZİCİ_GÜÇ >" COLOR_RESET; active_tags_len[tag_count++] = 13; }
        if (profile->skill >= 6) { active_tags[tag_count] = COLOR_WHITE "< ZANAAT_USTASI >" COLOR_RESET; active_tags_len[tag_count++] = 17; }
        if (profile->honor >= 6) { active_tags[tag_count] = COLOR_WHITE "< KIRILMAZ_İRADE >" COLOR_RESET; active_tags_len[tag_count++] = 18; }
        if (profile->faith == 0) { active_tags[tag_count] = COLOR_DARK "< TANRISIZ_VARLIK >" COLOR_RESET; active_tags_len[tag_count++] = 19; }
    } else { // English Tags
        if (profile->affinity >= 80) { active_tags[tag_count] = COLOR_CYAN "< DIVINE_RESONANCE >" COLOR_RESET; active_tags_len[tag_count++] = 20; }
        else if (profile->affinity <= 30) { active_tags[tag_count] = COLOR_RED "< SEVERE_ANOMALY >" COLOR_RESET; active_tags_len[tag_count++] = 18; }

        if (profile->is_pure) { active_tags[tag_count] = COLOR_GOLD "< HARMONIC_SOUL >" COLOR_RESET; active_tags_len[tag_count++] = 17; }
        else { active_tags[tag_count] = COLOR_RED "< FRACTURED_MIND >" COLOR_RESET; active_tags_len[tag_count++] = 18; }

        if (profile->intel >= 6) { active_tags[tag_count] = COLOR_WHITE "< TACTICAL_GENIUS >" COLOR_RESET; active_tags_len[tag_count++] = 19; }
        if (profile->might >= 6) { active_tags[tag_count] = COLOR_WHITE "< OVERWHELMING_FORCE >" COLOR_RESET; active_tags_len[tag_count++] = 22; }
        if (profile->skill >= 6) { active_tags[tag_count] = COLOR_WHITE "< MASTER_OF_CRAFTS >" COLOR_RESET; active_tags_len[tag_count++] = 20; }
        if (profile->honor >= 6) { active_tags[tag_count] = COLOR_WHITE "< UNBREAKABLE_WILL >" COLOR_RESET; active_tags_len[tag_count++] = 20; }
        if (profile->faith == 0) { active_tags[tag_count] = COLOR_DARK "< GODLESS_ENTITY >" COLOR_RESET; active_tags_len[tag_count++] = 18; }
    }

    printf(COLOR_DARK "\n   ╒════════════════════════════════════════════════════════════════════════╕\n");

    if (current_lang == 1) sprintf(buffer, " SİS-BAŞLAT :: ETERNAL DOSYA");
    else sprintf(buffer, " SYS-INIT :: AETHERIAL DOSSIER");

    vis_len = strlen(buffer) + 13;
    printf(COLOR_DARK "   │" COLOR_WHITE "%s", buffer);
    for(int i = 0; i < 72 - vis_len; i++) printf(" ");
    printf(COLOR_DARK "[ v. 1.0.4 ] │\n");

    printf("   ╞════════════════════════════════════════════════════════════════════════╡\n");
    printf("   │                                                                        │\n" COLOR_RESET);

    if (current_lang == 1) sprintf(buffer, "%s (%s)", profile->player_name, profile->final_title_tr);
    else sprintf(buffer, "%s (The %s)", profile->player_name, profile->final_title);

    vis_len = 23 + strlen(buffer);
    if (current_lang == 1) printf(COLOR_DARK "   │  " COLOR_CYAN "[ID]" COLOR_RESET " TANIMLAMA     : " COLOR_WHITE "%s" COLOR_DARK, buffer);
    else printf(COLOR_DARK "   │  " COLOR_CYAN "[ID]" COLOR_RESET " DESIGNATION   : " COLOR_WHITE "%s" COLOR_DARK, buffer);
    for(int i = 0; i < 72 - vis_len; i++) printf(" ");
    printf("│\n");

    if (current_lang == 1) sprintf(buffer, "%s - %s", profile->god_alignment, profile->archetype_alignment_tr);
    else sprintf(buffer, "%s - %s", profile->god_alignment, profile->archetype_alignment);

    vis_len = 23 + strlen(buffer);
    if (current_lang == 1) printf(COLOR_DARK "   │  " COLOR_CYAN "[KB]" COLOR_RESET " KAN BAĞI      : " COLOR_GOLD "%s" COLOR_DARK, buffer);
    else printf(COLOR_DARK "   │  " COLOR_CYAN "[BL]" COLOR_RESET " BLOODLINE     : " COLOR_GOLD "%s" COLOR_DARK, buffer);
    for(int i = 0; i < 72 - vis_len; i++) printf(" ");
    printf("│\n");

    if (current_lang == 1) {
        vis_len = 23 + strlen(profile->class_name_tr);
        printf(COLOR_DARK "   │  " COLOR_CYAN "[SN]" COLOR_RESET " SINIF         : " COLOR_WHITE "%s" COLOR_DARK, profile->class_name_tr);
    } else {
        vis_len = 23 + strlen(profile->class_name);
        printf(COLOR_DARK "   │  " COLOR_CYAN "[CL]" COLOR_RESET " CASTE         : " COLOR_WHITE "%s" COLOR_DARK, profile->class_name);
    }
    for(int i = 0; i < 72 - vis_len; i++) printf(" ");
    printf("│\n");

    printf("   │                                                                        │\n");
    if (current_lang == 1) printf("   │  ======================== [ " COLOR_CYAN "SENKRONİZASYON" COLOR_DARK " ] ========================  │\n");
    else printf("   │  ====================== [ " COLOR_CYAN "SYNCHRONIZATION" COLOR_DARK " ] =========================  │\n");

    sprintf(buffer, "%d%%", profile->affinity);
    vis_len = 20 + strlen(buffer);
    if (current_lang == 1) printf("   │  " COLOR_RESET "YATKINLIK SEVİYESİ :  %s" COLOR_DARK, buffer);
    else printf("   │  " COLOR_RESET "AFFINITY LEVEL :  %s" COLOR_DARK, buffer);
    for(int i = 0; i < 72 - vis_len; i++) printf(" ");
    printf("│\n");

    const char* m_str = profile->is_pure ? (current_lang == 1 ? "SAFKAN (Uyumlu)" : "PURE (Harmonious)") : (current_lang == 1 ? "ÇAPRAZ (Anormal)" : "CROSS (Anomalous)");
    const char* m_col = profile->is_pure ? COLOR_GOLD : COLOR_RED;
    vis_len = 20 + strlen(m_str);
    if (current_lang == 1) printf("   │  " COLOR_RESET "ZİHİN DURUMU       :  %s%s" COLOR_DARK, m_col, m_str);
    else printf("   │  " COLOR_RESET "MINDSET STATE  :  %s%s" COLOR_DARK, m_col, m_str);
    for(int i = 0; i < 72 - vis_len; i++) printf(" ");
    printf("│\n");

    printf("   │                                                                        │\n");
    if (current_lang == 1) printf("   │  ====================== [ " COLOR_CYAN "PARAMETRİK VEKTÖRLER" COLOR_DARK " ] ====================  │\n");
    else printf("   │  ====================== [ " COLOR_CYAN "PARAMETRIC VECTORS" COLOR_DARK " ] ======================  │\n");

    sprintf(buf1, "%2d", profile->intel);
    sprintf(buf2, "%2d", profile->honor);
    bar1 = profile->intel < 0 ? 0 : (profile->intel > 10 ? 10 : profile->intel);
    bar2 = profile->honor < 0 ? 0 : (profile->honor > 10 ? 10 : profile->honor);
    if (current_lang == 1) printf("   │  " COLOR_RESET "[ZEK] ZEKA : %s  ", buf1);
    else printf("   │  " COLOR_RESET "[INT] INTEL : %s  ", buf1);
    print_stat_bar(bar1, 10, COLOR_CYAN);
    if (current_lang == 1) printf(COLOR_DARK "  │   " COLOR_RESET "[ONR] ONUR : %s  ", buf2);
    else printf(COLOR_DARK "  │   " COLOR_RESET "[HNR] HONOR : %s  ", buf2);
    print_stat_bar(bar2, 10, COLOR_GOLD);
    vis_len = 60 + strlen(buf1) + strlen(buf2);
    for(int i = 0; i < 72 - vis_len; i++) printf(" ");
    printf(COLOR_DARK "│\n");

    sprintf(buf1, "%2d", profile->might);
    sprintf(buf2, "%2d", profile->skill);
    bar1 = profile->might < 0 ? 0 : (profile->might > 10 ? 10 : profile->might);
    bar2 = profile->skill < 0 ? 0 : (profile->skill > 10 ? 10 : profile->skill);
    if (current_lang == 1) printf(COLOR_DARK "   │  " COLOR_RESET "[GÜÇ] GÜÇ  : %s  ", buf1);
    else printf(COLOR_DARK "   │  " COLOR_RESET "[MGT] MIGHT : %s  ", buf1);
    print_stat_bar(bar1, 10, COLOR_RED);
    if (current_lang == 1) printf(COLOR_DARK "  │   " COLOR_RESET "[YET] YETENEK : %s  ", buf2);
    else printf(COLOR_DARK "  │   " COLOR_RESET "[SKL] SKILL : %s  ", buf2);
    print_stat_bar(bar2, 10, COLOR_WHITE);
    vis_len = 62 + strlen(buf1) + strlen(buf2); // Adjusted for longer TR text
    if (current_lang == 0) vis_len = 60 + strlen(buf1) + strlen(buf2);
    for(int i = 0; i < 72 - vis_len; i++) printf(" ");
    printf(COLOR_DARK "│\n");

    sprintf(buf1, "%2d", profile->faith);
    bar1 = profile->faith < 0 ? 0 : (profile->faith > 10 ? 10 : profile->faith);
    if (current_lang == 1) printf(COLOR_DARK "   │  " COLOR_RESET "[İNC] İNANÇ: %s  ", buf1);
    else printf(COLOR_DARK "   │  " COLOR_RESET "[FTH] FAITH : %s  ", buf1);
    print_stat_bar(bar1, 10, COLOR_CYAN);
    printf(COLOR_DARK "  │");
    vis_len = 31 + strlen(buf1);
    if (current_lang == 1) vis_len = 32 + strlen(buf1);
    for(int i = 0; i < 72 - vis_len; i++) printf(" ");
    printf("│\n");

    printf("   │                                                                        │\n");
    if (current_lang == 1) printf("   │  ======================== [ " COLOR_CYAN "AKTİF ANOMALİLER" COLOR_DARK " ] ========================  │\n");
    else printf("   │  ====================== [ " COLOR_CYAN "ACTIVE ANOMALIES" COLOR_DARK " ] ========================  │\n");

    int current_line_len = 0;
    printf("   │  " COLOR_RESET);
    current_line_len = 2;

    if (tag_count == 0) {
        if (current_lang == 1) { printf(COLOR_DARK "ANOMALİ TESPİT EDİLMEDİ"); current_line_len += 23; }
        else { printf(COLOR_DARK "NO ANOMALIES DETECTED"); current_line_len += 21; }
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
    for (int p = 0; p < 72 - current_line_len; p++) printf(" ");
    printf(COLOR_DARK "│\n");

    printf("   ╘════════════════════════════════════════════════════════════════════════╛\n\n" COLOR_RESET);

    if (current_lang == 1) printf(COLOR_DARK "   [Sistem Mühürlendi. Ana Menüye dönmek için HERHANGİ BİR TUŞA bas] " COLOR_RESET);
    else printf(COLOR_DARK "   [System Sealed. Press ANY KEY to enter the Overworld Menu] " COLOR_RESET);
    _getch();
    clear_screen();
}

// ============================================================================
// GİRİŞ YAPILAN ALT HARİTA (ŞİMDİLİK SADECE NOKTA)
// ============================================================================
void scene_inner_shrine(CharacterProfile* profile) {
    clear_screen();

    printf(COLOR_CYAN "\n\n\n\n\n\n\n\n\n                                  .\n\n\n\n\n\n\n\n\n" COLOR_RESET);

    if (current_lang == 1) {
        printf(COLOR_DARK " [Haritaya geri dönmek için HERHANGİ BİR TUŞA bas] " COLOR_RESET);
    } else {
        printf(COLOR_DARK " [Press ANY KEY to return to the Nexus] " COLOR_RESET);
    }
    _getch();
}

// ============================================================================
// THE ETHEREAL CASTLE OF THE SUN (MATHEMATICALLY PERFECT ASCII)
// ============================================================================
void scene_map(CharacterProfile* profile) {
    bool exploring = true;

    while (exploring) {
        clear_screen();

        // --- 1. FLAWLESS SYMMETRIC FORTRESS ON A CLIFF ---
        printf("\n");
        printf(COLOR_DARK  "     * " COLOR_RED "|>>>\n" COLOR_RESET);
        printf(COLOR_DARK  "            .               " COLOR_WHITE "|\n" COLOR_RESET);
        printf(COLOR_DARK  "       .    " COLOR_RED "|>>>" COLOR_WHITE "        _  _|_  _           " COLOR_RED "|>>>\n" COLOR_RESET);
        printf(COLOR_WHITE "            |          |;| |;| |;|          |\n" COLOR_RESET);
        printf(COLOR_DARK  "    * " COLOR_WHITE "_  _|_  _     " COLOR_CYAN "\\\\\\\\.         /" COLOR_WHITE "     _  _|_  _   " COLOR_DARK "*\n" COLOR_RESET);
        printf(COLOR_WHITE "       |;|_|;|_|;|    " COLOR_CYAN "\\\\\\\\:         /" COLOR_WHITE "    |;|_|;|_|;|\n" COLOR_RESET);
        printf(COLOR_CYAN  "       \\\\\\\\..      /     " COLOR_WHITE "||:       |     " COLOR_CYAN "\\\\\\\\..      /\n" COLOR_RESET);
        printf(COLOR_CYAN  "       \\\\\\\\.       /     " COLOR_WHITE "||:       |     " COLOR_CYAN "\\\\\\\\.       /\n" COLOR_RESET);
        printf(COLOR_WHITE "       ||:       |_____||:       |_____||:       |\n" COLOR_RESET);
        printf(COLOR_WHITE "       ||:       |     ||:       |     ||:       |\n" COLOR_RESET);
        printf(COLOR_WHITE "       ||:       |     ||:       |     ||:       |\n" COLOR_RESET);
        printf(COLOR_WHITE "       ||:       |     ||:_______|     ||:       |\n" COLOR_RESET);
        printf(COLOR_WHITE "       ||:       |     |/ _ _ _ \\|     ||:       |\n" COLOR_RESET);
        printf(COLOR_WHITE "       ||:       |     || | | | ||     ||:       |\n" COLOR_RESET);
        printf(COLOR_WHITE "       ||:_______|_____||_|_|_|_||_____||:_______|\n" COLOR_RESET);

        // --- 2. DEEP FLOATING ROCK FOUNDATION ---
        printf(COLOR_DARK  "   /=================================================\\\n" COLOR_RESET);
        printf(COLOR_DARK  "  /:::::::::::::::::::::::::::::::::::::::::::::::::::\\\n" COLOR_RESET);
        printf(COLOR_DARK  " /=====================================================\\\n" COLOR_RESET);
        printf(COLOR_DARK  " \\:::::::::::::::::::::::::::::::::::::::::::::::::::::/\n" COLOR_RESET);
        printf(COLOR_DARK  "     \\=============================================/\n" COLOR_RESET);
        printf(COLOR_DARK  "         \\:::::::::::::::::::::::::::::::::::::/\n" COLOR_RESET);
        printf(COLOR_DARK  "             \\=============================/\n" COLOR_RESET);
        printf(COLOR_DARK  "                 \\:::::::::::::::::::::/\n" COLOR_RESET);
        printf(COLOR_DARK  "                     \\=============/\n" COLOR_RESET);
        printf(COLOR_DARK  "                        \\:::::::/\n" COLOR_RESET);
        printf(COLOR_DARK  "                          \\===/\n" COLOR_RESET);
        printf(COLOR_DARK  "                            V\n" COLOR_RESET);
        printf("\n");

        // --- 3. Navigation Menu ---
        if (current_lang == 1) {
            printf(COLOR_GOLD "  === GÜNEŞ SARAYI MERKEZİ (THE GRAND SUN FORTRESS) ===\n\n" COLOR_RESET);
            printf("  [" COLOR_CYAN "1" COLOR_RESET "] %s Kulübesine Doğru İlerle (Inner Shrine)\n", profile->god_alignment);
            printf("  [" COLOR_CYAN "0" COLOR_RESET "] Sisteme Geri Dön (Çıkış)\n\n");
            printf(COLOR_CYAN "  Yol Seçimi (0-1): " COLOR_RESET);
        } else {
            printf(COLOR_GOLD "  === THE GRAND SUN FORTRESS ===\n\n" COLOR_RESET);
            printf("  [" COLOR_CYAN "1" COLOR_RESET "] Walk towards the Shrine of %s\n", profile->god_alignment);
            printf("  [" COLOR_CYAN "0" COLOR_RESET "] Return to System Menu (Exit)\n\n");
            printf(COLOR_CYAN "  Select Path (0-1): " COLOR_RESET);
        }

        // --- 4. Input Handling ---
        bool valid_input = false;
        while (!valid_input) {
            if (_kbhit()) {
                char ch = _getch();
                if (ch == '1') {
                    scene_inner_shrine(profile);
                    valid_input = true;
                } else if (ch == '0') {
                    exploring = false;
                    valid_input = true;
                }
            }
            Sleep(20);
        }
    }
}