#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <math.h>

// ============================================================================
// 1. SYSTEM DEFINITIONS & METRIC FLAGS
// ============================================================================
#define COLOR_GOLD    "\033[1;33m"
#define COLOR_CYAN    "\033[1;36m"
#define COLOR_RED     "\033[1;31m"
#define COLOR_GRN     "\033[1;32m"
#define COLOR_MAG     "\033[1;35m"
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


// ============================================================================
// BADGE (GİZLİ ETİKET) SİSTEMİ
// ============================================================================
typedef enum {
    BADGE_CURSE_OF_THANATOS = 0, // Ölüm Kılıcının Laneti
    BADGE_BROKEN_BONES,          // Kırık Kemikler (Seviye 1-5)
    BADGE_BLESSING_HELIOS,       // Helios'un Kutsaması
    BADGE_BLESSING_POSEIDON,     // Poseidon'un Kutsaması
    // YENİ ROZETLERİ BURAYA EKLEYEBİLİRSİN...

    MAX_BADGES                   // SİSTEM: Toplam rozet sayısını otomatik tutar
} BadgeID;

// ============================================================================
// NPC İLİŞKİ (RELATIONSHIP) SİSTEMİ
// ============================================================================
typedef enum {
    NPC_HELIOS = 0,
    NPC_POSEIDON,
    NPC_APOLLO,
    NPC_ATHENA,
    // Yeni karakterler eklendikçe buraya yazılacak...

    MAX_NPCS // Sistem otomatik sayar
} NPC_ID;

// ...

// CharacterProfile struct'ının İÇİNE şunları ekle:
// --- 6. SOSYAL İLİŞKİ AĞI (Yeni) ---
int npc_relationship[MAX_NPCS]; // Karakterlerle olan ilişki puanı (-100 ile +100 arası)
bool npc_met[MAX_NPCS];         // Karakterle tanışıldı mı? (İlişki barı açık mı?)

// Not: main() fonksiyonu içinde yeni oyun (1'e basıldığında) başlarken
// bu dizileri 0 ve false olarak sıfırlamayı unutma!
// ============================================================================
// GÜNCELLENMİŞ KARAKTER VERİTABANI (ESKİ VE YENİ SİSTEM BİRLEŞTİRİLDİ)
// ============================================================================
typedef struct {
    // --- 1. KİMLİK & HİKAYE (Eski) ---
    char player_name[50];
    char god_alignment[30];
    char archetype_alignment[50];
    char archetype_alignment_tr[50];

    // YENİ: FRAKSİYON (SINIF) BİLGİSİ
    char faction_class[20];    // SUN, OCEAN, EARTH
    char faction_class_tr[20]; // GÜNEŞ, OKYANUS, TOPRAK

    StoryState story;

    int affinity;

    // --- 3. RPG TEST STATLARI (Eski - Kozmik Sınav İçin) ---
    int intel;
    int might;
    int honor;
    int skill;
    int faith;

    // --- 4. DERS & BOSS SİSTEMİ (Yeni) ---
    int active_subject_count;
    char subject_names[15][50];   // Derslerin/Epiklerin Özel İsimleri
    int study_stats[15];          // Dersin seviyesi (Kazanılan EXP)
    int subject_exams[15];        // O derse ait Sınav (Boss) Sayısı
    int subject_projects[15];     // O derse ait Proje (Mini-Boss) Sayısı
    int total_exp;

    // --- 5. ZAMAN & LOG SİSTEMİ (Yeni) ---
    int last_login_day;
    int last_login_month;
    int last_login_year;

    // --- 6. SOSYAL İLİŞKİ AĞI (Yeni) ---
    int npc_relationship[MAX_NPCS]; // Karakterlerle olan ilişki puanı (-100 ile +100 arası)
    bool npc_met[MAX_NPCS];

    // GİZLİ VETO SİSTEMİ (Boğulma testini geçemeyenler için)
    int poseidon_veto;

    int badges[MAX_BADGES];

} CharacterProfile;

// Basit ve Saf C Veritabanı (I/O) İşlemleri
void save_game(CharacterProfile* profile) {
    FILE *outfile = fopen("mythic_save.dat", "wb");
    if (outfile != NULL) {
        fwrite(profile, sizeof(CharacterProfile), 1, outfile);
        fclose(outfile);
    }
}

bool load_game(CharacterProfile* profile) {
    FILE *infile = fopen("mythic_save.dat", "rb");
    if (infile != NULL) {
        fread(profile, sizeof(CharacterProfile), 1, infile);
        fclose(infile);
        return true;
    }
    return false; // Kayıt yoksa false döner (Yeni Oyun başlar)
}



typedef struct {
    char god[30];
    char archetype[50];
    char archetype_tr[50]; // Turkish Translation added
    char faction[20];      // YENİ: İngilizce Sınıf
    char faction_tr[20];   // YENİ: Türkçe Sınıf
    int intel, might, honor, skill, faith;
} ArchetypeMatrix;

// Global matrix of 33 calibrated unique historical profiles (Now with Turkish Translations)
ArchetypeMatrix database[33] = {
    // ZEUS
    {"Zeus", "The Thunder Wrath", "Yıldırım Öfkesi", "SUN", "GÜNEŞ", 5, 8, 4, 4, 4},
    {"Zeus", "The Cloud Arbiter", "Bulutların Hakemi", "OCEAN", "OKYANUS", 8, 4, 5, 5, 3},
    {"Zeus", "The Absolute Sovereign", "Mutlak Hükümdar", "EARTH", "TOPRAK", 5, 5, 8, 3, 4},
    // POSEIDON
    {"Poseidon", "The Tidal Ravager", "Gelgit Yıkıcısı", "SUN", "GÜNEŞ", 3, 8, 4, 5, 5},
    {"Poseidon", "The Abyssal Master", "Uçurumun Efendisi", "OCEAN", "OKYANUS", 6, 4, 4, 7, 4},
    {"Poseidon", "The Earth-Shaker", "Yeryüzü Titreten", "EARTH", "TOPRAK", 4, 7, 7, 4, 3},
    // HADES
    {"Hades", "The Relentless Executioner", "Amansız İnfazcı", "SUN", "GÜNEŞ", 5, 7, 5, 4, 4},
    {"Hades", "The Soul Collector", "Ruh Toplayıcı", "OCEAN", "OKYANUS", 7, 3, 4, 7, 4},
    {"Hades", "The Underworld Autocrat", "Yeraltı Otokratı", "EARTH", "TOPRAK", 6, 4, 8, 3, 4},
    // DEMETER
    {"Demeter", "The Winter Famine", "Kış Kıtlığı", "SUN", "GÜNEŞ", 4, 7, 4, 4, 6},
    {"Demeter", "The Harvest Cycle", "Hasat Döngüsü", "OCEAN", "OKYANUS", 6, 3, 5, 6, 5},
    {"Demeter", "The Earth Mother", "Toprak Ana", "EARTH", "TOPRAK", 5, 5, 8, 3, 4},
    // ATHENA
    {"Athena", "The War Vanguard", "Savaş Öncüsü", "SUN", "GÜNEŞ", 7, 6, 5, 4, 3},
    {"Athena", "The Supreme Tactician", "Yüce Taktisyen", "OCEAN", "OKYANUS", 9, 3, 4, 6, 3},
    {"Athena", "The Citadel Protector", "Hisar Koruyucusu", "EARTH", "TOPRAK", 6, 5, 8, 4, 2},
    // APOLLO
    {"Apollo", "The Radiant Bowman", "Parlak Okçu", "SUN", "GÜNEŞ", 4, 6, 4, 7, 4},
    {"Apollo", "The Golden Maestro", "Altın Maestro", "OCEAN", "OKYANUS", 6, 3, 4, 6, 6},
    {"Apollo", "The Oracle of Truth", "Hakikat Kahini", "EARTH", "TOPRAK", 6, 4, 6, 3, 6},
    // APHRODITE
    {"Aphrodite", "The Flame of Passion", "İhtiras Alevi", "SUN", "GÜNEŞ", 4, 6, 3, 7, 5},
    {"Aphrodite", "The Siren Puppetmaster", "Siren Kukla Ustası", "OCEAN", "OKYANUS", 7, 2, 3, 8, 5},
    {"Aphrodite", "The Flawless Form", "Kusursuz Form", "EARTH", "TOPRAK", 5, 4, 6, 6, 4},
    // ARES
    {"Ares", "The Blind Berserker", "Kör Savaşçı", "SUN", "GÜNEŞ", 2, 9, 4, 5, 5},
    {"Ares", "The Blood Tactician", "Kanlı Taktisyen", "OCEAN", "OKYANUS", 5, 7, 3, 6, 4},
    {"Ares", "The Dread Champion", "Dehşet Şampiyonu", "EARTH", "TOPRAK", 3, 8, 7, 4, 3},
    // HERMES
    {"Hermes", "The Hurricane Herald", "Kasırga Haberci", "SUN", "GÜNEŞ", 4, 5, 3, 9, 4},
    {"Hermes", "The Phantom Thief", "Hayalet Hırsız", "OCEAN", "OKYANUS", 7, 2, 3, 9, 4},
    {"Hermes", "The Crossroads Guide", "Kavşak Rehberi", "EARTH", "TOPRAK", 6, 4, 6, 6, 3},
    // HEPHAESTUS
    {"Hephaestus", "The Volcanic Juggernaut", "Volkanik Dev", "SUN", "GÜNEŞ", 4, 8, 4, 6, 3},
    {"Hephaestus", "The Outcast Innovator", "Dışlanmış Yenilikçi", "OCEAN", "OKYANUS", 8, 3, 4, 8, 2},
    {"Hephaestus", "The Forgemaster", "Usta Demirci", "EARTH", "TOPRAK", 6, 6, 7, 5, 1},
    // DIONYSUS
    {"Dionysus", "The Madness Bringer", "Delilik Getiren", "SUN", "GÜNEŞ", 3, 6, 2, 6, 8},
    {"Dionysus", "The Illusionist Vagabond", "İllüzyonist Serseri", "OCEAN", "OKYANUS", 6, 3, 2, 7, 7},
    {"Dionysus", "The Cult Leader", "Kült Lideri", "EARTH", "TOPRAK", 5, 4, 6, 4, 6}
};

// ============================================================================
// 2. FUNCTION PROTOTYPES
// ============================================================================
void set_cursor_visibility(bool visible);
void clear_screen(void) {
    system("cls");
}
bool render_lightning_storm(int frame);
void render_menu_options(bool is_flashing);

bool scene_start_journey(CharacterProfile* profile);
void execute_parametric_test(CharacterProfile* profile);
void evaluate_cosmic_alignment(CharacterProfile* profile);
void get_god_affinity_data(const char* god_name, Nature* nature, int* r, int* s, int* a, int* d, int* m);
int get_parametric_input(void);
void print_permanent_choices(void);
void scene_continue_journey(CharacterProfile* profile);
void scene_language_options(void);
void scene_system_status(CharacterProfile* profile);
void display_character_sheet(CharacterProfile* profile);
void scene_map(CharacterProfile* profile);              // Yeni harita fonksiyonu eklendi
void scene_inner_shrine(CharacterProfile* profile);
void scene_inner_shrine(CharacterProfile* profile);
void scene_main_school(CharacterProfile* profile);
void scene_own_shrine(CharacterProfile* profile);
void scene_init_subjects(CharacterProfile* profile);
void scene_library_stopwatch(CharacterProfile* profile);
void scene_inside_location(const char* loc_name_tr, const char* loc_name_en);
void print_mythic_date(void);
void append_study_log(const char* subject, int earned_exp, int minutes);
void scene_awaken_destiny(CharacterProfile* profile);
// Tüm rozetleri tek kalemde sıfırlar (Yeni oyun başlarken kullanılır)
void reset_all_badges(CharacterProfile* profile) {
    for (int i = 0; i < MAX_BADGES; i++) {
        profile->badges[i] = 0;
    }
}

// Karaktere rozet vermek veya seviyesini güncellemek için kullanılır
void set_badge(CharacterProfile* profile, BadgeID badge, int level) {
    if (badge >= 0 && badge < MAX_BADGES) {
        profile->badges[badge] = level;
    }
}

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
        .faction_class = "UNASSIGNED",
        .faction_class_tr = "ATANMADI",
        .story = STORY_UNASSIGNED,
        .affinity = 0,
        // Initialize base stats to perfectly balanced 5
        .intel = 5, .might = 5, .honor = 5, .skill = 5, .faith = 5
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

                    // Reset stats for a new session
                    player.intel = 5; player.might = 5; player.honor = 5; player.skill = 5; player.faith = 5;
                    player.affinity = 0;
                    player.poseidon_veto = 0;

                    // TÜM ROZETLERİ TEK SATIRDA SIFIRLA
                    reset_all_badges(&player);

                    for(int i=0; i<15; i++) player.study_stats[i] = 0;
                    player.total_exp = 0;

                    strcpy(player.god_alignment, "UNASSIGNED");

                    // Start journey
                    if (!scene_start_journey(&player)) running = false;
                    set_cursor_visibility(false);
                    break;
                case '2':
                    set_cursor_visibility(true);
                    scene_continue_journey(&player);
                    clear_screen();
                    set_cursor_visibility(false);
                    break;
                case '3':
                    scene_language_options();
                    break;
                case '4':
                    clear_screen();
                    if (current_lang == 1) printf(COLOR_WHITE "\n \"Kaderinden kaçamazsın; sadece onu geciktirebilirsin.\"\n" COLOR_RESET);
                    else printf(COLOR_WHITE "\n \"You cannot escape your destiny; you can only delay it.\"\n" COLOR_RESET);
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

// ============================================================================
// EKRAN SALLAMA (SCREEN SHAKE) EFEKTİ (HELIOS MİNİ-OYUNU İÇİN)
// ============================================================================
void shake_console(int intensity, int duration_ms) {
#ifdef _WIN32
    HWND console = GetConsoleWindow();
    if (console == NULL) return; // Güvenlik kontrolü

    RECT r;
    GetWindowRect(console, &r);
    int start_x = r.left;
    int start_y = r.top;
    int width = r.right - r.left;
    int height = r.bottom - r.top;

    int elapsed = 0;
    while (elapsed < duration_ms) {
        int dx = (rand() % (intensity * 2)) - intensity;
        int dy = (rand() % (intensity * 2)) - intensity;
        MoveWindow(console, start_x + dx, start_y + dy, width, height, TRUE);
        Sleep(20);
        elapsed += 20;
    }
    // Sallanma bitince pencereyi eski orijinal konumuna geri al
    MoveWindow(console, start_x, start_y, width, height, TRUE);
#endif
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

// --- GÜVENLİ VERİ GİRİŞİ YARDIMCILARI ---

// Güvenli String Okuyucu (Enter ' \n ' kalıntılarını yutmadan temiz okur)
void read_string_safe(char* buffer, int max_len) {
    if (fgets(buffer, max_len, stdin) != NULL) {
        if (buffer[0] == '\n') fgets(buffer, max_len, stdin);
        buffer[strcspn(buffer, "\n")] = 0; // Sondaki Enter'ı temizle
    }
}

// Güvenli Doğal Sayı Okuyucu (Harf girilmesini çökmeden engeller)
int get_safe_natural_number(int min_val, int max_val) {
    int val;
    char buffer[128];
    while (1) {
        if (fgets(buffer, sizeof(buffer), stdin)) {
            if (sscanf(buffer, "%d", &val) == 1) {
                if (val >= min_val && val <= max_val) {
                    return val;
                }
            }
        }
        if (current_lang == 1) {
            printf(COLOR_RED "  [!] Geçersiz. Lütfen %d ile %d arasında bir DOĞAL SAYI girin: " COLOR_RESET, min_val, max_val);
        } else {
            printf(COLOR_RED "  [!] Invalid. Please enter a NATURAL NUMBER between %d and %d: " COLOR_RESET, min_val, max_val);
        }
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

bool scene_start_journey(CharacterProfile* profile) {
    clear_screen();
    set_cursor_visibility(false);

    // 1. Absolute darkness and silence
    Sleep(2000); // Wait in darkness

    // 2. The subtle trigger
    if (current_lang == 1) {
        printf(COLOR_DARK "\n\n\n\n\n\n\n\n\n\n                                  * pısst *\n" COLOR_RESET);
    } else {
        printf(COLOR_DARK "\n\n\n\n\n\n\n\n\n\n                                  * psst *\n" COLOR_RESET);
    }

    Sleep(1800); // Let the realization sink in

    // 3. Sudden awakening (Lightning flash effect)
    for (int i = 0; i < 3; i++) {
        clear_screen();
        printf(COLOR_WHITE);
        printf("\n\n\n\n          ,/\n        ,///\n      ,/////\n    ,///////\n   /////////\n      //\n      /\n");
        printf(COLOR_RESET);
        Sleep(50);
        clear_screen();
        Sleep(50);
    }

    // Immediately jump into the chaos of Trial 1
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

// Helper function to safely get input between 1 and 5
int get_trial_input(void) {
    while (1) {
        if (_kbhit()) {
            char ch = _getch();
            if (ch >= '1' && ch <= '5') return ch - '0';
        }
        Sleep(20);
    }
}

// Function to print the dynamic trial questions
void print_dynamic_trial(int trial_num, const char* title_en, const char* title_tr, const char* desc_en, const char* desc_tr, const char* opts_en[], const char* opts_tr[]) {
    clear_screen();
    if (current_lang == 1) {
        printf(COLOR_GOLD "\n [SINAV %d] %s\n" COLOR_RESET, trial_num, title_tr);
        printf(COLOR_WHITE " %s\n\n" COLOR_RESET, desc_tr);
        for (int i = 0; i < 5; i++) printf("  [" COLOR_CYAN "%d" COLOR_RESET "] %s\n", i + 1, opts_tr[i]);
        printf(COLOR_CYAN "\n  Karar Vektörü (1-5): " COLOR_RESET);
    } else {
        printf(COLOR_GOLD "\n [TRIAL %d] %s\n" COLOR_RESET, trial_num, title_en);
        printf(COLOR_WHITE " %s\n\n" COLOR_RESET, desc_en);
        for (int i = 0; i < 5; i++) printf("  [" COLOR_CYAN "%d" COLOR_RESET "] %s\n", i + 1, opts_en[i]);
        printf(COLOR_CYAN "\n  Decision Vector (1-5): " COLOR_RESET);
    }
}

void execute_parametric_test(CharacterProfile* profile) {
    int choice;
    int prev_choice; // Memorizes Trial 1
    int choice_2;    // Memorizes Trial 2
    int choice_3 = 0; // Memorizes Trial 4 reaction
    clear_screen();

    // ========================================================================
    // TRIAL 1: THE AWAKENING IN CHAOS
    // ========================================================================
    const char* q1_en[] = {
        "Grab the thick ropes of the main mast and brace against the impact.",
        "Use the ship's tilt to quickly slide into the lower hatch.",
        "Open your arms to the wave, accepting the chaotic flow."
    };
    const char* q1_tr[] = {
        "Ana direğin kalın halatlarına sıkıca sarılıp çarpışmayı olduğum yerde karşılarım.",
        "Geminin eğiminden faydalanıp saniyeler içinde alt ambar kapağının boşluğuna kayarım.",
        "Kollarımı dalgaya doğru açarak suyun beni sürükleyeceği o kaotik akışı kabullenirim."
    };

    if (current_lang == 1) {
        printf(COLOR_GOLD "\n [SAHNE I] Uyanış\n\n" COLOR_RESET);
        printf(COLOR_WHITE " Yüzüne şiddetle çarpan tuzlu suyla gözlerini açıyorsun. Çatırdayan ahşap bir güvertedesin.\n");
        printf(" Ufku tamamen kaplayan, etrafındaki tüm ışığı yutan kapkara devasa bir yaratık denizin ortasında dikiliyor.\n");
        printf(" Tepesinde ise göğü yırtarak inen, altın ışıklar saçan efsanevi bir figür var.\n\n");
        printf(" Ancak bunu idrak etmeye vaktin yok. Devasa, kapkara bir dalga tam üzerine kırılmak üzere!\n\n" COLOR_RESET);

        for (int i = 0; i < 3; i++) printf("  [" COLOR_CYAN "%d" COLOR_RESET "] %s\n", i + 1, q1_tr[i]);
        printf(COLOR_CYAN "\n  İlk içgüdün nedir? (1-3): " COLOR_RESET);
    } else {
        printf(COLOR_GOLD "\n [SCENE I] The Awakening\n\n" COLOR_RESET);
        printf(COLOR_WHITE " You open your eyes to freezing saltwater striking your face. You are on a creaking wooden deck.\n");
        printf(" A colossal, pitch-black creature that swallows all light dominates the horizon.\n");
        printf(" Above it, a legendary figure radiating golden light descends, tearing through the sky.\n\n");
        printf(" But you have no time to comprehend this. A massive, dark wave is about to crash right on top of you!\n\n" COLOR_RESET);

        for (int i = 0; i < 3; i++) printf("  [" COLOR_CYAN "%d" COLOR_RESET "] %s\n", i + 1, q1_en[i]);
        printf(COLOR_CYAN "\n  What is your first instinct? (1-3): " COLOR_RESET);
    }

    while (1) {
        if (_kbhit()) {
            char ch = _getch();
            if (ch >= '1' && ch <= '3') { choice = ch - '0'; break; }
        }
        Sleep(20);
    }

    if (choice == 1) { profile->might += 1; profile->intel -= 1; }
    else if (choice == 2) { profile->skill += 1; profile->honor -= 1; }
    else if (choice == 3) { profile->faith += 1; profile->might -= 1; }
    prev_choice = choice;

    // ========================================================================
    // TRIAL 2: SHRAPNEL OF LIGHT (DYNAMIC BRANCHING)
    // ========================================================================
    clear_screen();

    const char* q2_set1_en[] = {
        "Quickly move behind the mast, leaning my entire weight against the trunk to secure myself.",
        "Grab a thick sail rope nearby and swing myself away milliseconds before the arrow strikes.",
        "Stare directly into the blinding light, believing this stray cosmic bullet will miss me entirely."
    };
    const char* q2_set1_tr[] = {
        "Direğin arkasına hızla geçip, tüm ağırlığımla kütüğe abanarak kendimi sağlama alırım.",
        "Yakınımdaki kalın bir yelken halatını kapıp, ok çarpmadan milisaniyeler önce kendimi savururum.",
        "Gözlerimi kamaştıran ışığa dimdik bakıp, tanrısal savaşın bu kör kurşununun beni teğet geçeceğine inanarak beklerim."
    };

    const char* q2_set2_en[] = {
        "Hearing the wood splinter, I instinctively and blindly dive and roll to the side in panic.",
        "Focus my hearing, pinpointing exactly where the ceiling burst, and crawl in the opposite direction.",
        "Drop to my knees, shield my head tightly with my arms, and brace for the debris to collapse on me."
    };
    const char* q2_set2_tr[] = {
        "Ahşabın kırıldığını duyduğum an içgüdüsel bir panikle körlemesine yana doğru atılıp yuvarlanırım.",
        "Kulaklarıma odaklanıp, tavanın tam olarak nereden patladığını sesinden tespit ederek zıt yöne doğru sürünürüm.",
        "Diz çöküp kollarımı başıma sıkıca siper eder, enkazın üzerime çökmesini kaskatı beklerim."
    };

    const char* q2_set3_en[] = {
        "Use my low center of gravity to quickly slide across the wet deck and out of the danger zone.",
        "Proclaim, 'If death descends from the sky, it must be met head-on,' and wait with my chest puffed out.",
        "Let the divine light blind me, trusting that this cosmic arrow will simply graze past a mere mortal like me."
    };
    const char* q2_set3_tr[] = {
        "Düşük ağırlık merkezimi kullanıp, ıslak zemin üzerinde hızla kayarak tehlike alanının dışına çıkarım.",
        "\"Ölüm gökten iniyorsa saklanarak değil, yüzleşerek karşılanmalı,\" diyerek oturduğum yerde başım dik beklerim.",
        "İlahi ışığın gözümü almasına izin verip, bu kozmik okun beni teğet geçeceğine inanarak yerimden hiç kıpırdamam."
    };

    if (current_lang == 1) {
        printf(COLOR_GOLD "\n [SAHNE II] Işığın Şarapneli\n\n" COLOR_RESET);
        if (prev_choice == 1) {
            printf(COLOR_WHITE " Fırtınanın vahşetine ucu ucuna dayandın. Sırılsıklamsın. Kaslarındaki gerilimi ufak bir\n");
            printf(" esnemeyle atıp solundaki kozmik savaşa bakıyorsun.\n\n");
            printf(COLOR_DARK " Apollon, yaratığa ardı ardına altından oklar yağdırıyor. Oklar denize mermi gibi düşüp suları\n");
            printf(" GÜM GÜM patlatıyor. O sırada seken kör edici oklardan birinin tam direğe doğru geldiğini görüyorsun!\n\n" COLOR_RESET);
            for (int i = 0; i < 3; i++) printf("  [" COLOR_CYAN "%d" COLOR_RESET "] %s\n", i + 1, q2_set1_tr[i]);
        }
        else if (prev_choice == 2) {
            printf(COLOR_WHITE " Ambarın içine son anda girdin ama dalganın çarpmasıyla içeride sağa sola savruldun.\n");
            printf(" Ufaktan sersemlemiş bir halde, karanlıkta doğrulmaya çalışıyorsun.\n\n");
            printf(COLOR_DARK " Yukarıda kıyamet kopuyor. Suları sarsan, sağır edici GÜM GÜM patlama sesleri duyuyorsun.\n");
            printf(" Aniden kör edici altın rengi bir ışık, karanlık ambarın tavanını paramparça ediyor!\n\n" COLOR_RESET);
            for (int i = 0; i < 3; i++) printf("  [" COLOR_CYAN "%d" COLOR_RESET "] %s\n", i + 1, q2_set2_tr[i]);
        }
        else if (prev_choice == 3) {
            printf(COLOR_WHITE " Dalganın şiddeti seni güvertede sürükledi ve sırtını ana direğe çarptın. Oturduğun\n");
            printf(" yerden, nefes nefese solundaki savaşı izlemeye başlıyorsun.\n\n");
            printf(COLOR_DARK " Apollon'un attığı oklar canavardan sekmeye başlıyor! Seken oklar denize düşüp suları patlatıyor.\n");
            printf(" O sırada seken kör edici oklardan birinin tam bulunduğun yere, ana yelkene geldiğini görüyorsun!\n\n" COLOR_RESET);
            for (int i = 0; i < 3; i++) printf("  [" COLOR_CYAN "%d" COLOR_RESET "] %s\n", i + 1, q2_set3_tr[i]);
        }
        printf(COLOR_CYAN "\n  İkinci içgüdün nedir? (1-3): " COLOR_RESET);
    } else {
        printf(COLOR_GOLD "\n [SCENE II] Shrapnel of Light\n\n" COLOR_RESET);
        if (prev_choice == 1) {
            printf(COLOR_WHITE " You barely held on against the storm's fury. You are soaked. You stretch your tense\n");
            printf(" muscles slightly and look to your left at the cosmic battle.\n\n");
            printf(COLOR_DARK " Apollo rains golden arrows. The follow-up arrows ricochet, plunging into the sea and exploding with\n");
            printf(" deafening BOOMS. Suddenly, you see one blinding arrow flying straight at the mast!\n\n" COLOR_RESET);
            for (int i = 0; i < 3; i++) printf("  [" COLOR_CYAN "%d" COLOR_RESET "] %s\n", i + 1, q2_set1_en[i]);
        }
        else if (prev_choice == 2) {
            printf(COLOR_WHITE " You slid into the hold just in time, but the impact sent you tumbling violently inside.\n");
            printf(" Slightly dazed, you try to steady yourself in the darkness.\n\n");
            printf(COLOR_DARK " You hear deafening BOOMs shaking the sea. You cannot see the battle, but suddenly a blinding\n");
            printf(" golden light shatters the deck above you, falling straight into the dark hold!\n\n" COLOR_RESET);
            for (int i = 0; i < 3; i++) printf("  [" COLOR_CYAN "%d" COLOR_RESET "] %s\n", i + 1, q2_set2_en[i]);
        }
        else if (prev_choice == 3) {
            printf(COLOR_WHITE " The wave swept you across the deck, crashing your back against the main mast. Sitting there,\n");
            printf(" catching your breath, you start watching the battle to your left.\n\n");
            printf(COLOR_DARK " Apollo's arrows begin to ricochet! They plunge into the sea, exploding with deafening BOOMS.\n");
            printf(" Suddenly, you see one blinding, ricocheting arrow flying straight at your exact position!\n\n" COLOR_RESET);
            for (int i = 0; i < 3; i++) printf("  [" COLOR_CYAN "%d" COLOR_RESET "] %s\n", i + 1, q2_set3_en[i]);
        }
        printf(COLOR_CYAN "\n  What is your second instinct? (1-3): " COLOR_RESET);
    }

    while (1) {
        if (_kbhit()) {
            char ch = _getch();
            if (ch >= '1' && ch <= '3') { choice = ch - '0'; break; }
        }
        Sleep(20);
    }

    if (prev_choice == 1) {
        if (choice == 1) { profile->might += 1; profile->intel -= 1; }
        else if (choice == 2) { profile->skill += 1; profile->faith -= 1; }
        else if (choice == 3) { profile->faith += 1; profile->skill -= 1; }
    } else if (prev_choice == 2) {
        if (choice == 1) { profile->skill += 1; profile->honor -= 1; }
        else if (choice == 2) { profile->intel += 1; profile->might -= 1; }
        else if (choice == 3) { profile->might += 1; profile->intel -= 1; }
    } else if (prev_choice == 3) {
        if (choice == 1) { profile->skill += 1; profile->might -= 1; }
        else if (choice == 2) { profile->honor += 1; profile->skill -= 1; }
        else if (choice == 3) { profile->faith += 1; profile->intel -= 1; }
    }
    choice_2 = choice;

    // ========================================================================
    // TRIAL 3 & 4: INDIVIDUAL DESTINIES AND FINAL REACTIONS
    // ========================================================================
    clear_screen();

    // ------------------------------------------------------------------------
    // PATH 1.1: Stand strong at mast -> Lean into it again
    // ------------------------------------------------------------------------
    if (prev_choice == 1 && choice_2 == 1) {
        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE III] İlahi Kilitlenme\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Kaba kuvvetin işe yaradı! Kozmik ok gövdeye değil, tam üstündeki ana yelkene saplanıp direği parçalıyor.\n\n");
            printf(" Devasa yelkenin devrilmesini ve küpeşteyi parçalayarak denize çakılmasını izliyorsun.\n");
            printf(" Bu çarpışmanın sarsıntısıyla ayakların yerden kesiliyor, hafifçe havalanıyorsun.\n");
            printf(" Tam o an, okyanusun derinliklerinden fırlayan parlayan devasa bir mızrak (Trident)\n");
            printf(" suları yararak hemen üzerinden mermi gibi göğe uçuyor!\n\n");
            printf(" Sarsıntıyla dizlerinin üzerine sertçe düştüğün o saniyede, devasa bir figür (Poseidon)\n");
            printf(" suları yararak geminin hemen yanından yükseliyor. Devasa yüzü güverte hizasından geçerken\n");
            printf(" göz göze geliyorsunuz. O ilahi bakışmanın ağırlığı altında tamamen kilitleniyorsun...\n\n" COLOR_RESET);
            printf(COLOR_CYAN " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n [SCENE III] Divine Paralysis\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Your brute force worked! The cosmic arrow strikes the main sail above you, shattering the mast.\n\n");
            printf(" You watch as the massive sail topples over, crashing into the sea.\n");
            printf(" The impact lifts your feet off the deck, sending you slightly airborne.\n");
            printf(" At that exact moment, a colossal glowing spear (Trident) shoots from the depths\n");
            printf(" tearing through the water and flying right over you like a missile into the sky!\n\n");
            printf(" As you land hard on your knees, a colossal figure (Poseidon) rises\n");
            printf(" from the waters right beside the ship. As his massive face passes the deck level,\n");
            printf(" your eyes lock. You are completely paralyzed under the weight of that divine gaze...\n\n" COLOR_RESET);
            printf(COLOR_CYAN " [Press ANY KEY to continue]\n" COLOR_RESET);
        }
        _getch();
        clear_screen();

        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE IV] İlahi Yargı\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Sular durulmuyor. Dizlerinin üzerindesin. Okyanusun hakimi, devasa gözleriyle sana \n");
            printf(" tepeden bakıyor. Seni kurtarmak için hiçbir hamle yapmadı, sadece yıkımı izliyor.\n\n" COLOR_RESET);

            printf("  [" COLOR_CYAN "1" COLOR_RESET "] Bu muazzam ihtişam ve güç karşısında istemsizce titrerim. (İnanç)\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] Bir tanrı olmasına rağmen yardıma gelmediği için içimde saf bir nefret büyür. (Güç)\n");
            printf("  [" COLOR_CYAN "3" COLOR_RESET "] İlahi felci kendi irademle kırar, ona dik dik bakarak ayağa kalkarım. (Onur)\n\n");
            printf(COLOR_CYAN "  Tepkin nedir? (1-3): " COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n [SCENE IV] Divine Judgment\n\n" COLOR_RESET);
            printf(COLOR_WHITE " The waters do not calm. You are on your knees. The lord of the ocean looks down upon you.\n");
            printf(" He made no move to save you; he merely watches the destruction.\n\n" COLOR_RESET);

            printf("  [" COLOR_CYAN "1" COLOR_RESET "] I involuntarily tremble before this overwhelming majesty and power.\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] A pure hatred grows within me because, despite being a god, he offered no help.\n");
            printf("  [" COLOR_CYAN "3" COLOR_RESET "] I break the divine paralysis with my own will, stand up, and stare right back at him.\n\n");
            printf(COLOR_CYAN "  What is your reaction? (1-3): " COLOR_RESET);
        }
        while (1) {
            if (_kbhit()) {
                char ch = _getch();
                if (ch == '1') { profile->faith += 2; profile->intel -= 1; choice_3 = 1; break; }
                else if (ch == '2') { profile->might += 2; profile->faith -= 1; choice_3 = 2; break; }
                else if (ch == '3') { profile->honor += 2; profile->might -= 1; choice_3 = 3; break; }
            }
            Sleep(20);
        }
    }
    // ------------------------------------------------------------------------
    // PATH 1.2: Drowning Mini-Game + Mindscape
    // ------------------------------------------------------------------------
    else if (prev_choice == 1 && choice_2 == 2) {
        if (current_lang == 1) {
             printf(COLOR_GOLD "\n [SAHNE III] Dipsiz Uçurum\n\n" COLOR_RESET);
             printf(COLOR_WHITE " Halatla savrulmayı planlıyordun, ancak kozmik ok ana yelkeni parçalayıp gemiyi şiddetle\n");
             printf(" yana yatırınca momentumunu kaybettin. Havada hızını alamayıp doğrudan okyanusun\n");
             printf(" dondurucu sularına uçtun!\n\n");
             printf(COLOR_DARK " Suyun altına gömülüyorsun... Derine... Daha derine...\n\n" COLOR_RESET);
             printf(COLOR_RED " [HAYATTA KALMAK İÇİN BOŞLUK (SPACE) TUŞUNA ART ARDA BAS!]\n");
             printf(" Başlamak için Space'e bas...\n" COLOR_RESET);
        } else {
             printf(COLOR_GOLD "\n [SCENE III] The Abyssal Chasm\n\n" COLOR_RESET);
             printf(COLOR_WHITE " You planned to swing, but the cosmic arrow shattered the main sail, violently tilting the ship.\n");
             printf(" You lost your momentum and flew straight into the freezing ocean waters!\n\n");
             printf(COLOR_DARK " You are sinking... Deeper... And deeper...\n\n" COLOR_RESET);
             printf(COLOR_RED " [MASH THE SPACEBAR TO STAY ALIVE!]\n");
             printf(" Press Space to begin...\n" COLOR_RESET);
        }

        while(1) { if (_kbhit()) { if (_getch() == ' ') break; } Sleep(20); }
        clear_screen();

        float oxygen = 100.0f; float depletion_rate = 0.5f; int ticks = 0; bool conscious = true;
        while (conscious) {
            printf("\033[H");
            if (current_lang == 1) printf(COLOR_RED "\n\n  BOĞULUYORSUN! (SPACE'E ABAN!)\n\n" COLOR_RESET);
            else printf(COLOR_RED "\n\n  DROWNING! (MASH SPACE!)\n\n" COLOR_RESET);
            while (_kbhit()) {
                char ch = _getch();
                if (ch == ' ') { oxygen += 4.0f; if (oxygen > 100.0f) oxygen = 100.0f; }
            }
            oxygen -= depletion_rate;
            if (ticks % 10 == 0) depletion_rate += 0.25f;
            if (oxygen <= 0) { oxygen = 0; conscious = false; }

            if (current_lang == 1) printf("  " COLOR_CYAN "Oksijen: " COLOR_RESET "[");
            else printf("  " COLOR_CYAN "Oxygen : " COLOR_RESET "[");
            for (int i = 0; i < 50; i++) {
                if (i < (int)(oxygen / 2)) printf(COLOR_CYAN "█" COLOR_RESET);
                else printf(COLOR_DARK "-" COLOR_RESET);
            }
            printf("] %02d%%\n\n", (int)oxygen);
            float time_survived = (ticks * 50) / 1000.0f;
            if (current_lang == 1) printf("  Süre: " COLOR_GOLD "%.1f sn" COLOR_RESET "\n", time_survived);
            else printf("  Time: " COLOR_GOLD "%.1f s" COLOR_RESET "\n", time_survived);
            ticks++; Sleep(50);
        }

        clear_screen();
        float time_survived = (ticks * 50) / 1000.0f;
        if (time_survived < 9.0f) {
            profile->intel -= 5;
            profile->skill -= 5;
        }

        if (current_lang == 1) {
            printf(COLOR_DARK "\n\n  Oksijenin tamamen tükendi...\n");
            printf("  Su basıncı ciğerlerini eziyor, bilincin kapanıyor.\n\n");
            printf("  Gözlerin karanlığa teslim olmadan hemen önce, uzaklardan kan donduran bir ciyaklama sesi\n");
            printf("  duyuyorsun. Okyanusun dibinden devasa, parlayan bir mızrak (Trident) yanından mermi gibi\n");
            printf("  geçip gökyüzüne fırlıyor. Arkasından Poseidon'un devasa silüeti yanından geçerek yükseliyor...\n");
            printf("  Ve sonra her şey zifiri karanlığa bürünüyor...\n\n" COLOR_RESET);
            Sleep(2500);
            printf(COLOR_CYAN "  [ Zihninin derinliklerinde, suların altından gelen, yankılı, kadim bir ses duyuluyor ]\n\n" COLOR_RESET);
            printf(COLOR_WHITE "  \"Boğuluyorsun galiba...\"\n\n" COLOR_RESET);
            printf(COLOR_DARK " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
        } else {
            printf(COLOR_DARK "\n\n  Your oxygen is completely depleted...\n");
            printf("  Water pressure crushes your lungs, your consciousness fades.\n\n");
            printf("  Right before your eyes surrender to the darkness, you hear a bloodcurdling screech from afar.\n");
            printf("  From the ocean floor, a colossal, glowing spear (Trident) shoots past you like a missile\n");
            printf("  towards the sky. Poseidon's massive silhouette rises past you right after it...\n");
            printf("  And then, everything fades to pitch black...\n\n" COLOR_RESET);
            Sleep(2500);
            printf(COLOR_CYAN "  [ Deep within your mind, an echoing, ancient voice from beneath the waters resonates ]\n\n" COLOR_RESET);
            printf(COLOR_WHITE "  \"You seem to be drowning...\"\n\n" COLOR_RESET);
            printf(COLOR_DARK " [Press ANY KEY to continue]\n" COLOR_RESET);
        }
        _getch();
        clear_screen();

        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE IV] Hiçliğin Kıyısı\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Zihnin karanlığa gömülmek üzere. Bedenini hissetmiyorsun, sadece o yankılı sesi duyuyorsun.\n");
            printf(" Hiçliğe karışmadan saniyeler önceki son düşüncen nedir?\n\n" COLOR_RESET);
            printf("  [" COLOR_CYAN "1" COLOR_RESET "] \"Burada ölmeyi reddediyorum!\" (İradeni zorla - Güç)\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] \"Beni kurtar, ruhum senindir.\" (Boyun eğ - İnanç)\n");
            printf("  [" COLOR_CYAN "3" COLOR_RESET "] \"Sen kimsin?\" (Mantığını son ana kadar koru - Zeka)\n\n");
            printf(COLOR_CYAN "  Son düşüncen (1-3): " COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n [SCENE IV] Edge of the Void\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Your mind is fading into darkness. What is your final thought before the abyss?\n\n" COLOR_RESET);
            printf("  [" COLOR_CYAN "1" COLOR_RESET "] \"I refuse to die here!\" (Might)\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] \"Save me, and my soul is yours.\" (Faith)\n");
            printf("  [" COLOR_CYAN "3" COLOR_RESET "] \"Who are you?\" (Intel)\n\n");
            printf(COLOR_CYAN "  Final thought (1-3): " COLOR_RESET);
        }
        while (1) {
            if (_kbhit()) {
                char ch = _getch();
                if (ch == '1') { profile->might += 2; profile->intel -= 1; choice_3 = 1; break; }
                else if (ch == '2') { profile->faith += 2; profile->honor -= 1; choice_3 = 2; break; }
                else if (ch == '3') { profile->intel += 2; profile->faith -= 1; choice_3 = 3; break; }
            }
            Sleep(20);
        }
    }
    // ------------------------------------------------------------------------
    // PATH 2.X: The Hold Survivors
    // ------------------------------------------------------------------------
    else if (prev_choice == 2) {
        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE III] Karanlıktaki Yankı\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Çatırdama sesleriyle beklediğin o yıkıcı son ambarın alt katına ulaşmıyor.\n");
            printf(" Karanlık ambarın içinde güvendesin ancak yukarıdaki savaş giderek şiddetleniyor.\n");
            printf(" Suları yaran devasa bir gürleme ve birbirine çarpan ilahi silahların sağır edici\n");
            printf(" yankısı geminin ahşap gövdesinde çınlıyor.\n\n" COLOR_RESET);
            printf(COLOR_CYAN " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n [SCENE III] Echoes in the Dark\n\n" COLOR_RESET);
            printf(COLOR_WHITE " The devastating impact you braced for never reaches the lower hold.\n");
            printf(" You are safe in the dark, but the battle above is escalating wildly.\n");
            printf(" A deafening roar that splits the waters and the clash of divine weapons\n");
            printf(" echo terrifyingly through the ship's wooden hull.\n\n" COLOR_RESET);
            printf(COLOR_CYAN " [Press ANY KEY to continue]\n" COLOR_RESET);
        }
        _getch();
        clear_screen();

        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE IV] Tabuttaki Karar\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Gemi adeta yüzen bir tabuta dönüştü. Savaş güvertede tüm dehşetiyle sürüyor.\n");
            printf(" Karanlığın içinde, etrafında kraliyet ganimetleriyle dolu kapalı ahşap sandıklar var.\n\n" COLOR_RESET);
            printf("  [" COLOR_CYAN "1" COLOR_RESET "] Ambar kapağını tekmeyle kırıp kıyametle yüzleşmek için güverteye çıkarım. (Onur)\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] Kaos tamamen geçene kadar karanlıkta sessizce bekler, hayatta kalmayı garantilerim. (Zeka)\n");
            printf("  [" COLOR_CYAN "3" COLOR_RESET "] Bu kaosu fırsat bilip, karanlıkta sandıkları kırarak kutsal/büyülü bir silah ararım. (Yetenek)\n\n");
            printf(COLOR_CYAN "  Kararın nedir? (1-3): " COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n [SCENE IV] Decision in the Coffin\n\n" COLOR_RESET);
            printf(COLOR_WHITE " The ship is now a floating coffin. The war continues above in all its terror.\n");
            printf(" In the dark, you are surrounded by sealed wooden crates full of royal loot.\n\n" COLOR_RESET);
            printf("  [" COLOR_CYAN "1" COLOR_RESET "] Kick the hatch open and go to the deck to face the apocalypse head-on. (Honor)\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] Stay perfectly still in the dark until the chaos passes, ensuring survival. (Intel)\n");
            printf("  [" COLOR_CYAN "3" COLOR_RESET "] Use the chaos to blindly smash the crates, searching for a divine artifact or weapon. (Skill)\n\n");
            printf(COLOR_CYAN "  What is your decision? (1-3): " COLOR_RESET);
        }
        while (1) {
            if (_kbhit()) {
                char ch = _getch();
                if (ch == '1') { profile->honor += 2; profile->intel -= 1; choice_3 = 1; break; }
                else if (ch == '2') { profile->intel += 2; profile->might -= 1; choice_3 = 2; break; }
                else if (ch == '3') { profile->skill += 2; profile->honor -= 1; choice_3 = 3; break; }
            }
            Sleep(20);
        }
    }
    // ------------------------------------------------------------------------
    // PATH 3.1: The Airborne Survivor
    // ------------------------------------------------------------------------
    else if (prev_choice == 3 && choice_2 == 1) {
        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE III] Uçurumun Çekimi\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Islak güvertede kayarken kozmik ok ana yelkeni parçalıyor ve gemi şiddetle yana yatıyor.\n");
            printf(" Kayışını kontrollü bir şekilde sürdürüp geminin köşesine çarpıyor ve tırabzanlara tutunuyorsun.\n");
            printf(" Tam o saniyede, okyanusun derinliklerinden fırlayan parlayan devasa bir mızrak (Trident)\n");
            printf(" suları yararak hemen arkandan mermi gibi göğe uçuyor!\n\n");
            printf(" Bu fırlayışın yarattığı devasa şok dalgası seni yerinden söküyor ve metrelerce havaya fırlatıyor.\n");
            printf(" Havadayken Poseidon'un devasa, ihtişamlı silüetini hemen altında, savaşı izlerken görüyorsun.\n\n" COLOR_RESET);
            printf(COLOR_CYAN " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n [SCENE III] Pull of the Abyss\n\n" COLOR_RESET);
            printf(COLOR_WHITE " As you slide, the arrow shatters the sail and the ship violently tilts.\n");
            printf(" You control your slide, slam into the ship's corner, and grab the railing tightly.\n");
            printf(" At that exact second, a colossal glowing spear (Trident) shoots from the depths\n");
            printf(" tearing through the water right behind you like a missile!\n\n");
            printf(" The massive shockwave from its launch rips you from the ship, launching you meters into the air.\n");
            printf(" Suspended in mid-air, you see Poseidon's majestic silhouette directly below you.\n\n" COLOR_RESET);
            printf(COLOR_CYAN " [Press ANY KEY to continue]\n" COLOR_RESET);
        }
        _getch();
        clear_screen();

        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE IV] Havada Asılı\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Saniyeler içinde ölümcül sulara doğru düşeceksin. Altında bir tanrı,\n");
            printf(" arkanda ise parçalanan bir gemi var.\n\n" COLOR_RESET);
            printf("  [" COLOR_CYAN "1" COLOR_RESET "] Havadayken vücudumu çevirip, suya düşmemek için tekrar geminin tırabzanlarına atlarım. (Yetenek)\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] Gözlerimi kapatıp düşüşü kabullenir, kendimi Poseidon'un merhametine bırakırım. (İnanç)\n");
            printf("  [" COLOR_CYAN "3" COLOR_RESET "] Düşerken dikkatini çekmek için var gücümle Poseidon'a doğru bağırırım! (Güç/Onur)\n\n");
            printf(COLOR_CYAN "  Hamlen nedir? (1-3): " COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n [SCENE IV] Suspended in Air\n\n" COLOR_RESET);
            printf(COLOR_WHITE " You will hit the deadly waters in seconds. A god is below you, a shattered ship behind you.\n\n" COLOR_RESET);
            printf("  [" COLOR_CYAN "1" COLOR_RESET "] Twist my body mid-air to frantically grab the broken railing before hitting the water. (Skill)\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] Close my eyes, accept the fall, and surrender myself to Poseidon's mercy. (Faith)\n");
            printf("  [" COLOR_CYAN "3" COLOR_RESET "] Roar at the top of my lungs at Poseidon to demand his attention as I fall! (Might)\n\n");
            printf(COLOR_CYAN "  What is your move? (1-3): " COLOR_RESET);
        }
        while (1) {
            if (_kbhit()) {
                char ch = _getch();
                if (ch == '1') { profile->skill += 2; profile->faith -= 1; choice_3 = 1; break; }
                else if (ch == '2') { profile->faith += 2; profile->might -= 1; choice_3 = 2; break; }
                else if (ch == '3') { profile->might += 1; profile->honor += 1; profile->intel -= 1; choice_3 = 3; break; }
            }
            Sleep(20);
        }
    }
    // ------------------------------------------------------------------------
    // PATH 1.3, 3.2, 3.3: The Watchers
    // ------------------------------------------------------------------------
    else if ((prev_choice == 1 && choice_2 == 3) || (prev_choice == 3 && (choice_2 == 2 || choice_2 == 3))) {
        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE III] Tanrıların Çarpışması\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Kozmik ok gövdeye değil, tam üstündeki ana yelkene saplanıp direği parçalıyor.\n");
            if (prev_choice == 1) {
                printf(" Direğe tutunan ellerin kayıyor ve sarsıntıyla dizlerinin üzerine çöküyorsun.\n");
            } else {
                printf(" Oturduğun yerden kıpırdamıyorsun. Zaten güvendesin, sadece izliyorsun.\n");
            }
            printf(" Devasa yelkenin ağır çekimde devrilip denize çakılmasını izlerken,\n");
            printf(" okyanusun derinliklerinden fırlayan devasa bir mızrak (Trident) suları yararak göğe uçuyor.\n");
            printf(" Ardından Poseidon'un muazzam silüeti okyanustan yükselip gökyüzündeki Apollon'a bakıyor.\n");
            printf(" Sen, bu iki ilahi gücün savaşının ortasında, sadece bir izleyicisin.\n\n" COLOR_RESET);
            printf(COLOR_CYAN " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n [SCENE III] Clash of Gods\n\n" COLOR_RESET);
            printf(COLOR_WHITE " The cosmic arrow strikes the main sail above you, shattering the mast.\n");
            if (prev_choice == 1) {
                printf(" Your hands slip from the mast and the shockwave drops you to your knees.\n");
            } else {
                printf(" You do not move from your seated position. You are safe, merely watching.\n");
            }
            printf(" As you watch the massive sail topple into the sea in slow motion,\n");
            printf(" a colossal spear (Trident) shoots from the depths, tearing into the sky.\n");
            printf(" Then, Poseidon's magnificent silhouette rises from the ocean, glaring up at Apollo.\n");
            printf(" In the midst of this clash of divine powers, you are just a spectator.\n\n" COLOR_RESET);
            printf(COLOR_CYAN " [Press ANY KEY to continue]\n" COLOR_RESET);
        }
        _getch();
        clear_screen();

        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE IV] Karıncanın Bakışı\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Güvertede hareketsiz kaldın. Apollon gökyüzünden dalışa geçiyor, Poseidon suları yükseltiyor.\n");
            printf(" Bu muazzam kozmik savaşı izleyen basit bir ölümlü olarak aklından geçen ilk düşünce nedir?\n\n" COLOR_RESET);
            printf("  [" COLOR_CYAN "1" COLOR_RESET "] \"Bir gün ben de gökleri ve denizleri sarsacak bu güce ulaşacağım.\" (Hırs/Güç)\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] \"Sadece hayatta kalıp bu destanı torunlarıma anlatmalıyım.\" (Zeka/Gerçekçilik)\n");
            printf("  [" COLOR_CYAN "3" COLOR_RESET "] \"Kim kazanırsa kazansın, iradesine boyun eğeceğim.\" (İnanç/Teslimiyet)\n\n");
            printf(COLOR_CYAN "  Düşüncen nedir? (1-3): " COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n [SCENE IV] Gaze of the Ant\n\n" COLOR_RESET);
            printf(COLOR_WHITE " You remain motionless on the deck. Apollo dives from the sky, Poseidon raises the seas.\n");
            printf(" As a mere mortal watching this cosmic war, what is your primary thought?\n\n" COLOR_RESET);
            printf("  [" COLOR_CYAN "1" COLOR_RESET "] \"One day, I too will attain this power to shake the heavens and seas.\" (Ambition/Might)\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] \"I just need to survive so I can tell this epic tale to my grandchildren.\" (Intel/Realism)\n");
            printf("  [" COLOR_CYAN "3" COLOR_RESET "] \"Whoever wins, I shall bow to their will.\" (Faith/Submission)\n\n");
            printf(COLOR_CYAN "  What is your thought? (1-3): " COLOR_RESET);
        }
        while (1) {
            if (_kbhit()) {
                char ch = _getch();
                if (ch == '1') { profile->might += 2; profile->faith -= 1; choice_3 = 1; break; }
                else if (ch == '2') { profile->intel += 2; profile->honor -= 1; choice_3 = 2; break; }
                else if (ch == '3') { profile->faith += 2; profile->skill -= 1; choice_3 = 3; break; }
            }
            Sleep(20);
        }
    }

    // ========================================================================
    // TRIAL 5: THE GRAND FINALE (CINEMATICS & MICRO-BRANCHES)
    // ========================================================================
    clear_screen();

    // 1. İzleyici Kontrolü (1.1.1, 1.1.2, 1.3, 3.2, 3.3)
    bool is_spectator = false;
    if (prev_choice == 1 && choice_2 == 1 && (choice_3 == 1 || choice_3 == 2)) is_spectator = true;
    if (prev_choice == 1 && choice_2 == 3) is_spectator = true;
    if (prev_choice == 3 && (choice_2 == 2 || choice_2 == 3)) is_spectator = true;

    // A - GİRDAP KURBANI (Sadece 1.1.3: Ayağa Kalkıp Poseidon'a Diklenen)
    if (prev_choice == 1 && choice_2 == 1 && choice_3 == 3) {
        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE V] Kibrin Bedeli\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Poseidon'un gözlerinin içine dik dik baktığın an, denizler tanrısı savaşla ilgilenmeden\n");
            printf(" önce sana doğru sadece ufak bir parmak hareketi yapıyor.\n\n");
            printf(" Aniden, altındaki güverte tahtaları paramparça olup yok oluyor!\n");
            printf(" Ne olduğunu bile anlayamadan, suların altında ansızın açılan devasa, kapkara bir\n");
            printf(" girdabın (whirlpool) içine çekiliyorsun.\n\n");
            printf(COLOR_DARK " Gökyüzündeki savaşı göremeden karanlık suların derinliklerine yutuluyorsun.\n");
            printf(" Girdabın amansız gücü seni dibe çekerken ciğerlerin tuzlu suyla doluyor\n");
            printf(" ve karanlık okyanusun dibinde çaresizce boğuluyorsun...\n\n" COLOR_RESET);
            printf(COLOR_CYAN " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n [SCENE V] Price of Hubris\n\n" COLOR_RESET);
            printf(COLOR_WHITE " The moment you stare directly into Poseidon's eyes, the god of the seas makes\n");
            printf(" a tiny, dismissive finger movement towards you before returning to the war.\n\n");
            printf(" Suddenly, the deck planks beneath your feet shatter and vanish completely!\n");
            printf(" Before you can even comprehend it, you are sucked into a colossal, pitch-black\n");
            printf(" whirlpool that opens instantly beneath the waves.\n\n");
            printf(COLOR_DARK " Swallowed by the dark depths, you will never witness the battle in the sky.\n");
            printf(" As the relentless force of the whirlpool drags you down, saltwater fills your lungs,\n");
            printf(" and you helplessly drown at the bottom of the dark ocean...\n\n" COLOR_RESET);
            printf(COLOR_CYAN " [Press ANY KEY to continue]\n" COLOR_RESET);
        }
        _getch();
    }
    // B - İZLEYİCİLERİN ORTAK KADERİ (Savaş Sinematiği ve TSUNAMI)
    else if (is_spectator) {

        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE V] Kıyametin Kırılma Anı\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Savaşın zirvesi... Poseidon aniden elini uzatıyor ve mermi gibi göğe uçan Trident'i geri çağırıyor.\n");
            printf(" Tam o sırada gökyüzü yarılıyor! Helios, tüm kibriyle bulutların arasından saf, kör edici\n");
            printf(" ilahi bir enerji ışını yolluyor. Ancak karanlık canavar, bu devasa gücü bir karadelik gibi soğuruyor!\n\n");
            printf(COLOR_DARK " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
            _getch(); clear_screen();

            printf(COLOR_GOLD "\n [SAHNE V] Tsunami\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Poseidon okyanusu dalgalandırıp canavarı devasa bir su sütunuyla havaya kaldırıyor\n");
            printf(" ve Trident'iyle alttan 3 ölümcül çizik atıyor!\n\n");
            printf(" Bunu fırsat bilen Apollon, güneş gibi parlayan o devasa bitirici okunu hışımla fırlatıyor.\n");
            printf(" Ancak ok canavara saplanacakken, yaratık Helios'tan soğurduğu tüm enerjiyi dışarı kusuyor!\n");
            printf(" İki muazzam ilahi güç çarpışıyor. Ok sekiyor ve okyanusa çakılarak denizi ortadan ikiye yaran,\n");
            printf(" gökyüzüne kadar uzanan devasa bir TSUNAMİ yaratıyor!\n\n" COLOR_RESET);
            printf(COLOR_DARK " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
            _getch(); clear_screen();

            printf(COLOR_RED "\n GÜÜÜÜMMMM!\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Tsunami gemiyi kürdan gibi yutuyor. Suyun devasa gücüyle metrelerce havaya fırlatılıyorsun!\n");
            printf(" Şu an gökyüzündesin. Şaşkınlık içinde etrafına bakıyorsun.\n\n" COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n [SCENE V] The Breaking Point\n\n" COLOR_RESET);
            printf(COLOR_WHITE " The climax of war... Poseidon suddenly extends his hand, calling back the Trident like a missile.\n");
            printf(" At that moment, the sky tears open! Helios sends down a pure, blinding ray of divine energy\n");
            printf(" through the clouds. But the dark monster absorbs this colossal power like a black hole!\n\n");
            printf(COLOR_DARK " [Press ANY KEY to continue]\n" COLOR_RESET);
            _getch(); clear_screen();

            printf(COLOR_GOLD "\n [SCENE V] Tsunami\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Poseidon churns the ocean, lifting the beast with a massive water pillar\n");
            printf(" and delivers 3 deadly slashes from below with his Trident!\n\n");
            printf(" Seizing the opportunity, Apollo fiercely fires his sun-bright finishing arrow.\n");
            printf(" But just as the arrow is about to strike, the creature violently expels all the energy it absorbed!\n");
            printf(" Two immense divine forces collide. The arrow ricochets and crashes into the ocean,\n");
            printf(" splitting the sea in half and creating a colossal TSUNAMI that reaches the heavens!\n\n" COLOR_RESET);
            printf(COLOR_DARK " [Press ANY KEY to continue]\n" COLOR_RESET);
            _getch(); clear_screen();

            printf(COLOR_RED "\n BOOOOOM!\n\n" COLOR_RESET);
            printf(COLOR_WHITE " The Tsunami swallows the ship like a toothpick. The massive force launches you meters into the air!\n");
            printf(" You are now in the sky, looking around in utter bewilderment.\n\n" COLOR_RESET);
        }

        // --- GİZLİ İNANÇ ZARI (HELIOS MİNİ-OYUNU) ---
        if (profile->faith >= 7) {
            if (current_lang == 1) {
                printf(COLOR_CYAN " [ GİZLİ ZAR BAŞARILI: İNANCIN (>=7) İLAHİ BİR FIRSAT YARATTI ]\n" COLOR_RESET);
                printf(COLOR_RED " Havadan düşüyorsun! Debelenmek ve dikkat çekmek için SPACE tuşuna ABAN!\n\n" COLOR_RESET);
            } else {
                printf(COLOR_CYAN " [ HIDDEN DICE SUCCESS: YOUR FAITH (>=7) CREATED A DIVINE OPPORTUNITY ]\n" COLOR_RESET);
                printf(COLOR_RED " You are falling! Mash SPACE to struggle and draw attention!\n\n" COLOR_RESET);
            }

            int struggle_hits = 0;
            // 3.5 saniyelik debelenme süresi
            for (int i = 0; i < 70; i++) {
                if (_kbhit()) {
                    if (_getch() == ' ') {
                        struggle_hits++;
                        shake_console(15, 30); // SPACE'e basıldıkça ekran gerçekten sallanır!
                        if (current_lang == 1) printf("\r" COLOR_CYAN " Debelenme Gücü: [%d / 15] " COLOR_RESET, struggle_hits);
                        else printf("\r" COLOR_CYAN " Struggle Power: [%d / 15] " COLOR_RESET, struggle_hits);
                    }
                }
                Sleep(50);
            }

            clear_screen();
            if (struggle_hits >= 15) {
                if (current_lang == 1) {
                    printf(COLOR_GOLD "\n [SAHNE V] Helios'un Lütfu\n\n" COLOR_RESET);
                    printf(COLOR_WHITE " Muazzam çırpınışın gökyüzünde parlayan güneş tanrısının dikkatini çekti!\n");
                    printf(" Helios, alev alev yanan arabasıyla yanından geçerken seni ensenden yakaladığı gibi\n");
                    printf(" arabanın arkasına fırlatıyor. Okyanusun ölümcül soğuğundan kurtuldun!\n\n" COLOR_RESET);
                } else {
                    printf(COLOR_GOLD "\n [SCENE V] Grace of Helios\n\n" COLOR_RESET);
                    printf(COLOR_WHITE " Your fierce struggle caught the eye of the shining sun god in the sky!\n");
                    printf(" As Helios blazes past in his fiery chariot, he grabs you by the scruff of your neck\n");
                    printf(" and throws you into the back. You are saved from the deadly freezing ocean!\n\n" COLOR_RESET);
                }
                profile->honor += 2; // Kurtuluş Bonusu
            } else {
                if (current_lang == 1) {
                    printf(COLOR_DARK "\n [SAHNE V] Düşüş\n\n" COLOR_RESET);
                    printf(COLOR_WHITE " Çırpınışların yetersiz kaldı... Tanrıların gözünde bir hiçsin.\n");
                    printf(" Son sürat denize çakılıyor ve muazzam çarpmanın etkisiyle anında sersemliyorsun.\n");
                    printf(" Bilincin yavaşça kapanırken, fırtınanın içinde dibe gömülmeye başlıyorsun...\n\n" COLOR_RESET);
                } else {
                    printf(COLOR_DARK "\n [SCENE V] The Fall\n\n" COLOR_RESET);
                    printf(COLOR_WHITE " Your struggles were in vain... You are nothing in the eyes of the gods.\n");
                    printf(" You plunge into the sea at terminal velocity, instantly stunned by the massive impact.\n");
                    printf(" As your consciousness slowly fades, you begin to sink into the dark storm...\n\n" COLOR_RESET);
                }
            }
            if (current_lang == 1) printf(COLOR_CYAN " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
            else printf(COLOR_CYAN " [Press ANY KEY to continue]\n" COLOR_RESET);
            _getch();
        } else {
            // İnancı zayıf olanlar (Faith < 7) ekran sallanması (QTE) şansını bulamazlar
            if (current_lang == 1) {
                printf(COLOR_DARK "\n [SAHNE V] Kaçınılmaz Düşüş\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Havada çaresizce süzülüyorsun. Hiçbir ilahi varlık seni fark etmiyor bile.\n");
                printf(" Son sürat denize çakılıyor ve muazzam çarpmanın etkisiyle anında sersemliyorsun.\n");
                printf(" Bilincin yavaşça kapanırken, fırtınanın karanlık dibine gömülmeye başlıyorsun...\n\n" COLOR_RESET);
                printf(COLOR_CYAN " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
            } else {
                printf(COLOR_DARK "\n [SCENE V] The Inevitable Fall\n\n" COLOR_RESET);
                printf(COLOR_WHITE " You drift helplessly in the air. No divine being even notices you.\n");
                printf(" You plunge into the sea at terminal velocity, instantly stunned by the massive impact.\n");
                printf(" As your consciousness slowly fades, you sink into the dark abyss of the storm...\n\n" COLOR_RESET);
                printf(COLOR_CYAN " [Press ANY KEY to continue]\n" COLOR_RESET);
            }
            _getch();
        }
    }
    // C - BOĞULANLARIN KADERİ (Sadece 1.2: Halatla uçup denize düşenler)
    else if (prev_choice == 1 && choice_2 == 2) {
        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE V] Sessizliğin Dibi\n\n" COLOR_RESET);

            if (choice_3 == 2) {
                // Seçim 2: Poseidon'a yakaranlar (İnanç)
                printf(COLOR_WHITE " Zihnindeki o karanlık varlığa değil, yukarıda suları yaran denizler tanrısına\n");
                printf(" yakarıyorsun. Ancak tanrıların kulakları fani karıncaların çığlıklarına sağırdır.\n");
                printf(" Zihnindeki ses soğuk ve alaycı bir tonda fısıldar:\n\n" COLOR_RESET);
                printf(COLOR_CYAN " \"Sen bilirsin...\"\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Sonra zihnindeki ses tamamen susar. Poseidon'un devasa silüeti umursamazca\n");
                printf(" yukarıda kaybolurken, dondurucu soğuk bedenini ele geçirir. Hiçbir ilahi el sana uzanmaz.\n");
                printf(" Mutlak karanlığın içine doğru yavaşça batmaya devam edersin...\n\n" COLOR_RESET);
            }
            else if (choice_3 == 3) {
                // Seçim 3: Sen kimsin diye soranlar (Zeka)
                printf(COLOR_WHITE " Son nefesinde bile mantığını korumaya, karşındaki bilinmezi çözmeye çalışıyorsun.\n");
                printf(" \"Sen kimsin?\" diye yankılanır düşüncelerin suların altında.\n\n");
                printf(" Cevap, sadece sağır edici bir sessizliktir.\n\n");
                printf(" Ne okyanus, ne de o kadim varlık senin fani merakını gidermeye tenezzül etmez.\n");
                printf(" Suyun amansız basıncı son düşüncelerini de ezerken, bilincin tamamen kapanır\n");
                printf(" ve zifiri karanlığa, hiçliğe doğru batarsın...\n\n" COLOR_RESET);
            }
            else {
                // Seçim 1: İnat edenler (Güç)
                printf(COLOR_WHITE " Son bir can havliyle çırpınmaya, hayatta kalmaya çalışıyorsun. Adalelerindeki\n");
                printf(" son oksijeni de bu isyanla yakıyorsun. O yankılı, kadim ses bu kez zihnini\n");
                printf(" uyuşturan bir ninni gibi, sakinleştirici bir tonda fısıldar:\n\n" COLOR_RESET);
                printf(COLOR_CYAN " \"Debelenmeyi bırak fani... Acı çekiyorsun. O etten kabuğu,\n");
                printf(" fani bedenini serbest bırak... Sulara karış ve sadece rahatla...\"\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Sesin manipülatif ağırlığı iradeni kırar. Kaslarındaki gerilim boşalır,\n");
                printf(" kolların iki yana düşer ve karanlığın soğuk kucaklamasıyla yavaşça batmaya devam edersin...\n\n" COLOR_RESET);
            }
            printf(COLOR_DARK " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);

        } else {
            printf(COLOR_GOLD "\n [SCENE V] Depth of Silence\n\n" COLOR_RESET);

            if (choice_3 == 2) {
                printf(COLOR_WHITE " You plead not to the dark entity in your mind, but to the god of the seas tearing\n");
                printf(" through the waters above. Yet, the ears of gods are deaf to the screams of mortal ants.\n");
                printf(" The voice in your mind whispers in a cold, mocking tone:\n\n" COLOR_RESET);
                printf(COLOR_CYAN " \"Have it your way...\"\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Then, the voice goes completely silent. As Poseidon's colossal silhouette carelessly\n");
                printf(" disappears above, the freezing cold claims your body. No divine hand reaches out to you.\n");
                printf(" You continue to sink slowly into absolute darkness...\n\n" COLOR_RESET);
            }
            else if (choice_3 == 3) {
                printf(COLOR_WHITE " Even with your last breath, you try to maintain your logic, to solve the unknown.\n");
                printf(" \"Who are you?\" your thoughts echo beneath the waters.\n\n");
                printf(" The only answer is deafening silence.\n\n");
                printf(" Neither the ocean nor that ancient entity deigns to satisfy your mortal curiosity.\n");
                printf(" As the relentless water pressure crushes your final thoughts, your consciousness\n");
                printf(" fades completely, and you sink into the pitch black void...\n\n" COLOR_RESET);
            }
            else {
                printf(COLOR_WHITE " You struggle with your last ounce of strength, desperately trying to survive. You burn\n");
                printf(" the last oxygen in your muscles with this rebellion. The echoing, ancient voice now\n");
                printf(" whispers in a soothing tone, like a mind-numbing lullaby:\n\n" COLOR_RESET);
                printf(COLOR_CYAN " \"Cease your struggling, mortal... You are in pain. Release that shell of flesh,\n");
                printf(" your mortal body... Blend with the waters, and just relax...\"\n\n" COLOR_RESET);
                printf(COLOR_WHITE " The manipulative weight of the voice breaks your will. The tension in your muscles\n");
                printf(" vanishes, your arms fall to your sides, and embraced by the dark cold, you continue to sink...\n\n" COLOR_RESET);
            }
            printf(COLOR_DARK " [Press ANY KEY to continue]\n" COLOR_RESET);
        }
        _getch();
    }
    // C - AMBARDAKİLERİN KADERİ (2.1, 2.2, 2.3)
    else if (prev_choice == 2) {
        // 2.1 - Kapağı Kırıp Çıkanlar (Savaşı İzleyip Tsunami ile Boğulanlar)
        if (choice_3 == 1) {
            if (current_lang == 1) {
                printf(COLOR_GOLD "\n [SAHNE V] Kıyamete Adım\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Ambar kapağını tekmeyle kırıp güverteye adımını attığın an, gökyüzünün yarıldığını görüyorsun.\n");
                printf(" Savaşın zirvesi... Helios'un kör edici ilahi ışını ve Poseidon'un Trident'i canavarla çarpışıyor.\n");
                printf(" Apollon'un devasa oku sekerek denize çakılıyor ve gökyüzüne uzanan devasa bir TSUNAMİ yaratıyor!\n\n");
                printf(" Kaçacak hiçbir yerin yok. Devasa su kütlesi gemiyi bir kibrit çöpü gibi yutuyor.\n");
                printf(" Suların şiddeti seni anında eziyor ve karanlık okyanusun dibinde boğuluyorsun...\n\n" COLOR_RESET);
                printf(COLOR_CYAN " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
            } else {
                printf(COLOR_GOLD "\n [SCENE V] Step into the Apocalypse\n\n" COLOR_RESET);
                printf(COLOR_WHITE " The moment you kick the hatch open and step on deck, you see the sky tear apart.\n");
                printf(" The climax of war... Helios's blinding divine ray and Poseidon's Trident clash with the beast.\n");
                printf(" Apollo's massive arrow ricochets, crashing into the sea to create a colossal TSUNAMI!\n\n");
                printf(" You have nowhere to run. The massive wall of water swallows the ship like a matchstick.\n");
                printf(" The violence of the waters instantly crushes you, and you drown at the bottom of the dark ocean...\n\n" COLOR_RESET);
                printf(COLOR_CYAN " [Press ANY KEY to continue]\n" COLOR_RESET);
            }
            _getch();
        }
        // 2.2 - Sessizce Bekleyenler (Yanlış Karar ve Basınçla Ezilme)
        // 2.2 - Sessizce Bekleyenler (Yanlış Karar, Ezilme ve Boğulma)
        else if (choice_3 == 2) {
            set_badge(profile, BADGE_BROKEN_BONES, 5); // Ezilip kırılma badge'i eklendi!
            if (current_lang == 1) {
                printf(COLOR_GOLD "\n [SAHNE V] Hiçliğin Baskısı\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Karanlıkta güvende olduğunu sanarak sessizce beklerken, zihninin içinde\n");
                printf(" yankılanan kadim ve ürpertici bir ses konuşuyor:\n\n");
                printf(COLOR_DARK " \"Çok... Yanlış... Bir karar...\"\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Ses kesildiği an, gemi birden hiçliğin dibine doğru inanılmaz bir hızla batmaya başlıyor.\n");
                printf(" Devasa su basıncı saniyeler içinde geminin gövdesini kağıt gibi eziyor!\n");
                printf(" Bütün kemiklerinin aynı anda kırıldığını hissediyorsun. Ambar saniyeler içinde suyla doluyor.\n");
                printf(" Kırık bedenini kıpırdatamadan, karanlık ve dondurucu suların içinde boğuluyorsun...\n\n" COLOR_RESET);
                printf(COLOR_CYAN " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
            } else {
                printf(COLOR_GOLD "\n [SCENE V] Pressure of the Void\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Thinking you are safe, an ancient, chilling voice echoes directly inside your mind:\n\n");
                printf(COLOR_DARK " \"A very... wrong... decision...\"\n\n" COLOR_RESET);
                printf(COLOR_WHITE " The ship suddenly begins sinking to the bottom of the abyss at incredible speed.\n");
                printf(" Massive water pressure crushes the ship's hull like paper in seconds!\n");
                printf(" You feel all your bones shatter simultaneously. The hold fills with water instantly.\n");
                printf(" Unable to move your broken body, you drown in the dark, freezing waters...\n\n" COLOR_RESET);
                printf(COLOR_CYAN " [Press ANY KEY to continue]\n" COLOR_RESET);
            }
            _getch();
        }
        // 2.3 - Sandık Yağmalayanlar (Thanatos'un Laneti ve Boğulma)
        else if (choice_3 == 3) {
            set_badge(profile, BADGE_CURSE_OF_THANATOS, 1);
            if (current_lang == 1) {
                printf(COLOR_GOLD "\n [SAHNE V] Thanatos'un Dokunuşu\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Kaosun içinde bulduğun bronz kılıç aniden elinden kurtulup havaya kalkıyor!\n");
                printf(" Kılıcın yüzeyinden Ölüm Tanrısı Thanatos'un kapkara silüeti süzülüyor ve\n");
                printf(" soğuk parmaklarıyla alnına dokunuyor. Kafanın içinde paranoyak bir lanet çınlıyor!\n\n");
                printf(COLOR_DARK " Kılıç bir anda hiçliğe karışıyor.\n" COLOR_RESET);
                printf(COLOR_WHITE " Sen ne olduğunu bile anlayamadan, gemi devasa bir şiddetle paramparça oluyor!\n");
                printf(" Deliliğin eşiğindeyken suların karanlık dibine çekiliyor ve enkazın arasında\n");
                printf(" ciğerlerine dolan okyanus sularıyla çaresizce boğuluyorsun...\n\n" COLOR_RESET);
                printf(COLOR_CYAN " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
            } else {
                printf(COLOR_GOLD "\n [SCENE V] Touch of Thanatos\n\n" COLOR_RESET);
                printf(COLOR_WHITE " The bronze sword you found suddenly slips from your grip and levitates!\n");
                printf(" Thanatos's silhouette emerges, touching your forehead. A paranoid curse echoes in your mind!\n\n");
                printf(COLOR_DARK " The sword vanishes into nothingness.\n" COLOR_RESET);
                printf(COLOR_WHITE " Before you can process it, the ship violently shatters!\n");
                printf(" On the verge of madness, you are dragged to the dark bottom of the waters, helplessly\n");
                printf(" drowning among the debris as the ocean fills your lungs...\n\n" COLOR_RESET);
                printf(COLOR_CYAN " [Press ANY KEY to continue]\n" COLOR_RESET);
            }
            _getch();
        }
        // 2.3 - Sandık Yağmalayanlar (Thanatos'un Laneti)
        else if (choice_3 == 3) {
            set_badge(profile, BADGE_CURSE_OF_THANATOS, 1);
            if (current_lang == 1) {
                printf(COLOR_GOLD "\n [SAHNE V] Thanatos'un Dokunuşu\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Kaosun içinde karanlıkta sandıkları parçalarken, eline ağır, soğuk bir cisim geliyor.\n");
                printf(" Bu bronz bir kılıç... Ancak kılıç aniden elinden kurtulup havaya kalkıyor!\n\n");
                printf(" Kılıcın bronz yüzeyinden Ölüm Tanrısı Thanatos'un kapkara silüeti dışarı süzülüyor.\n");
                printf(" Silüet yavaşça yaklaşıp soğuk parmaklarıyla alnına dokunuyor.\n");
                printf(" Kafanın içinde aniden, seni delirtecek kadar paranoyak bir lanet sesi çınlıyor!\n\n");
                printf(COLOR_DARK " Kılıç bir anda hiçliğe karışıp kayboluyor.\n" COLOR_RESET);
                printf(COLOR_WHITE " Sen ne olduğunu bile anlayamadan, gemi devasa bir şiddetle parçalanıyor\n");
                printf(" ve enkazla birlikte suyun karanlık dibine doğru hızla çekilmeye başlıyorsun...\n\n" COLOR_RESET);
                printf(COLOR_CYAN " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
            } else {
                printf(COLOR_GOLD "\n [SCENE V] Touch of Thanatos\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Smashing crates in the dark amidst the chaos, your hand grasps a heavy, cold object.\n");
                printf(" It is a bronze sword... But suddenly, it slips from your grip and levitates!\n\n");
                printf(" From its bronze surface, the pitch-black silhouette of Thanatos, God of Death, emerges.\n");
                printf(" The silhouette glides closer and touches your forehead with its freezing fingers.\n");
                printf(" Suddenly, a paranoid curse echoes violently inside your head, enough to drive you mad!\n\n");
                printf(COLOR_DARK " The sword vanishes into nothingness in an instant.\n" COLOR_RESET);
                printf(COLOR_WHITE " Before you can even process what happened, the ship violently shatters\n");
                printf(" and you are rapidly dragged down to the dark bottom of the water with the debris...\n\n" COLOR_RESET);
                printf(COLOR_CYAN " [Press ANY KEY to continue]\n" COLOR_RESET);
            }
            _getch();
        }
    }
    // D - HAVADAKİLERİN KADERİ (3.1: Kayarak kaçıp havaya uçanlar)
    else if (prev_choice == 3 && choice_2 == 1) {

        // 3.1.1 - Tırabzana Tutunanlar (Güvenli Limanın Yıkılışı ve Boğulma)
        if (choice_3 == 1) {
            if (current_lang == 1) {
                printf(COLOR_GOLD "\n [SAHNE V] Denizin Dibine\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Tırabzanlara tutunduğun an, Poseidon mermi gibi göğe uçan Trident'ini geri çağırıyor.\n\n");
                printf(" Devasa mızrak okyanustan çıkıp efendisine dönerken yarattığı muazzam şok dalgası,\n");
                printf(" tutunduğun gemi enkazını tuzla buz ediyor!\n");
                printf(" Tırabzan parçalanıyor... Suların muazzam şiddeti seni okyanusun derinliklerine gömerken,\n");
                printf(" yukarı çıkamıyor ve karanlık suların içinde ciğerlerin patlayarak boğuluyorsun...\n\n" COLOR_RESET);
                printf(COLOR_CYAN " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
            } else {
                printf(COLOR_GOLD "\n [SCENE V] To the Bottom\n\n" COLOR_RESET);
                printf(COLOR_WHITE " The moment you grab the railing, Poseidon recalls his Trident like a missile.\n\n");
                printf(" As the colossal spear rips out of the ocean, the massive shockwave shatters\n");
                printf(" the wreckage you are holding onto into dust!\n");
                printf(" The railing splinters... The massive force of the waters buries you deep in the ocean.\n");
                printf(" Unable to surface, your lungs burst and you drown in the dark waters...\n\n" COLOR_RESET);
                printf(COLOR_CYAN " [Press ANY KEY to continue]\n" COLOR_RESET);
            }
            _getch();
        }

        // 3.1.2 - Poseidon'a Güvenenler (Tanrının Avucunda ve PARRY Mekaniği)
        else if (choice_3 == 2) {
            set_badge(profile, BADGE_BLESSING_POSEIDON, 1);
            if (current_lang == 1) {
                printf(COLOR_GOLD "\n [SAHNE V] Tanrıların Avucunda\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Gözlerini kapatıp düşüşü kabulleniyorsun. Sulara çakılmadan milisaniyeler önce,\n");
                printf(" devasa, buz gibi bir el seni havada yakalıyor!\n\n");
                printf(" Poseidon'un muazzam avucunun içindesin. Savaşı bizzat onun göz hizasından izliyorsun.\n");
                printf(" Ancak Apollon'un devasa oku sekip okyanusta patladığında, Poseidon şiddetle yalpalıyor.\n");
                printf(" Parmakları aralanıyor ve boşluğa, ölüme doğru düşmeye başlıyorsun!\n\n");
                printf(" Tam okyanusa çakılacakken, alev alev yanan bir savaş arabası yanından geçiyor\n");
                printf(" ve Helios seni ensenden yakaladığı gibi arabasının arkasına fırlatıyor!\n\n" COLOR_RESET);
                printf(COLOR_DARK " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
            } else {
                printf(COLOR_GOLD "\n [SCENE V] In the Palm of Gods\n\n" COLOR_RESET);
                printf(COLOR_WHITE " You close your eyes and accept the fall. Milliseconds before hitting the water,\n");
                printf(" a colossal, freezing hand catches you mid-air!\n\n");
                printf(" You are in the palm of Poseidon. You watch the war from his exact POV.\n");
                printf(" But when Apollo's massive arrow ricochets and explodes in the ocean, Poseidon violently stumbles.\n");
                printf(" His fingers part, and you begin falling into the void towards certain death!\n\n");
                printf(" Just as you are about to crash into the ocean, a blazing chariot streaks past\n");
                printf(" and Helios grabs you by the scruff, throwing you into the back of his chariot!\n\n" COLOR_RESET);
                printf(COLOR_DARK " [Press ANY KEY to continue]\n" COLOR_RESET);
            }
            _getch(); clear_screen();

            // PARRY MINI-GAME
            int attack_dir = rand() % 2; // 0 = Sol, 1 = Sağ
            if (current_lang == 1) {
                printf(COLOR_GOLD "\n [SAHNE V] Göklerde Hayatta Kalma\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Güneş arabasında, gökyüzünde korkunç bir hızla ilerliyorsunuz. Savaşın yıkımı her yerde.\n");
                printf(" Savaş anlatılırken aniden devasa bir enkaz parçası hızla üzerine doğru savruluyor!\n\n" COLOR_RESET);

                if (attack_dir == 0) printf(COLOR_RED "  >>> SOL TARAFTAN ENKAZ GELİYOR! (PARRY İÇİN SAĞ OK TUŞUNA BAS) >>>\n\n" COLOR_RESET);
                else printf(COLOR_RED "  <<< SAĞ TARAFTAN ENKAZ GELİYOR! (PARRY İÇİN SOL OK TUŞUNA BAS) <<<\n\n" COLOR_RESET);
            } else {
                printf(COLOR_GOLD "\n [SCENE V] Survival in the Skies\n\n" COLOR_RESET);
                printf(COLOR_WHITE " You are moving at terrifying speed in the sun chariot. Destruction is everywhere.\n");
                printf(" As the battle unfolds, suddenly a massive piece of debris hurtles towards you!\n\n" COLOR_RESET);

                if (attack_dir == 0) printf(COLOR_RED "  >>> DEBRIS INCOMING FROM LEFT! (PRESS RIGHT ARROW TO PARRY) >>>\n\n" COLOR_RESET);
                else printf(COLOR_RED "  <<< DEBRIS INCOMING FROM RIGHT! (PRESS LEFT ARROW TO PARRY) <<<\n\n" COLOR_RESET);
            }

            int required_key = (attack_dir == 0) ? 77 : 75; // 77 = Sağ Ok, 75 = Sol Ok
            int parry_success = 0;
            long start_time = clock();

            // 2 saniyelik reaksiyon süresi (Zamanlayıcı)
            while ((clock() - start_time) * 1000 / CLOCKS_PER_SEC < 2000) {
                if (_kbhit()) {
                    int ch = _getch();
                    if (ch == 0 || ch == 224) { // Windows Ok tuşu tanımlayıcıları
                        ch = _getch();
                        if (ch == required_key) { parry_success = 1; break; }
                        else if (ch == 75 || ch == 77 || ch == 72 || ch == 80) { parry_success = -1; break; } // Yanlış yön
                    }
                }
                Sleep(10);
            }

            clear_screen();
            if (parry_success == 1) { // PARRY BAŞARILI
                set_badge(profile, BADGE_BLESSING_HELIOS, 1);
                if (current_lang == 1) {
                    printf(COLOR_CYAN " [ PARRY BAŞARILI! ]\n\n" COLOR_RESET);
                    printf(COLOR_WHITE " Vücudunu mükemmel bir refleksle zıt yöne savuruyorsun!\n");
                    printf(" Devasa enkaz arabayı sıyırıp geçiyor. Helios arabasını göğe doğru sürerken\n");
                    printf(" hayatta kalmayı başarıyorsun...\n\n" COLOR_RESET);
                } else {
                    printf(COLOR_CYAN " [ PARRY SUCCESSFUL! ]\n\n" COLOR_RESET);
                    printf(COLOR_WHITE " You throw your body in the opposite direction with perfect reflex!\n");
                    printf(" The massive debris grazes past the chariot. As Helios drives his chariot\n");
                    printf(" towards the heavens, you manage to survive...\n\n" COLOR_RESET);
                }
                profile->skill += 2; // Başarı bonusu
            }
            else { // PARRY BAŞARISIZ
                if (current_lang == 1) {
                    printf(COLOR_RED " [ PARRY BAŞARISIZ! ]\n\n" COLOR_RESET);
                    printf(COLOR_WHITE " Yanlış hamle yaptın! Devasa enkaz sana şiddetle çarpıyor.\n");
                    printf(" Arabadan sökülüp gökyüzünden okyanusa doğru ölümcül bir düşüşe geçiyorsun!\n\n" COLOR_RESET);
                } else {
                    printf(COLOR_RED " [ PARRY FAILED! ]\n\n" COLOR_RESET);
                    printf(COLOR_WHITE " Wrong move! The massive debris violently crashes into you.\n");
                    printf(" You are ripped from the chariot, entering a fatal freefall towards the ocean!\n\n" COLOR_RESET);
                }

                // Gizli İnanç Zarı Devreye Giriyor
                if (profile->faith >= 7) {
                    set_badge(profile, BADGE_BLESSING_HELIOS, 1);
                    if (current_lang == 1) {
                        printf(COLOR_CYAN " [ GİZLİ İNANÇ ZARI BAŞARILI: İNANCIN (>=7) SENİ KORUYOR ]\n\n" COLOR_RESET);
                        printf(COLOR_GOLD " Ancak Helios seni bir kez daha gözden çıkarmıyor!\n");
                        printf(" Güneş arabasıyla inanılmaz bir dalış yapıp sulara çakılmadan hemen önce\n");
                        printf(" seni tekrar ensenden yakalıyor. İkinci bir şans elde ettin.\n\n" COLOR_RESET);
                    } else {
                        printf(COLOR_CYAN " [ HIDDEN FAITH DICE SUCCESS: YOUR FAITH (>=7) PROTECTS YOU ]\n\n" COLOR_RESET);
                        printf(COLOR_GOLD " But Helios does not give up on you!\n");
                        printf(" He makes an incredible dive with his sun chariot, catching you by the scruff\n");
                        printf(" right before you hit the water. You have been granted a second chance.\n\n" COLOR_RESET);
                    }
                } else {
                    if (current_lang == 1) {
                        printf(COLOR_DARK " [ GİZLİ İNANÇ ZARI BAŞARISIZ ]\n\n" COLOR_RESET);
                        printf(COLOR_WHITE " Havada çaresizce süzülüyorsun. Tanrılar artık seninle ilgilenmiyor.\n");
                        printf(" Son sürat denize çakılıyor ve boğularak karanlık sulara gömülüyorsun...\n\n" COLOR_RESET);
                    } else {
                        printf(COLOR_DARK " [ HIDDEN FAITH DICE FAILED ]\n\n" COLOR_RESET);
                        printf(COLOR_WHITE " You drift helplessly in the air. The gods no longer care for you.\n");
                        printf(" You plunge into the sea at terminal velocity, drowning in the dark waters...\n\n" COLOR_RESET);
                    }
                }
            }
            if (current_lang == 1) printf(COLOR_CYAN " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
            else printf(COLOR_CYAN " [Press ANY KEY to continue]\n" COLOR_RESET);
            _getch();
        }

        // 3.1.3 - Poseidon'a Bağıranlar (Kemikleri Kırılan Karınca)
        else if (choice_3 == 3) {
            set_badge(profile, BADGE_BROKEN_BONES, 5);
            if (current_lang == 1) {
                printf(COLOR_GOLD "\n [SAHNE V] Karıncanın Düşüşü\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Bütün gücünle Poseidon'a bağırıyorsun! Ama okyanusların efendisi,\n");
                printf(" senin gibi bir karıncayı umursamıyor bile.\n\n");
                printf(" Havada savrulup, onun devasa, kayadan sert omuzlarına şiddetle çarpıyorsun!\n");
                printf(" Kaburgalarının ve kollarının aynı anda kırılma sesini duyuyorsun.\n");
                printf(" Sersemlemiş, acıdan çıldıracak bir halde, metrelerce yüksekten tekrar\n");
                printf(" geminin ahşap güvertesine çakılıyorsun. Bedenin paramparça...\n\n" COLOR_RESET);

                printf("  [" COLOR_CYAN "1" COLOR_RESET "] Kırık kemiklerime rağmen inatla güvenli bir yere doğru sürünmeye çalışırım.\n");
                printf("  [" COLOR_CYAN "2" COLOR_RESET "] Olduğum yerde kanlar içinde yatarak, ölümcül kaderimi sessizce kabullenirim.\n\n");
                printf(COLOR_CYAN "  Hamlen nedir? (1-2): " COLOR_RESET);
            } else {
                printf(COLOR_GOLD "\n [SCENE V] Fall of the Ant\n\n" COLOR_RESET);
                printf(COLOR_WHITE " You roar at Poseidon with all your might! But the lord of the oceans\n");
                printf(" does not even care about an ant like you.\n\n");
                printf(" You are tossed through the air, violently crashing into his massive, rock-hard shoulder!\n");
                printf(" You hear the sickening snap of your ribs and arms breaking simultaneously.\n");
                printf(" Dazed and mad with pain, you plummet from meters high, crashing back down\n");
                printf(" onto the ship's wooden deck. Your body is shattered...\n\n" COLOR_RESET);

                printf("  [" COLOR_CYAN "1" COLOR_RESET "] Despite my broken bones, I stubbornly try to crawl to a safe spot.\n");
                printf("  [" COLOR_CYAN "2" COLOR_RESET "] I lie there covered in blood, silently accepting my fatal destiny.\n\n");
                printf(COLOR_CYAN "  What is your move? (1-2): " COLOR_RESET);
            }

            int crawl_choice = 0;
            while (1) {
                if (_kbhit()) {
                    char ch = _getch();
                    if (ch == '1') { profile->might += 1; profile->intel -= 1; crawl_choice = 1; break; }
                    else if (ch == '2') { profile->faith += 1; profile->might -= 1; crawl_choice = 2; break; }
                }
                Sleep(20);
            }

            clear_screen();

            // SÜRÜNENLER (HELIOS'UN YENİ KURTARIŞI)
            if (crawl_choice == 1) {
                set_badge(profile, BADGE_BLESSING_HELIOS, 1);
                if (current_lang == 1) {
                    printf(COLOR_GOLD "\n [SAHNE V] Yaşama İnadı\n\n" COLOR_RESET);
                    printf(COLOR_WHITE " Kanlar içinde güvertede sürünürken gökyüzünü izliyorsun.\n");
                    printf(" Apollon'un o devasa oku canavara çarpıyor ve denizi yaran Tsunami göklere yükseliyor.\n");
                    printf(" Savaşın kaosundan iğrenip ortamı terk etmeye hazırlanan Helios, güneş arabasını\n");
                    printf(" ufka doğru çeviriyor...\n\n");
                    printf(" Ancak tam o an, alev alev yanan gözleri, kıyametin ortasında paramparça bir halde\n");
                    printf(" inatla sürünmeye devam eden senin kanlı silüetine takılıyor.\n\n");
                    printf(COLOR_CYAN " Tanrıları bile şaşırtan bu saf yaşama arzusu karşısında, Helios arabasını\n");
                    printf(" inanılmaz bir hızla aşağı kırıyor!\n\n" COLOR_RESET);
                    printf(COLOR_WHITE " Tsunami seni yutmadan milisaniyeler önce arabasıyla yanından geçerek seni\n");
                    printf(" ensenden yakaladığı gibi o cehennemden çıkarıp göklere yükseltiyor!\n\n" COLOR_RESET);
                    printf(COLOR_DARK " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
                } else {
                    printf(COLOR_GOLD "\n [SCENE V] Defiance of Death\n\n" COLOR_RESET);
                    printf(COLOR_WHITE " Crawling in blood, you watch the sky. Apollo's massive arrow strikes the beast,\n");
                    printf(" and the sea-splitting Tsunami rises to the heavens. Helios, disgusted by the chaos,\n");
                    printf(" turns his sun chariot to leave the realm...\n\n");
                    printf(" But at that exact moment, his burning eyes catch your bloody silhouette,\n");
                    printf(" stubbornly crawling on the deck amidst the apocalypse.\n\n");
                    printf(COLOR_CYAN " Astonished by this pure mortal will to live, Helios dives his chariot\n");
                    printf(" down at blinding speed!\n\n" COLOR_RESET);
                    printf(COLOR_WHITE " Milliseconds before the Tsunami swallows you, he streaks past, grabs you by\n");
                    printf(" the scruff, and rips you out of that hell, soaring into the heavens!\n\n" COLOR_RESET);
                    printf(COLOR_DARK " [Press ANY KEY to continue]\n" COLOR_RESET);
                }
            }
            // KADERİ KABULLENENLER (BOĞULMA)
            else {
                if (current_lang == 1) {
                    printf(COLOR_DARK "\n [SAHNE V] Kaçınılmaz Son\n\n" COLOR_RESET);
                    printf(COLOR_WHITE " Yerde kanlar içinde yatarken, savaşın zirvesinde iki ilahi gücün çarpıştığını görüyorsun.\n");
                    printf(" Apollon'un oku sekiyor ve göklere uzanan devasa TSUNAMİ üzerine doğru kapanıyor.\n");
                    printf(" Artık kaçacak hiçbir yerin yok.\n\n");
                    printf(" Devasa su kütlesi gemiyi yuttuğu an, okyanusun karanlık derinliklerine gömülüyorsun.\n");
                    printf(" Kemiklerin kırık olduğu için çırpınamıyorsun bile... Ciğerlerine dolan soğuk sularla\n");
                    printf(" yavaşça boğularak hiçliğe karışıyorsun...\n\n" COLOR_RESET);
                    printf(COLOR_CYAN " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
                } else {
                    printf(COLOR_DARK "\n [SCENE V] The Inevitable End\n\n" COLOR_RESET);
                    printf(COLOR_WHITE " Lying in blood, you see two divine forces clash at the climax of the battle.\n");
                    printf(" Apollo's arrow ricochets, and a colossal TSUNAMI crashes down upon you.\n");
                    printf(" You have nowhere left to run.\n\n");
                    printf(" The moment the massive wall of water swallows the ship, you are buried in the dark depths.\n");
                    printf(" With shattered bones, you cannot even struggle... You slowly drown as the freezing waters\n");
                    printf(" fill your lungs, fading into the void...\n\n" COLOR_RESET);
                    printf(COLOR_CYAN " [Press ANY KEY to continue]\n" COLOR_RESET);
                }
            }
            _getch();
        }
    }

    // E - SON EKSİK (Sadece 1.2: Boğulanlar kaldı)
    else {
        if (current_lang == 1) printf(COLOR_DARK "\n [Sadece 1.2 Boğulanlar Grubunun Sahne 5 Finali Eksik...]\n" COLOR_RESET);
        else printf(COLOR_DARK "\n [Only Group 1.2 Drowners' Scene 5 Final is Missing...]\n" COLOR_RESET);
        Sleep(1500);
    }
    // ========================================================================
    // SAHNE VI: GÜNEŞ ARABASI (Sadece Helios Tarafından Kurtarılanlar)
    // ========================================================================
    if (profile->badges[BADGE_BLESSING_HELIOS] == 1) {
        clear_screen();
        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE VI] Güneşin Merhameti\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Savaş geride kalıyor. Bulutların arasında hızla yükseliyorsunuz.\n");
            printf(" Etrafı saran o sağır edici kaos yerini rüzgarın uğultusuna bırakıyor.\n\n");
            printf(" Helios yavaşça sana dönüyor. Arabasından yayılan güneş ışığının tenini kavurduğunu,\n");
            printf(" ilahi ve sert bir ısıyla bedenini sardığını hissediyorsun.\n\n" COLOR_RESET);

            // Eğer karakterin kemikleri kırıksa (Rozet kontrolü)
            if (profile->badges[BADGE_BROKEN_BONES] > 0) {
                printf(COLOR_CYAN " Güneşin bu saf gücü, kırık kemiklerine nüfuz ediyor. Acı yerini sıcak bir uyuşmaya bırakırken,\n");
                printf(" paramparça olmuş bedeninin mucizevi bir şekilde saniyeler içinde iyileştiğini hissediyorsun!\n\n" COLOR_RESET);
                profile->badges[BADGE_BROKEN_BONES] = 0; // Kemikler iyileşti, rozet sıfırlandı.
            }

            Sleep(1500);
            printf(COLOR_DARK " Tam o huzur anında, zihninin derinliklerinden gelen o tanıdık, kadim ses yankılanıyor:\n\n" COLOR_RESET);
            printf(COLOR_MAG " \"Eğer kaçacaksan... Orası burası...\"\n\n" COLOR_RESET);

            printf("  [" COLOR_CYAN "1" COLOR_RESET "] Bu delilikten tamamen kaçar ve kaderi reddederim. (Oyundan Çık)\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] Tanrılara güvenilmez! Arabadan aşağı, okyanusa atlarım.\n");
            printf("  [" COLOR_CYAN "3" COLOR_RESET "] Helios'u takip etmeye devam eder, bu ilahi yolculuğu kabullenirim.\n");
            printf("  [" COLOR_CYAN "4" COLOR_RESET "] Zihnimdeki sese ardı ardına hızlıca sorular sorar, onunla konuşmaya çalışırım.\n\n");
            printf(COLOR_CYAN "  Kararın nedir? (1-4): " COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n [SCENE VI] Mercy of the Sun\n\n" COLOR_RESET);
            printf(COLOR_WHITE " The war is left behind. You ascend rapidly through the clouds.\n");
            printf(" The deafening chaos that surrounded you gives way to the howl of the wind.\n\n");
            printf(" Helios slowly turns to you. You feel the sunlight radiating from his chariot scorch your skin,\n");
            printf(" enveloping your body in a harsh, divine heat.\n\n" COLOR_RESET);

            if (profile->badges[BADGE_BROKEN_BONES] > 0) {
                printf(COLOR_CYAN " This pure power of the sun penetrates your broken bones. As the pain yields to a warm numbness,\n");
                printf(" you feel your shattered body miraculously healing in seconds!\n\n" COLOR_RESET);
                profile->badges[BADGE_BROKEN_BONES] = 0;
            }

            Sleep(1500);
            printf(COLOR_DARK " At that moment of peace, the familiar, ancient voice from the depths of your mind echoes:\n\n" COLOR_RESET);
            printf(COLOR_MAG " \"If you are going to escape... This is the place...\"\n\n" COLOR_RESET);

            printf("  [" COLOR_CYAN "1" COLOR_RESET "] I completely escape this madness and reject destiny. (Quit Game)\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] Gods cannot be trusted! I jump off the chariot into the ocean.\n");
            printf("  [" COLOR_CYAN "3" COLOR_RESET "] I continue following Helios, accepting this divine journey.\n");
            printf("  [" COLOR_CYAN "4" COLOR_RESET "] I rapidly ask the voice questions, trying to communicate with it.\n\n");
            printf(COLOR_CYAN "  What is your decision? (1-4): " COLOR_RESET);
        }

        int helios_choice = 0;
        while (1) {
            if (_kbhit()) {
                char ch = _getch();
                if (ch >= '1' && ch <= '4') { helios_choice = ch - '0'; break; }
            }
            Sleep(20);
        }

        clear_screen();

        if (helios_choice == 1) {
            // ŞIK 1: KADERİ REDDETME VE OYUNU KAPATMA
            if (current_lang == 1) {
                printf(COLOR_RED "\n  Zihnindeki sese hak veriyorsun...\n");
                printf("  Sen bu kozmik savaşın bir parçası değilsin. Kaderini reddediyorsun.\n");
                printf("  Sistem bağlantısı koparılıyor...\n" COLOR_RESET);
            } else {
                printf(COLOR_RED "\n  You agree with the voice in your mind...\n");
                printf("  You are not a part of this cosmic war. You reject your destiny.\n");
                printf("  System connection severed...\n" COLOR_RESET);
            }
            Sleep(2500);
            exit(0); // Oyunu anında kapatır
        }
        else if (helios_choice == 2) {
            // ŞIK 2: ARABADAN ATLAYIP BOĞULMAYA GİTME
            if (current_lang == 1) {
                printf(COLOR_DARK "\n [SAHNE VI] Düşüş\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Tanrılara olan güvensizliğin ağır basıyor. Arabanın kenarından kendini boşluğa bırakıyorsun!\n");
                printf(" Kilometrelerce yüksekten karanlık okyanusa doğru çakılıyorsun.\n");
                printf(" Suya çarptığın an bütün kemiklerin paramparça oluyor. Ciğerlerine dolan soğuk sularla\n");
                printf(" yavaşça boğularak hiçliğe karışıyorsun...\n\n" COLOR_RESET);
            } else {
                printf(COLOR_DARK "\n [SCENE VI] The Plunge\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Your distrust of the gods prevails. You throw yourself off the edge of the chariot into the void!\n");
                printf(" You plummet from kilometers high into the dark ocean.\n");
                printf(" The moment you hit the water, your body shatters. As the freezing waters fill your lungs,\n");
                printf(" you slowly drown, fading into the abyss...\n\n" COLOR_RESET);
            }
            profile->faith -= 2;
            printf(COLOR_CYAN " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
            _getch();
        }
        else if (helios_choice == 3) {
            // ŞIK 3: KALIP DEVAM ETME
            if (current_lang == 1) {
                printf(COLOR_GOLD "\n [SAHNE VI] İlahi Yolculuk\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Zihnindeki sesi tamamen görmezden geliyorsun. Güneş arabası bulutları yararak\n");
                printf(" kozmik bir geçide doğru ilerliyor. Kaderine doğru uçuyorsun...\n\n" COLOR_RESET);
            } else {
                printf(COLOR_GOLD "\n [SCENE VI] Divine Journey\n\n" COLOR_RESET);
                printf(COLOR_WHITE " You completely ignore the voice in your mind. The sun chariot tears through\n");
                printf(" the clouds heading towards a cosmic gateway. You fly towards your destiny...\n\n" COLOR_RESET);
            }
            profile->faith += 1;
            printf(COLOR_CYAN " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
            _getch();
        }
        else if (helios_choice == 4) {
            // ŞIK 4: SORGULAMA AMA CEVAP ALAMAMA
            if (current_lang == 1) {
                printf(COLOR_GOLD "\n [SAHNE VI] Sessizliğin Cevabı\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Zihnindeki sese ardı ardına ve hızlıca sorular soruyorsun!\n");
                printf(COLOR_MAG " \"Sen kimsin? Ne istiyorsun? Neden buradayım?!\"\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Ancak hiçbir cevap gelmiyor. Ses zihninden tamamen siliniyor.\n");
                printf(" Yalnızca rüzgarın uğultusu ve Helios'un arabasının alev sesleri kalıyor.\n");
                printf(" Arabanın içinde kaderine doğru uçmaya devam ediyorsun...\n\n" COLOR_RESET);
            } else {
                printf(COLOR_GOLD "\n [SCENE VI] Answer of Silence\n\n" COLOR_RESET);
                printf(COLOR_WHITE " You rapidly fire question after question at the voice in your mind!\n");
                printf(COLOR_MAG " \"Who are you? What do you want? Why am I here?!\"\n\n" COLOR_RESET);
                printf(COLOR_WHITE " But there is no answer. The voice fades completely from your mind.\n");
                printf(" Only the howl of the wind and the roar of Helios's chariot flames remain.\n");
                printf(" You continue flying towards your destiny inside the chariot...\n\n" COLOR_RESET);
            }
            profile->intel += 1;
            printf(COLOR_CYAN " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
            _getch();
        }
    }
    // ========================================================================
    // SAHNE VI: BOĞULANLARIN KARANLIĞI (Helios'un Kurtarmadıkları)
    // ========================================================================
    else {
        clear_screen();
        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE VI] Derinlerdeki Lakayt Ses\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Okyanusun karanlık ve dondurucu derinliklerine doğru batıyorsun.\n");
            printf(" Ciğerlerin tuzlu suyla doluyor, bilincin yavaşça karanlığa teslim olmak üzere.\n\n");
            printf(" Tam o anda, zihninin derinliklerinden gelen o tanıdık, kadim varlık,\n");
            printf(" etrafınızdaki bu dehşet verici ölüme tamamen zıt, inanılmaz lakayt bir tonda konuşuyor:\n\n" COLOR_RESET);
            printf(COLOR_MAG " \"Deniz hiç benim tarzım değil ya... Buradan çıkabilir miyiz?\"\n\n" COLOR_RESET);

            printf("  [" COLOR_CYAN "1" COLOR_RESET "] Bu lakayt tavra sinirlenip zihnimden 'Buradan çıkamayız...' diye karşılık veririm.\n");

            // Kemikleri kırıksa 2. şık KİLİTLİ görünür
            if (profile->badges[BADGE_BROKEN_BONES] > 0) {
                printf(COLOR_DARK "  [2] [ KİLİTLİ - Kemiklerin Paramparça ] Sesi umursamayıp debelenerek kurtulmaya çalışmak.\n" COLOR_RESET);
            } else {
                printf("  [" COLOR_CYAN "2" COLOR_RESET "] Sese odaklanmaktansa kurtulmaya odaklanır ve var gücümle debelenirim.\n");
            }

            printf("  [" COLOR_CYAN "3" COLOR_RESET "] Sessizce ölüme teslim olurum. (Oyundan Çık)\n");
            printf("  [" COLOR_CYAN "4" COLOR_RESET "] Bedenimi bırakıp sadece beynimin içindeki o sese odaklanırım.\n\n");
            printf(COLOR_CYAN "  Kararın nedir? (1-4): " COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n [SCENE VI] The Casual Voice in the Deep\n\n" COLOR_RESET);
            printf(COLOR_WHITE " You are sinking into the dark, freezing depths of the ocean.\n");
            printf(" Your lungs fill with saltwater, your consciousness slowly surrendering to the dark.\n\n");
            printf(" Just then, that familiar, ancient entity from the depths of your mind speaks\n");
            printf(" in an incredibly casual tone, completely contrasting the horrifying death around you:\n\n" COLOR_RESET);
            printf(COLOR_MAG " \"The sea is really not my style... Can we get out of here?\"\n\n" COLOR_RESET);

            printf("  [" COLOR_CYAN "1" COLOR_RESET "] Get tense at this casual attitude and reply in my mind: 'We can't get out...'\n");

            if (profile->badges[BADGE_BROKEN_BONES] > 0) {
                printf(COLOR_DARK "  [2] [ LOCKED - Shattered Bones ] Ignore the voice and struggle with all my might to escape.\n" COLOR_RESET);
            } else {
                printf("  [" COLOR_CYAN "2" COLOR_RESET "] Ignore the voice, focus on escaping, and struggle with all my might.\n");
            }

            printf("  [" COLOR_CYAN "3" COLOR_RESET "] Silently surrender to death. (Quit Game)\n");
            printf("  [" COLOR_CYAN "4" COLOR_RESET "] Let go of my physical body and focus entirely on the voice in my brain.\n\n");
            printf(COLOR_CYAN "  What is your decision? (1-4): " COLOR_RESET);
        }

        int drown_choice = 0;
        while (1) {
            if (_kbhit()) {
                char ch = _getch();
                if (ch == '1') { drown_choice = 1; break; }
                // Sadece kemikleri kırık değilse 2'ye basabilir
                if (ch == '2' && profile->badges[BADGE_BROKEN_BONES] == 0) { drown_choice = 2; break; }
                if (ch == '3') { drown_choice = 3; break; }
                if (ch == '4') { drown_choice = 4; break; }
            }
            Sleep(20);
        }

        clear_screen();

        if (drown_choice == 1) { // 1. ŞIK: Lakayt cevaba gerilme (+Zeka, -İnanç)
            profile->intel += 1; profile->faith -= 1;
            if (current_lang == 1) {
                printf(COLOR_GOLD "\n [SAHNE VI] Umutsuzluk\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Zihnindeki sesin seni anlamaması seni daha da geriyor.\n");
                printf(COLOR_CYAN " \"Çıkamayız... Ölüyoruz,\" " COLOR_WHITE "diye fısıldıyorsun içinden.\n\n");
                printf(" Ses derin bir iç çekiyor. Ardından bilincin suların soğukluğuyla tamamen kapanıyor...\n\n" COLOR_RESET);
            } else {
                printf(COLOR_GOLD "\n [SCENE VI] Despair\n\n" COLOR_RESET);
                printf(COLOR_WHITE " The fact that the voice doesn't understand your peril makes you even more tense.\n");
                printf(COLOR_CYAN " \"We can't get out... We're dying,\" " COLOR_WHITE "you whisper internally.\n\n");
                printf(" The voice sighs deeply. Then, your consciousness completely shuts down in the freezing waters...\n\n" COLOR_RESET);
            }
            if (current_lang == 1) printf(COLOR_CYAN " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
            else printf(COLOR_CYAN " [Press ANY KEY to continue]\n" COLOR_RESET);
            _getch();
        }
        else if (drown_choice == 2) { // 2. ŞIK: Debelenme (+Güç, -Zeka) (Sadece Kırıksızlar)
            profile->might += 1; profile->intel -= 1;
            if (current_lang == 1) {
                printf(COLOR_GOLD "\n [SAHNE VI] Beyhude Çırpınış\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Sesi tamamen göz ardı edip kollarını ve bacaklarını var gücünle çırpıyorsun.\n");
                printf(" Yukarı doğru yüzmeye çalışıyorsun ancak akıntı çok güçlü. Çırpındıkça enerjin tükeniyor\n");
                printf(" ve gittikçe karanlığın daha da aşağısına batıyorsun. Direnişin son buluyor...\n\n" COLOR_RESET);
            } else {
                printf(COLOR_GOLD "\n [SCENE VI] Futile Struggle\n\n" COLOR_RESET);
                printf(COLOR_WHITE " You ignore the voice completely and thrash your arms and legs with all your might.\n");
                printf(" You try to swim up, but the current is too strong. Your struggling only drains your energy,\n");
                printf(" pulling you deeper into the darkness. Your resistance ends...\n\n" COLOR_RESET);
            }
            if (current_lang == 1) printf(COLOR_CYAN " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
            else printf(COLOR_CYAN " [Press ANY KEY to continue]\n" COLOR_RESET);
            _getch();
        }
        else if (drown_choice == 3) { // 3. ŞIK: Sessizce Öl ve Çık
            if (current_lang == 1) {
                printf(COLOR_RED "\n  Hiçbir şey yapmıyorsun.\n");
                printf("  Suların karanlığı seni yutarken, acı son buluyor. Ölümü kucaklıyorsun.\n");
                printf("  Sistem bağlantısı koparılıyor...\n" COLOR_RESET);
            } else {
                printf(COLOR_RED "\n  You do nothing.\n");
                printf("  As the dark waters swallow you, the pain ends. You embrace death.\n");
                printf("  System connection severed...\n" COLOR_RESET);
            }
            Sleep(2500);
            exit(0);
        }
        else if (drown_choice == 4) { // 4. ŞIK: Sese Odaklanma (+İnanç, -Güç)
            profile->faith += 1; profile->might -= 1;

            // EĞER THANATOS'UN LANETİ VARSA ÖZEL ALT-DALLANMA BAŞLAR
            if (profile->badges[BADGE_CURSE_OF_THANATOS] > 0) {
                if (current_lang == 1) {
                    printf(COLOR_GOLD "\n [SAHNE VI] Zihnin Kokusu\n\n" COLOR_RESET);
                    printf(COLOR_WHITE " Bedenin ölürken sen tüm algını o sese çeviriyorsun. Ses bir süre duraksıyor,\n");
                    printf(" zihninin içinde dolaşıyormuş gibi hissettiriyor. Sonra tiksinmiş bir tonla konuşuyor:\n\n");
                    printf(COLOR_MAG " \"Ne yedin sen be? Beyninin içi çok kötü kokuyor... Ölüm gibi kokuyor.\n");
                    printf(" Hazır buradayken şu lanetini kaldırmamı ister misin?\"\n\n" COLOR_RESET);

                    printf("  [" COLOR_CYAN "1" COLOR_RESET "] \"Evet, lütfen bu laneti benden al!\" (Laneti Temizle)\n");
                    printf("  [" COLOR_CYAN "2" COLOR_RESET "] \"Hayır, bu lanet benim günahım, onunla öleceğim.\" (Laneti Tut)\n\n");
                    printf(COLOR_CYAN "  Cevabın nedir? (1-2): " COLOR_RESET);
                } else {
                    printf(COLOR_GOLD "\n [SCENE VI] Scent of the Mind\n\n" COLOR_RESET);
                    printf(COLOR_WHITE " As your body dies, you turn all your perception to the voice. It pauses for a moment,\n");
                    printf(" feeling as if it is wandering inside your mind. Then it speaks in a disgusted tone:\n\n");
                    printf(COLOR_MAG " \"What did you eat? The inside of your brain smells awful... It smells like death.\n");
                    printf(" While I'm here, do you want me to lift this curse?\"\n\n" COLOR_RESET);

                    printf("  [" COLOR_CYAN "1" COLOR_RESET "] \"Yes, please take this curse from me!\" (Cleanse Curse)\n");
                    printf("  [" COLOR_CYAN "2" COLOR_RESET "] \"No, this curse is my sin, I will die with it.\" (Keep Curse)\n\n");
                    printf(COLOR_CYAN "  What is your answer? (1-2): " COLOR_RESET);
                }

                int curse_choice = 0;
                while (1) {
                    if (_kbhit()) {
                        char ch = _getch();
                        if (ch == '1') { curse_choice = 1; break; }
                        if (ch == '2') { curse_choice = 2; break; }
                    }
                    Sleep(20);
                }

                clear_screen();
                if (curse_choice == 1) { // Laneti Kaldır (+Onur, -Zeka)
                    profile->honor += 1; profile->intel -= 1;
                    profile->badges[BADGE_CURSE_OF_THANATOS] = 0; // Lanet kaldırıldı!
                    if (current_lang == 1) {
                        printf(COLOR_GOLD "\n [SAHNE VI] Temizlenen Zihin\n\n" COLOR_RESET);
                        printf(COLOR_WHITE " Kafanın içindeki o paranoyak uğultu aniden sökülüp alınıyor.\n");
                        printf(COLOR_MAG " \"İşte, şimdi daha iyi...\" " COLOR_WHITE "diyor ses rahatlamış bir şekilde.\n");
                        printf(" Zihnin tertemiz, dingin bir halde suların altında bilincini kaybediyorsun...\n\n" COLOR_RESET);
                    } else {
                        printf(COLOR_GOLD "\n [SCENE VI] Cleansed Mind\n\n" COLOR_RESET);
                        printf(COLOR_WHITE " The paranoid buzzing inside your head is suddenly ripped away.\n");
                        printf(COLOR_MAG " \"There, that's much better...\" " COLOR_WHITE "the voice says, relieved.\n");
                        printf(" With a pure, tranquil mind, you lose consciousness beneath the waters...\n\n" COLOR_RESET);
                    }
                } else { // Laneti Tut (+Yetenek, -Onur)
                    profile->skill += 1; profile->honor -= 1;
                    if (current_lang == 1) {
                        printf(COLOR_GOLD "\n [SAHNE VI] Deliliğe Sarılış\n\n" COLOR_RESET);
                        printf(COLOR_MAG " \"Sen bilirsin, iğrenç zevklerin var,\" " COLOR_WHITE "diyor ses alaycı bir şekilde.\n");
                        printf(" Thanatos'un laneti beynini kemirmeye devam ederken, o deliliğin içinde\n");
                        printf(" suların dibine gömülüp bilincini kaybediyorsun...\n\n" COLOR_RESET);
                    } else {
                        printf(COLOR_GOLD "\n [SCENE VI] Embracing Madness\n\n" COLOR_RESET);
                        printf(COLOR_MAG " \"Suit yourself, you have disgusting tastes,\" " COLOR_WHITE "the voice says mockingly.\n");
                        printf(" As Thanatos's curse continues to gnaw at your brain, you sink to the bottom\n");
                        printf(" of the waters and lose consciousness in the midst of that madness...\n\n" COLOR_RESET);
                    }
                }
            }
            // LANETİ OLMAYANLAR İÇİN STANDART 4. ŞIK
            else {
                if (current_lang == 1) {
                    printf(COLOR_GOLD "\n [SAHNE VI] Karanlıkla Bütünleşme\n\n" COLOR_RESET);
                    printf(COLOR_WHITE " Bedenin acı içinde ölürken sen tüm algını o sese çeviriyorsun.\n");
                    printf(" Sesin frekansıyla bütünleştiğinde korkun yok oluyor. Okyanusun karanlığı seni\n");
                    printf(" sıcak bir battaniye gibi sararken bilincin sakince kapanıyor...\n\n" COLOR_RESET);
                } else {
                    printf(COLOR_GOLD "\n [SCENE VI] Merging with the Dark\n\n" COLOR_RESET);
                    printf(COLOR_WHITE " As your body dies in pain, you turn all your perception to that voice.\n");
                    printf(" When you resonate with its frequency, your fear vanishes. The darkness of the ocean\n");
                    printf(" wraps you like a warm blanket as your consciousness calmly fades...\n\n" COLOR_RESET);
                }
            }
            if (current_lang == 1) printf(COLOR_CYAN " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
            else printf(COLOR_CYAN " [Press ANY KEY to continue]\n" COLOR_RESET);
            _getch();
        }
    }
    // ========================================================================
    // SAHNE VI: GÜNEŞ ARABASI (Sadece Helios Tarafından Kurtarılanlar)
    // ========================================================================
    int helios_choice = 0;
    int drown_choice = 0;

    if (profile->badges[BADGE_BLESSING_HELIOS] == 1) {
        clear_screen();
        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE VI] Güneşin Merhameti\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Savaş geride kalıyor. Bulutların arasında hızla yükseliyorsunuz.\n");
            printf(" Etrafı saran o sağır edici kaos yerini rüzgarın uğultusuna bırakıyor.\n\n");
            printf(" Helios yavaşça sana dönüyor. Arabasından yayılan güneş ışığının tenini kavurduğunu,\n");
            printf(" ilahi ve sert bir ısıyla bedenini sardığını hissediyorsun.\n\n" COLOR_RESET);

            if (profile->badges[BADGE_BROKEN_BONES] > 0) {
                printf(COLOR_CYAN " Güneşin bu saf gücü, kırık kemiklerine nüfuz ediyor. Acı yerini sıcak bir uyuşmaya bırakırken,\n");
                printf(" paramparça olmuş bedeninin mucizevi bir şekilde saniyeler içinde iyileştiğini hissediyorsun!\n\n" COLOR_RESET);
                profile->badges[BADGE_BROKEN_BONES] = 0;
            }

            Sleep(1500);
            printf(COLOR_DARK " Tam o huzur anında, zihninin derinliklerinden gelen o tanıdık, kadim ses yankılanıyor:\n\n" COLOR_RESET);
            printf(COLOR_MAG " \"Eğer kaçacaksan... Orası burası...\"\n\n" COLOR_RESET);

            printf("  [" COLOR_CYAN "1" COLOR_RESET "] Bu delilikten tamamen kaçar ve kaderi reddederim. (Oyundan Çık)\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] Tanrılara güvenilmez! Arabadan aşağı, okyanusa atlarım.\n");
            printf("  [" COLOR_CYAN "3" COLOR_RESET "] Helios'u takip etmeye devam eder, bu ilahi yolculuğu kabullenirim.\n");
            printf("  [" COLOR_CYAN "4" COLOR_RESET "] Zihnimdeki sese ardı ardına hızlıca sorular sorar, onunla konuşmaya çalışırım.\n\n");
            printf(COLOR_CYAN "  Kararın nedir? (1-4): " COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n [SCENE VI] Mercy of the Sun\n\n" COLOR_RESET);
            printf(COLOR_WHITE " The war is left behind. You ascend rapidly through the clouds.\n");
            printf(" Helios slowly turns to you. You feel the sunlight radiating from his chariot scorch your skin...\n\n" COLOR_RESET);

            if (profile->badges[BADGE_BROKEN_BONES] > 0) {
                printf(COLOR_CYAN " This pure power penetrates your broken bones. Your shattered body heals in seconds!\n\n" COLOR_RESET);
                profile->badges[BADGE_BROKEN_BONES] = 0;
            }

            Sleep(1500);
            printf(COLOR_DARK " At that moment of peace, the familiar, ancient voice echoes:\n\n" COLOR_RESET);
            printf(COLOR_MAG " \"If you are going to escape... This is the place...\"\n\n" COLOR_RESET);

            printf("  [" COLOR_CYAN "1" COLOR_RESET "] I completely escape this madness and reject destiny. (Quit Game)\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] Gods cannot be trusted! I jump off the chariot into the ocean.\n");
            printf("  [" COLOR_CYAN "3" COLOR_RESET "] I continue following Helios, accepting this divine journey.\n");
            printf("  [" COLOR_CYAN "4" COLOR_RESET "] I rapidly ask the voice questions, trying to communicate with it.\n\n");
            printf(COLOR_CYAN "  What is your decision? (1-4): " COLOR_RESET);
        }

        while (1) {
            if (_kbhit()) {
                char ch = _getch();
                if (ch >= '1' && ch <= '4') { helios_choice = ch - '0'; break; }
            }
            Sleep(20);
        }

        clear_screen();
        if (helios_choice == 1) {
            if (current_lang == 1) printf(COLOR_RED "\n  Sistem bağlantısı koparılıyor...\n" COLOR_RESET);
            Sleep(2500); exit(0);
        }
        else if (helios_choice == 2) {
            // HELIOS'U REDDEDİP OKYANUSA ATLAYANLAR "BOĞULANLAR" GRUBUNA DAHİL OLUR!
            profile->badges[BADGE_BLESSING_HELIOS] = 0;
            profile->faith -= 2;
            if (current_lang == 1) {
                printf(COLOR_DARK "\n [SAHNE VI] Düşüş\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Kilometrelerce yüksekten karanlık okyanusa doğru çakılıyorsun.\n");
                printf(" Suya çarptığın an bilincin kapanıyor...\n\n" COLOR_RESET);
            } else {
                printf(COLOR_DARK "\n [SCENE VI] The Plunge\n\n" COLOR_RESET);
                printf(COLOR_WHITE " You plummet from kilometers high into the dark ocean.\n");
                printf(" The moment you hit the water, you lose consciousness...\n\n" COLOR_RESET);
            }
            printf(COLOR_CYAN " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
            _getch();
        }
        else {
            if (helios_choice == 3) profile->faith += 1;
            if (helios_choice == 4) profile->intel += 1;

            // SOHBET VE BAYILMA SEKANSI
            if (current_lang == 1) {
                printf(COLOR_GOLD "\n [SAHNE VI] Güneşin Sohbeti\n\n" COLOR_RESET);
                printf(COLOR_WHITE " (sohbet)\n\n" COLOR_RESET);
                printf(COLOR_DARK " Güneşin ve ilahi enerjinin o muazzam yoğunluğuna fani bedenin daha fazla\n");
                printf(" dayanamıyor. Gözlerin kamaşıyor, zihnin uyuşuyor ve arabanın içinde\n");
                printf(" bilincini tamamen kaybederek karanlığa gömülüyorsun...\n\n" COLOR_RESET);
            } else {
                printf(COLOR_GOLD "\n [SCENE VI] Chat with the Sun\n\n" COLOR_RESET);
                printf(COLOR_WHITE " (sohbet)\n\n" COLOR_RESET);
                printf(COLOR_DARK " Your mortal body can no longer withstand the immense density of the sun and\n");
                printf(" divine energy. Your eyes are blinded, your mind numbs, and you completely\n");
                printf(" lose consciousness inside the chariot, fading into darkness...\n\n" COLOR_RESET);
            }
            printf(COLOR_CYAN " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
            _getch();
        }
    }

    // ========================================================================
    // SAHNE VI: BOĞULANLARIN KARANLIĞI (Okyanusta Kalan Herkes)
    // ========================================================================
    if (profile->badges[BADGE_BLESSING_HELIOS] == 0) {
        clear_screen();
        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE VI] Derinlerdeki Lakayt Ses\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Okyanusun karanlık ve dondurucu derinliklerine doğru batıyorsun.\n");
            printf(" Ciğerlerin tuzlu suyla doluyor, bilincin yavaşça karanlığa teslim olmak üzere.\n\n");
            printf(" Tam o anda, zihninin derinliklerinden gelen o tanıdık, kadim varlık,\n");
            printf(" etrafınızdaki bu dehşet verici ölüme tamamen zıt, inanılmaz lakayt bir tonda konuşuyor:\n\n" COLOR_RESET);
            printf(COLOR_MAG " \"Deniz hiç benim tarzım değil ya... Buradan çıkabilir miyiz?\"\n\n" COLOR_RESET);

            printf("  [" COLOR_CYAN "1" COLOR_RESET "] Bu lakayt tavra sinirlenip zihnimden 'Çıkamayız...' diye karşılık veririm.\n");
            if (profile->badges[BADGE_BROKEN_BONES] > 0) printf(COLOR_DARK "  [2] [ KİLİTLİ - Kemiklerin Kırık ] Sesi umursamayıp debelenerek kurtulmaya çalışmak.\n" COLOR_RESET);
            else printf("  [" COLOR_CYAN "2" COLOR_RESET "] Sese odaklanmaktansa kurtulmaya odaklanır ve var gücümle debelenirim.\n");
            printf("  [" COLOR_CYAN "3" COLOR_RESET "] Sessizce ölüme teslim olurum. (Oyundan Çık)\n");
            printf("  [" COLOR_CYAN "4" COLOR_RESET "] Bedenimi bırakıp sadece beynimin içindeki o sese odaklanırım.\n\n");
            printf(COLOR_CYAN "  Kararın nedir? (1-4): " COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n [SCENE VI] The Casual Voice in the Deep\n\n" COLOR_RESET);
            printf(COLOR_MAG " \"The sea is really not my style... Can we get out of here?\"\n\n" COLOR_RESET);
            printf("  [" COLOR_CYAN "1" COLOR_RESET "] Get tense and reply: 'We can't get out...'\n");
            if (profile->badges[BADGE_BROKEN_BONES] > 0) printf(COLOR_DARK "  [2] [ LOCKED - Broken Bones ] Struggle with all my might.\n" COLOR_RESET);
            else printf("  [" COLOR_CYAN "2" COLOR_RESET "] Ignore the voice and struggle with all my might.\n");
            printf("  [" COLOR_CYAN "3" COLOR_RESET "] Silently surrender to death. (Quit Game)\n");
            printf("  [" COLOR_CYAN "4" COLOR_RESET "] Focus entirely on the voice in my brain.\n\n");
            printf(COLOR_CYAN "  What is your decision? (1-4): " COLOR_RESET);
        }

        while (1) {
            if (_kbhit()) {
                char ch = _getch();
                if (ch == '1') { drown_choice = 1; break; }
                if (ch == '2' && profile->badges[BADGE_BROKEN_BONES] == 0) { drown_choice = 2; break; }
                if (ch == '3') { drown_choice = 3; break; }
                if (ch == '4') { drown_choice = 4; break; }
            }
            Sleep(20);
        }

        if (drown_choice == 3) { exit(0); } // Sessizce ölenler sistemi kapatır.

        clear_screen();
        if (drown_choice == 1) { profile->intel += 1; profile->faith -= 1; }
        else if (drown_choice == 2) { profile->might += 1; profile->intel -= 1; }
        else if (drown_choice == 4) {
            profile->faith += 1; profile->might -= 1;

            // THANATOS LANETİ KONTROLÜ
            if (profile->badges[BADGE_CURSE_OF_THANATOS] > 0) {
                if (current_lang == 1) {
                    printf(COLOR_GOLD "\n [SAHNE VI] Zihnin Kokusu\n\n" COLOR_RESET);
                    printf(COLOR_MAG " \"Ne yedin sen be? Beyninin içi çok kötü kokuyor... Ölüm gibi kokuyor.\n");
                    printf(" Hazır buradayken şu lanetini kaldırmamı ister misin?\"\n\n" COLOR_RESET);
                    printf("  [" COLOR_CYAN "1" COLOR_RESET "] \"Evet, lütfen bu laneti benden al!\"\n");
                    printf("  [" COLOR_CYAN "2" COLOR_RESET "] \"Hayır, bu lanet benim günahım.\"\n\n");
                } else {
                    printf(COLOR_MAG " \"What did you eat? The inside of your brain smells awful... Like death.\n");
                    printf(" Want me to lift this curse?\"\n\n" COLOR_RESET);
                    printf("  [" COLOR_CYAN "1" COLOR_RESET "] \"Yes, take it from me!\"\n");
                    printf("  [" COLOR_CYAN "2" COLOR_RESET "] \"No, I will die with it.\"\n\n");
                }

                int curse_choice = 0;
                while (1) {
                    if (_kbhit()) {
                        char ch = _getch();
                        if (ch == '1' || ch == '2') { curse_choice = ch - '0'; break; }
                    }
                    Sleep(20);
                }
                if (curse_choice == 1) { profile->honor += 1; profile->intel -= 1; profile->badges[BADGE_CURSE_OF_THANATOS] = 0; }
                else { profile->skill += 1; profile->honor -= 1; }
            }
        }
        if (current_lang == 1) printf(COLOR_CYAN " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
        else printf(COLOR_CYAN " [Press ANY KEY to continue]\n" COLOR_RESET);
        _getch();
    }

    // ========================================================================
    // SAHNE VII: NİHAİ KOZMİK YÜZLEŞME VE 5. KARAR (Herkes İçin Geçerli)
    // ========================================================================
    clear_screen();

    if (profile->badges[BADGE_BLESSING_HELIOS] == 0) {
        // Boğulanlar için Silüet Görünümü
        if (current_lang == 1) {
            printf(COLOR_WHITE " Suların altında bilincini bir anda kaybediyorsun...\n\n");
            if (drown_choice == 4) {
                printf(COLOR_CYAN " Bedenin öldü ama zihnin uyanık. Karşında çok net ve devasa bir silüet beliriyor.\n");
                printf(" Anlık olarak tüm gökyüzünün ve okyanusun ağırlığı altında ezilirmiş gibi hissediyorsun.\n\n" COLOR_RESET);
            } else {
                printf(COLOR_DARK " Bedenin öldü ama zihnin uyanık. Karanlığın içinde belli belirsiz, büyük bir silüet görüyorsun.\n\n" COLOR_RESET);
            }
        } else {
            printf(COLOR_WHITE " Beneath the waters, you suddenly lose consciousness...\n\n");
            if (drown_choice == 4) printf(COLOR_CYAN " Your body is dead but your mind is awake. A clear, massive silhouette appears. You feel crushed by the sky.\n\n" COLOR_RESET);
            else printf(COLOR_DARK " You see a vague, massive silhouette in the darkness.\n\n" COLOR_RESET);
        }
    } else {
        // Helios ile bayılanlar için
        if (current_lang == 1) {
            printf(COLOR_WHITE " Gözlerini ışıktan kör olmuş bir hiçlikte açıyorsun...\n\n" COLOR_RESET);
            printf(COLOR_CYAN " Bedenin arabada baygın yatıyor ama zihnin uyanık. Karşında çok net ve devasa bir silüet beliriyor.\n");
            printf(" Anlık olarak tüm gökyüzünün ağırlığı altında ezilirmiş gibi hissediyorsun.\n\n" COLOR_RESET);
        } else {
            printf(COLOR_WHITE " You open your eyes in a void blinded by light...\n\n" COLOR_RESET);
            printf(COLOR_CYAN " Your body lies unconscious in the chariot, but your mind is awake. A massive silhouette appears.\n");
            printf(" You momentarily feel as if you are being crushed under the weight of the entire sky.\n\n" COLOR_RESET);
        }
    }

    Sleep(2000);

    // 5. SINAV: KESİN STAT DAĞILIMLARI (+1 ve -1 KURALI)
    if (current_lang == 1) {
        printf(COLOR_MAG " Ses, kozmik bir yankıyla ve oldukça alaycı bir tonda fısıldıyor:\n\n");
        printf(" \"Ölmekten korkuyor musun fani? Diyelim ki ölmedin... Ölmesen ne yapacaksın ki?\"\n\n" COLOR_RESET);

        printf("  [" COLOR_CYAN "1" COLOR_RESET "] Tanrılar seviyesinde mutlak bir güç olacağım. (+Güç, -İnanç)\n");
        printf("  [" COLOR_CYAN "2" COLOR_RESET "] Olimpos'u yerle bir edecek bir kuvvete gelene kadar çalışacağım. (+Onur, -Zeka)\n");
        printf("  [" COLOR_CYAN "3" COLOR_RESET "] Önüme çıkan her şeyi acımasızca yok edeceğim. (+Güç, -Onur)\n");
        printf("  [" COLOR_CYAN "4" COLOR_RESET "] Büyük bir bilge olup, alimler seviyesine ulaşacağım. (+Zeka, -Güç)\n");
        printf("  [" COLOR_CYAN "5" COLOR_RESET "] Kadere boyun eğmeyip efsanevi bir kahraman olacağım. (+Yetenek, -İnanç)\n");
        printf("  [" COLOR_CYAN "6" COLOR_RESET "] Zayıpları koruyup dünyaya adaleti getireceğim. (+Onur, -Güç)\n");
        printf("  [" COLOR_CYAN "7" COLOR_RESET "] Olimpos'un aydınlık yolunu herkese göstereceğim. (+İnanç, -Yetenek)\n\n");
        printf(COLOR_CYAN "  Nihai cevabın nedir? (1-7): " COLOR_RESET);
    } else {
        printf(COLOR_MAG " The voice whispers with a cosmic echo in a mocking tone:\n\n");
        printf(" \"Are you afraid of dying, mortal? Say you didn't die... What would you even do?\"\n\n" COLOR_RESET);
        printf("  [" COLOR_CYAN "1" COLOR_RESET "] Become an absolute god-level power. (+Might, -Faith)\n");
        printf("  [" COLOR_CYAN "2" COLOR_RESET "] Work until I can tear down Olympus. (+Honor, -Intel)\n");
        printf("  [" COLOR_CYAN "3" COLOR_RESET "] Destroy everything in my path. (+Might, -Honor)\n");
        printf("  [" COLOR_CYAN "4" COLOR_RESET "] Become a great sage reaching the scholars' level. (+Intel, -Might)\n");
        printf("  [" COLOR_CYAN "5" COLOR_RESET "] Not bow to fate and become a legendary hero. (+Skill, -Faith)\n");
        printf("  [" COLOR_CYAN "6" COLOR_RESET "] Protect the weak and bring justice. (+Honor, -Might)\n");
        printf("  [" COLOR_CYAN "7" COLOR_RESET "] Show the path of Olympus to everyone. (+Faith, -Skill)\n\n");
        printf(COLOR_CYAN "  What is your final answer? (1-7): " COLOR_RESET);
    }

    while (1) {
        if (_kbhit()) {
            char ch = _getch();
            if (ch >= '1' && ch <= '7') {
                int final_ans = ch - '0';
                if (final_ans == 1) { profile->might += 1; profile->faith -= 1; }
                if (final_ans == 2) { profile->honor += 1; profile->intel -= 1; }
                if (final_ans == 3) { profile->might += 1; profile->honor -= 1; }
                if (final_ans == 4) { profile->intel += 1; profile->might -= 1; }
                if (final_ans == 5) { profile->skill += 1; profile->faith -= 1; }
                if (final_ans == 6) { profile->honor += 1; profile->might -= 1; }
                if (final_ans == 7) { profile->faith += 1; profile->skill -= 1; }
                break;
            }
        }
        Sleep(20);
    }

    // ========================================================================
    // GİZLİ KOZMİK MATRİS HESAPLAMASI (Ekrana Hiçbir Şey Yazdırmaz!)
    // ========================================================================
    double max_cosine = -2.0;
    int best_match_idx = 0;

    for (int i = 0; i < 33; i++) {
        if (profile->poseidon_veto == 1 && strcmp(database[i].god, "Poseidon") == 0) continue;

        double dot = (profile->intel * database[i].intel) + (profile->might * database[i].might) +
                     (profile->honor * database[i].honor) + (profile->skill * database[i].skill) + (profile->faith * database[i].faith);
        double mag_A = sqrt(pow(profile->intel, 2) + pow(profile->might, 2) + pow(profile->honor, 2) + pow(profile->skill, 2) + pow(profile->faith, 2));
        double mag_B = sqrt(pow(database[i].intel, 2) + pow(database[i].might, 2) + pow(database[i].honor, 2) + pow(database[i].skill, 2) + pow(database[i].faith, 2));

        double cos_sim = (mag_A > 0 && mag_B > 0) ? (dot / (mag_A * mag_B)) : 0.0;
        if (cos_sim > max_cosine) { max_cosine = cos_sim; best_match_idx = i; }
    }

    profile->affinity = (int)(max_cosine * 100.0);
    strcpy(profile->god_alignment, database[best_match_idx].god);
    strcpy(profile->archetype_alignment, database[best_match_idx].archetype);
    strcpy(profile->archetype_alignment_tr, database[best_match_idx].archetype_tr);
    strcpy(profile->faction_class, database[best_match_idx].faction);
    strcpy(profile->faction_class_tr, database[best_match_idx].faction_tr);

  // ========================================================================
    // SAHNE VIII: UYANIŞ (Matris hesaplandıktan sonraki olaylar)
    // ========================================================================
    clear_screen();

    // BOĞULANLARIN ORTAK SONU (Devasa El ve Poseidon Uyanışı)
    if (profile->badges[BADGE_BLESSING_HELIOS] == 0) {
        if (strcmp(profile->god_alignment, "Poseidon") == 0) {
            if (current_lang == 1) {
                printf(COLOR_CYAN "\n  Kafandaki ses kıkırdayarak fısıldar:\n");
                printf("  \"Nefes almaya çalışsana...\"\n\n" COLOR_RESET);
                Sleep(2000);
                printf(COLOR_WHITE "  İstemsizce derin bir nefes alıyorsun... ve ciğerlerine su yerine hava dolduğunu fark ediyorsun!\n");
                printf("  Okyanus seni boğmuyor. Sen okyanusa aitsin.\n");
                printf("  Var gücünle yukarı, ışığa doğru yüzmeye başlıyorsun...\n\n" COLOR_RESET);
            } else {
                printf(COLOR_CYAN "\n  The voice in your head chuckles and whispers:\n");
                printf("  \"Try taking a breath...\"\n\n" COLOR_RESET);
                Sleep(2000);
                printf(COLOR_WHITE "  You involuntarily take a deep breath... and realize your lungs fill with air, not water!\n");
                printf("  The ocean is not drowning you. You belong to the ocean.\n");
                printf("  You start swimming upwards with all your might, towards the light...\n\n" COLOR_RESET);
            }
            Sleep(2500);
        }

        if (current_lang == 1) {
            printf(COLOR_RED "  TAM O AN...\n\n");
            printf("  Karanlığın içinden devasa, tanrısal bir el müthiş bir hızla yükseliyor!\n");
            printf("  Seni parmaklarının arasına aldığı gibi acımasızca, kemiklerini çatırdatırcasına sıkıyor!\n\n" COLOR_RESET);
            printf(COLOR_DARK "  Nefesin kesiliyor...\n  Gözlerin kararıyor...\n  Hiçliğe karışıyorsun...\n\n" COLOR_RESET);
        } else {
            printf(COLOR_RED "  JUST THEN...\n\n");
            printf("  A colossal, divine hand rises from the darkness at terrifying speed!\n");
            printf("  It catches you in its fingers and crushes you mercilessly, making your bones crack!\n\n" COLOR_RESET);
            printf(COLOR_DARK "  You lose your breath...\n  Your vision fades to black...\n  You merge with the void...\n\n" COLOR_RESET);
        }
        Sleep(4000);

        // Doğrudan ders tanımlama ve Shrine (Tapınak) ekranına atlar
        scene_init_subjects(profile);
        scene_own_shrine(profile);
    }
    // HELIOS İLE UÇANLARIN UYANIŞI VE MÜDÜR ODASI
    else {
        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE VIII] Güneşin Şehri\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Gözlerini yavaşça aralıyorsun... Görme yetin yavaş yavaş geri geliyor.\n");
            printf(" Helios arabasında sana tepeden bakıyor.\n\n" COLOR_RESET);
            printf(COLOR_GOLD " \"Uyandın demek...\"\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Ona bomboş, anlamlandıramayan gözlerle bakıyorsun. Zihnin az önceki kozmik\n");
            printf(" yüzleşmenin ağırlığıyla darmadağın olmuş durumda. Helios bu boş bakışını affederek\n");
            printf(" ihtişamlı ve devasa bir tavırla kollarını açıyor:\n\n" COLOR_RESET);
            printf(COLOR_GOLD " \"Ben Helios! Göğün yanan gözü, Güneşin kudretli Titanı!\"\n" COLOR_RESET);
            printf(COLOR_WHITE " Sesinin tınısı bile gök gürültüsü gibi havayı titretiyor.\n" COLOR_RESET);
            printf(COLOR_GOLD " \"Ve sen de, benim bizzat müdürü olduğum bu kadim akademinin yeni öğrencisisin.\"\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Şaşkınlıkla etrafına bakınıp fısıldıyorsun: " COLOR_CYAN "\"Peki... Ben kimim?\"\n\n" COLOR_RESET);
            printf(COLOR_GOLD " \"Madem soruyorsun,\" " COLOR_WHITE "diyor Helios hafifçe gülümseyerek, " COLOR_GOLD "\"Sana ne olarak seslenmemi istersin?\"\n\n" COLOR_RESET);
            printf(COLOR_CYAN "  Adını Gir: " COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n [SCENE VIII] City of the Sun\n\n" COLOR_RESET);
            printf(COLOR_WHITE " You slowly open your eyes... Your vision gradually returns.\n");
            printf(" Helios is looking down at you in his chariot.\n\n" COLOR_RESET);
            printf(COLOR_GOLD " \"So you are awake...\"\n\n" COLOR_RESET);
            printf(COLOR_WHITE " You stare at him with blank, uncomprehending eyes. Your mind is shattered by the\n");
            printf(" weight of the cosmic confrontation. Forgiving your blank stare, Helios spreads\n");
            printf(" his arms with magnificent grandeur:\n\n" COLOR_RESET);
            printf(COLOR_GOLD " \"I am Helios! The burning eye of the sky, the mighty Titan of the Sun!\"\n" COLOR_RESET);
            printf(COLOR_WHITE " Even the timbre of his voice vibrates the air like thunder.\n" COLOR_RESET);
            printf(COLOR_GOLD " \"And you are the new student of this ancient academy, where I serve as principal.\"\n\n" COLOR_RESET);
            printf(COLOR_WHITE " You look around in bewilderment and whisper: " COLOR_CYAN "\"Then... Who am I?\"\n\n" COLOR_RESET);
            printf(COLOR_GOLD " \"Since you ask,\" " COLOR_WHITE "says Helios with a slight smile, " COLOR_GOLD "\"What would you like me to call you?\"\n\n" COLOR_RESET);
            printf(COLOR_CYAN "  Enter your Name: " COLOR_RESET);
        }

        // Oyuncu adını kaydeder (Önceden kalma "Wastrel" ismini siler)
        set_cursor_visibility(true);
        read_string_safe(profile->player_name, 50);
        set_cursor_visibility(false);
        clear_screen();

        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE VIII] Kan Bağı\n\n" COLOR_RESET);
            printf(COLOR_GOLD " \"Hmm, %s...\"\n" COLOR_RESET, profile->player_name);
            printf(COLOR_WHITE " Helios gözlerini kısarak ruhunun derinliklerine bakıyor.\n" COLOR_RESET);
            printf(COLOR_GOLD " \"Sen herhangi bir insan değilsin. Sen bir melezsin.\n");
            printf(" Kanında %s kudreti akıyor.\"\n\n" COLOR_RESET, profile->god_alignment);
            printf(COLOR_GOLD " \"Ruhunun matrisini, gerçekte kim olduğunu görmek ister misin?\"\n\n" COLOR_RESET);
            printf("  [" COLOR_CYAN "1" COLOR_RESET "] Evet, bana kim olduğumu göster. (Karakter Kağıdı)\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] Hayır, şu an buna hazır değilim.\n\n");
            printf(COLOR_CYAN "  Kararın (1-2): " COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n [SCENE VIII] Bloodline\n\n" COLOR_RESET);
            printf(COLOR_GOLD " \"Hmm, %s...\"\n" COLOR_RESET, profile->player_name);
            printf(COLOR_WHITE " Helios narrows his eyes, looking into the depths of your soul.\n" COLOR_RESET);
            printf(COLOR_GOLD " \"You are not just any human. You are a demigod.\n");
            printf(" The might of %s flows in your blood.\"\n\n" COLOR_RESET, profile->god_alignment);
            printf(COLOR_GOLD " \"Do you wish to see the matrix of your soul, who you truly are?\"\n\n" COLOR_RESET);
            printf("  [" COLOR_CYAN "1" COLOR_RESET "] Yes, show me who I am. (Character Sheet)\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] No, I am not ready for that yet.\n\n");
            printf(COLOR_CYAN "  Decision (1-2): " COLOR_RESET);
        }

        char sheet_choice = '0';
        while (1) {
            if (_kbhit()) {
                sheet_choice = _getch();
                if (sheet_choice == '1' || sheet_choice == '2') break;
            }
            Sleep(20);
        }

        if (sheet_choice == '1') {
            display_character_sheet(profile);
            clear_screen();
        } else {
            if (current_lang == 1) printf(COLOR_GOLD "\n \"Sen bilirsin...\" " COLOR_WHITE "diyerek gülümsüyor Helios.\n\n" COLOR_RESET);
            else printf(COLOR_GOLD "\n \"Suit yourself...\" " COLOR_WHITE "Helios smiles.\n\n" COLOR_RESET);
            Sleep(1500);
            clear_screen();
        }

        // AKADEMİYE İNİŞ VE YANMA SEKANSI
        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE VIII] Düşüş ve Uyanış\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Güneş arabası bulutları yırtarak aşağı doğru süzülüyor.\n");
            printf(COLOR_GOLD " \"Aşağıya bak...\"\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Aşağı baktığında nefesin kesiliyor. Uçsuz bucaksız bir uçurumun kenarında, havada\n");
            printf(" süzülen devasa bir toprak parçasının üzerinde kristalimsi, görkemli bir kale var.\n");
            printf(" Altından gürül gürül bir nehir akıyor. Şehir kuşbakışı bakıldığında tıpkı devasa\n");
            printf(" bir güneş sembolü gibi inşa edilmiş. Köşelerinde farklı tanrılara ait kulübeler,\n");
            printf(" tam merkezinde ise ihtişamlı ana bina (Akademi) yer alıyor.\n\n");

            printf(COLOR_DARK " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
            _getch(); clear_screen();

            printf(COLOR_RED "\n  YANIYORSUN!\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Helios arabasını inanılmaz bir hızla doğrudan ana binanın çatısına indiriyor!\n");
            printf(" Çatıya adımını attığın an, güneşin koruyucu kalkanından çıktığın için o saf ve\n");
            printf(" vahşi enerji tenini yakmaya başlıyor. Alevler içinde kalıyorsun!\n\n");
            printf(COLOR_DARK " Acıdan çığlık atmana bile fırsat kalmadan...\n Gözünü kırptığın o milisaniyede...\n\n" COLOR_RESET);
            printf(COLOR_CYAN " Kendini serin, loş ve kitap kokan bir odada buluyorsun.\n");
            printf(" Müdür Odasındasın.\n\n" COLOR_RESET);
            printf(COLOR_DARK " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n [SCENE VIII] Fall and Awakening\n\n" COLOR_RESET);
            printf(COLOR_WHITE " The sun chariot tears through the clouds, gliding downwards.\n");
            printf(COLOR_GOLD " \"Look below...\"\n\n" COLOR_RESET);
            printf(COLOR_WHITE " You look down and gasp. On the edge of an endless cliff, resting on a colossal\n");
            printf(" floating landmass, lies a crystalline, magnificent fortress.\n");
            printf(" A roaring river flows beneath it. From a bird's-eye view, the city is built exactly\n");
            printf(" like a massive sun symbol. Shrines are placed at its edges, with a magnificent\n");
            printf(" main building (The Academy) right in the center.\n\n");

            printf(COLOR_DARK " [Press ANY KEY to continue]\n" COLOR_RESET);
            _getch(); clear_screen();

            printf(COLOR_RED "\n  YOU ARE BURNING!\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Helios dives his chariot directly onto the roof of the main building at incredible speed!\n");
            printf(" The moment you step onto the roof, stepping out of the sun's protective shield, that\n");
            printf(" pure and wild energy begins to scorch your skin. You burst into flames!\n\n");
            printf(COLOR_DARK " Before you even have a chance to scream in agony...\n In the millisecond you blink...\n\n" COLOR_RESET);
            printf(COLOR_CYAN " You find yourself in a cool, dimly lit room smelling of old books.\n");
            printf(" You are in the Principal's Office.\n\n" COLOR_RESET);
            printf(COLOR_DARK " [Press ANY KEY to continue]\n" COLOR_RESET);
        }
        _getch();

        // --------------------------------------------------------------------
        // MÜDÜR ODASI SOHBETİ VE İLK İLİŞKİ BARININ AÇILMASI
        // --------------------------------------------------------------------

        // SİSTEM: Helios ile ilk kez yüz yüze gelindi. İlişki barı kilidi açıldı!
        profile->npc_met[NPC_HELIOS] = true;

        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE VIII] Büyükler Ligi\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Helios devasa meşe masasının arkasında duruyor. Odanın içi bunaltıcı olmayan,\n");
            printf(" huzurlu ama kudretli bir güneş sıcaklığıyla dolu. Sana keskin gözlerle bakıyor:\n\n" COLOR_RESET);

            printf(COLOR_GOLD " \"Burada kalabilmek için çok çalışman gerek evlat...\"\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Sesi az önceki fırtınaya kıyasla daha sakin ama kelimeleri dağ gibi ağır.\n\n" COLOR_RESET);
            printf(COLOR_GOLD " \"Burası melezler için büyükler ligi. Bir tanrı gücünde olmanı bekliyoruz senin.\n");
            printf(" %s sınıfındasın. Belaya bulaşma.\"\n\n" COLOR_RESET, profile->faction_class_tr);

            printf(COLOR_WHITE " Bir süre sessizce gözlerinin içine bakarak seni süzüyor. Sonra hafifçe öne eğilip soruyor:\n\n" COLOR_RESET);
            printf(COLOR_GOLD " \"Şimdi söyle bana... İçindeki o kudreti evcilleştirip bu akademiye layık mı olacaksın,\n");
            printf(" yoksa o gücün altında ezilip yok mu olacaksın?\"\n\n" COLOR_RESET);

            printf("  [" COLOR_CYAN "1" COLOR_RESET "] \"O güce hükmedeceğim. Hatta bir gün hepinizi aşacağım.\" " COLOR_DARK "(Kibirli)" COLOR_RESET "\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] \"Kurallarınıza uyacağım ve elimden gelenin en iyisini yapacağım, efendim.\" " COLOR_DARK "(Saygılı)" COLOR_RESET "\n");
            printf("  [" COLOR_CYAN "3" COLOR_RESET "] \"Bunu zaman gösterecek. Şimdilik sadece hayatta kalmaya çalışıyorum.\" " COLOR_DARK "(Gerçekçi/Soğukkanlı)" COLOR_RESET "\n");
            printf("  [" COLOR_CYAN "4" COLOR_RESET "] \"Beni buraya zorla getirdiniz. Benden hiçbir şey beklemeyin.\" " COLOR_DARK "(İsyankar)" COLOR_RESET "\n\n");
            printf(COLOR_CYAN "  Cevabın nedir? (1-4): " COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n [SCENE VIII] The Big Leagues\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Helios stands behind a massive oak desk. The room is filled with a peaceful yet\n");
            printf(" mighty solar warmth. He looks at you with piercing eyes:\n\n" COLOR_RESET);

            printf(COLOR_GOLD " \"You need to work hard to stay here, kid...\"\n\n" COLOR_RESET);
            printf(COLOR_WHITE " His voice is calmer compared to the storm, but his words carry the weight of mountains.\n\n" COLOR_RESET);
            printf(COLOR_GOLD " \"This is the big leagues for demigods. We expect you to be at god-level power.\n");
            printf(" You are in %s class. Don't get into trouble.\"\n\n" COLOR_RESET, profile->faction_class);

            printf(COLOR_WHITE " He silently scrutinizes you for a moment. Then he leans forward slightly and asks:\n\n" COLOR_RESET);
            printf(COLOR_GOLD " \"Now tell me... Will you tame that might inside you and prove worthy of this academy,\n");
            printf(" or will you be crushed and destroyed under its weight?\"\n\n" COLOR_RESET);

            printf("  [" COLOR_CYAN "1" COLOR_RESET "] \"I will command that power. One day, I will even surpass all of you.\"\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] \"I will follow your rules and do my absolute best, sir.\"\n");
            printf("  [" COLOR_CYAN "3" COLOR_RESET "] \"Only time will tell. For now, I am just trying to survive.\"\n");
            printf("  [" COLOR_CYAN "4" COLOR_RESET "] \"You brought me here by force. Do not expect anything from me.\"\n\n");
            printf(COLOR_CYAN "  What is your answer? (1-4): " COLOR_RESET);
        }

        int dialog_choice = 0;
        while (1) {
            if (_kbhit()) {
                char ch = _getch();
                if (ch >= '1' && ch <= '4') { dialog_choice = ch - '0'; break; }
            }
            Sleep(20);
        }

        clear_screen();

        // --------------------------------------------------------------------
        // İLİŞKİ TEPKİLERİ VE GERİ BİLDİRİMLER (Sadece Diyalog, Stat Değişmez)
        // --------------------------------------------------------------------
        if (dialog_choice == 1) {
            // Helios kibri ve özgüveni sever (Sonuçta Güneş Tanrısı)
            profile->npc_relationship[NPC_HELIOS] += 5;
            if (current_lang == 1) {
                printf(COLOR_GOLD "\n [SAHNE VIII] Güneşin Tebessümü\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Helios duyduğu bu kibirli cevaba kahkaha atmamak için kendini zor tutuyor.\n");
                printf(COLOR_GOLD " \"Büyük sözler... Güneşin bu kadar yakınındayken yanmamaya dikkat et o zaman.\"\n\n" COLOR_RESET);
                printf(COLOR_GRN " [SİSTEM] Helios sana saygı duydu. (İlişki: %+d)\n\n" COLOR_RESET, profile->npc_relationship[NPC_HELIOS]);
            } else {
                printf(COLOR_GOLD "\n [SCENE VIII] Smile of the Sun\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Helios struggles not to laugh at this arrogant answer.\n");
                printf(COLOR_GOLD " \"Big words... Make sure you don't burn when you are this close to the sun.\"\n\n" COLOR_RESET);
                printf(COLOR_GRN " [SYSTEM] Helios respects your ambition. (Affinity: %+d)\n\n" COLOR_RESET, profile->npc_relationship[NPC_HELIOS]);
            }
        }
        else if (dialog_choice == 2) {
            // Sıkıcı bulur ama makul karşılar
            profile->npc_relationship[NPC_HELIOS] += 2;
            if (current_lang == 1) {
                printf(COLOR_GOLD "\n [SAHNE VIII] İtaatkar Fani\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Helios başını hafifçe sallıyor, cevabını biraz sıkıcı ama disiplinli buluyor.\n");
                printf(COLOR_GOLD " \"Göreceğiz... Sözlerin kadar icraatın da itaatkar olmalı.\"\n\n" COLOR_RESET);
                printf(COLOR_CYAN " [SİSTEM] Helios seni disiplinli buldu. (İlişki: %+d)\n\n" COLOR_RESET, profile->npc_relationship[NPC_HELIOS]);
            } else {
                printf(COLOR_GOLD "\n [SCENE VIII] Obedient Mortal\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Helios nods slightly, finding your answer a bit boring but disciplined.\n");
                printf(COLOR_GOLD " \"We shall see... Your actions must be as obedient as your words.\"\n\n" COLOR_RESET);
                printf(COLOR_CYAN " [SYSTEM] Helios finds you disciplined. (Affinity: %+d)\n\n" COLOR_RESET, profile->npc_relationship[NPC_HELIOS]);
            }
        }
        else if (dialog_choice == 3) {
            // Dürüstlüğe saygı duyar
            profile->npc_relationship[NPC_HELIOS] += 3;
            if (current_lang == 1) {
                printf(COLOR_GOLD "\n [SAHNE VIII] Soğukkanlılık\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Helios bu dürüst ve pragmatik yaklaşıma hafifçe gülümsüyor.\n");
                printf(COLOR_GOLD " \"Hayatta kalmak iyi bir başlangıçtır evlat. Ama burada kalıcı olmak için yetmez.\"\n\n" COLOR_RESET);
                printf(COLOR_CYAN " [SİSTEM] Helios dürüstlüğünü takdir etti. (İlişki: %+d)\n\n" COLOR_RESET, profile->npc_relationship[NPC_HELIOS]);
            } else {
                printf(COLOR_GOLD "\n [SCENE VIII] Cold-Blooded\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Helios smiles slightly at this honest and pragmatic approach.\n");
                printf(COLOR_GOLD " \"Surviving is a good start, kid. But it's not enough to be permanent here.\"\n\n" COLOR_RESET);
                printf(COLOR_CYAN " [SYSTEM] Helios appreciates your honesty. (Affinity: %+d)\n\n" COLOR_RESET, profile->npc_relationship[NPC_HELIOS]);
            }
        }
        else if (dialog_choice == 4) {
            // Güneş tanrısına saygısızlık ters teper
            profile->npc_relationship[NPC_HELIOS] -= 5;
            if (current_lang == 1) {
                printf(COLOR_GOLD "\n [SAHNE VIII] Güneşin Öfkesi\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Odanın içindeki sıcaklık bir anda boğucu ve kavurucu bir seviyeye ulaşıyor.\n");
                printf(COLOR_GOLD " \"Okyanusun dibinde balıklara yem olmanı izlemeliydim... Bir daha benimle\n");
                printf(" konuşurken haddini bil, yoksa o saygısız dilini kül ederim.\"\n\n" COLOR_RESET);
                printf(COLOR_RED " [SİSTEM] Helios'u kızdırdın! (İlişki: %d)\n\n" COLOR_RESET, profile->npc_relationship[NPC_HELIOS]);
            } else {
                printf(COLOR_GOLD "\n [SCENE VIII] Wrath of the Sun\n\n" COLOR_RESET);
                printf(COLOR_WHITE " The temperature in the room suddenly rises to a suffocating, scorching level.\n");
                printf(COLOR_GOLD " \"I should have watched you become fish food at the bottom of the ocean...\n");
                printf(" Know your place when you speak to me, or I will turn that disrespectful tongue to ash.\"\n\n" COLOR_RESET);
                printf(COLOR_RED " [SYSTEM] You angered Helios! (Affinity: %d)\n\n" COLOR_RESET, profile->npc_relationship[NPC_HELIOS]);
            }
        }

        if (current_lang == 1) printf(COLOR_DARK " [Oda kapısından çıkmak için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
        else printf(COLOR_DARK " [Press ANY KEY to walk out the office door]\n" COLOR_RESET);
        _getch();

        // --------------------------------------------------------------------
        // Doğrudan ders tanımlama ve Shrine (Tapınak) ekranına atlar
        // --------------------------------------------------------------------
        scene_init_subjects(profile);
        scene_own_shrine(profile);

        // Ders tanımlama ekranına ve Tapınağa gönderir
        scene_init_subjects(profile);
        scene_own_shrine(profile);
    }
} // <-- Fonksiyonun kapanış parantezi



void evaluate_cosmic_alignment(CharacterProfile* profile) {
    clear_screen();

    // Variables for Cosine Similarity calculations
    double max_cosine = -2.0; // Started lower to ensure proper overwrite
    int best_match_indices[33];
    int match_count = 0;

    // Calculate Cosine Similarity against all 33 archetypes
    for (int i = 0; i < 33; i++) {
        // [ GİZLİ VETO KONTROLÜ ]
        // Eğer oyuncu boğulma testini geçemediyse ve incelenen tanrı Poseidon ise,
        // matematiği hiç hesaplamadan döngüyü atla! Bu sayede bir sonraki en iyi tanrıya kalır.
        if (profile->poseidon_veto == 1 && strcmp(database[i].god, "Poseidon") == 0) {
            continue;
        }

        // Dot Product (A . B)
        double dot_product = (profile->intel * database[i].intel) +
                             (profile->might * database[i].might) +
                             (profile->honor * database[i].honor) +
                             (profile->skill * database[i].skill) +
                             (profile->faith * database[i].faith);

        // Magnitude of Player Vector ||A||
        double mag_A = sqrt(pow(profile->intel, 2) + pow(profile->might, 2) +
                            pow(profile->honor, 2) + pow(profile->skill, 2) + pow(profile->faith, 2));

        // Magnitude of Archetype Vector ||B||
        double mag_B = sqrt(pow(database[i].intel, 2) + pow(database[i].might, 2) +
                            pow(database[i].honor, 2) + pow(database[i].skill, 2) + pow(database[i].faith, 2));

        // Cosine Theta
        double cos_sim = 0.0;
        if (mag_A > 0 && mag_B > 0) {
            cos_sim = dot_product / (mag_A * mag_B);
        }

        // Floating point comparison with epsilon (0.0001) for accuracy
        if (cos_sim > max_cosine + 0.0001) {
            max_cosine = cos_sim;
            match_count = 0;
            best_match_indices[match_count] = i;
            match_count++;
        } else if (fabs(cos_sim - max_cosine) <= 0.0001) {
            best_match_indices[match_count] = i;
            match_count++;
        }
    }

    profile->affinity = (int)(max_cosine * 100.0);

    // UI Rendering
    if (current_lang == 1) {
        printf(COLOR_GOLD " =============================================================\n");
        printf("                     KOZMİK HÜKÜM                        \n");
        printf(" =============================================================\n\n" COLOR_RESET);
    } else {
        printf(COLOR_GOLD " =============================================================\n");
        printf("                    THE COSMIC VERDICT                        \n");
        printf(" =============================================================\n\n" COLOR_RESET);
    }

    if (match_count == 1) {
        int idx = best_match_indices[0];
        strcpy(profile->god_alignment, database[idx].god);
        strcpy(profile->archetype_alignment, database[idx].archetype);
        strcpy(profile->archetype_alignment_tr, database[idx].archetype_tr);
        strcpy(profile->faction_class, database[idx].faction);
        strcpy(profile->faction_class_tr, database[idx].faction_tr);

        if (current_lang == 1) {
            printf(COLOR_WHITE "  Kozmik Vektör Eşleşmesi: " COLOR_CYAN "%%%d\n" COLOR_RESET, profile->affinity);
            printf(COLOR_WHITE "  Kozmik Varlık devasa gözlerini kısıyor, alaycı bir şekilde gülüyor:\n\n" COLOR_RESET);
            printf(COLOR_CYAN "  \"Aaaa, %s'un bir çocuğu [ %s ]... Ne kadar şok edici derecede tahmin edilebilir.\"\n\n" COLOR_RESET,
                   profile->god_alignment, profile->archetype_alignment_tr);
        } else {
            printf(COLOR_WHITE "  Cosmic Vector Match: " COLOR_CYAN "%%%d\n" COLOR_RESET, profile->affinity);
            printf(COLOR_WHITE "  The Cosmic Entity narrows its massive eyes, laughing sarcastically:\n\n" COLOR_RESET);
            printf(COLOR_CYAN "  \"Aaaa, a child of %s [ %s ]... How shockingly predictable.\"\n\n" COLOR_RESET,
                   profile->god_alignment, profile->archetype_alignment);
        }
    }
    else {
        int primary_idx = best_match_indices[0];
        strcpy(profile->god_alignment, database[primary_idx].god);
        strcpy(profile->archetype_alignment, database[primary_idx].archetype);
        strcpy(profile->archetype_alignment_tr, database[primary_idx].archetype_tr);
        strcpy(profile->faction_class, database[primary_idx].faction);
        strcpy(profile->faction_class_tr, database[primary_idx].faction_tr);

        if (current_lang == 1) {
            printf(COLOR_WHITE "  Kozmik Vektör Eşleşmesi: " COLOR_CYAN "%%%d\n" COLOR_RESET, profile->affinity);
            printf(COLOR_WHITE "  Kozmik Varlık boyut matrisi üzerinden inceliyor:\n\n" COLOR_RESET);
            printf("  \"Bana daha çok " COLOR_CYAN "%s [ %s ]" COLOR_RESET " çocuğuna benziyorsun.\"\n\n",
                   profile->god_alignment, profile->archetype_alignment_tr);

            printf(COLOR_WHITE "  Yine de kozmik frekanslar çarpık. Gerçek soy alternatifini seç:\n\n" COLOR_RESET);
            printf("  [1] Varlığın görüşünü kabul et (%s - %s)\n", database[primary_idx].god, database[primary_idx].archetype_tr);

            for (int i = 1; i < match_count; i++) {
                int idx = best_match_indices[i];
                printf("  [%d] Aslında ben %s [ %s ] çocuğuyum\n", i + 1, database[idx].god, database[idx].archetype_tr);
            }
            printf("\n" COLOR_CYAN " Gerçek soy rezonansını seç (1-%d): " COLOR_RESET, match_count);

        } else {
            printf(COLOR_WHITE "  Cosmic Vector Match: " COLOR_CYAN "%%%d\n" COLOR_RESET, profile->affinity);
            printf(COLOR_WHITE "  The Cosmic Entity shifts through the dimension matrix:\n\n" COLOR_RESET);
            printf("  \"You look a lot like a child of " COLOR_CYAN "%s [ %s ]" COLOR_RESET " to me.\"\n\n",
                   profile->god_alignment, profile->archetype_alignment);

            printf(COLOR_WHITE "  Yet, the cosmic frequencies are warped. Choose your true bloodline alternative:\n\n" COLOR_RESET);
            printf("  [1] Accept the Entity's vision (%s - %s)\n", database[primary_idx].god, database[primary_idx].archetype);

            for (int i = 1; i < match_count; i++) {
                int idx = best_match_indices[i];
                printf("  [%d] Actually, I am the child of %s [ %s ]\n", i + 1, database[idx].god, database[idx].archetype);
            }
            printf("\n" COLOR_CYAN " Select your real bloodline resonance (1-%d): " COLOR_RESET, match_count);
        }

        char ch;
        while(1) {
            if(_kbhit()) {
                ch = _getch();
                int sel = ch - '0';
                if(sel >= 1 && sel <= match_count) {
                    int final_idx = best_match_indices[sel - 1];
                    strcpy(profile->god_alignment, database[final_idx].god);
                    strcpy(profile->archetype_alignment, database[final_idx].archetype);
                    strcpy(profile->archetype_alignment_tr, database[final_idx].archetype_tr);
                    strcpy(profile->faction_class, database[final_idx].faction);
                    strcpy(profile->faction_class_tr, database[final_idx].faction_tr);
                    break;
                }
            }
            Sleep(2);
        }
        if (current_lang == 1) printf(COLOR_GOLD "\n\n Matris son seçimine göre yeniden kalibre edildi!\n" COLOR_RESET);
        else printf(COLOR_GOLD "\n\n Matrix re-calibrated to your final choice!\n" COLOR_RESET);
    }

    if (current_lang == 1) printf("\n\n" COLOR_WHITE " [Kader Yoluna devam etmek için HERHANGİ BİR TUŞA bas] " COLOR_RESET);
    else printf("\n\n" COLOR_WHITE " [Press ANY KEY to continue to the Path of Destiny] " COLOR_RESET);
    _getch();
}

// ============================================================================
// DESTINY AWAKENING SCENE (REPLACES CLASS SELECTION)
// ============================================================================
void scene_awaken_destiny(CharacterProfile* profile) {
    clear_screen();

    if (current_lang == 1) {
        printf(COLOR_RED " =============================================================\n");
        printf("                      GERÇEK FORMUN UYANIYOR                  \n");
        printf(" =============================================================\n\n" COLOR_RESET);

        printf(COLOR_WHITE "  Kan Bağı Rezonansı : " COLOR_CYAN "%%%d\n" COLOR_RESET, profile->affinity);
        printf(COLOR_WHITE "  Kozmik Arketip     : " COLOR_GOLD "[ %s ]\n\n" COLOR_RESET, profile->archetype_alignment_tr);

        printf(COLOR_DARK " [Kaderini onaylamak için HERHANGİ BİR TUŞA bas] " COLOR_RESET);
    } else {
        printf(COLOR_RED " =============================================================\n");
        printf("                      YOUR TRUE FORM AWAKENS                  \n");
        printf(" =============================================================\n\n" COLOR_RESET);

        printf(COLOR_WHITE "  Bloodline Affinity : " COLOR_CYAN "%%%d\n" COLOR_RESET, profile->affinity);
        printf(COLOR_WHITE "  Cosmic Archetype   : " COLOR_GOLD "[ %s ]\n\n" COLOR_RESET, profile->archetype_alignment);

        printf(COLOR_DARK " [Press ANY KEY to finalize your destiny] " COLOR_RESET);
    }
    _getch();

    // --- DEĞİŞEN KISIM: AKIŞ SIRALAMASI ---

    // 1. Önce dersler (statlar) ve hedefler belirlenir
    scene_init_subjects(profile);

    // 2. Ardından tüm ayarları bitmiş "Karakter Kağıdı" gösterilir (Görkemli final)
    display_character_sheet(profile);

    // 3. Karakter kağıdı kapatılınca oyuncu doğrudan Kulübesinde uyanır
    scene_own_shrine(profile);
}







// ============================================================================
// YOLCULUĞA DEVAM ET (KAYIT DOSYASINI YÜKLEME)
// ============================================================================
void scene_continue_journey(CharacterProfile* profile) {
    clear_screen();

    if (current_lang == 1) printf(COLOR_CYAN "\n  SİSTEM: Kozmik hafıza taranıyor...\n" COLOR_RESET);
    else printf(COLOR_CYAN "\n  SYSTEM: Scanning cosmic memory...\n" COLOR_RESET);

    Sleep(800); // Gerçekçi bir yükleme hissiyatı için

    if (load_game(profile)) {
        if (current_lang == 1) {
            printf(COLOR_GRN "  Kayıt bulundu! Ebedi döngüye geri dönülüyor...\n" COLOR_RESET);
        } else {
            printf(COLOR_GRN "  Save found! Returning to the eternal cycle...\n" COLOR_RESET);
        }
        Sleep(1000);

        // Karakter yüklendikten sonra doğrudan kendi kulübesinde uyanır
        scene_own_shrine(profile);
    }
    else {
        if (current_lang == 1) {
            printf(COLOR_RED "\n  HATA: Hiçbir kader izi bulunamadı. Önce bir yolculuğa başlamalısın.\n" COLOR_RESET);
            printf(COLOR_DARK "  [Menüye dönmek için HERHANGİ BİR TUŞA bas]\n" COLOR_RESET);
        } else {
            printf(COLOR_RED "\n  ERROR: No trace of destiny found. You must start a journey first.\n" COLOR_RESET);
            printf(COLOR_DARK "  [Press ANY KEY to return to the menu]\n" COLOR_RESET);
        }
        _getch();
    }
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

    if (current_lang == 1) {
        printf(COLOR_GOLD " =============================================================\n");
        printf("                   SAHNE: MATRİS DOĞRULAMA                 \n");
        printf(" =============================================================\n" COLOR_RESET);
        printf("  --- KAHRAMAN HAFIZA KAYDI DURUMU ---\n");
        printf("  * Karakter Adı       : %s\n", profile->player_name);

        printf("\n  --- ATANAN TANRI & KADER MATRİSİ ---\n");
        printf("  * Tanrı Hizalaması   : " COLOR_GOLD "%s\n" COLOR_RESET, profile->god_alignment);
        printf("  * Kozmik Arketip     : " COLOR_CYAN "%s\n" COLOR_RESET, profile->archetype_alignment_tr);
        printf("  * Fraksiyon Sınıfı   : " COLOR_GOLD "%s\n" COLOR_RESET, profile->faction_class_tr);
        printf("  * Kan Rezonansı      : %d%%\n", profile->affinity);

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

        printf("\n  --- ASSIGNED GOD & DESTINY MATRICES ---\n");
        printf("  * God Alignment      : " COLOR_GOLD "%s\n" COLOR_RESET, profile->god_alignment);
        printf("  * Cosmic Archetype   : " COLOR_CYAN "%s\n" COLOR_RESET, profile->archetype_alignment);
        printf("  * Faction Class      : " COLOR_GOLD "%s\n" COLOR_RESET, profile->faction_class);
        printf("  * Blood Affinity     : %d%%\n", profile->affinity);

        printf("\n  --- ACTIVE 5-PARAMETRELİ ATTRIBUTES ---\n");
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
    int vis_len;

    printf(COLOR_DARK "\n   ╒════════════════════════════════════════════════════════════════════════╕\n");

    // Replace the top section of display_character_sheet with this:
    if (current_lang == 1) sprintf(buffer, "%s", profile->player_name);
    else sprintf(buffer, "%s", profile->player_name);

    vis_len = 23 + strlen(buffer);
    if (current_lang == 1) printf(COLOR_DARK "   │  " COLOR_CYAN "[ID]" COLOR_RESET " TANIMLAMA     : " COLOR_WHITE "%s" COLOR_DARK, buffer);
    else printf(COLOR_DARK "   │  " COLOR_CYAN "[ID]" COLOR_RESET " NAME   : " COLOR_WHITE "%s" COLOR_DARK, buffer);
    for(int i = 0; i < 72 - vis_len; i++) printf(" ");
    printf("│\n");

    if (current_lang == 1) sprintf(buffer, "%s - %s", profile->god_alignment, profile->archetype_alignment_tr);
    else sprintf(buffer, "%s - %s", profile->god_alignment, profile->archetype_alignment);

    vis_len = 23 + strlen(buffer);
    if (current_lang == 1) printf(COLOR_DARK "   │  " COLOR_CYAN "[KB]" COLOR_RESET " KAN BAĞI      : " COLOR_GOLD "%s" COLOR_DARK, buffer);
    else printf(COLOR_DARK "   │  " COLOR_CYAN "[BL]" COLOR_RESET " BLOODLINE     : " COLOR_GOLD "%s" COLOR_DARK, buffer);
    for(int i = 0; i < 72 - vis_len; i++) printf(" ");
    printf("│\n");

    if (current_lang == 1) sprintf(buffer, "%s Sınıfı", profile->faction_class_tr);
    else sprintf(buffer, "%s Faction", profile->faction_class);

    vis_len = 23 + strlen(buffer);
    if (current_lang == 1) printf(COLOR_DARK "   │  " COLOR_CYAN "[FR]" COLOR_RESET " FRAKSİYON     : " COLOR_WHITE "%s" COLOR_DARK, buffer);
    else printf(COLOR_DARK "   │  " COLOR_CYAN "[FC]" COLOR_RESET " FACTION       : " COLOR_WHITE "%s" COLOR_DARK, buffer);
    for(int i = 0; i < 72 - vis_len; i++) printf(" ");
    printf("│\n");


    printf("   │                                                                        │\n");
    if (current_lang == 1) printf("   │  ======================== [ " COLOR_CYAN "DERS İSTATİSTİKLERİ" COLOR_DARK " ] =======================  │\n");
    else printf("   │  ========================= [ " COLOR_CYAN "STUDY STATISTICS" COLOR_DARK " ] =========================  │\n");

    // Karakter kağıdında sadece aktif statları yazdır
    char stat_chars[] = "ABCDEFGHIJKLMNO";
    int active = profile->active_subject_count;
    int rows = (active + 2) / 3; // Gereken satır sayısını matematiksel olarak hesapla

    for (int i = 0; i < rows; i++) {
        printf(COLOR_DARK "   │  " COLOR_RESET);
        for (int j = 0; j < 3; j++) {
            int idx = i * 3 + j;
            if (idx < active) {
                // Aktif olan statları yazdır
                printf("[%c] STAT %c: " COLOR_CYAN "%04d" COLOR_RESET, stat_chars[idx], stat_chars[idx], profile->study_stats[idx]);
            } else {
                // Kutu tasarımını bozmamak için boşluk (padding) at
                printf("                ");
            }
            if (j < 2) printf(COLOR_DARK "  │  " COLOR_RESET);
        }
        printf(COLOR_DARK "            │\n");
    }

    printf("   │                                                                        │\n");
    if (current_lang == 1) printf("   │  ====================== [ " COLOR_GOLD "TOPLAM TECRÜBE (EXP)" COLOR_DARK " ] =======================  │\n");
    else printf("   │  ====================== [ " COLOR_GOLD "TOTAL EXPERIENCE (EXP)" COLOR_DARK " ] =====================  │\n");

    char exp_line[100];
    if (current_lang == 1) sprintf(exp_line, "KAZANILAN DENEYİM (EXP) : %d Puan", profile->total_exp);
    else sprintf(exp_line, "ACQUIRED EXPERIENCE (EXP) : %d Pts", profile->total_exp);

    int pad = 72 - 2 - strlen(exp_line);
    printf("   │  " COLOR_GOLD "%s" COLOR_DARK, exp_line);
    for(int p = 0; p < pad; p++) printf(" ");
    printf("│\n");

    printf("   ╘════════════════════════════════════════════════════════════════════════╛\n\n" COLOR_RESET);

    if (current_lang == 1) printf(COLOR_DARK "   [Sistem Mühürlendi. Ana Menüye dönmek için HERHANGİ BİR TUŞA bas] " COLOR_RESET);
    else printf(COLOR_DARK "   [System Sealed. Press ANY KEY to enter the Overworld Menu] " COLOR_RESET);
    _getch();
    clear_screen();
}

// ============================================================================
// LOKASYON İÇİ (GEÇİCİ TASLAK EKRANI)
// ============================================================================
void scene_inside_location(const char* loc_name_tr, const char* loc_name_en) {
    clear_screen();
    printf("\n\n\n");
    if (current_lang == 1) {
        printf(COLOR_CYAN "  >>> %s <<<\n\n" COLOR_RESET, loc_name_tr);
    } else {
        printf(COLOR_CYAN "  >>> %s <<<\n\n" COLOR_RESET, loc_name_en);
    }

    // Şimdilik sadece bir nokta
    printf(COLOR_WHITE "  .\n\n\n" COLOR_RESET);

    if (current_lang == 1) {
        printf(COLOR_DARK "  [Köy meydanına dönmek için HERHANGİ BİR TUŞA bas]\n" COLOR_RESET);
    } else {
        printf(COLOR_DARK "  [Press ANY KEY to return to the village]\n" COLOR_RESET);
    }
    _getch();
}

// ============================================================================
// KÜTÜPHANE SAYACI (POMODORO / ODAK ZAMANLAYICI)
// ============================================================================
void scene_library_timer(CharacterProfile* profile) {
    clear_screen();
    int minutes = 0;

    if (current_lang == 1) {
        printf(COLOR_CYAN "\n  >>> KÜTÜPHANE: BÜYÜK ARŞİV ODAK SAYACI <<<\n\n" COLOR_RESET);
        printf(COLOR_WHITE "  Kaç dakika odaklanacaksın? (1-120): " COLOR_RESET);
    } else {
        printf(COLOR_CYAN "\n  >>> THE LIBRARY: FOCUS TIMER <<<\n\n" COLOR_RESET);
        printf(COLOR_WHITE "  How many minutes will you focus? (1-120): " COLOR_RESET);
    }

    set_cursor_visibility(true);
    scanf("%d", &minutes);
    // Klavyedeki 'Enter' tuşu bellekte kalmasın diye temizliyoruz:
    int c; while ((c = getchar()) != '\n' && c != EOF);
    set_cursor_visibility(false);

    if (minutes < 1) minutes = 1;
    if (minutes > 120) minutes = 120;

    int total_seconds = minutes * 60;
    bool completed = true;

    while (total_seconds > 0) {
        clear_screen();
        printf("\n\n\n");
        printf(COLOR_GOLD "       ==========================================\n" COLOR_RESET);
        printf(COLOR_WHITE "             KÜTÜPHANE ODAK SÜRESİ: %02d:%02d\n" COLOR_RESET, total_seconds / 60, total_seconds % 60);
        printf(COLOR_GOLD "       ==========================================\n\n" COLOR_RESET);
        if (current_lang == 1) {
            printf(COLOR_DARK "       (Derse odaklan... Çıkmak ve bozmak için 'Q' tuşuna bas)\n" COLOR_RESET);
        } else {
            printf(COLOR_DARK "       (Focus on study... Press 'Q' to abort and exit)\n" COLOR_RESET);
        }

        Sleep(1000);
        total_seconds--;

        // Eğer Q tuşuna basılırsa sayacı iptal et
        if (_kbhit()) {
            char ch = _getch();
            if (ch == 'q' || ch == 'Q') {
                completed = false;
                break;
            }
        }
    }

    clear_screen();
    printf("\n\n\n");
    if (completed) {
        // Her başarılı odaklanmada STAT A'yı 1 artırıyoruz (Maksimum 15)
        profile->intel += 1;
        if (profile->intel > 15) profile->intel = 15;

        if (current_lang == 1) printf(COLOR_GRN "  Odaklanma başarıyla tamamlandı! [ STAT A ] +1 arttı.\n\n" COLOR_RESET);
        else printf(COLOR_GRN "  Focus completed successfully! [ STAT A ] increased by +1.\n\n" COLOR_RESET);
    } else {
        if (current_lang == 1) printf(COLOR_RED "  Odaklanma bozuldu! İrade zayıfladı, tecrübe kazanılamadı.\n\n" COLOR_RESET);
        else printf(COLOR_RED "  Focus broken! Willpower weakened, no experience gained.\n\n" COLOR_RESET);
    }

    if (current_lang == 1) printf(COLOR_DARK "  [Koridora dönmek için HERHANGİ BİR TUŞA bas]\n" COLOR_RESET);
    else printf(COLOR_DARK "  [Press ANY KEY to return to the hallway]\n" COLOR_RESET);
    _getch();
}

// ============================================================================
// KÜTÜPHANE KRONOMETRESİ (MOLA VE DEVAM ETTİRİLEBİLİR ODAK SAYACI)
// ============================================================================
void scene_library_stopwatch(CharacterProfile* profile) {
    clear_screen();
    int choice = -1;
    char letters[] = "ABCDEFGHIJKLMNO";

    if (current_lang == 1) {
        printf(COLOR_CYAN "\n  >>> KÜTÜPHANE: ODAK KRONOMETRESİ <<<\n\n" COLOR_RESET);
        printf(COLOR_WHITE "  Hangi ders/stat üzerine çalışacaksın?\n\n" COLOR_RESET);
    } else {
        printf(COLOR_CYAN "\n  >>> THE LIBRARY: FOCUS STOPWATCH <<<\n\n" COLOR_RESET);
        printf(COLOR_WHITE "  Which subject/stat will you study?\n\n" COLOR_RESET);
    }

    // Sadece aktif dersleri listele
    int active = profile->active_subject_count;
    for(int i = 0; i < active; i++) {
        if (i % 3 == 0) printf("  "); // Satır başı boşluğu
        printf("[" COLOR_CYAN "%c" COLOR_RESET "] %s\t", letters[i], profile->subject_names[i]);
        if (i % 3 == 2 || i == active - 1) printf("\n");
    }

    if (current_lang == 1) printf(COLOR_CYAN "\n  Seçiminiz (A-%c): " COLOR_RESET, letters[active-1]);
    else printf(COLOR_CYAN "\n  Your choice (A-%c): " COLOR_RESET, letters[active-1]);

    // Klavyeden sadece aktif harflerin seçimine izin ver
    while(choice == -1) {
        if (_kbhit()) {
            char ch = _getch();
            if (ch >= 'a' && ch < 'a' + active) choice = ch - 'a';
            else if (ch >= 'A' && ch < 'A' + active) choice = ch - 'A';
        }
        Sleep(20);
    }

    int seconds = 0;
    bool running = true;
    bool is_paused = false;

    while(running) {
        clear_screen();
        printf("\n\n\n");
        printf(COLOR_GOLD "       ==========================================\n" COLOR_RESET);
        if (current_lang == 1) {
            printf(COLOR_WHITE "         ÇALIŞILAN DERS : STAT %c\n" COLOR_RESET, letters[choice]);
            printf(COLOR_WHITE "         GEÇEN SÜRE     : %02d:%02d:%02d\n" COLOR_RESET, seconds / 3600, (seconds % 3600) / 60, seconds % 60);
            if (is_paused) printf(COLOR_RED "         [ DURAKLATILDI ]\n" COLOR_RESET);
            else printf(COLOR_GRN "         [ ÇALIŞILIYOR... ]\n" COLOR_RESET);
        } else {
            printf(COLOR_WHITE "         STUDYING       : STAT %c\n" COLOR_RESET, letters[choice]);
            printf(COLOR_WHITE "         ELAPSED TIME   : %02d:%02d:%02d\n" COLOR_RESET, seconds / 3600, (seconds % 3600) / 60, seconds % 60);
            if (is_paused) printf(COLOR_RED "         [ PAUSED ]\n" COLOR_RESET);
            else printf(COLOR_GRN "         [ FOCUSING... ]\n" COLOR_RESET);
        }
        printf(COLOR_GOLD "       ==========================================\n\n" COLOR_RESET);

        if (current_lang == 1) printf(COLOR_DARK "       [P] Başlat / Duraklat    [Q] Bitir ve Kaydet\n" COLOR_RESET);
        else printf(COLOR_DARK "       [P] Play / Pause         [Q] Stop & Save\n" COLOR_RESET);

        // 1 saniyeyi daha akıcı klavye tepkisi almak için 10 parçaya böldük
        for(int i=0; i<10; i++) {
            if (_kbhit()) {
                char ch = _getch();
                if (ch == 'p' || ch == 'P') { is_paused = !is_paused; break; }
                else if (ch == 'q' || ch == 'Q') { running = false; break; }
            }
            Sleep(100);
        }
        if (!is_paused && running) seconds++;
    }

    int earned_exp = seconds / 60; // 1 Dakika = 1 EXP

    clear_screen();
    printf("\n\n\n");
    if (current_lang == 1) {
        printf(COLOR_GOLD "  Çalışma tamamlandı!\n" COLOR_RESET);
        printf(COLOR_WHITE "  Geçen Toplam Süre: %d dakika, %d saniye\n\n", seconds / 60, seconds % 60);

        if (earned_exp > 0) {
            profile->study_stats[choice] += earned_exp;
            profile->total_exp += earned_exp;
            printf(COLOR_GRN "  Tebrikler! +%d EXP kazandın.\n" COLOR_RESET, earned_exp);
            printf(COLOR_CYAN "  [ STAT %c ] seviyen %d oldu.\n\n" COLOR_RESET, letters[choice], profile->study_stats[choice]);
        } else {
            printf(COLOR_RED "  1 dakikadan az çalıştığın için EXP kazanamadın.\n\n" COLOR_RESET);
        }
        printf(COLOR_DARK "  [Koridora dönmek için HERHANGİ BİR TUŞA bas]\n" COLOR_RESET);
    } else {
        printf(COLOR_GOLD "  Study session completed!\n" COLOR_RESET);
        printf(COLOR_WHITE "  Total Elapsed Time: %d minutes, %d seconds\n\n", seconds / 60, seconds % 60);

        if (earned_exp > 0) {
            profile->study_stats[choice] += earned_exp;
            profile->total_exp += earned_exp;
            printf(COLOR_GRN "  Congratulations! You gained +%d EXP.\n" COLOR_RESET, earned_exp);
            printf(COLOR_CYAN "  [ STAT %c ] level is now %d.\n\n" COLOR_RESET, letters[choice], profile->study_stats[choice]);
        } else {
            printf(COLOR_RED "  You studied for less than 1 minute, no EXP gained.\n\n" COLOR_RESET);
        }
        printf(COLOR_DARK "  [Press ANY KEY to return to the hallway]\n" COLOR_RESET);
    }
    _getch();
}

// Renk Paleti
#define M_RED "\033[1;31m"
#define M_GRN "\033[1;32m"
#define M_YEL "\033[1;33m"
#define M_BLU "\033[1;34m"
#define M_MAG "\033[1;35m"
#define M_CYN "\033[1;36m"
#define M_WHT "\033[1;37m"
#define M_DRK "\033[1;90m"
#define M_RST "\033[0m"

// ============================================================================
// THE MAIN SCHOOL INTERIOR: ANA OKUL İÇ PLANI (KÜTÜPHANE VE SINIFLAR)
// ============================================================================
void scene_main_school(CharacterProfile* profile) {
    bool in_school = true;

    while (in_school) {
        clear_screen();
        printf("\n");

        printf(M_DRK "  ==============================================================================================================\n" M_RST);

        // ASCII Nokta Map (Evrensel İkonlar)
        printf("\n");
        printf(M_CYN "                                              /\\ \n" M_RST);
        printf(M_CYN "                                             |  | \n" M_RST);
        printf(M_CYN "                                           [ L I B ] \n" M_RST);
        printf(M_DRK "                                               .  \n" M_RST);
        printf(M_DRK "                                               .  \n" M_RST);
        printf(M_DRK "                                 . . . . . . . . . . . . . . . \n" M_RST);
        printf(M_DRK "                                 .                           . \n" M_RST);
        printf(M_WHT "                             [ CLS I ]                   [ CLS II ] \n" M_RST);
        printf(M_DRK "                                 .                           . \n" M_RST);
        printf(M_DRK "                                 . . . . . . . . . . . . . . . \n" M_RST);
        printf(M_DRK "                                               .  \n" M_RST);
        printf(M_DRK "                                               .  \n" M_RST);
        printf(M_WHT "                                           [ CLS III ] \n" M_RST);
        printf("\n");

        printf(M_DRK "  ==============================================================================================================\n" M_RST);
        printf("\n");

        // Menü İsimlendirmeleri ve Kontroller
        if (current_lang == 1) {
            printf(M_YEL "  === ANA OKUL MERKEZİ ===\n\n" M_RST);
            printf("  [" M_CYN "1" M_RST "] Kütüphane (Okuma, Araştırma ve Uzun Vadeli Hedefler)\n");
            printf("  [" M_CYN "2" M_RST "] 1. Sınıf  \n");
            printf("  [" M_CYN "3" M_RST "] 2. Sınıf  (Sözel & Dil Çalışmaları)\n");
            printf("  [" M_CYN "4" M_RST "] 3. Sınıf  (Serbest Odak & Tasarım)\n\n");
            printf("  [" M_CYN "0" M_RST "] Tapınak Köyüne (Dışarı) Çık\n\n");
            printf(M_CYN "  Nereye gitmek istiyorsun? (0-4): " M_RST);
        } else {
            printf(M_YEL "  === MAIN SCHOOL CENTRAL ===\n\n" M_RST);
            printf("  [" M_CYN "1" M_RST "] The Library (Reading, Research & Long-term Goals)\n");
            printf("  [" M_CYN "2" M_RST "] Class I     \n");
            printf("  [" M_CYN "3" M_RST "] Class II    \n");
            printf("  [" M_CYN "4" M_RST "] Class III   \n\n");
            printf("  [" M_CYN "0" M_RST "] Exit to the Village Shrine\n\n");
            printf(M_CYN "  Where do you want to go? (0-4): " M_RST);
        }

        bool valid_input = false;
        while (!valid_input) {
            if (_kbhit()) {
                char ch = _getch();
                if (ch == '0') {
                    in_school = false;
                    valid_input = true;
                } else if (ch == '1') {
                    scene_library_stopwatch(profile);
                    valid_input = true;
                } else if (ch == '2') {
                    scene_inside_location("1. Sınıf Masası", "Class I Desk");
                    valid_input = true;
                } else if (ch == '3') {
                    scene_inside_location("2. Sınıf Masası", "Class II Desk");
                    valid_input = true;
                } else if (ch == '4') {
                    scene_inside_location("3. Sınıf Masası", "Class III Desk");
                    valid_input = true;
                }
            }
            Sleep(20);
        }
    }
}

// ============================================================================
// THE INNER SHRINE: ANA OKUL KALESİ (V9 SANATI + MİLİMETRİK BOŞLUK KALİBRASYONU)
// ============================================================================
void scene_inner_shrine(CharacterProfile* profile) {


    bool in_village = true;

    while (in_village) {
        clear_screen();
        printf("\n");

        // Dış Çerçeve Genişliği: İçerik tam 106 karakter. Toplam genişlik 112.
        printf(M_DRK "  ==============================================================================================================\n" M_RST);

        // --- SATIR 1: Zeus (30) | Ana Okul (46) | Poseidon (30) ---
        printf(M_DRK "  ||"
               M_YEL "             _/Z_             "
               M_WHT "                      |>>>                    "
               M_BLU "             _W_              "
               M_DRK "||\n" M_RST);

        printf(M_DRK "  ||"
               M_YEL "           //\\\\//\\\\           "
               M_WHT "                      |                       "
               M_BLU "            //|\\\\             "
               M_DRK "||\n" M_RST);

        printf(M_DRK "  ||"
               M_YEL "           || \\/ ||           "
               M_WHT "                _ _  _|_  _ _                 "
               M_BLU "           ||~|~||            "
               M_DRK "||\n" M_RST);

        printf(M_DRK "  ||"
               M_YEL "           ||____||           "
               M_WHT "               |;|_|;|_|;|_|;|                "
               M_BLU "           ||___||            "
               M_DRK "||\n" M_RST);

        printf(M_DRK "  ||"
               M_YEL "            [ZEUS]            "
               M_WHT "               |             |                "
               M_BLU "          [POSEIDON]          "
               M_DRK "||\n" M_RST);

        // --- SATIR 2: Hades (30) | Ana Okul (46) | Demeter (30) ---
        printf(M_DRK "  ||"
               M_MAG "             ____             "
               M_WHT "            ___|             |___             "
               M_GRN "             _/\\_             "
               M_DRK "||\n" M_RST);

        printf(M_DRK "  ||"
               M_MAG "            |    |            "
               M_WHT "           |;|_|;|_|;|_|;|_|;|_|;|            "
               M_GRN "            //**\\\\            "
               M_DRK "||\n" M_RST);

        printf(M_DRK "  ||"
               M_MAG "           ||_||_||           "
               M_WHT "           |                     |            "
               M_GRN "           ||\\&&/||           "
               M_DRK "||\n" M_RST);

        if (current_lang == 1) {
            printf(M_DRK "  ||"
                   M_MAG "           ||____||           "
                   M_WHT "           |    [ ANA OKUL ]     |            "
                   M_GRN "           ||____||           "
                   M_DRK "||\n" M_RST);
        } else {
            printf(M_DRK "  ||"
                   M_MAG "           ||____||           "
                   M_WHT "           |   [ MAIN SCHOOL ]   |            "
                   M_GRN "           ||____||           "
                   M_DRK "||\n" M_RST);
        }

        printf(M_DRK "  ||"
               M_MAG "           [HADES]            "
               M_WHT "           |                     |            "
               M_GRN "          [DEMETER]           "
               M_DRK "||\n" M_RST);

        // --- SATIR 3: Ares (30) | Ana Okul (46) | Athena (30) ---
        printf(M_DRK "  ||"
               M_RED "            | /\\ |            "
               M_WHT "           |=====================|            "
               M_WHT "             ____             "
               M_DRK "||\n" M_RST);

        printf(M_DRK "  ||"
               M_RED "            | XX |            "
               M_WHT "           |   | | |     | | |   |            "
               M_WHT "            /____\\            "
               M_DRK "||\n" M_RST);

        printf(M_DRK "  ||"
               M_RED "           ||====||           "
               M_WHT "           |   |_|_|     |_|_|   |            "
               M_WHT "           || || ||           "
               M_DRK "||\n" M_RST);

        printf(M_DRK "  ||"
               M_RED "           ||____||           "
               M_WHT "           |_____________________|            "
               M_WHT "           ||_||_||           "
               M_DRK "||\n" M_RST);

        printf(M_DRK "  ||"
               M_RED "            [ARES]            "
               M_WHT "                                              "
               M_WHT "           [ATHENA]           "
               M_DRK "||\n" M_RST);

        // --- SATIR 4: 5 Tanrı (Sırasıyla 21 + 21 + 22 + 21 + 21 = 106) ---
        printf(M_DRK "  ||"
               M_YEL "        \\ | /        "
               M_MAG "        _()_         "
               M_CYN "        >>/\\<<        "
               M_RED "         ||          "
               M_GRN "        _\\/_         "
               M_DRK "||\n" M_RST);

        printf(M_DRK "  ||"
               M_YEL "        - O -        "
               M_MAG "       //  \\\\        "
               M_CYN "        //  \\\\        "
               M_RED "        _\\/_         "
               M_GRN "       //~~\\\\        "
               M_DRK "||\n" M_RST);

        printf(M_DRK "  ||"
               M_YEL "       ||===||       "
               M_MAG "      || \\/ ||       "
               M_CYN "       || == ||       "
               M_RED "      ||_[]_||       "
               M_GRN "      || \\/ ||       "
               M_DRK "||\n" M_RST);

        printf(M_DRK "  ||"
               M_YEL "       ||___||       "
               M_MAG "      ||____||       "
               M_CYN "       ||____||       "
               M_RED "      ||____||       "
               M_GRN "      ||____||       "
               M_DRK "||\n" M_RST);

        printf(M_DRK "  ||"
               M_YEL "      [APOLLO]       "
               M_MAG "     [APHRODITE]     "
               M_CYN "       [HERMES]       "
               M_RED "    [HEPHAESTUS]     "
               M_GRN "     [DIONYSUS]      "
               M_DRK "||\n" M_RST);

// Alt Çerçeve
        printf(M_DRK "  ==============================================================================================================\n" M_RST);
        printf("\n");

        // --- Menü Arayüzünün Çizilmesi (Sadece Kendi Kulüben ve Okul Görünür) ---
        if (current_lang == 1) {
            printf(M_YEL "  === TAPINAK KÖYÜ (INNER SHRINE) ===\n\n" M_RST);
            printf("  [" M_CYN "1" M_RST "] Ana Okul'a Gir (Çalışma Merkezi)\n");
            printf("  [" M_CYN "2" M_RST "] %s Kulübesi (Açık)\n\n", profile->god_alignment);
            printf("  [" M_CYN "0" M_RST "] Güneş Sarayı Merkezine (Haritaya) Dön\n\n");
            printf(M_CYN "  Eylem Seçimi: " M_RST);
        } else {
            printf(M_YEL "  === THE VILLAGE OF SHRINES ===\n\n" M_RST);
            printf("  [" M_CYN "1" M_RST "] Enter Main School (Study Center)\n");
            printf("  [" M_CYN "2" M_RST "] Shrine of %s (Open)\n\n", profile->god_alignment);
            printf("  [" M_CYN "0" M_RST "] Return to The Sun Nexus (Map)\n\n");
            printf(M_CYN "  Select Action: " M_RST);
        }

        // --- Girdi Kontrolü ---
        bool valid_input = false;
        while (!valid_input) {
            if (_kbhit()) {
                char ch = _getch();

                if (ch == '0') {
                    in_village = false; // Haritaya Dön
                    valid_input = true;
                }
                else if (ch == '1') {
                    scene_main_school(profile);
                    valid_input = true;
                }
                else if (ch == '2') {
                    // Sadece oyuncunun atandığı tanrının kulübesi
                    char loc_tr[50], loc_en[50];
                    sprintf(loc_tr, "%s Kulübesi", profile->god_alignment);
                    sprintf(loc_en, "Shrine of %s", profile->god_alignment);
                    scene_inside_location(loc_tr, loc_en);
                    valid_input = true;
                }
            }
            Sleep(20);
        }
    } // while (in_village) döngüsünün kapanışı

    clear_screen();
} // void scene_inner_shrine kapanışı


// ============================================================================
// THE ETHEREAL CASTLE OF THE SUN (MATHEMATICALLY PERFECT ASCII)
// ============================================================================
void scene_map(CharacterProfile* profile) {
    bool exploring = true;

    while (exploring) {
        clear_screen();

        // --- 1. FLAWLESS SYMMETRIC FORTRESS ON A CLIFF ---
        printf("\n");
        printf(COLOR_DARK  "                          * " COLOR_RED "|>>>\n" COLOR_RESET);
        printf(COLOR_DARK  "          .                 " COLOR_WHITE "|\n" COLOR_RESET);
        printf(COLOR_DARK  "       .    " COLOR_RED "|>>>" COLOR_WHITE "      _ _  _|_  _ _         " COLOR_RED "|>>>\n" COLOR_RESET);
        printf(COLOR_WHITE "            |        |;| |;| |;| |;|        |\n" COLOR_RESET);
        printf(COLOR_DARK  "    * " COLOR_WHITE "_ _  _|_  _ _ " COLOR_CYAN "  \\\\\\.        /" COLOR_WHITE "   _ _  _|_  _ _   " COLOR_DARK "*\n" COLOR_RESET);
        printf(COLOR_WHITE "     |;|_|;|_|;|_|;|  " COLOR_CYAN " \\\\\\:      /" COLOR_WHITE "   |;|_|;|_|;|_|;|\n" COLOR_RESET);
        printf(COLOR_CYAN  "      \\\\\\\..       /    " COLOR_WHITE "||:       |    " COLOR_CYAN "\\\\\\\..       /\n" COLOR_RESET);
        printf(COLOR_CYAN  "       \\\\\\\.      /     " COLOR_WHITE "||:       |    " COLOR_CYAN " \\\\\\\.      /\n" COLOR_RESET);
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

// ============================================================================
// CHRONOS ZAMAN MODÜLÜ
// ============================================================================
void print_mythic_date() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    int real_year = tm.tm_year + 1900;
    int mythic_year = real_year - 1600;

    printf(COLOR_CYAN "  Tarih: %02d.%02d.%d (Karanlık Çağ)\n" COLOR_RESET, tm.tm_mday, tm.tm_mon + 1, mythic_year);
}

void append_study_log(const char* subject, int earned_exp, int minutes) {
    // Çalışma bitince kulübedeki günlüğe (log) yazdırır
    FILE *logfile = fopen("shrine_chronicles.txt", "a");
    if (logfile != NULL) {
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        int mythic_year = (tm.tm_year + 1900) - 1600;

        fprintf(logfile, "[%02d.%02d.%d] %s Parşömeni üzerinde %d dakika odaklanıldı. (+%d EXP)\n",
                tm.tm_mday, tm.tm_mon + 1, mythic_year, subject, minutes, earned_exp);
        fclose(logfile);
    }
}

void scene_init_subjects(CharacterProfile* profile) {
    clear_screen();
    printf(COLOR_GOLD " =============================================================\n");
    if (current_lang == 1) {
        printf("                  KADERİNİN YAZILDIĞI PARŞÖMENLER               \n");
        printf(" =============================================================\n\n" COLOR_RESET);
        printf(COLOR_WHITE "  Önünde fethedilmeyi bekleyen yollar (Dersler) var.\n");
        printf("  Sisteme toplamda kaç Epik Hedef tanımlayacaksın? (1-15): " COLOR_RESET);
    } else {
        printf("                  THE SCROLLS OF YOUR DESTINY               \n");
        printf(" =============================================================\n\n" COLOR_RESET);
        printf(COLOR_WHITE "  There are paths (Subjects) waiting to be conquered.\n");
        printf("  How many Epic Targets will you define? (1-15): " COLOR_RESET);
    }

    set_cursor_visibility(true);

    // Klavyedeki gereksiz Enter kalıntılarını temizleyip güvenli sayıyı al
    profile->active_subject_count = get_safe_natural_number(1, 15);

    for(int i = 0; i < profile->active_subject_count; i++) {
        printf("\n");
        if (current_lang == 1) printf(COLOR_CYAN "  [%d. Epik Hedef / Ders Adı]: " COLOR_RESET, i+1);
        else printf(COLOR_CYAN "  [%d. Epic Target / Subject Name]: " COLOR_RESET, i+1);

        read_string_safe(profile->subject_names[i], 50);

        if (current_lang == 1) printf(COLOR_RED "  Kaç Büyük Boss (Sınav) var? (0 ve üstü): " COLOR_RESET);
        else printf(COLOR_RED "  How many Great Bosses (Exams)? (0+): " COLOR_RESET);
        profile->subject_exams[i] = get_safe_natural_number(0, 100);

        if (current_lang == 1) printf(COLOR_GOLD "  Kaç Kuşatma (Proje) var? (0 ve üstü): " COLOR_RESET);
        else printf(COLOR_GOLD "  How many Sieges (Projects)? (0+): " COLOR_RESET);
        profile->subject_projects[i] = get_safe_natural_number(0, 100);

        profile->study_stats[i] = 0; // EXP seviyesi sıfırlanır
    }

    // Kullanılmayan (arka planda kalan) statları güvene al
    for(int i = profile->active_subject_count; i < 15; i++) {
        strcpy(profile->subject_names[i], "BOS");
        profile->subject_exams[i] = 0;
        profile->subject_projects[i] = 0;
        profile->study_stats[i] = 0;
    }

    set_cursor_visibility(false);
    save_game(profile); // Dosyaya kaydet
}



void scene_own_shrine(CharacterProfile* profile) {
    bool in_shrine = true;

    while(in_shrine) {
        clear_screen();
        printf("\n");
        printf(COLOR_GOLD "  === %s KULÜBESİ (KİŞİSEL MABEDİN) ===\n\n" COLOR_RESET, profile->god_alignment);

        print_mythic_date(); // Mitolojik tarihi yazdırır

        printf(COLOR_WHITE "\n  Yatağından kalktın. Raflarda kadim parşömenler ve savaş planları duruyor.\n\n" COLOR_RESET);

        printf("  [" COLOR_CYAN "1" COLOR_RESET "] Tapınak Köyüne Çık (Ana Okul'a gitmek için)\n");
        printf("  [" COLOR_CYAN "2" COLOR_RESET "] Savaş Günlüğünü Oku (Çalışma Logları)\n");
        printf("  [" COLOR_CYAN "3" COLOR_RESET "] Kehanet Takvimini İncele (Boss Savaşları ve Projeler)\n");
        printf("  [" COLOR_CYAN "0" COLOR_RESET "] Sisteme Geri Dön (Çıkış)\n\n");

        printf(COLOR_CYAN "  Eylem Seçimi: " COLOR_RESET);

        bool valid_input = false;
        while (!valid_input) {
            if (_kbhit()) {
                char ch = _getch();
                if (ch == '1') {
                    // Tapınak Köyüne çıkış (Buradan Okula gidilebilir)
                    scene_inner_shrine(profile);
                    valid_input = true;
                } else if (ch == '2') {
                    // shrine_chronicles.txt dosyasını okuyan bir fonksiyon çağrılacak
                    // scene_read_logs();
                    valid_input = true;
                } else if (ch == '3') {
                    // Boss listesini (subject_exams sayılarını) gösteren fonksiyon
                    // scene_view_calendar(profile);
                    valid_input = true;
                } else if (ch == '0') {
                    in_shrine = false;
                    valid_input = true;
                }
            }
            Sleep(20);
        }
    }
}

