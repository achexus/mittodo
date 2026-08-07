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

    // --- 7. ÇOKLU KAYIT SİSTEMİ (Yeni) ---
    int save_slot; // Karakterin 0-9 arasındaki dosya numarası

} CharacterProfile;

#define MAX_SAVES 10

// Tüm kozmik hafızayı diskten okur
bool load_all_saves(CharacterProfile saves[MAX_SAVES]) {
    FILE *infile = fopen("mythic_saves_v2.dat", "rb");
    if (infile != NULL) {
        fread(saves, sizeof(CharacterProfile), MAX_SAVES, infile);
        fclose(infile);
        return true;
    }
    // Dosya yoksa hepsini sıfırla (Boş slotlar)
    memset(saves, 0, sizeof(CharacterProfile) * MAX_SAVES);
    return false;
}

// Tüm kozmik hafızayı diske yazar
void save_all_saves(CharacterProfile saves[MAX_SAVES]) {
    FILE *outfile = fopen("mythic_saves_v2.dat", "wb");
    if (outfile != NULL) {
        fwrite(saves, sizeof(CharacterProfile), MAX_SAVES, outfile);
        fclose(outfile);
    }
}

// Sadece aktif oynanan karakteri kendi slotuna kaydeder
void save_game(CharacterProfile* profile) {
    CharacterProfile saves[MAX_SAVES];
    load_all_saves(saves); // Önce tüm hafızayı al
    saves[profile->save_slot] = *profile; // Oyuncunun slotunu güncelle
    save_all_saves(saves); // Geri yaz
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
    char archetype_tr[50];
    char faction[20];
    char faction_tr[20];
    int intel, might, honor, skill, faith;
} ArchetypeMatrix;

ArchetypeMatrix database[33] = {
    // ZEUS (Odak: Zeka, Güç, Onur | Zayıflık: Yetenek, İnanç)
    {"Zeus", "The Thunder Wrath", "Yıldırım Öfkesi", "SUN", "GÜNEŞ", 5, 10, 7, 3, 1},
    {"Zeus", "The Cloud Arbiter", "Bulutların Hakemi", "OCEAN", "OKYANUS", 10, 7, 5, 1, 3},
    {"Zeus", "The Absolute Sovereign", "Mutlak Hükümdar", "EARTH", "TOPRAK", 5, 7, 10, 1, 3},

    // POSEIDON (Odak: Güç, Yetenek | Zayıflık: Zeka, Onur)
    {"Poseidon", "The Tidal Ravager", "Gelgit Yıkıcısı", "SUN", "GÜNEŞ", 1, 10, 3, 7, 5},
    {"Poseidon", "The Abyssal Master", "Uçurumun Efendisi", "OCEAN", "OKYANUS", 3, 7, 1, 10, 5},
    {"Poseidon", "The Earth-Shaker", "Yeryüzü Titreten", "EARTH", "TOPRAK", 1, 7, 10, 5, 3},

    // HADES (Odak: Zeka, Onur | Zayıflık: Güç, İnanç)
    {"Hades", "The Relentless Executioner", "Amansız İnfazcı", "SUN", "GÜNEŞ", 5, 10, 7, 1, 3},
    {"Hades", "The Soul Collector", "Ruh Toplayıcı", "OCEAN", "OKYANUS", 10, 1, 5, 7, 3},
    {"Hades", "The Underworld Autocrat", "Yeraltı Otokratı", "EARTH", "TOPRAK", 7, 3, 10, 1, 5},

    // DEMETER (Odak: Onur, İnanç, Hayatta Kalma | Zayıflık: Kaba Güç)
    // ESKİ SUN  : [3, 7, 5, 1, 10] -> YENİ SUN  : [1, 5, 7, 3, 10] (Kış kaba kuvvet değil, dondurucu bir sabırdır)
    // ESKİ OCEAN: [7, 1, 5, 3, 10] -> YENİ OCEAN: [5, 1, 7, 3, 10] (Hasat döngüsü zeka değil, sadakat ve onurdur)
    // ESKİ EARTH: [1, 5, 10, 3, 7] -> YENİ EARTH: [3, 1, 10, 5, 7] (Toprak ana kaba güç değil, yaratım yeteneğidir)
    {"Demeter", "The Winter Famine", "Kış Kıtlığı", "SUN", "GÜNEŞ", 3, 5, 7, 1, 10},
    {"Demeter", "The Harvest Cycle", "Hasat Döngüsü", "OCEAN", "OKYANUS", 5, 1, 7, 3, 10},
    {"Demeter", "The Earth Mother", "Toprak Ana", "EARTH", "TOPRAK", 3, 1, 10, 5, 7},

    // ATHENA (Odak: Zeka, Onur, Yetenek | Zayıflık: Güç)
    {"Athena", "The War Vanguard", "Savaş Öncüsü", "SUN", "GÜNEŞ", 7, 5, 3, 10, 1},
    {"Athena", "The Supreme Tactician", "Yüce Taktisyen", "OCEAN", "OKYANUS", 10, 3, 5, 7, 1},
    {"Athena", "The Citadel Protector", "Hisar Koruyucusu", "EARTH", "TOPRAK", 7, 1, 10, 5, 3},

    // APOLLO (Dengeli Sanat/Kehanet)
    {"Apollo", "The Radiant Bowman", "Parlak Okçu", "SUN", "GÜNEŞ", 3, 5, 1, 10, 7},
    {"Apollo", "The Golden Maestro", "Altın Maestro", "OCEAN", "OKYANUS", 7, 1, 3, 5, 10},
    {"Apollo", "The Oracle of Truth", "Hakikat Kahini", "EARTH", "TOPRAK", 5, 3, 1, 7, 10},

    // APHRODITE (Dengeli Tutku/İllüzyon)
    {"Aphrodite", "The Flame of Passion", "İhtiras Alevi", "SUN", "GÜNEŞ", 1, 5, 3, 7, 10},
    {"Aphrodite", "The Siren Puppetmaster", "Siren Kukla Ustası", "OCEAN", "OKYANUS", 5, 1, 3, 10, 7},
    {"Aphrodite", "The Flawless Form", "Kusursuz Form", "EARTH", "TOPRAK", 3, 1, 7, 10, 5},

    // ARES (Odak: Güç, Yetenek | Zayıflık: Zeka, Onur)
    {"Ares", "The Blind Berserker", "Kör Savaşçı", "SUN", "GÜNEŞ", 1, 10, 3, 5, 7},
    {"Ares", "The Blood Tactician", "Kanlı Taktisyen", "OCEAN", "OKYANUS", 5, 7, 1, 10, 3},
    {"Ares", "The Dread Champion", "Dehşet Şampiyonu", "EARTH", "TOPRAK", 1, 10, 7, 5, 3},

    // HERMES (Odak: Hız/Yetenek, Zeka)
    {"Hermes", "The Hurricane Herald", "Kasırga Haberci", "SUN", "GÜNEŞ", 7, 5, 1, 10, 3}, // <- DÜZELTİLDİ
    {"Hermes", "The Phantom Thief", "Hayalet Hırsız", "OCEAN", "OKYANUS", 7, 3, 1, 10, 5},
    {"Hermes", "The Crossroads Guide", "Kavşak Rehberi", "EARTH", "TOPRAK", 10, 3, 5, 1, 7},

    // HEPHAESTUS (Dengeli Zanaat/Dayanıklılık)
    {"Hephaestus", "The Volcanic Juggernaut", "Volkanik Dev", "SUN", "GÜNEŞ", 3, 10, 5, 7, 1},
    {"Hephaestus", "The Outcast Innovator", "Dışlanmış Yenilikçi", "OCEAN", "OKYANUS", 7, 3, 5, 10, 1}, // <- DÜZELTİLDİ
    {"Hephaestus", "The Forgemaster", "Usta Demirci", "EARTH", "TOPRAK", 5, 7, 10, 3, 1},

    // DIONYSUS (Dengeli Kaos/Kült)
    {"Dionysus", "The Madness Bringer", "Delilik Getiren", "SUN", "GÜNEŞ", 1, 7, 3, 5, 10},
    {"Dionysus", "The Illusionist Vagabond", "İllüzyonist Serseri", "OCEAN", "OKYANUS", 10, 1, 3, 5, 7},
    {"Dionysus", "The Cult Leader", "Kült Lideri", "EARTH", "TOPRAK", 1, 5, 7, 3, 10}
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
bool scene_continue_journey(CharacterProfile* profile);
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
void scene_library_menu(CharacterProfile* profile);
void scene_read_logs(void);
void scene_view_calendar(CharacterProfile* profile);
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
// STAT ŞELALESİ (CASCADE) SİSTEMİ - KOZMİK SÜRTÜNME İLE
// ============================================================================
typedef enum {
    STAT_INTEL,
    STAT_SKILL,
    STAT_MIGHT,
    STAT_HONOR,
    STAT_FAITH
} StatType;

// Eksi puanların kaybolmasını önleyen ve "Kozmik Sürtünme" ile yığılmayı engelleyen algoritma
void update_stat(CharacterProfile* p, StatType stat, int amount) {
    if (amount > 0) {
        switch(stat) {
            case STAT_INTEL: p->intel += amount; break;
            case STAT_SKILL: p->skill += amount; break;
            case STAT_MIGHT: p->might += amount; break;
            case STAT_HONOR: p->honor += amount; break;
            case STAT_FAITH: p->faith += amount; break;
        }
        return;
    }

    int deficit = -amount;
    StatType current_stat = stat;

    while (deficit > 0) {
        int* target_val = NULL;
        StatType next_stat;

        switch(current_stat) {
            case STAT_INTEL: target_val = &p->intel; next_stat = STAT_SKILL; break;
            case STAT_SKILL: target_val = &p->skill; next_stat = STAT_MIGHT; break;
            case STAT_MIGHT: target_val = &p->might; next_stat = STAT_HONOR; break;
            case STAT_HONOR: target_val = &p->honor; next_stat = STAT_FAITH; break;
            case STAT_FAITH: target_val = &p->faith; next_stat = STAT_INTEL; break;
        }

        if (*target_val >= deficit) {
            *target_val -= deficit;
            deficit = 0;
        }
        else {
            deficit -= *target_val;
            *target_val = 0;
            current_stat = next_stat;

            // YARATICI ÇÖZÜM 1: Kozmik Sürtünme
            // Eksi puan başka bir stata atlarken enerjisinin yarısını kaybeder.
            deficit = deficit / 2;
        }
    }
}

// ============================================================================
// STAT UYGULAYICI (KUSURSUZ SİMETRİ İÇİN BASİTLEŞTİRİLMİŞTİR)
// ============================================================================
void apply_stat(int* stat, int amount) {
    *stat += amount;
    if (*stat < 1) *stat = 1;
}

// ============================================================================
// HIZLI MATRİS TEST MODU (KOZMİK SİMETRİ VE ALTIN DİZİ VERİTABANI İLE)
// ============================================================================
void test_matrix_simulation(CharacterProfile* dummy_profile) {
    clear_screen();
    set_cursor_visibility(false);

    printf(COLOR_GOLD " === KOZMİK MATRİS OTOMATİK TARAMA ===\n\n" COLOR_RESET);
    printf(COLOR_DARK " Sistem tüm ihtimalleri hesaplıyor...\n");
    printf(" Lütfen bekleyin, 'Kusursuz Simetri' devrede...\n\n" COLOR_RESET);

    int archetype_counts[33] = {0};
    int first_path[33][5] = {0};
    int first_stats[33][5] = {0};
    int first_affinity[33] = {0};

    int total_combinations_tested = 0;
    int unique_results = 0;

    for(int c1 = 1; c1 <= 3; c1++) {
        for(int c2 = 1; c2 <= 3; c2++) {
            for(int c3 = 1; c3 <= 3; c3++) {
                for(int c4 = 1; c4 <= 4; c4++) {
                    for(int final_ans = 1; final_ans <= 7; final_ans++) {

                        CharacterProfile p;
                        memset(&p, 0, sizeof(CharacterProfile));

                        // Herkes tam merkezde, nötr başlar
                        p.intel = 3; p.might = 3; p.honor = 3; p.skill = 3; p.faith = 3;
                        p.poseidon_veto = 0;

                        // ====================================================================
                        // KUSURSUZ 40'LI MATRİS (Her stat tam 8 kez +, 8 kez - alır)
                        // ====================================================================

                        // --- 1. SINAV (Blok 1-3) ---
                        if (c1 == 1) { apply_stat(&p.might, 3); apply_stat(&p.intel, -1); }
                        else if (c1 == 2) { apply_stat(&p.intel, 3); apply_stat(&p.honor, -1); }
                        else if (c1 == 3) { apply_stat(&p.faith, 3); apply_stat(&p.might, -1); }

                        // --- 2. SINAV (Blok 4-12) ---
                        if (c1 == 1) {
                            if (c2 == 1) { apply_stat(&p.honor, 3); apply_stat(&p.skill, -1); }
                            else if (c2 == 2) { apply_stat(&p.skill, 3); apply_stat(&p.faith, -1); }
                            else if (c2 == 3) { apply_stat(&p.faith, 3); apply_stat(&p.intel, -1); }
                        } else if (c1 == 2) {
                            if (c2 == 1) { apply_stat(&p.skill, 3); apply_stat(&p.might, -1); }
                            else if (c2 == 2) { apply_stat(&p.intel, 3); apply_stat(&p.might, -1); }
                            else if (c2 == 3) { apply_stat(&p.honor, 3); apply_stat(&p.skill, -1); }
                        } else if (c1 == 3) {
                            if (c2 == 1) { apply_stat(&p.skill, 3); apply_stat(&p.honor, -1); }
                            else if (c2 == 2) { apply_stat(&p.honor, 3); apply_stat(&p.might, -1); }
                            else if (c2 == 3) { apply_stat(&p.faith, 3); apply_stat(&p.skill, -1); }
                        }

                        // --- 3. SINAV REAKSİYONU (Blok 13-27) ---
                        if (c1 == 1 && c2 == 1) {
                            if (c3 == 1) { apply_stat(&p.faith, 3); apply_stat(&p.honor, -1); }
                            else if (c3 == 2) { apply_stat(&p.might, 3); apply_stat(&p.faith, -1); }
                            else if (c3 == 3) { apply_stat(&p.honor, 3); apply_stat(&p.skill, -1); }
                        } else if (c1 == 1 && c2 == 2) {
                            if (c3 == 1) { apply_stat(&p.might, 3); apply_stat(&p.intel, -1); }
                            else if (c3 == 2) { apply_stat(&p.faith, 3); apply_stat(&p.honor, -1); }
                            else if (c3 == 3) { apply_stat(&p.intel, 3); apply_stat(&p.faith, -1); }
                        } else if (c1 == 2) {
                            if (c3 == 1) { apply_stat(&p.honor, 3); apply_stat(&p.intel, -1); }
                            else if (c3 == 2) { apply_stat(&p.intel, 3); apply_stat(&p.skill, -1); }
                            else if (c3 == 3) { apply_stat(&p.skill, 3); apply_stat(&p.honor, -1); }
                        } else if (c1 == 3 && c2 == 1) {
                            if (c3 == 1) { apply_stat(&p.skill, 3); apply_stat(&p.faith, -1); }
                            else if (c3 == 2) { apply_stat(&p.faith, 3); apply_stat(&p.might, -1); }
                            else if (c3 == 3) { apply_stat(&p.might, 3); apply_stat(&p.intel, -1); }
                        } else {
                            if (c3 == 1) { apply_stat(&p.might, 3); apply_stat(&p.honor, -1); }
                            else if (c3 == 2) { apply_stat(&p.intel, 3); apply_stat(&p.faith, -1); }
                            else if (c3 == 3) { apply_stat(&p.honor, 3); apply_stat(&p.skill, -1); }
                        }

                        // Hikaye Mantığı Tespiti
                        bool helios_path = false;
                        if ((c1 == 1 && c2 == 1 && (c3 == 1 || c3 == 2)) || (c1 == 1 && c2 == 3) || (c1 == 3 && (c2 == 2 || c2 == 3))) helios_path = true;
                        if (c1 == 3 && c2 == 1 && c3 == 2) helios_path = true;
                        if (c1 == 3 && c2 == 1 && c3 == 3) helios_path = true;
                        if (c1 == 1 && c2 == 1 && c3 == 3) p.poseidon_veto = 1;

                        // --- 4. SAHNE VI (Blok 28-33) ---
                        if (helios_path) {
                            if (c4 == 1) continue;
                            else if (c4 == 2) { apply_stat(&p.might, 3); apply_stat(&p.intel, -1); }
                            else if (c4 == 3) { apply_stat(&p.faith, 3); apply_stat(&p.skill, -1); }
                            else if (c4 == 4) { apply_stat(&p.intel, 3); apply_stat(&p.honor, -1); }
                        } else {
                            if (c4 == 3) continue;
                            if (c4 == 1) { apply_stat(&p.intel, 3); apply_stat(&p.faith, -1); }
                            else if (c4 == 2) { apply_stat(&p.might, 3); apply_stat(&p.faith, -1); }
                            else if (c4 == 4) { apply_stat(&p.skill, 3); apply_stat(&p.might, -1); }
                        }

                        // --- 5. ALTIN VURUŞ (Blok 34-40) ---
                        if (final_ans == 1) { apply_stat(&p.might, 5); apply_stat(&p.faith, -2); }
                        else if (final_ans == 2) { apply_stat(&p.honor, 5); apply_stat(&p.intel, -2); }
                        else if (final_ans == 3) { apply_stat(&p.skill, 5); apply_stat(&p.honor, -2); }
                        else if (final_ans == 4) { apply_stat(&p.intel, 5); apply_stat(&p.might, -2); }
                        else if (final_ans == 5) { apply_stat(&p.faith, 5); apply_stat(&p.skill, -2); }
                        else if (final_ans == 6) { apply_stat(&p.honor, 5); apply_stat(&p.might, -2); }
                        else if (final_ans == 7) { apply_stat(&p.skill, 5); apply_stat(&p.intel, -2); }

                        total_combinations_tested++;

                        // --- STANDART KOSİNÜS MATEMATİĞİ ---
                        double max_cosine = -2.0;
                        int best_match_idx = 0;

                        for (int i = 0; i < 33; i++) {
                            if (p.poseidon_veto == 1 && strcmp(database[i].god, "Poseidon") == 0) continue;

                            double dot = (p.intel * database[i].intel) + (p.might * database[i].might) +
                                         (p.honor * database[i].honor) + (p.skill * database[i].skill) + (p.faith * database[i].faith);
                            double mag_A = sqrt(pow(p.intel, 2) + pow(p.might, 2) + pow(p.honor, 2) + pow(p.skill, 2) + pow(p.faith, 2));
                            double mag_B = sqrt(pow(database[i].intel, 2) + pow(database[i].might, 2) + pow(database[i].honor, 2) + pow(database[i].skill, 2) + pow(database[i].faith, 2));

                            double cos_sim = (mag_A > 0 && mag_B > 0) ? (dot / (mag_A * mag_B)) : 0.0;
                            if (cos_sim > max_cosine) { max_cosine = cos_sim; best_match_idx = i; }
                        }

                        if (archetype_counts[best_match_idx] == 0) {
                            first_path[best_match_idx][0] = c1;
                            first_path[best_match_idx][1] = c2;
                            first_path[best_match_idx][2] = c3;
                            first_path[best_match_idx][3] = c4;
                            first_path[best_match_idx][4] = final_ans;

                            first_stats[best_match_idx][0] = p.intel;
                            first_stats[best_match_idx][1] = p.might;
                            first_stats[best_match_idx][2] = p.honor;
                            first_stats[best_match_idx][3] = p.skill;
                            first_stats[best_match_idx][4] = p.faith;

                            first_affinity[best_match_idx] = (int)(max_cosine * 100.0);
                            unique_results++;
                        }
                        archetype_counts[best_match_idx]++;
                    }
                }
            }
        }
    }

    for (int i = 0; i < 33; i++) {
        if (archetype_counts[i] > 0) {
            printf(" YOL [%d.%d.%d.%d.%d] -> " COLOR_CYAN "Z:%02d G:%02d O:%02d Y:%02d I:%02d" COLOR_RESET " | " COLOR_GOLD "%%%02d %s - %s" COLOR_RESET " | " COLOR_RED "[ %d Kez Çıktı ]" COLOR_RESET "\n",
                first_path[i][0], first_path[i][1], first_path[i][2], first_path[i][3], first_path[i][4],
                first_stats[i][0], first_stats[i][1], first_stats[i][2], first_stats[i][3], first_stats[i][4],
                first_affinity[i], database[i].god,
                (current_lang == 1 ? database[i].archetype_tr : database[i].archetype),
                archetype_counts[i]
            );
        }
    }

    printf("\n" COLOR_WHITE "  -------------------------------------------------" COLOR_RESET "\n");
    printf(COLOR_WHITE "  Toplam Hesaplanıp Geçilen Kombinasyon : %d\n" COLOR_RESET, total_combinations_tested);
    printf(COLOR_CYAN "  Sistemin Ulaşabildiği Arketip Sayısı  : %d / 33\n" COLOR_RESET, unique_results);
    printf(COLOR_WHITE "  -------------------------------------------------" COLOR_RESET "\n");

    if (current_lang == 1) printf("\n" COLOR_DARK " [Menüye dönmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
    else printf("\n" COLOR_DARK " [Press ANY KEY to return to menu]\n" COLOR_RESET);

    while (_kbhit()) _getch();
    _getch();
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
        .intel = 1, .might = 1, .honor = 1, .skill = 1, .faith = 1
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
                case '1': { // YOLCULUĞA BAŞLA
                    set_cursor_visibility(true);

                    // Boş slot kontrolü yap
                    CharacterProfile saves[MAX_SAVES];
                    load_all_saves(saves);
                    int empty_slot = -1;
                    for(int i = 0; i < MAX_SAVES; i++) {
                        if(strlen(saves[i].player_name) == 0) {
                            empty_slot = i;
                            break;
                        }
                    }

                    if(empty_slot == -1) {
                        clear_screen();
                        if(current_lang == 1) {
                            printf(COLOR_RED "\n  [!] Tüm kader iplikleri (10/10) dolu.\n");
                            printf("  Yeni bir ruhun uyanabilmesi için 'Yolculuğa Devam Et' menüsünden eski bir kaydı silmelisin.\n" COLOR_RESET);
                            printf(COLOR_DARK "  [Menüye dönmek için HERHANGİ BİR TUŞA bas]\n" COLOR_RESET);
                        } else {
                            printf(COLOR_RED "\n  [!] All threads of destiny (10/10) are full.\n");
                            printf("  To awaken a new soul, you must delete an old save from the 'Continue Journey' menu.\n" COLOR_RESET);
                            printf(COLOR_DARK "  [Press ANY KEY to return to menu]\n" COLOR_RESET);
                        }
                        _getch();
                        set_cursor_visibility(false);
                        clear_screen();
                        break;
                    }

                    // Boş slot bulundu, yeni oturum için temizle
                    memset(&player, 0, sizeof(CharacterProfile));
                    player.save_slot = empty_slot; // Slotu karaktere mühürle

                    player.intel = 5; player.might = 5; player.honor = 5; player.skill = 5; player.faith = 5;
                    strcpy(player.god_alignment, "UNASSIGNED");
                    strcpy(player.player_name, "Wastrel");

                    // Start journey
                    if (!scene_start_journey(&player)) running = false;
                    set_cursor_visibility(false);
                    clear_screen(); // HATA BURADAYDI: Ekranı temizleyip ana menüye dönmesini sağlıyoruz.
                    break;
                }
                case '2': { // YOLCULUĞA DEVAM ET
                    set_cursor_visibility(true);
                    if (scene_continue_journey(&player)) {
                        scene_own_shrine(&player); // Başarıyla yüklendiyse doğrudan Mabet'e gir
                    }
                    clear_screen();
                    set_cursor_visibility(false);
                    break;
                }
                case '3':
                    scene_language_options();
                    break;
                case '4':
                    clear_screen();
                    if (current_lang == 1) printf(COLOR_WHITE "\n \"Kaderinden kaçamazsın; sadece onu geciktirebilirsin.\"\n" COLOR_RESET);
                    else printf(COLOR_WHITE "\n \"You cannot escape your destiny; you can only delay it.\"\n" COLOR_RESET);
                    running = false;
                    break;
                case '5':
                    test_matrix_simulation(&player);
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
        printf("   [%s5%s] TEST MATRİSİ (Hızlı Simülasyon)\033[K\n", option_color, border_color);
        printf("   -------------------------------------------------------------\033[K\n");
        printf("   [%s0%s] Sistem Durumu & Matris Doğrulama\033[K\n", title_color, border_color);
        printf("   -------------------------------------------------------------\033[K\n\n");
        printf("%s Hedef Eylem (0-4): \033[K" COLOR_RESET, option_color);
    } else { // English Menu
        printf("   [%s1%s] START JOURNEY\033[K\n", option_color, border_color);
        printf("   [%s2%s] CONTINUE JOURNEY\033[K\n", option_color, border_color);
        printf("   [%s3%s] LANGUAGE OPTIONS\033[K\n", option_color, border_color);
        printf("   [%s4%s] ESCAPE DESTINY (Exit Game)\033[K\n", option_color, border_color);
        printf("   [%s5%s] TEST MATRIX (Quick Simulation)\033[K\n", option_color, border_color);
        printf("   -------------------------------------------------------------\033[K\n");
        printf("   [%s0%s] System Status & Matrix Verification\033[K\n", title_color, border_color);
        printf("   -------------------------------------------------------------\033[K\n\n");
        printf("%s Target Action (0-4): \033[K" COLOR_RESET, option_color);
    }
}

bool scene_start_journey(CharacterProfile* profile) {
    clear_screen();
    set_cursor_visibility(false);

    // 1. Zifiri karanlık ve sessizlik
    Sleep(2000);

    // 2. The subtle trigger (Gerilimi başlatan fısıltı)
    if (current_lang == 1) {
        printf(COLOR_DARK "\n\n\n\n\n\n\n\n\n\n                                  * pısst *\n" COLOR_RESET);
    } else {
        printf(COLOR_DARK "\n\n\n\n\n\n\n\n\n\n                                  * psst *\n" COLOR_RESET);
    }

    Sleep(1500); // Oyuncunun tepki vermesi ve gerilim için kısa bir süre

    // 3. SANATSAL VE GENİŞ ALAN ŞİMŞEK ANİMASYONU (Gök Yarığı)

    // ÇERÇEVE 1: Kör Edici Beyaz Patlama (Ana Şimşekler)
    clear_screen();
    printf(COLOR_WHITE "\n\n\n");
    printf("        /|        \\\\               / /        _.-''-._        \\ \\               //        |\\   \n");
    printf("      _/_|_       _\\\\_     _      / /      ,-'        '-,      \\ \\      _     _//_       _|_\\_ \n");
    printf("     / |/ /        \\\\     /' \\    / /      /              \\      \\ \\    / '\\     //        \\ \\| \\\n");
    printf("    /    /          \\\\  /'    \\  / /      /                \\      \\ \\  /    '\\  //          \\    \\\n");
    printf("   /    /           _\\\\/'      \\/ /      /                  \\      \\ \\/      '\\_          \\    \\\n");
    printf("  /    /          /'  \\\\       / /      /                    \\      \\ \\       //  '\\        \\    \\\n");
    printf("      /         /'     \\\\     / /     /'                      '\\     \\ \\     //     '\\       \\    \n");
    printf("     /        /'        \\\\   / /     /                          \\     \\ \\   //        '\\      \\   \n");
    printf("    /       /'           \\\\ / /     /                            \\     \\ \\ //           '\\     \\  \n");
    printf("   /      /'              \\V /     /                              \\     \\ V/              '\\    \\ \n");
    printf("  /     /'                  V     /                                \\     V                  '\\   \\\n");
    printf(" /    /'                         /                                  \\                         '\\  \\\n");
    printf(COLOR_RESET);
    Sleep(70);

    // ÇERÇEVE 2: Retina İzi (Cyan Afterglow)
    clear_screen();
    printf(COLOR_CYAN "\n\n\n");
    printf("        /|        \\\\               / /        _.-''-._        \\ \\               //        |\\   \n");
    printf("      _/_|_       _\\\\_     _      / /      ,-'        '-,      \\ \\      _     _//_       _|_\\_ \n");
    printf("     / |/ /        \\\\     /' \\    / /      /              \\      \\ \\    / '\\     //        \\ \\| \\\n");
    printf("    /    /          \\\\  /'    \\  / /      /                \\      \\ \\  /    '\\  //          \\    \\\n");
    printf("   /    /           _\\\\/'      \\/ /      /                  \\      \\ \\/      '\\_          \\    \\\n");
    printf("  /    /          /'  \\\\       / /      /                    \\      \\ \\       //  '\\        \\    \\\n");
    printf("      /         /'     \\\\     / /     /'                      '\\     \\ \\     //     '\\       \\    \n");
    printf("     /        /'        \\\\   / /     /                          \\     \\ \\   //        '\\      \\   \n");
    printf("    /       /'           \\\\ / /     /                            \\     \\ \\ //           '\\     \\  \n");
    printf("   /      /'              \\V /     /                              \\     \\ V/              '\\    \\ \n");
    printf("  /     /'                  V     /                                \\     V                  '\\   \\\n");
    printf(" /    /'                         /                                  \\                         '\\  \\\n");
    printf(COLOR_RESET);
    Sleep(90);

    // ÇERÇEVE 3: Karanlıkta Kayboluş (Fading Out)
    clear_screen();
    printf(COLOR_DARK "\n\n\n");
    printf("        /|        \\\\               / /        _.-''-._        \\ \\               //        |\\   \n");
    printf("      _/_|_       _\\\\_     _      / /      ,-'        '-,      \\ \\      _     _//_       _|_\\_ \n");
    printf("     / |/ /        \\\\     /' \\    / /      /              \\      \\ \\    / '\\     //        \\ \\| \\\n");
    printf("    /    /          \\\\  /'    \\  / /      /                \\      \\ \\  /    '\\  //          \\    \\\n");
    printf("   /    /           _\\\\/'      \\/ /      /                  \\      \\ \\/      '\\_          \\    \\\n");
    printf("  /    /          /'  \\\\       / /      /                    \\      \\ \\       //  '\\        \\    \\\n");
    printf("      /         /'     \\\\     / /     /'                      '\\     \\ \\     //     '\\       \\    \n");
    printf("     /        /'        \\\\   / /     /                          \\     \\ \\   //        '\\      \\   \n");
    printf("    /       /'           \\\\ / /     /                            \\     \\ \\ //           '\\     \\  \n");
    printf("   /      /'              \\V /     /                              \\     \\ V/              '\\    \\ \n");
    printf("  /     /'                  V     /                                \\     V                  '\\   \\\n");
    printf(" /    /'                         /                                  \\                         '\\  \\\n");
    printf(COLOR_RESET);
    Sleep(120);

    // ÇERÇEVE 4: Tam Sessizlik ve Boşluk
    clear_screen();
    Sleep(800);

    // Kaosun içine (1. Sınava) doğrudan atlayış
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
    int prev_choice;  // c1'i tutar (Sahne 1)
    int choice_2;     // c2'yi tutar (Sahne 2)
    int choice_3 = 0; // c3'ü tutar (Sahne 3 ve 4 reaksiyonları)

    clear_screen();

    // ========================================================================
    // SAHNE I: KAOSUN İÇİNDE UYANIŞ (c1)
    // ========================================================================
    const char* q1_en[] = {
        "Grab the thick ropes of the main mast, brace my muscles, and face the impact head-on.",
        "Use the ship's violent tilt to slide directly into the pitch-black lower hatch.",
        "Drop to my knees, open my arms wide, and surrender to the chaotic will of the ocean."
    };
    const char* q1_tr[] = {
        "Ana direğin kalın halatlarına kilitlenip, kaslarımı olanca gücümle sıkarak dalgayı göğüslerim.",
        "Geminin şiddetli devrilme açısından faydalanıp, alt ambarın zifiri karanlığına doğru hızla kayarım.",
        "Dizlerimin üzerine çöküp kollarımı iki yana açar, okyanusun bu kaotik iradesine kendimi tamamen teslim ederim."
    };

    if (current_lang == 1) {
        printf(COLOR_GOLD "\n [SAHNE I] Uyanış\n\n" COLOR_RESET);
        printf(COLOR_WHITE " Yüzüne şarapnel gibi çarpan dondurucu, tuzlu suyla gözlerini açıyorsun. Ciğerlerin yanıyor.\n");
        printf(" Çatırdıyan, her an ikiye bölünmek üzere olan devasa bir ahşap güvertedesin.\n\n");
        printf(COLOR_DARK " Denizin tam ortasında, gök kubbeyi paramparça eden kapkara bir silüet dikiliyor.\n");
        printf(" Bu sadece bir canavar değil; etrafındaki tüm ışığı yutan, suları kaynatan antik bir dehşet.\n");
        printf(" Tepesinde ise göğü yırtarak inen, kör edici altın ışıklar saçan efsanevi bir figür...\n");
        printf(" Apollon. Güneşin kudretiyle karanlığın üzerine mermi gibi dalıyor.\n\n" COLOR_RESET);
        printf(COLOR_WHITE " Ancak bu ilahi savaşı idrak etmeye vaktin yok. Titanların varlığı okyanusu yerinden oynatıyor.\n");
        printf(" Dağ büyüklüğünde, kapkara bir dalga gemiyi tek lokmada yutmak, tam üzerine kırılmak üzere!\n\n" COLOR_RESET);

        for (int i = 0; i < 3; i++) printf("  [" COLOR_CYAN "%d" COLOR_RESET "] %s\n", i + 1, q1_tr[i]);
        printf(COLOR_CYAN "\n  İlk içgüdün nedir? (1-3): " COLOR_RESET);
    } else {
        printf(COLOR_GOLD "\n [SCENE I] The Awakening\n\n" COLOR_RESET);
        printf(COLOR_WHITE " You open your eyes to freezing saltwater striking your face like shrapnel. Your lungs burn.\n");
        printf(" You are on a violently creaking wooden deck, on the verge of splitting in half.\n\n");
        printf(COLOR_DARK " In the middle of the sea stands a pitch-black silhouette shattering the sky vault.\n");
        printf(" This is not just a monster; it is an ancient terror that swallows all light around it.\n");
        printf(" Above it, a legendary figure radiating blinding golden light descends, tearing through the sky...\n");
        printf(" Apollo. He dives like a missile onto the darkness with the might of the sun.\n\n" COLOR_RESET);
        printf(COLOR_WHITE " But you have no time to comprehend this divine war. The titans' presence displaces the ocean.\n");
        printf(" A mountain-sized, pitch-black wave is about to swallow the ship, crashing right on top of you!\n\n" COLOR_RESET);

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

    // Sahne 1 Stat Uygulaması (+3 / -1)
    if (choice == 1) {
        update_stat(profile, STAT_MIGHT, 3);
        update_stat(profile, STAT_INTEL, -1);
    }
    else if (choice == 2) {
        update_stat(profile, STAT_INTEL, 3);
        update_stat(profile, STAT_HONOR, -1);
    }
    else if (choice == 3) {
        update_stat(profile, STAT_FAITH, 3);
        update_stat(profile, STAT_MIGHT, -1);
    }
    prev_choice = choice;

    // ========================================================================
    // TRIAL 2: SHRAPNEL OF LIGHT (DYNAMIC BRANCHING)
    // ========================================================================
    clear_screen();

    const char* q2_set1_en[] = {
        "Quickly move behind the mast, lean my back against the wood, and brace my feet for the incoming shockwave.",
        "Grab a loose, thick sail rope nearby and jump away from the mast milliseconds before the arrow strikes.",
        "Stare directly into the blinding divine light, believing this stray cosmic bullet will not hit me, and stand my ground."
    };
    const char* q2_set1_tr[] = {
        "Direğin arkasına hızla geçip sırtımı ahşaba dayar, ayaklarımı güverteye çivileyerek yaklaşan sarsıntıyı beklerim.",
        "Yakınımdaki boşta sallanan kalın bir yelken halatını kapıp, ok direğe çarpmadan milisaniyeler önce direkten uzağa atlarım.",
        "Gözlerimi kamaştıran o ilahi ışığa dimdik bakıp, tanrıların bu kör kurşununun bana çarpmayacağına inanarak olduğum yerde kalırım."
    };

    const char* q2_set2_en[] = {
        "Hearing the wood shatter, I succumb to an instinctive panic reflex, blindly diving and rolling to the side in the dark.",
        "Focusing purely on my hearing, I pinpoint exactly where the ceiling burst and crawl in the opposite direction with calculated precision.",
        "Dropping to my knees, I shield my head tightly with my arms, bracing rigidly for the massive deck debris to collapse on me."
    };
    const char* q2_set2_tr[] = {
        "Ahşabın kırıldığını duyduğum an içgüdüsel bir panik refleksine kapılır, karanlıkta körlemesine yana doğru atılıp yuvarlanırım.",
        "Kulaklarıma odaklanıp, tavanın tam olarak nereden patladığını sesinden tespit ederek milimetrik bir hesapla zıt yöne doğru sürünürüm.",
        "Diz çöküp kollarımı başıma sıkıca siper eder, parçalanan güvertenin devasa enkazının üzerime çökmesini kaskatı beklerim."
    };

    const char* q2_set3_en[] = {
        "Using my low center of gravity, I rapidly slide across the soaked wood, throwing myself out of the deadly blast zone.",
        "Proclaiming, 'If death descends from the sky, it must be met head-on,' I wait sitting perfectly upright and rigid.",
        "Letting the divine light blind me, I remain completely motionless, trusting that this cosmic arrow will ignore a mere mortal like me."
    };
    const char* q2_set3_tr[] = {
        "Düşük ağırlık merkezimi kullanıp, sırılsıklam ahşap üzerinde hızla kayarak kendimi o ölümcül patlama alanının dışına atarım.",
        "\"Ölüm gökten iniyorsa saklanarak değil, yüzleşerek karşılanmalı,\" diyerek oturduğum yerde başım dik, kaskatı beklerim.",
        "İlahi ışığın gözümü almasına izin verip, bu kozmik okun benim gibi basit bir faniyi umursamayacağına inancımla yerimden hiç kıpırdamam."
    };

    if (current_lang == 1) {
        printf(COLOR_GOLD "\n [SAHNE II] Işığın Şarapneli\n\n" COLOR_RESET);
        if (prev_choice == 1) {
            printf(COLOR_WHITE " Dalganın o ezici ağırlığı bedeninden sıyrılıp okyanusa geri dökülürken, kollarındaki\n");
            printf(" kasların yırtılacak noktaya geldiğini hissediyorsun. Avuçlarından sızan kan tuzlu suya karışıyor.\n\n");
            printf(COLOR_DARK " Zar zor başını kaldırıp solundaki cehenneme bakıyorsun. Apollon, karanlık anomaliye ardı ardına\n");
            printf(" altından oklar kusuyor. Ancak bu oklar canavarın derisinden mermi gibi sekip denize çakılıyor ve\n");
            printf(" suları sağır edici patlamalarla göğe fırlatıyor.\n\n");
            printf(COLOR_WHITE " Ve o an... Havayı cızırdatan o ilahi oklardan birinin canavardan sekerek doğrudan sana,\n");
            printf(" tutunduğun direğe doğru uçtuğunu fark ediyorsun. Zaman adeta yavaşlıyor...\n\n" COLOR_RESET);
            for (int i = 0; i < 3; i++) printf("  [" COLOR_CYAN "%d" COLOR_RESET "] %s\n", i + 1, q2_set1_tr[i]);
        }
        else if (prev_choice == 2) {
            printf(COLOR_WHITE " Devasa dalga gemiye çarptığında, ambar kapağının aralığından içeriye şelale gibi dondurucu\n");
            printf(" sular boşalıyor. Dalganın amansız şiddeti gemiyi bir beşik gibi savururken, zifiri karanlık\n");
            printf(" ambarın içinde oradan oraya sürüklenip ahşap sandıklara çarpıyorsun.\n\n");
            printf(COLOR_DARK " Sersemlemiş, ciğerleri su dolmuş bir halde kapkaranlık ve sırılsıklam zeminde zar zor doğruluyorsun.\n");
            printf(" Yukarıda kıyamet kopuyor. Suları sarsan, kemiklerini titreten sağır edici patlama sesleri adeta kafanın\n");
            printf(" içinde yankılanıyor. Geminin devasa ahşap gövdesi, sonunu hisseden bir canavar gibi acı içinde inliyor.\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Aniden... Karanlığı bıçak gibi kesen kör edici, yakıcı altın rengi bir ışık hüzmesi ambarın\n");
            printf(" kalın tavanını paramparça ediyor! Apollon'un seken ilahi oku, doğrudan senin bulunduğun karanlık\n");
            printf(" mahzene, tam tependen içeri giriyor!\n\n" COLOR_RESET);
            for (int i = 0; i < 3; i++) printf("  [" COLOR_CYAN "%d" COLOR_RESET "] %s\n", i + 1, q2_set2_tr[i]);
        }
        else if (prev_choice == 3) {
            printf(COLOR_WHITE " Dalganın amansız şiddeti seni ahşap güvertede bir bez bebek gibi acımasızca sürükledi\n");
            printf(" ve sırtını ana direğe sertçe çarptın. Omurganın sızladığını hissediyorsun. Oturduğun yerden,\n");
            printf(" nefes nefese ve çaresiz bir halde solundaki titanların savaşını izliyorsun.\n\n");
            printf(COLOR_DARK " Apollon'un öfkeyle fırlattığı oklar canavardan sekmeye başlıyor! Altın oklar okyanusa düştükçe\n");
            printf(" suları nükleer bir bomba gibi patlatıp gökyüzüne devasa su sütunları fırlatıyor.\n\n" COLOR_RESET);
            printf(COLOR_WHITE " O sırada seken kör edici oklardan birinin, havayı yakarak ve etrafındaki yağmur damlalarını\n");
            printf(" buharlaştırarak tam bulunduğun yere, arkana yaslandığın direğe doğru geldiğini görüyorsun!\n\n" COLOR_RESET);
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

    // Trial 2 Stat Application (Cascade Sistemi: +2 / -1)
    if (prev_choice == 1) {
        if (choice == 1) { update_stat(profile, STAT_HONOR, 2); update_stat(profile, STAT_SKILL, -1); }
        else if (choice == 2) { update_stat(profile, STAT_SKILL, 2); update_stat(profile, STAT_FAITH, -1); }
        else if (choice == 3) { update_stat(profile, STAT_FAITH, 2); update_stat(profile, STAT_INTEL, -1); }
    } else if (prev_choice == 2) {
        if (choice == 1) { update_stat(profile, STAT_SKILL, 2); update_stat(profile, STAT_INTEL, -1); }
        else if (choice == 2) { update_stat(profile, STAT_INTEL, 2); update_stat(profile, STAT_MIGHT, -1); }
        else if (choice == 3) { update_stat(profile, STAT_MIGHT, 2); update_stat(profile, STAT_SKILL, -1); }
    } else if (prev_choice == 3) {
        if (choice == 1) { update_stat(profile, STAT_SKILL, 2); update_stat(profile, STAT_HONOR, -1); }
        else if (choice == 2) { update_stat(profile, STAT_HONOR, 2); update_stat(profile, STAT_INTEL, -1); }
        else if (choice == 3) { update_stat(profile, STAT_FAITH, 2); update_stat(profile, STAT_MIGHT, -1); }
    }
    choice_2 = choice;

    // ========================================================================
    // TRIAL 3 & 4: INDIVIDUAL DESTINIES AND REACTIONS
    // ========================================================================
    clear_screen();

    // ------------------------------------------------------------------------
    // PATH 1.1: İLAHİ KİLİTLENME (Direkte direnenlerin kaderi)
    // ------------------------------------------------------------------------
    if (prev_choice == 1 && choice_2 == 1) {
        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE III] İlahi Kilitlenme\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Kaba kuvvetin işe yaradı! O sarsıntıya rağmen ayakların güverteye kenetlendi.\n");
            printf(" Kozmik ok gövdeye değil, tam üstündeki devasa ana yelkene saplanıp direği ortadan ikiye yarıyor.\n\n");
            printf(COLOR_DARK " Tonlarca ağırlıktaki yelkenin ağır çekimde devrilmesini, küpeşteyi paramparça ederek\n");
            printf(" denize çakılmasını izliyorsun. Bu dehşet verici çarpışmanın sarsıntısıyla ayakların\n");
            printf(" yerden kesiliyor, havaya savruluyorsun.\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Tam o an, okyanusun karanlık derinliklerinden fırlayan, güneş kadar parlak devasa\n");
            printf(" bir mızrak (Trident) suları ortadan ikiye yararak hemen üzerinden mermi gibi göğe uçuyor!\n\n");
            printf(COLOR_CYAN " Sarsıntıyla dizlerinin üzerine sertçe düştüğün o saniyede, okyanus kabarıyor.\n");
            printf(" Devasa bir figür (Poseidon) suları yararak geminin hemen yanından usulca yükseliyor.\n");
            printf(" O muazzam, kayıtsız yüzü güverte hizasından geçerken aniden yavaşlıyor ve göz göze\n");
            printf(" geliyorsunuz. O ilahi bakışmanın ezici ağırlığı altında tamamen felç oluyorsun...\n\n" COLOR_RESET);
            printf(COLOR_CYAN " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n [SCENE III] Divine Paralysis\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Your brute force worked! Despite the shockwave, your feet remain glued to the deck.\n");
            printf(" The cosmic arrow strikes the massive main sail above you, splitting the mast in half.\n\n");
            printf(COLOR_DARK " You watch the multi-ton sail topple in slow motion, shattering the railing and\n");
            printf(" crashing into the sea. The terrifying impact of this collision lifts your feet\n");
            printf(" off the ground, launching you into the air.\n\n" COLOR_RESET);
            printf(COLOR_WHITE " At that exact moment, a colossal spear (Trident) as bright as the sun shoots from\n");
            printf(" the dark depths, tearing the ocean in half and flying right over you like a missile!\n\n");
            printf(COLOR_CYAN " As you land hard on your knees, the ocean swells. A colossal figure (Poseidon)\n");
            printf(" rises smoothly right beside the ship. As his massive, indifferent face passes the\n");
            printf(" deck level, he slows down. Your eyes lock. You are completely paralyzed under the\n");
            printf(" crushing weight of that divine gaze...\n\n" COLOR_RESET);
            printf(COLOR_CYAN " [Press ANY KEY to continue]\n" COLOR_RESET);
        }
        _getch(); clear_screen();

        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE IV] İlahi Yargı\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Sular çıldırmışçasına çalkalanıyor. Dizlerinin üzerindesin. Okyanusun hakimi, dipsiz kuyu\n");
            printf(" gibi gözleriyle sana tepeden bakıyor. Seni kurtarmak için parmağını bile kıpırdatmıyor,\n");
            printf(" sadece bu fani karıncanın yaklaşan ölümünü izliyor.\n\n" COLOR_RESET);

            printf("  [" COLOR_CYAN "1" COLOR_RESET "] Bu devasa ihtişam ve okyanusun ezici gücü karşısında istemsizce dizlerimin üzerine çöker, başımı eğerim.\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] İliklerime kadar donarken, tanrıların bu kibirli umursamazlığına karşı içimde saf, yakıcı bir öfke büyür.\n");
            printf("  [" COLOR_CYAN "3" COLOR_RESET "] Bedenimi kilitleyen bu ilahi baskıyı irademle parçalar, ayağa kalkıp o devasa gözlerin içine dimdik bakarım.\n\n");
            printf(COLOR_CYAN "  Tepkin nedir? (1-3): " COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n [SCENE IV] Divine Judgment\n\n" COLOR_RESET);
            printf(COLOR_WHITE " The waters churn madly. You are on your knees. The lord of the ocean looks down upon you\n");
            printf(" with eyes like bottomless wells. He doesn't lift a finger to save you, merely watching\n");
            printf(" the impending death of a mortal ant.\n\n" COLOR_RESET);

            printf("  [" COLOR_CYAN "1" COLOR_RESET "] I involuntarily drop to my knees and bow my head before this colossal majesty and crushing power.\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] Freezing to my core, a pure, burning rage grows within me against this arrogant divine indifference.\n");
            printf("  [" COLOR_CYAN "3" COLOR_RESET "] I shatter this divine paralysis with my own will, stand up, and stare directly into those massive eyes.\n\n");
            printf(COLOR_CYAN "  What is your reaction? (1-3): " COLOR_RESET);
        }
        while (1) {
            if (_kbhit()) {
                char ch = _getch();
                if (ch == '1') { update_stat(profile, STAT_FAITH, 3); update_stat(profile, STAT_HONOR, -1); choice_3 = 1; break; }
                else if (ch == '2') { update_stat(profile, STAT_MIGHT, 3); update_stat(profile, STAT_FAITH, -1); choice_3 = 2; break; }
                else if (ch == '3') { update_stat(profile, STAT_HONOR, 3); update_stat(profile, STAT_SKILL, -1); choice_3 = 3; break; }
            }
            Sleep(20);
        }
    }
    // ------------------------------------------------------------------------
    // PATH 1.2: DİPSİZ UÇURUM (Mini-Game ve Boğulanların kaderi)
    // ------------------------------------------------------------------------
    else if (prev_choice == 1 && choice_2 == 2) {
        if (current_lang == 1) {
             printf(COLOR_GOLD "\n [SAHNE III] Dipsiz Uçurum\n\n" COLOR_RESET);
             printf(COLOR_WHITE " Halatla güvenli bir yere savrulmayı planlıyordun, ancak kozmik ok ana yelkeni\n");
             printf(" paramparça edip gemiyi ölümcül bir açıyla yana yatırınca momentumunu kaybettin.\n");
             printf(" Havada hızını alamayıp, tutunamadan doğrudan okyanusun dondurucu sularına uçtun!\n\n");
             printf(COLOR_DARK " Suyun altına gömülüyorsun... Derine... Daha derine...\n");
             printf(" Yüzeydeki savaşın sesleri uğultuya dönüşüyor...\n\n" COLOR_RESET);
             printf(COLOR_RED " [HAYATTA KALMAK İÇİN BOŞLUK (SPACE) TUŞUNA ART ARDA BAS!]\n");
             printf(" Başlamak için Space'e bas...\n" COLOR_RESET);
        } else {
             printf(COLOR_GOLD "\n [SCENE III] The Abyssal Chasm\n\n" COLOR_RESET);
             printf(COLOR_WHITE " You planned to swing to safety, but the cosmic arrow shattered the main sail,\n");
             printf(" violently tilting the ship at a fatal angle. You lost your momentum.\n");
             printf(" Unable to hold on, you flew straight into the freezing ocean waters!\n\n");
             printf(COLOR_DARK " You are sinking... Deeper... And deeper...\n");
             printf(" The sounds of the surface war turn into a muffled hum...\n\n" COLOR_RESET);
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
                if (ch == ' ') { oxygen += 6.0f; if (oxygen > 100.0f) oxygen = 100.0f; }
            }
            oxygen -= depletion_rate;
            if (ticks % 10 == 0) depletion_rate += 0.15f;
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
        // Mini game stat cezaları KALDIRILDI! Saf hikaye deneyimi.

        if (current_lang == 1) {
            printf(COLOR_DARK "\n\n  Oksijenin tamamen tükendi...\n");
            printf("  Su basıncı ciğerlerini eziyor, bilincin yavaşça kapanıyor.\n\n");
            printf("  Gözlerin karanlığa teslim olmadan hemen önce, suyun yüzeyinde devasa bir çarpışmanın\n");
            printf("  şok dalgasını hissediyorsun. Okyanusun dibinden devasa, parlayan bir mızrak (Trident)\n");
            printf("  yanından mermi gibi geçip yüzeye fırlıyor ve saniyeler sonra aynı hızla geri dönüyor!\n\n");
            printf("  Hemen ardından, Poseidon'un devasa silüeti suları yararak yüzeye, karanlık canavarın\n");
            printf("  üzerine doğru atılıyor! Su yüzeyinde üç devasa çizik açıldığını ve okyanusa parlayan,\n");
            printf("  altın rengi tanrı kanı (İkor) döküldüğünü görüyorsun...\n");
            printf("  Altın damlalar karanlık suları aydınlatırken, her şey zifiri karanlığa bürünüyor...\n\n" COLOR_RESET);
            Sleep(2500);
            printf(COLOR_CYAN "  [ Zihninin derinliklerinde, suların çok altından gelen, yankılı, kadim bir ses duyuluyor ]\n\n" COLOR_RESET);
            printf(COLOR_MAG "  \"Boğuluyorsun galiba...\"\n\n" COLOR_RESET);
            printf(COLOR_DARK " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
        } else {
            printf(COLOR_DARK "\n\n  Your oxygen is completely depleted...\n");
            printf("  Water pressure crushes your lungs, your consciousness slowly fades.\n\n");
            printf("  Right before your eyes surrender to the darkness, you feel the shockwave of a massive\n");
            printf("  collision on the surface. From the ocean floor, a colossal, glowing spear (Trident)\n");
            printf("  shoots past you like a missile to the surface, returning just as fast seconds later!\n\n");
            printf("  Immediately after, Poseidon's massive silhouette tears through the waters, lunging towards\n");
            printf("  the surface at the dark monster! You see three colossal slashes open above, and glowing,\n");
            printf("  golden god blood (Ichor) spills into the ocean...\n");
            printf("  As the golden drops illuminate the dark waters, everything fades to pitch black...\n\n" COLOR_RESET);
            Sleep(2500);
            printf(COLOR_CYAN "  [ Deep within your mind, an echoing, ancient voice from beneath the waters resonates ]\n\n" COLOR_RESET);
            printf(COLOR_MAG "  \"You seem to be drowning...\"\n\n" COLOR_RESET);
            printf(COLOR_DARK " [Press ANY KEY to continue]\n" COLOR_RESET);   }
        _getch(); clear_screen();

        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE IV] Hiçliğin Kıyısı\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Zihnin karanlığa gömülmek üzere. Bedenini hissetmiyorsun, sadece o yankılı, kadim sesi duyuyorsun.\n");
            printf(" Dipsiz hiçliğe karışmadan saniyeler önceki son düşüncen nedir?\n\n" COLOR_RESET);
            printf("  [" COLOR_CYAN "1" COLOR_RESET "] Karanlığın içinde yankılanan sese, \"Burada ölmeyi reddediyorum!\" diye haykırır ve debelenmeye başlarım.\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] Zihnimi serbest bırakır, \"Beni bu karanlıktan çıkar, ruhum senindir...\" diye fısıldarım.\n");
            printf("  [" COLOR_CYAN "3" COLOR_RESET "] Son nefesimi tüketirken bile mantığımı korumaya çalışır, sese \"Sen kimsin?\" diye sorarım.\n\n");
            printf(COLOR_CYAN "  Son düşüncen (1-3): " COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n [SCENE IV] Edge of the Void\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Your mind is about to sink into darkness. You can't feel your body, only that echoing voice.\n");
            printf(" What is your final thought seconds before blending into the bottomless void?\n\n" COLOR_RESET);
            printf("  [" COLOR_CYAN "1" COLOR_RESET "] I roar at the voice echoing in the dark, \"I refuse to die here!\" and start struggling.\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] I release my mind, whispering, \"Pull me from this dark, my soul is yours...\"\n");
            printf("  [" COLOR_CYAN "3" COLOR_RESET "] Maintaining my logic even as I consume my last breath, I ask the voice, \"Who are you?\"\n\n");
            printf(COLOR_CYAN "  Final thought (1-3): " COLOR_RESET);
        }
        while (1) {
            if (_kbhit()) {
                char ch = _getch();
                if (ch == '1') { update_stat(profile, STAT_MIGHT, 3); update_stat(profile, STAT_INTEL, -1); choice_3 = 1; break; }
                else if (ch == '2') { update_stat(profile, STAT_FAITH, 3); update_stat(profile, STAT_HONOR, -1); choice_3 = 2; break; }
                else if (ch == '3') { update_stat(profile, STAT_INTEL, 3); update_stat(profile, STAT_FAITH, -1); choice_3 = 3; break; }
            }
            Sleep(20);
        }
    }
    // ------------------------------------------------------------------------
    // PATH 2.X: KARANLIKTAKİ YANKI (Ambarda hayatta kalanların kaderi)
    // ------------------------------------------------------------------------
    else if (prev_choice == 2) {
        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE III] Karanlıktaki Yankı\n\n" COLOR_RESET);
            printf(COLOR_WHITE " O sağır edici çatırdama sesleriyle beklediğin yıkıcı son, ambarın alt katına ulaşmıyor.\n");
            printf(" Mucizevi bir şekilde tavan senin olduğun bölgeye çökmedi. Zifiri karanlık ambarın\n");
            printf(" sularla kaplı zemininde, etrafında kraliyet ganimetleriyle dolu kapalı ahşap\n");
            printf(" sandıkların arasında güvendesin.\n\n");
            printf(COLOR_DARK " Ancak yukarıdaki savaş giderek şiddetleniyor. Suları yaran devasa bir gürleme\n");
            printf(" ve birbirine çarpan ilahi silahların yarattığı şok dalgası, geminin ahşap gövdesini\n");
            printf(" bir tabut gibi titretiyor.\n\n" COLOR_RESET);
            printf(COLOR_CYAN " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n [SCENE III] Echoes in the Dark\n\n" COLOR_RESET);
            printf(COLOR_WHITE " The devastating end you braced for never reaches the lower hold.\n");
            printf(" Miraculously, the ceiling did not collapse on your section. You are safe in the\n");
            printf(" pitch-black, water-covered hold, surrounded by sealed royal loot crates.\n\n");
            printf(COLOR_DARK " But the battle above is escalating wildly. A deafening roar that splits the waters\n");
            printf(" and the shockwave of clashing divine weapons rattle the ship's wooden hull like a coffin.\n\n" COLOR_RESET);
            printf(COLOR_CYAN " [Press ANY KEY to continue]\n" COLOR_RESET);
        }
        _getch(); clear_screen();

        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE IV] Tabuttaki Karar\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Gemi adeta sulara gömülmeyi bekleyen yüzen bir tabuta dönüştü. Savaş yukarıda tüm\n");
            printf(" dehşetiyle, gökleri yararak sürüyor.\n\n" COLOR_RESET);
            printf("  [" COLOR_CYAN "1" COLOR_RESET "] Ambar kapağını tekmeyle parçalar, bu korkakça bekleyişe son verip kıyametle yüzleşmek için güverteye çıkarım.\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] Yukarıdaki kaos tamamen dinene kadar nefesimi bile kontrol ederek karanlıkta, güvenli bir şekilde beklerim.\n");
            printf("  [" COLOR_CYAN "3" COLOR_RESET "] Bu kaosu bir fırsat bilip, zifiri karanlıkta sandıkları parçalayarak kutsal veya büyülü bir emanet ararım.\n\n");
            printf(COLOR_CYAN "  Kararın nedir? (1-3): " COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n [SCENE IV] Decision in the Coffin\n\n" COLOR_RESET);
            printf(COLOR_WHITE " The ship has become a floating coffin waiting to sink. The war rages above in all\n");
            printf(" its terror, tearing the skies apart.\n\n" COLOR_RESET);
            printf("  [" COLOR_CYAN "1" COLOR_RESET "] I kick the hatch open, ending this cowardly wait, and go to the deck to face the apocalypse.\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] I wait securely in the dark, controlling even my breath, until the chaos above completely subsides.\n");
            printf("  [" COLOR_CYAN "3" COLOR_RESET "] I use this chaos as an opportunity, blindly smashing crates in the dark to search for a divine artifact.\n\n");
            printf(COLOR_CYAN "  What is your decision? (1-3): " COLOR_RESET);
        }
        while (1) {
            if (_kbhit()) {
                char ch = _getch();
                if (ch == '1') { update_stat(profile, STAT_HONOR, 3); update_stat(profile, STAT_INTEL, -1); choice_3 = 1; break; }
                else if (ch == '2') { update_stat(profile, STAT_INTEL, 3); update_stat(profile, STAT_MIGHT, -1); choice_3 = 2; break; }
                else if (ch == '3') { update_stat(profile, STAT_SKILL, 3); update_stat(profile, STAT_HONOR, -1); choice_3 = 3; break; }
            }
            Sleep(20);
        }
    }
    // ------------------------------------------------------------------------
    // PATH 3.1: UÇURUMUN ÇEKİMİ (Havaya fırlayanların kaderi)
    // ------------------------------------------------------------------------
    else if (prev_choice == 3 && choice_2 == 1) {
        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE III] Uçurumun Çekimi\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Islak güvertede kontrollü bir şekilde kayarken kozmik ok ana yelkeni parçalıyor\n");
            printf(" ve gemi şiddetle yana yatıyor. Kayışını mükemmel sürdürüp geminin köşesine çarpıyor\n");
            printf(" ve kırık tırabzanlara tutunmayı başarıyorsun.\n\n");
            printf(COLOR_DARK " Ancak tam o saniyede, okyanusun karanlık derinliklerinden fırlayan devasa, parlayan\n");
            printf(" bir mızrak (Trident) suları yararak hemen arkandan mermi gibi göğe uçuyor!\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Bu fırlayışın yarattığı devasa, sonik şok dalgası seni yerinden söküyor ve bir paçavra\n");
            printf(" gibi metrelerce havaya fırlatıyor! Havadayken dönerek süzüldüğün o birkaç saniyede,\n");
            printf(" Poseidon'un devasa, ihtişamlı silüetini hemen altında, okyanustan yükselirken görüyorsun.\n\n" COLOR_RESET);
            printf(COLOR_CYAN " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n [SCENE III] Pull of the Abyss\n\n" COLOR_RESET);
            printf(COLOR_WHITE " As you slide on the wet deck, the arrow shatters the sail and the ship violently tilts.\n");
            printf(" You execute your slide perfectly, slamming into the corner and gripping the broken railing.\n\n");
            printf(COLOR_DARK " But at that exact second, a colossal glowing spear (Trident) shoots from the dark depths,\n");
            printf(" tearing through the water right behind you like a missile!\n\n" COLOR_RESET);
            printf(COLOR_WHITE " The massive sonic shockwave from its launch rips you from your spot, launching you\n");
            printf(" meters into the air like a ragdoll! Suspended in mid-air for those few seconds,\n");
            printf(" you see Poseidon's majestic silhouette rising from the ocean directly below you.\n\n" COLOR_RESET);
            printf(COLOR_CYAN " [Press ANY KEY to continue]\n" COLOR_RESET);
        }
        _getch(); clear_screen();

        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE IV] Havada Asılı\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Saniyeler içinde metrelerce yüksekten o ölümcül sulara doğru düşeceksin. Altında bir\n");
            printf(" tanrı, arkanda ise yavaş çekimde paramparça olan bir gemi var.\n\n" COLOR_RESET);
            printf("  [" COLOR_CYAN "1" COLOR_RESET "] Havadayken vücudumu inanılmaz bir esneklikle çevirip, suya çakılmamak için tekrar geminin kırık tırabzanlarına atlarım.\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] Gözlerimi kapatıp yerçekiminin beni çekmesini kabullenir, kaderimi Poseidon'un merhametine bırakırım.\n");
            printf("  [" COLOR_CYAN "3" COLOR_RESET "] Ölümcül düşüşe geçerken korkmak yerine, var gücümle altımdaki o devasa tanrıya doğru kükrerim!\n\n");
            printf(COLOR_CYAN "  Hamlen nedir? (1-3): " COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n [SCENE IV] Suspended in Air\n\n" COLOR_RESET);
            printf(COLOR_WHITE " You will hit the deadly waters from meters high in seconds. A god is below you,\n");
            printf(" a shattering ship in slow motion is behind you.\n\n" COLOR_RESET);
            printf("  [" COLOR_CYAN "1" COLOR_RESET "] I twist my body mid-air with incredible flexibility to frantically grab the broken railing before hitting the water.\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] I close my eyes, accepting the pull of gravity, and leave my fate to Poseidon's mercy.\n");
            printf("  [" COLOR_CYAN "3" COLOR_RESET "] Instead of fearing the fatal fall, I roar with all my might at the colossal god below me!\n\n");
            printf(COLOR_CYAN "  What is your move? (1-3): " COLOR_RESET);
        }
        while (1) {
            if (_kbhit()) {
                char ch = _getch();
                if (ch == '1') { update_stat(profile, STAT_SKILL, 3); update_stat(profile, STAT_FAITH, -1); choice_3 = 1; break; }
                else if (ch == '2') { update_stat(profile, STAT_FAITH, 3); update_stat(profile, STAT_MIGHT, -1); choice_3 = 2; break; }
                else if (ch == '3') { update_stat(profile, STAT_MIGHT, 3); update_stat(profile, STAT_INTEL, -1); choice_3 = 3; break; }
            }
            Sleep(20);
        }
    }
    // ------------------------------------------------------------------------
    // PATH 1.3, 3.2, 3.3: TANRILARIN ÇARPIŞMASI (İzleyicilerin kaderi)
    // ------------------------------------------------------------------------
    else if ((prev_choice == 1 && choice_2 == 3) || (prev_choice == 3 && (choice_2 == 2 || choice_2 == 3))) {
        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE III] Tanrıların Çarpışması\n\n" COLOR_RESET);
            printf(COLOR_WHITE " İraden o kadar kesindi ki, ilahi ok sana veya tutunduğun direğe değil, tam üstündeki\n");
            printf(" ana yelkene saplanıp direği ortadan ikiye yarıyor.\n\n");
            if (prev_choice == 1) {
                printf(COLOR_DARK " Direğe tutunan ellerin kayıyor ve sarsıntının devasa şiddetiyle dizlerinin üzerine çöküyorsun.\n\n" COLOR_RESET);
            } else {
                printf(COLOR_DARK " O büyük sarsıntıya rağmen oturduğun yerden kıpırdamıyorsun. Güvendesin, sadece izliyorsun.\n\n" COLOR_RESET);
            }
            printf(COLOR_WHITE " Devasa yelkenin ağır çekimde devrilip denize çakılmasını izlerken,\n");
            printf(" okyanusun derinliklerinden fırlayan güneş kadar parlak bir mızrak (Trident)\n");
            printf(" suları yararak mermi gibi göğe uçuyor.\n\n");
            printf(COLOR_CYAN " Ardından Poseidon'un muazzam silüeti okyanustan usulca yükselip gökyüzündeki Apollon'a bakıyor.\n");
            printf(" Sen, dünyayı yok edebilecek bu iki ilahi gücün savaşının ortasında, sadece bir izleyicisin.\n\n" COLOR_RESET);
            printf(COLOR_CYAN " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n [SCENE III] Clash of Gods\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Your resolve was so absolute that the divine arrow doesn't hit you, but strikes the\n");
            printf(" main sail above you, shattering the mast in half.\n\n");
            if (prev_choice == 1) {
                printf(COLOR_DARK " Your hands slip from the mast and the massive shockwave drops you to your knees.\n\n" COLOR_RESET);
            } else {
                printf(COLOR_DARK " Despite the shockwave, you do not move from your seated position. You are watching.\n\n" COLOR_RESET);
            }
            printf(COLOR_WHITE " As you watch the massive sail topple into the sea in slow motion,\n");
            printf(" a colossal spear (Trident) bright as the sun shoots from the depths, tearing into the sky.\n\n");
            printf(COLOR_CYAN " Then, Poseidon's magnificent silhouette smoothly rises from the ocean, glaring up at Apollo.\n");
            printf(" In the midst of this clash of divine powers that could end the world, you are just a spectator.\n\n" COLOR_RESET);
            printf(COLOR_CYAN " [Press ANY KEY to continue]\n" COLOR_RESET);
        }
        _getch(); clear_screen();

        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE IV] Karıncanın Bakışı\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Güvertede hareketsiz kaldın. Apollon gökyüzünden alevler içinde dalışa geçiyor,\n");
            printf(" Poseidon ise tüm okyanusu göğe doğru yükseltiyor.\n");
            printf(" Bu muazzam kozmik savaşı izleyen basit bir fani olarak aklından geçen ilk düşünce nedir?\n\n" COLOR_RESET);
            printf("  [" COLOR_CYAN "1" COLOR_RESET "] Gözlerimi kısmadan izlerken, \"Bir gün ben de gökleri ve denizleri sarsacak bu güce ulaşacağım,\" diye yemin ederim.\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] Tanrıların savaşında bir karınca olduğumu kabullenir, \"Sadece hayatta kalıp bu destanı anlatmalıyım,\" diye düşünürüm.\n");
            printf("  [" COLOR_CYAN "3" COLOR_RESET "] Bu kozmik terazi karşısında küçülür, \"Kim kazanırsa kazansın, onun iradesine boyun eğeceğim,\" derim.\n\n");
            printf(COLOR_CYAN "  Düşüncen nedir? (1-3): " COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n [SCENE IV] Gaze of the Ant\n\n" COLOR_RESET);
            printf(COLOR_WHITE " You remain motionless on the deck. Apollo dives in flames from the sky,\n");
            printf(" while Poseidon raises the entire ocean towards the heavens.\n");
            printf(" As a mere mortal watching this cosmic war, what is your primary thought?\n\n" COLOR_RESET);
            printf("  [" COLOR_CYAN "1" COLOR_RESET "] Staring unblinking, I vow, \"One day, I too will attain this power to shake the heavens and seas.\"\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] Accepting I am an ant in a war of gods, I think, \"I just need to survive so I can tell this epic tale.\"\n");
            printf("  [" COLOR_CYAN "3" COLOR_RESET "] Shrinking before this cosmic scale, I say, \"Whoever wins, I shall bow to their will.\"\n\n");
            printf(COLOR_CYAN "  What is your thought? (1-3): " COLOR_RESET);
        }
        while (1) {
            if (_kbhit()) {
                char ch = _getch();
                if (ch == '1') { update_stat(profile, STAT_MIGHT, 3); update_stat(profile, STAT_HONOR, -1); choice_3 = 1; break; }
                else if (ch == '2') { update_stat(profile, STAT_INTEL, 3); update_stat(profile, STAT_FAITH, -1); choice_3 = 2; break; }
                else if (ch == '3') { update_stat(profile, STAT_FAITH, 3); update_stat(profile, STAT_SKILL, -1); choice_3 = 3; break; }
            }
            Sleep(20);
        }
    }

    // ========================================================================
    // SAHNE V: KIYAMETİN ZİRVESİ (SİNEMATİKLER VE KADERİN KIRILMASI)
    // ========================================================================
    clear_screen();

    // KUSURSUZ MATRİS KADER KİLİTLERİ
    bool helios_path = false;

    // İzleyici ve Direnenlerin Helios Yolu (1.1, 1.3, 3.2, 3.3)
    if ((prev_choice == 1 && choice_2 == 1 && (choice_3 == 1 || choice_3 == 2)) ||
        (prev_choice == 1 && choice_2 == 3) ||
        (prev_choice == 3 && (choice_2 == 2 || choice_2 == 3))) {
        helios_path = true;
    }
    // Havada asılı kalanların Helios Yolu (3.1.2 ve 3.1.3)
    if (prev_choice == 3 && choice_2 == 1 && choice_3 == 2) helios_path = true;
    if (prev_choice == 3 && choice_2 == 1 && choice_3 == 3) helios_path = true;

    // Poseidon'un Girdabı (1.1.3 - Tanrıya diklenenlerin Vetosu)
    if (prev_choice == 1 && choice_2 == 1 && choice_3 == 3) profile->poseidon_veto = 1;

    // ------------------------------------------------------------------------
    // A - POSEIDON'UN GİRDABI (Veto Yiyenler)
    // ------------------------------------------------------------------------
    if (profile->poseidon_veto == 1) {
        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE V] Kibrin Bedeli\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Poseidon'un o devasa, ruhsuz gözlerinin içine dimdik baktığın an, denizlerin hakimi\n");
            printf(" yukarıdaki kozmik savaşla ilgilenmeden önce sadece sana doğru bakıyor. Öfkelenmiyor bile.\n");
            printf(" Sadece rahatsız edici bir sineği kovar gibi, parmağıyla ufak, umursamaz bir hareket yapıyor.\n\n");
            printf(COLOR_DARK " Aniden, üzerinde durduğun güverte tahtaları un ufak olup hiçliğe karışıyor!\n" COLOR_RESET);
            printf(COLOR_WHITE " Sen ne olduğunu bile anlayamadan, suların tam altında ansızın açılan devasa, kapkara\n");
            printf(" bir girdabın (whirlpool) ölü merkezine çekiliyorsun. Suyun dönüş hızı o kadar şiddetli ki,\n");
            printf(" etrafındaki okyanus adeta beton bir duvara dönüşüyor.\n\n");
            printf(COLOR_DARK " Gökyüzündeki savaşı bir daha asla göremeden, ışığın ulaşamadığı o zifiri karanlığa yutuluyorsun.\n");
            printf(" Girdabın amansız, kemik kıran basıncı seni dibe çekerken, ciğerlerin tuzlu suyla dolup taşıyor\n");
            printf(" ve okyanusun dibinde ezilerek bilincini tamamen kaybediyorsun...\n\n" COLOR_RESET);
            printf(COLOR_CYAN " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n [SCENE V] Price of Hubris\n\n" COLOR_RESET);
            printf(COLOR_WHITE " The moment you stare directly into Poseidon's massive, soulless eyes, the lord of the seas\n");
            printf(" looks down at you before returning to the cosmic war. He isn't even angry.\n");
            printf(" Like swatting a mildly annoying fly, he makes a tiny, dismissive finger movement.\n\n");
            printf(COLOR_DARK " Suddenly, the solid deck planks beneath your feet turn to dust and vanish!\n" COLOR_RESET);
            printf(COLOR_WHITE " Before you can even comprehend it, you are sucked into the dead center of a colossal,\n");
            printf(" pitch-black whirlpool that opens instantly beneath the waves. The water's rotation is so\n");
            printf(" violent that the ocean around you turns into a concrete wall.\n\n");
            printf(COLOR_DARK " Swallowed by the absolute dark where no light reaches, you will never see the sky again.\n");
            printf(" As the bone-crushing pressure drags you down, your lungs overflow with saltwater,\n");
            printf(" and you are crushed at the bottom of the ocean, losing consciousness entirely...\n\n" COLOR_RESET);
            printf(COLOR_CYAN " [Press ANY KEY to continue]\n" COLOR_RESET);
        }
        _getch();
    }
    // ------------------------------------------------------------------------
    // B - TSUNAMİ VE GÜNEŞ ARABASI (Helios Yolundakiler)
    // ------------------------------------------------------------------------
    else if (helios_path) {
        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE V] Kıyametin Kırılma Anı\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Savaşın zirvesi... Denizdeki karanlık canavar, gökyüzünde yorulmaya başlayan\n");
            printf(" Apollon'un üzerine ölümcül bir şiddetle atılıyor. Tam o saniyede suları yaran,\n");
            printf(" ışık hızındaki Trident mermi gibi canavara çarpıyor ve onun devasa atılma ivmesini\n");
            printf(" bıçak gibi kesiyor! Poseidon, okyanusu titreten bir hamleyle mızrağını geri çağırıyor.\n\n");

            printf(COLOR_DARK " O sırada gökyüzü, sanki bir kumaşmış gibi ortadan ikiye yırtılıyor!\n" COLOR_RESET);
            printf(COLOR_WHITE " Güneşin Titanı Helios, bulutların arasından saf, kör edici ilahi bir enerji ışını\n");
            printf(" yolluyor. Ancak karanlık canavar, bu devasa gücü adeta bir karadelik gibi soğuruyor.\n\n");

            printf(COLOR_CYAN " Helios'un saldırısı etkisiz kalınca, Poseidon suların içinden devasa bir gölge\n");
            printf(" gibi fırlayarak bizzat canavarın üzerine atılıyor! Trident'inin ucuyla yaratığın\n");
            printf(" gövdesini boydan boya yaran ince, uzun üç çizgi çekiyor. Karanlık bedenden parlayan\n");
            printf(" altın rengi ikor damlaları okyanusa dökülüyor...\n\n" COLOR_RESET);

            printf(COLOR_WHITE " Apollon kalan son gücüyle yayını geriyor ve tüm enerjisini toplayıp, ikor sızan\n");
            printf(" bu taze yaraları hedefleyen bitirici bir ok yolluyor! Ancak yaratık soğurduğu tüm\n");
            printf(" enerjiyi tam o an dışarı kusuyor. Ok yaraya ulaşamadan şiddetle sekiyor ve okyanusa\n");
            printf(" çakılarak denizi ortadan ikiye yarıyor!\n\n" COLOR_RESET);

            printf(COLOR_DARK " Önce suların çekilmesiyle oluşan o ürkütücü sessizlik...\n");
            printf(" Ardından havayı paramparça eden inanılmaz bir şok dalgası ve ciğerlerini yerinden\n");
            printf(" söken sağır edici bir 'GÜMM!' sesi tüm evrende yankılanıyor!\n\n" COLOR_RESET);

            printf(COLOR_RED " Çarpışmanın merkezinden kopan, gökyüzüne uzanan devasa bir kıyamet dalgası,\n");
            printf(" güneşi bile kapatarak geminin üzerine doğru katlanarak büyüyor!\n\n" COLOR_RESET);

            printf(COLOR_DARK " Devasa dalga gemiyi bir kibrit çöpü gibi yutuyor. Suyun o amansız, ezici gücüyle\n");
            printf(" metrelerce havaya, fırtına bulutlarının içine doğru fırlatılıyorsun!\n\n" COLOR_RESET);
            printf(COLOR_CYAN " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n [SCENE V] The Breaking Point\n\n" COLOR_RESET);
            printf(COLOR_WHITE " The climax of war... The dark monster in the sea lunges with fatal ferocity at\n");
            printf(" an exhausted Apollo. At that exact second, tearing through the waters, the light-speed\n");
            printf(" Trident strikes the beast, halting its massive momentum instantly!\n");
            printf(" With an ocean-shaking motion, Poseidon calls his spear back to his hand.\n\n");

            printf(COLOR_DARK " At that moment, the sky tears in half like fabric!\n" COLOR_RESET);
            printf(COLOR_WHITE " Helios, the Titan of the Sun, sends down a pure, blinding ray of divine energy.\n");
            printf(" But the dark monster absorbs this colossal power like a black hole.\n\n");

            printf(COLOR_CYAN " Seeing Helios's attack fail, Poseidon himself leaps from the waters like a\n");
            printf(" colossal shadow, lunging at the beast! With the tip of his Trident, he slashes\n");
            printf(" three long, thin wounds across its body. Glowing, golden drops of ichor begin\n");
            printf(" to spill into the ocean from the dark flesh...\n\n" COLOR_RESET);

            printf(COLOR_WHITE " Apollo draws his bow with his last ounce of strength, channeling all his energy,\n");
            printf(" and fires a finishing arrow aiming directly at these fresh wounds seeping with ichor!\n");
            printf(" But the creature expels all the absorbed energy at once. The arrow fiercely ricochets\n");
            printf(" before reaching the wound, crashing into the ocean and splitting the sea in half!\n\n" COLOR_RESET);

            printf(COLOR_DARK " First, the eerie silence of the waters drawing back...\n");
            printf(" Then, an incredible shockwave that shatters the air, followed by a deafening 'BOOM!'\n");
            printf(" that tears at your lungs and echoes through the entire universe!\n\n" COLOR_RESET);

            printf(COLOR_RED " From the center of the collision, an apocalyptic, massive wave reaching the heavens\n");
            printf(" rises, blocking out the sun as it folds over the ship!\n\n" COLOR_RESET);

            printf(COLOR_DARK " The giant wave swallows the ship like a matchstick. With the massive, crushing force\n");
            printf(" of the water, you are launched meters into the sky, straight into the storm clouds!\n\n" COLOR_RESET);
            printf(COLOR_CYAN " [Press ANY KEY to continue]\n" COLOR_RESET);
        }
        _getch(); clear_screen();

        // MİNİ OYUN (Hikayesel Yansıma)
        int attack_dir = rand() % 2;
        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE V] Göklerde Hayatta Kalma\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Havada çaresizce, bulutların arasında savrulurken, savaş ortamından alev alev yanan\n");
            printf(" arabasıyla hızla uzaklaşan Helios seni fark ediyor. Hiç düşünmeden arabasını üstüne\n");
            printf(" doğru kırıyor! Tam arabanın sıcak zeminine sertçe düştüğün saniyede, fırtınanın kopardığı\n");
            printf(" devasa bir ahşap gemi direği, mermi hızında arabaya doğru savruluyor!\n\n" COLOR_RESET);
            if (attack_dir == 0) printf(COLOR_RED "  >>> SOL TARAFTAN ENKAZ GELİYOR! (KAÇINMAK İÇİN SAĞ OK TUŞUNA BAS) >>>\n\n" COLOR_RESET);
            else printf(COLOR_RED "  <<< SAĞ TARAFTAN ENKAZ GELİYOR! (KAÇINMAK İÇİN SOL OK TUŞUNA BAS) <<<\n\n" COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n [SCENE V] Survival in the Skies\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Hurled helplessly among the clouds, Helios, speeding away from the battlefield in his\n");
            printf(" blazing chariot, spots you. He ruthlessly swerves his chariot right at you!\n");
            printf(" The exact second you crash onto the hot floor of the chariot, a massive wooden ship mast\n");
            printf(" torn by the storm hurtles towards the chariot at bullet speed!\n\n" COLOR_RESET);
            if (attack_dir == 0) printf(COLOR_RED "  >>> DEBRIS INCOMING FROM LEFT! (PRESS RIGHT ARROW TO DODGE) >>>\n\n" COLOR_RESET);
            else printf(COLOR_RED "  <<< DEBRIS INCOMING FROM RIGHT! (PRESS LEFT ARROW TO DODGE) <<<\n\n" COLOR_RESET);
        }

        int required_key = (attack_dir == 0) ? 77 : 75;
        int parry_success = 0;
        long start_time = clock();

        while ((clock() - start_time) * 1000 / CLOCKS_PER_SEC < 3500) {
            if (_kbhit()) {
                int ch = _getch();
                if (ch == 0 || ch == 224) {
                    ch = _getch();
                    if (ch == required_key) { parry_success = 1; break; }
                    else if (ch == 75 || ch == 77 || ch == 72 || ch == 80) { parry_success = -1; break; }
                } else { parry_success = -1; break; }
            }
            Sleep(10);
        }

        clear_screen();
        if (parry_success == 1) {
            set_badge(profile, BADGE_BLESSING_HELIOS, 1);
            if (current_lang == 1) {
                printf(COLOR_CYAN " [ REFLEKS BAŞARILI ]\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Adrenalinle vücudunu mükemmel bir refleksle zıt yöne savuruyorsun!\n");
                printf(" Devasa enkaz, saçlarını havalandırarak arabayı sıyırıp geçiyor. Arabanın yanan zemininde,\n");
                printf(" fırtınadan uzağa, göklere doğru yükseliyorsun...\n\n" COLOR_RESET);
            } else {
                printf(COLOR_CYAN " [ REFLEX SUCCESSFUL ]\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Fueled by adrenaline, you throw your body in the opposite direction perfectly!\n");
                printf(" The massive debris grazes the chariot, ruffling your hair. Safe on the burning floor,\n");
                printf(" you ascend towards the heavens, far from the storm...\n\n" COLOR_RESET);
            }
        } else {
            set_badge(profile, BADGE_BROKEN_BONES, 3);
            set_badge(profile, BADGE_BLESSING_HELIOS, 1);
            if (current_lang == 1) {
                printf(COLOR_RED " [ REFLEKS BAŞARISIZ! ]\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Vücudun yeterince hızlı tepki veremiyor! Devasa tahta enkaz omzuna şiddetle çarpıyor.\n");
                printf(" Kemiklerinin iğrenç bir sesle çatladığını duyuyorsun. Acıdan çığlık atarak arabanın\n");
                printf(" zeminine yığılıyorsun. Görüşün bulanıklaşıyor ama Helios'un arabasının içindesin, hayattasın.\n\n" COLOR_RESET);
            } else {
                printf(COLOR_RED " [ REFLEX FAILED! ]\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Your body isn't fast enough! The massive wooden debris violently crashes into your shoulder.\n");
                printf(" You hear the sickening crack of your bones. Screaming in agony, you collapse on the\n");
                printf(" floor. Your vision blurs, but you are inside Helios's chariot, alive.\n\n" COLOR_RESET);
            }
        }
        if (current_lang == 1) printf(COLOR_CYAN " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
        else printf(COLOR_CYAN " [Press ANY KEY to continue]\n" COLOR_RESET);
        _getch();
    }
    // ------------------------------------------------------------------------
    // C - BOĞULANLAR VE AMBARDAKİLER (!helios_path)
    // ------------------------------------------------------------------------
    else {
        if (prev_choice == 1 && choice_2 == 2 && choice_3 == 3) {
            clear_screen();
            if (current_lang == 1) {
                printf(COLOR_DARK "\n\n  ...Cevapsız kalıyor...\n\n" COLOR_RESET);
                Sleep(5000);
                clear_screen();
                printf(COLOR_DARK " Zihnindeki o kadim ses, senin bu çaresizliğine inat soğuk ve alaycı bir tonda fısıldar:\n\n" COLOR_RESET);
                printf(COLOR_MAG " \"Görünüşe göre fanilerin kaderi hep denizin dibinde son buluyor...\"\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Suların amansız basıncı göğüs kafesini santim santim eziyor. Kurtulmak için\n");
                printf(" çırpınmaya mecalin kalmadı.\n");
                printf(" Mutlak karanlığın, o dipsiz hiçliğin içine doğru batarken bilincin tamamen kapanıyor...\n\n" COLOR_RESET);
                printf(COLOR_CYAN " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
            } else {
                printf(COLOR_DARK "\n\n  ...No answer...\n\n" COLOR_RESET);
                Sleep(5000);
                clear_screen();
                printf(COLOR_DARK " Mocking your despair, that ancient voice in your mind whispers in a cold tone:\n\n" COLOR_RESET);
                printf(COLOR_MAG " \"It seems the fate of mortals always ends at the bottom of the sea...\"\n\n" COLOR_RESET);
                printf(COLOR_WHITE " The relentless pressure of the waters crushes your ribcage inch by inch.\n");
                printf(" You have no strength left to struggle.\n");
                printf(" Sinking into the absolute darkness, that bottomless void, your consciousness shuts down completely...\n\n" COLOR_RESET);
                printf(COLOR_CYAN " [Press ANY KEY to continue]\n" COLOR_RESET);
            }
            _getch();
        } else {
            if (current_lang == 1) {
                printf(COLOR_GOLD "\n [SAHNE V] Sessizliğin Dibi\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Gemi göğe uzanan o devasa Tsunami tarafından saniyeler içinde yutulduğunda,\n");
                printf(" okyanusun ezici, karanlık derinliklerine doğru hızla batmaya başlıyorsun.\n");
                printf(" Etrafındaki tüm ışık, gök gürültüsü ve savaşın o sağır edici kaosu bir anda yok oluyor.\n");
                printf(" Geriye sadece derini kesen dondurucu bir soğuk ve mutlak bir karanlık kalıyor...\n\n");
                printf(COLOR_DARK " Zihnindeki o kadim ses, senin bu çaresizliğine inat soğuk ve alaycı bir tonda fısıldar:\n\n" COLOR_RESET);
                printf(COLOR_MAG " \"Görünüşe göre fanilerin kaderi hep denizin dibinde son buluyor...\"\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Suların amansız basıncı göğüs kafesini santim santim eziyor. Kurtulmak için\n");
                printf(" çırpınmaya mecalin kalmadı.\n");
                printf(" Mutlak karanlığın, o dipsiz hiçliğin içine doğru batarken bilincin tamamen kapanıyor...\n\n" COLOR_RESET);
                printf(COLOR_CYAN " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
            } else {
                printf(COLOR_GOLD "\n [SCENE V] Depth of Silence\n\n" COLOR_RESET);
                printf(COLOR_WHITE " When the ship is swallowed in seconds by the colossal Tsunami reaching the sky,\n");
                printf(" you begin to sink rapidly into the crushing, dark depths of the ocean.\n");
                printf(" All light, thunder, and the deafening chaos of war around you vanish instantly.\n");
                printf(" Only a skin-piercing freezing cold and absolute darkness remain...\n\n");
                printf(COLOR_DARK " Mocking your despair, that ancient voice in your mind whispers in a cold tone:\n\n" COLOR_RESET);
                printf(COLOR_MAG " \"It seems the fate of mortals always ends at the bottom of the sea...\"\n\n" COLOR_RESET);
                printf(COLOR_WHITE " The relentless pressure of the waters crushes your ribcage inch by inch.\n");
                printf(" You have no strength left to struggle.\n");
                printf(" Sinking into the absolute darkness, that bottomless void, your consciousness shuts down completely...\n\n" COLOR_RESET);
                printf(COLOR_CYAN " [Press ANY KEY to continue]\n" COLOR_RESET);
            }
            _getch();
        }
    }

    // ========================================================================
    // SAHNE VI: İKİLEM (c4) - GÜNEŞİN KAVURUCU ARABASI VEYA HİÇLİK
    // ========================================================================
    int choice_4 = 0;
    clear_screen();

    if (helios_path) {
        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE VI] Güneşin Merhameti\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Dünyayı yok eden o savaş çok gerilerde kaldı. Bulutların arasından, yerçekimini\n");
            printf(" parçalayan inanılmaz bir hızla yükseliyorsunuz. Etrafı saran o sağır edici ölüm kaosu,\n");
            printf(" yerini yüksek irtifadaki rüzgarın huzurlu ama ürkütücü uğultusuna bırakıyor.\n\n");
            printf(COLOR_DARK " Helios yavaşça sana dönüyor. Arabasından yayılan güneş ışığının tenini kavurduğunu,\n");
            printf(" kemiklerini eritecek kadar ilahi ve sert bir ısıyla bedenini sardığını hissediyorsun.\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Tam o sahte huzur anında, zihninin derinliklerinden, kanını donduran o tanıdık,\n");
            printf(" kadim ses fısıldıyor:\n\n" COLOR_RESET);
            printf(COLOR_MAG " \"Eğer bu delilikten, bu tanrıların kibirli oyunundan kaçacaksan... Tam zamanı... Atla...\"\n\n" COLOR_RESET);

            printf("  [" COLOR_CYAN "1" COLOR_RESET "] Bu delilikten, tanrılardan ve bana dayatılan bu kaderden tamamen kaçarım. (Oyundan Çık)\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] Sese kulak verip tanrılara güvenmeyerek, kendimi arabanın kenarından aşağı okyanusa bırakırım.\n");
            printf("  [" COLOR_CYAN "3" COLOR_RESET "] Zihnimdeki sesi susturup, bu kavurucu ilahi yolculuğu ve kaderimi sessizce kabullenirim.\n");
            printf("  [" COLOR_CYAN "4" COLOR_RESET "] Zihnimdeki sese ardı ardına mantıklı sorular sorarak bu kozmik karmaşayı çözmeye çalışırım.\n\n");
            printf(COLOR_CYAN "  Kararın nedir? (1-4): " COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n [SCENE VI] Mercy of the Sun\n\n" COLOR_RESET);
            printf(COLOR_WHITE " The world-ending war is left far behind. You ascend through the clouds at a speed\n");
            printf(" that shatters gravity. The deafening chaos of death gives way to the peaceful yet\n");
            printf(" eerie howl of the high-altitude wind.\n\n");
            printf(COLOR_DARK " Helios slowly turns to you. You feel the sunlight radiating from his chariot scorch\n");
            printf(" your skin, enveloping your body in a harsh, divine heat enough to melt your bones.\n\n" COLOR_RESET);
            printf(COLOR_WHITE " In that moment of false peace, the blood-chilling, familiar ancient voice whispers\n");
            printf(" from the depths of your mind:\n\n" COLOR_RESET);
            printf(COLOR_MAG " \"If you are going to escape this madness, this arrogant game of gods... Now is the time... Jump...\"\n\n" COLOR_RESET);

            printf("  [" COLOR_CYAN "1" COLOR_RESET "] I completely escape this madness, the gods, and this forced destiny. (Quit Game)\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] Listening to the voice and distrusting the gods, I throw myself off the chariot into the ocean.\n");
            printf("  [" COLOR_CYAN "3" COLOR_RESET "] I silence the voice in my mind and quietly accept this scorching divine journey and my fate.\n");
            printf("  [" COLOR_CYAN "4" COLOR_RESET "] I rapidly ask the voice logical questions, trying to decode this cosmic chaos.\n\n");
            printf(COLOR_CYAN "  What is your decision? (1-4): " COLOR_RESET);
        }

        while (1) {
            if (_kbhit()) {
                char ch = _getch();
                if (ch >= '1' && ch <= '4') { choice_4 = ch - '0'; break; }
            }
            Sleep(20);
        }

        if (choice_4 == 1) { exit(0); }
        else if (choice_4 == 2) { update_stat(profile, STAT_MIGHT, 3); update_stat(profile, STAT_INTEL, -1); }
        else if (choice_4 == 3) { update_stat(profile, STAT_FAITH, 3); update_stat(profile, STAT_SKILL, -1); }
        else if (choice_4 == 4) { update_stat(profile, STAT_INTEL, 3); update_stat(profile, STAT_HONOR, -1); }
    }
    else {
        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE VI] Derinlerdeki Lakayt Ses\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Okyanusun karanlık ve dondurucu derinliklerine doğru, bir taş gibi batmaya devam ediyorsun.\n");
            printf(" Ciğerlerin sızlıyor, bedenin tamamen uyuşmuş ve felç olmuş durumda.\n\n");
            printf(COLOR_DARK " Tam o anda, zihninin derinliklerinden gelen o tanıdık, kadim varlık,\n");
            printf(" etrafınızdaki bu ezici ve dehşet verici ölüme tamamen zıt, inanılmaz lakayt bir tonda konuşuyor:\n\n" COLOR_RESET);
            printf(COLOR_MAG " \"Deniz hiç benim tarzım değil ya... Buralar fazla ıslak. Çıkabilir miyiz artık?\"\n\n" COLOR_RESET);

            printf("  [" COLOR_CYAN "1" COLOR_RESET "] Bu lakayt tavra sinirlenip, irademi toplayarak zihnimden 'Buradan çıkamayız...' diye kesip atarım.\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] Zihnimdeki bu deliliği tamamen görmezden gelip, fiziksel olarak hayatta kalmak için son gücümle debelenirim.\n");
            printf("  [" COLOR_CYAN "3" COLOR_RESET "] Çırpınmayı bırakır, karanlık suların içinde sessizce ve huzurla ölüme teslim olurum.\n");
            printf("  [" COLOR_CYAN "4" COLOR_RESET "] Ölmekte olan fani fiziksel bedenimi terk edip, tamamen beynimin içindeki o sese odaklanırım.\n\n");
            printf(COLOR_CYAN "  Kararın nedir? (1-4): " COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n [SCENE VI] The Casual Voice in the Deep\n\n" COLOR_RESET);
            printf(COLOR_WHITE " You continue sinking like a stone into the dark, freezing depths of the ocean.\n");
            printf(" Your lungs ache, your body is completely numb and paralyzed.\n\n");
            printf(COLOR_DARK " Just then, that familiar, ancient entity from the depths of your mind speaks in an\n");
            printf(" incredibly casual tone, completely contrasting the crushing, horrifying death around you:\n\n" COLOR_RESET);
            printf(COLOR_MAG " \"The sea is really not my style... Too wet down here. Can we get out now?\"\n\n" COLOR_RESET);

            printf("  [" COLOR_CYAN "1" COLOR_RESET "] Angered by this casual attitude, I gather my will and mentally snap back, 'We can't get out...'\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] Ignoring this madness in my mind completely, I struggle physically with my last ounce of strength.\n");
            printf("  [" COLOR_CYAN "3" COLOR_RESET "] I stop struggling and silently, peacefully surrender to death in the dark waters.\n");
            printf("  [" COLOR_CYAN "4" COLOR_RESET "] I abandon my dying mortal physical body and focus entirely on the voice inside my brain.\n\n");
            printf(COLOR_CYAN "  What is your decision? (1-4): " COLOR_RESET);
        }

        while (1) {
            if (_kbhit()) {
                char ch = _getch();
                if (ch >= '1' && ch <= '4') { choice_4 = ch - '0'; break; }
            }
            Sleep(20);
        }

        if (choice_4 == 3) { exit(0); }
        else if (choice_4 == 1) { update_stat(profile, STAT_INTEL, 3); update_stat(profile, STAT_FAITH, -1); }
        else if (choice_4 == 2) { update_stat(profile, STAT_MIGHT, 3); update_stat(profile, STAT_FAITH, -1); }
        else if (choice_4 == 4) { update_stat(profile, STAT_SKILL, 3); update_stat(profile, STAT_MIGHT, -1); }
    }

    // ========================================================================
    // SAHNE VII: NİHAİ KOZMİK YÜZLEŞME (Altın Vuruş / Matris Kilitleme)
    // ========================================================================
    clear_screen();

    if (current_lang == 1) {
        printf(COLOR_WHITE " Fani bilincini tamamen kaybediyorsun...\n\n" COLOR_RESET);
        Sleep(2500); // Eklenen ilk gecikme
        printf(COLOR_CYAN " Bir anda beynin olmaması gereken bir hızda çalışmaya başladı.\n\n" COLOR_RESET);
        printf(COLOR_DARK " Karşında, fani gözlerinle algılayamayacağın, sınırları uzaya taşan devasa bir silüet beliriyor.\n");
        printf(" Anlık olarak tüm evrenin, zamanın ve kaderin ağırlığı altında bir kum tanesi gibi eziliyorsun.\n\n" COLOR_RESET);
        Sleep(5000); // Süre iki katına (5 saniyeye) çıkarıldı
        printf(COLOR_MAG " O kozmik varlık, zihninde yıldızları titreten ağır ve yargılayıcı bir yankıyla fısıldıyor:\n\n");
        printf(" \"Fani etten kabuğun parçalandı... Şimdi bana söyle... İçinden çıkacak olan ilahi özün nedir?\"\n\n" COLOR_RESET);

        printf("  [" COLOR_CYAN "1" COLOR_RESET "] Okyanusları yaracak ve dağları toza çevirecek mutlak bir kudret.\n");
        printf("  [" COLOR_CYAN "2" COLOR_RESET "] Tanrıların bile boyun eğeceği, sarsılmaz bir adalet terazisi.\n");
        printf("  [" COLOR_CYAN "3" COLOR_RESET "] Her hareketin ölümcül bir kusursuzlukla işlendiği saf bir yetenek.\n");
        printf("  [" COLOR_CYAN "4" COLOR_RESET "] Evrenin en karanlık sırlarını aydınlatacak dipsiz bir zeka.\n");
        printf("  [" COLOR_CYAN "5" COLOR_RESET "] Hiçliğin ortasında bile titremeden yanan ilahi bir inanç.\n");
        printf("  [" COLOR_CYAN "6" COLOR_RESET "] Dünyalar uğruna kendini feda etmekten çekinmeyen bir merhamet.\n");
        printf("  [" COLOR_CYAN "7" COLOR_RESET "] Ölümü bile kendi oyununda alt edecek kadar keskin bir kurnazlık.\n\n");
        printf(COLOR_CYAN "  Nihai cevabın nedir? (1-7): " COLOR_RESET);
    } else {
        printf(COLOR_WHITE " You lose your mortal consciousness completely...\n\n" COLOR_RESET);
        Sleep(2500);
        printf(COLOR_CYAN " Suddenly, your brain starts working at a speed it shouldn't be capable of.\n\n" COLOR_RESET);
        printf(COLOR_DARK " A massive silhouette, incomprehensible to mortal eyes and overflowing into space, appears before you.\n");
        printf(" You momentarily feel crushed like a grain of sand under the weight of the entire universe, time, and fate.\n\n" COLOR_RESET);
        Sleep(5000);
        printf(COLOR_MAG " That cosmic entity whispers with a heavy, judging echo that makes the stars tremble:\n\n");
        printf(" \"Your mortal shell of flesh has crumbled... Now tell me... What is the divine essence that emerges from within?\"\n\n" COLOR_RESET);

        printf("  [" COLOR_CYAN "1" COLOR_RESET "] Absolute might that will cleave oceans and grind mountains to dust.\n");
        printf("  [" COLOR_CYAN "2" COLOR_RESET "] An unshakable scale of justice that even gods will bow before.\n");
        printf("  [" COLOR_CYAN "3" COLOR_RESET "] Pure skill where every movement is executed with lethal perfection.\n");
        printf("  [" COLOR_CYAN "4" COLOR_RESET "] A bottomless intellect that will illuminate the universe's darkest secrets.\n");
        printf("  [" COLOR_CYAN "5" COLOR_RESET "] A divine faith burning without a flicker, even in the heart of the void.\n");
        printf("  [" COLOR_CYAN "6" COLOR_RESET "] A mercy that does not hesitate to sacrifice itself for the sake of worlds.\n");
        printf("  [" COLOR_CYAN "7" COLOR_RESET "] A cunning sharp enough to beat death at its own game.\n\n");
        printf(COLOR_CYAN "  What is your final answer? (1-7): " COLOR_RESET);
    }

    while (1) {
        if (_kbhit()) {
            char ch = _getch();
            if (ch >= '1' && ch <= '7') {
                int final_ans = ch - '0';
                // Altın Vuruş Matrisi: +5 / -2 (Test Modülü ile Birebir)
                if (final_ans == 1) { update_stat(profile, STAT_MIGHT, 5); update_stat(profile, STAT_FAITH, -2); }
                if (final_ans == 2) { update_stat(profile, STAT_HONOR, 5); update_stat(profile, STAT_INTEL, -2); }
                if (final_ans == 3) { update_stat(profile, STAT_SKILL, 5); update_stat(profile, STAT_HONOR, -2); }
                if (final_ans == 4) { update_stat(profile, STAT_INTEL, 5); update_stat(profile, STAT_MIGHT, -2); }
                if (final_ans == 5) { update_stat(profile, STAT_FAITH, 5); update_stat(profile, STAT_SKILL, -2); }
                if (final_ans == 6) { update_stat(profile, STAT_HONOR, 5); update_stat(profile, STAT_MIGHT, -2); }
                if (final_ans == 7) { update_stat(profile, STAT_SKILL, 5); update_stat(profile, STAT_INTEL, -2); }
                break;
            }
        }
        Sleep(20);
    }

    clear_screen();
    if (current_lang == 1) {
        printf(COLOR_CYAN "\n\n  [ KOZMİK MATRİS MÜHÜRLENİYOR... KADERİN AĞLARI ÖRÜLÜYOR... ]\n" COLOR_RESET);
    } else {
        printf(COLOR_CYAN "\n\n  [ SEALING COSMIC MATRIX... WEAVING THE THREADS OF DESTINY... ]\n" COLOR_RESET);
    }
    Sleep(2500);

    // ========================================================================
    // SESSİZ KOZMİK MATRİS HESAPLAMASI (Kusursuz Simetri)
    // ========================================================================
    double max_cosine = -2.0;
    int best_match_idx = 0;

    for (int i = 0; i < 33; i++) {
        // Veto yediyse o tanrıyı (Poseidon) eşleştirmeye alma
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
    // SCENE VIII: THE AWAKENING
    // ========================================================================
    clear_screen();

    if (profile->badges[BADGE_BLESSING_HELIOS] == 0) {
        // --- DROWNER AWAKENING ---
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
            printf(COLOR_RED "  TAM O AN...\n\n" COLOR_RESET);
            printf(COLOR_CYAN "  Zihnin hala olmaması gerektiği kadar zinde ve hızlı çalışıyor.\n");
            printf("  Sanki iki kişi kadar düşünüyor, aynı oranda da çevreni algılıyorsun.\n");
            printf("  Gözlerini derin denizlerin en dibinde açıyorsun. Ölmüşsün gibi, pamuk gibi bir his bedenini sarıyor.\n\n" COLOR_RESET);

            printf(COLOR_DARK "  Aşağıdan gelen karanlık bir hareketlilik seni alttan desteklemeye başlıyor.\n");
            printf("  Devasa bir platformun üzerindeymişsin gibi hızla yukarı doğru yükseliyorsun...\n");
            printf("  Etrafında sütuna benzer ince, uzun kapkara yapılar yavaşça üzerine doğru kapanıyor.\n\n" COLOR_RESET);

            printf(COLOR_WHITE "  O an idrak ediyorsun: Devasa bir avucun içindesin.\n\n" COLOR_RESET);

            printf(COLOR_DARK "  Nefes yok...\n  Nabız yok...\n  Hiçlik yavaşça bilincine işliyor...\n\n  Ve karanlık.\n\n" COLOR_RESET);

            printf(COLOR_CYAN "  [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
            _getch();
        } else {
            printf(COLOR_RED "  JUST THEN...\n\n" COLOR_RESET);
            printf(COLOR_CYAN "  Your mind is still incredibly sharp and working faster than it should be.\n");
            printf("  You are thinking with the capacity of two people, aware of your surroundings just as intensely.\n");
            printf("  You open your eyes at the very bottom of the deep sea. A feeling as soft as cotton surrounds you, as if you were dead.\n\n" COLOR_RESET);

            printf(COLOR_DARK "  A dark movement from below begins to support you.\n");
            printf("  You rise rapidly, as if standing on a colossal platform...\n");
            printf("  Tall, thin, pitch-black structures resembling pillars slowly close in around you.\n\n" COLOR_RESET);

            printf(COLOR_WHITE "  In that moment, you realize: You are inside a massive palm.\n\n" COLOR_RESET);

            printf(COLOR_DARK "  No breath...\n  No pulse...\n  The void slowly seeps into your consciousness...\n\n  And darkness.\n\n" COLOR_RESET);

            printf(COLOR_CYAN "  [Press ANY KEY to continue]\n" COLOR_RESET);
            _getch();
        }

    } else {
        // --- HELIOS AWAKENING ---
        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE VIII] Güneşin Şehri\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Gözlerini yavaşça aralıyorsun... Görme yetin yavaş yavaş geri geliyor.\n");
            printf(" Helios arabasında sana tepeden bakıyor.\n\n" COLOR_RESET);
            printf(COLOR_GOLD " \"Uyandın demek... Ben Helios! Göğün yanan gözü, Güneşin kudretli Titanı!\"\n" COLOR_RESET);
            printf(COLOR_WHITE " Sesinin tınısı bile gök gürültüsü gibi havayı titretiyor.\n" COLOR_RESET);
            printf(COLOR_GOLD " \"Ve sen de, benim bizzat müdürü olduğum bu kadim akademinin yeni öğrencisisin.\"\n\n" COLOR_RESET);
            printf(COLOR_DARK " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n [SCENE VIII] City of the Sun\n\n" COLOR_RESET);
            printf(COLOR_WHITE " You slowly open your eyes... Your vision gradually returns.\n");
            printf(" Helios is looking down at you in his chariot.\n\n" COLOR_RESET);
            printf(COLOR_GOLD " \"So you are awake... I am Helios! The burning eye of the sky, the mighty Titan of the Sun!\"\n" COLOR_RESET);
            printf(COLOR_WHITE " Even the timbre of his voice vibrates the air like thunder.\n" COLOR_RESET);
            printf(COLOR_GOLD " \"And you are the new student of this ancient academy, where I serve as principal.\"\n\n" COLOR_RESET);
            printf(COLOR_DARK " [Press ANY KEY to continue]\n" COLOR_RESET);
        }

        _getch();
        clear_screen();
        strcpy(profile->player_name, "Bilinmeyen"); // Form doldurulana kadar geçici atama

        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE VIII] Kan Bağı\n\n" COLOR_RESET);
            printf(COLOR_GOLD " \"Hmm, ufaklık...\"\n" COLOR_RESET);
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
        clear_screen();

        // --------------------------------------------------------------------
        // THE PRINCIPAL'S OFFICE (HELIOS ENCOUNTER)
        // --------------------------------------------------------------------
        profile->npc_met[NPC_HELIOS] = true;

        if (current_lang == 1) {
            printf(COLOR_GOLD "\n [SAHNE VIII] Büyükler Ligi\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Helios devasa meşe masasının arkasında duruyor. Odanın içi bunaltıcı olmayan,\n");
            printf(" huzurlu ama kudretli bir güneş sıcaklığıyla dolu. Sana keskin gözlerle bakıyor:\n\n" COLOR_RESET);

            printf(COLOR_GOLD " \"Burada kalabilmek için çok çalışman gerek evlat...\"\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Sesi az önceki fırtınaya kıyasla daha sakin ama kelimeleri dağ gibi ağır.\n\n" COLOR_RESET);
            printf(COLOR_GOLD " \"Burası melezler için büyükler ligi. Bir tanrı gücünde olmanı bekliyoruz senin.\n");
            printf(" %s sınıfındasın. Belaya bulaşma.\"\n\n" COLOR_RESET, profile->faction_class_tr);

            printf(COLOR_WHITE " Gözlerini aniden senden ayırıp masadaki parşömenlere dönerken tekdüze bir sesle ekliyor:\n\n" COLOR_RESET);
            printf(COLOR_GOLD " \"Şimdi çıkabilirsin.\"\n\n" COLOR_RESET);

            printf("  [" COLOR_CYAN "1" COLOR_RESET "] Odadan sessizce çık.\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] \"Ama daha sorularım var...\"\n\n");
            printf(COLOR_CYAN "  Kararın nedir? (1-2): " COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n [SCENE VIII] The Big Leagues\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Helios stands behind a massive oak desk. The room is filled with a peaceful yet\n");
            printf(" mighty solar warmth. He looks at you with piercing eyes:\n\n" COLOR_RESET);

            printf(COLOR_GOLD " \"You need to work hard to stay here, kid...\"\n\n" COLOR_RESET);
            printf(COLOR_WHITE " His voice is calmer compared to the storm, but his words carry the weight of mountains.\n\n" COLOR_RESET);
            printf(COLOR_GOLD " \"This is the big leagues for demigods. We expect you to be at god-level power.\n");
            printf(" You are in %s class. Don't get into trouble.\"\n\n" COLOR_RESET, profile->faction_class);

            printf(COLOR_WHITE " Looking away from you abruptly to the scrolls on his desk, he adds in a flat tone:\n\n" COLOR_RESET);
            printf(COLOR_GOLD " \"You may leave now.\"\n\n" COLOR_RESET);

            printf("  [" COLOR_CYAN "1" COLOR_RESET "] Leave the room silently.\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] \"But I have more questions...\"\n\n");
            printf(COLOR_CYAN "  What is your decision? (1-2): " COLOR_RESET);
        }

        int dialog_choice = 0;
        while (1) {
            if (_kbhit()) {
                char ch = _getch();
                if (ch == '1' || ch == '2') { dialog_choice = ch - '0'; break; }
            }
            Sleep(20);
        }

        clear_screen();

        if (dialog_choice == 1) {
            profile->npc_relationship[NPC_HELIOS] += 2;
            if (current_lang == 1) {
                printf(COLOR_GOLD "\n [SAHNE VIII] Emir ve İtaat\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Başını hafifçe eğip arkana dönüyorsun. Ancak kapıya doğru ilk adımını atıp\n");
                printf(" ayağını yere bastığın an...\n\n" COLOR_RESET);
            } else {
                printf(COLOR_GOLD "\n [SCENE VIII] Order and Obedience\n\n" COLOR_RESET);
                printf(COLOR_WHITE " You nod slightly and turn around. But the moment you take your first step\n");
                printf(" towards the door and your foot touches the ground...\n\n" COLOR_RESET);
            }
        }
        else {
            profile->npc_relationship[NPC_HELIOS] -= 2;
            if (current_lang == 1) {
                printf(COLOR_GOLD "\n [SAHNE VIII] Sabrın Sınırı\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Sen tam \"Ama daha sorularım var...\" diyerek öne atıldığın anda,\n");
                printf(" Helios sana bakmaya bile tenezzül etmeden masada sadece parmağını hafifçe oynatıyor.\n");
                printf(" Altındaki uzun ahşap sandalye bir anda şiddetle geriye doğru çekiliyor!\n");
                printf(" Dengeni kaybedip yere düşerken...\n\n" COLOR_RESET);
            } else {
                printf(COLOR_GOLD "\n [SCENE VIII] Limit of Patience\n\n" COLOR_RESET);
                printf(COLOR_WHITE " Just as you lean forward saying \"But I have more questions...\",\n");
                printf(" Helios merely twitches his finger on the desk without even bothering to look at you.\n");
                printf(" The long wooden chair beneath you is violently yanked backwards!\n");
                printf(" As you lose your balance and fall to the ground...\n\n" COLOR_RESET);
            }
        }

        Sleep(1000);
        if (current_lang == 1) {
            printf(COLOR_RED "  YANIYORSUN!\n\n" COLOR_RESET);
            printf(COLOR_WHITE " Güneşin o saf ve vahşi enerjisi bir anda tenini kavurmaya başlıyor.\n");
            printf(" Alevler içinde kalıyorsun! Acıdan çığlık atmana bile fırsat kalmadan...\n");
            printf(" Gözünü kırptığın o milisaniyede...\n\n" COLOR_RESET);
        } else {
            printf(COLOR_RED "  YOU ARE BURNING!\n\n" COLOR_RESET);
            printf(COLOR_WHITE " That pure and wild energy of the sun suddenly begins to scorch your skin.\n");
            printf(" You burst into flames! Before you even have a chance to scream in agony...\n");
            printf(" In the millisecond you blink...\n\n" COLOR_RESET);
        }

        if (current_lang == 1) printf(COLOR_DARK " [Devam etmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
        else printf(COLOR_DARK " [Press ANY KEY to continue]\n" COLOR_RESET);
        _getch();
    }

    // ========================================================================
    // SCENE IX: THE AWAKENING (SHRINE OR INFIRMARY) & FORM FILLING
    // ========================================================================
    clear_screen();
    bool in_infirmary = (profile->badges[BADGE_BROKEN_BONES] > 0);

    if (in_infirmary) {
        if (current_lang == 1) {
            printf(COLOR_CYAN "\n\n  Gözlerini keskin bir ışık ve yoğun bir ambrosia (tanrı nektarı) kokusu eşliğinde açıyorsun.\n");
            printf("  Bembeyaz, tertemiz bir yataktasın. Omzundaki kırıklar sarılmış ve ilahi bir ısıyla iyileşiyor.\n");
            printf("  Apollon Kulübesi'nin bitişiğindeki Revir (Infirmary) odasındasın.\n\n" COLOR_RESET);
            printf(COLOR_WHITE "  Yanına altın sarısı saçlı, üzerinde şifacı cübbesi olan bir Apollon çocuğu yaklaşıyor.\n");
            printf("  Elinde kalın bir parşömen ve tüy kalem var. Hafifçe gülümseyerek konuşuyor:\n\n" COLOR_RESET);
            printf(COLOR_GOLD "  \"Sonunda uyandın. Helios'un arabasına o hızla çarpıp hayatta kalman bir mucize.\n");
            printf("  Müdür Helios akademiye kaydın için bu formu doldurmanı istedi. Adın ve odaklanacağın epik hedefler...\"\n\n" COLOR_RESET);
        } else {
            printf(COLOR_CYAN "\n\n  You open your eyes to a sharp light and the intense scent of ambrosia (nectar of the gods).\n");
            printf("  You are in a pristine, white bed. Your broken shoulder is bandaged, healing with a divine warmth.\n");
            printf("  You are in the Infirmary adjacent to the Apollo Shrine.\n\n" COLOR_RESET);
            printf(COLOR_WHITE "  A child of Apollo with golden hair and a healer's robe approaches you.\n");
            printf("  Holding a thick parchment and a quill, they speak with a slight smile:\n\n" COLOR_RESET);
            printf(COLOR_GOLD "  \"You're finally awake. It's a miracle you survived crashing into Helios's chariot at that speed.\n");
            printf("  Principal Helios wants you to fill out this form for your registration. Your name and your epic targets...\"\n\n" COLOR_RESET);
        }
    } else {
        if (current_lang == 1) {
            printf(COLOR_CYAN "\n\n  Gözlerini ahşap, tanıdık bir kokusu olan, sessiz bir odada açıyorsun.\n");
            printf("  Alevlerden, boğucu sulardan veya ölümden eser yok.\n");
            printf("  Ebeveynin %s'un sana ayırdığı o kişisel kulübedesin.\n\n" COLOR_RESET, profile->god_alignment);
            printf(COLOR_WHITE "  Odanın köşesinden, seninle aynı kanı taşıyan, kulübenden bir melez (kardeşin) yaklaşıyor.\n");
            printf("  Elinde kalın bir parşömen ve tüy kalem var. Hafifçe omuz silkerek konuşuyor:\n\n" COLOR_RESET);
            printf(COLOR_GOLD "  \"Aramıza hoş geldin. Müdür Helios akademiye kaydın için bu formu doldurmanı istedi.\n");
            printf("  Adın ve odaklanacağın epik hedefler...\"\n\n" COLOR_RESET);
        } else {
            printf(COLOR_CYAN "\n\n  You open your eyes in a quiet room with a familiar wooden scent.\n");
            printf("  There is no trace of flames, suffocating waters, or death.\n");
            printf("  You are in the personal shrine prepared for you by your parent, %s.\n\n" COLOR_RESET, profile->god_alignment);
            printf(COLOR_WHITE "  From the corner of the room, a demigod from your cabin (your sibling) approaches.\n");
            printf("  Holding a thick parchment and a quill, they speak with a slight shrug:\n\n" COLOR_RESET);
            printf(COLOR_GOLD "  \"Welcome to the cabin. Principal Helios wants you to fill out this form for your registration.\n");
            printf("  Your name and your epic targets...\"\n\n" COLOR_RESET);
        }
    }

    if (current_lang == 1) printf(COLOR_DARK "  [Kayıt Formunu Doldurmak İçin HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
    else printf(COLOR_DARK "  [Press ANY KEY to Fill the Registration Form]\n" COLOR_RESET);
    _getch();
    clear_screen();

    // --- NPC ELİNDEKİ FORMU DOLDURMA (ESKİ scene_init_subjects MANTIĞI) ---
    if (current_lang == 1) {
        printf(COLOR_GOLD " =============================================================\n");
        printf("                  AKADEMİ KAYIT VE KADER FORMU                \n");
        printf(" =============================================================\n\n" COLOR_RESET);
        printf(COLOR_CYAN "  [Mühürlenecek Adın]: " COLOR_RESET);
    } else {
        printf(COLOR_GOLD " =============================================================\n");
        printf("              ACADEMY REGISTRATION & DESTINY FORM             \n");
        printf(" =============================================================\n\n" COLOR_RESET);
        printf(COLOR_CYAN "  [Your Name to be Sealed]: " COLOR_RESET);
    }

    set_cursor_visibility(true);
    read_string_safe(profile->player_name, 50);

    if (current_lang == 1) printf(COLOR_WHITE "\n  Sisteme toplamda kaç Epik Hedef (Ders) tanımlayacaksın? (1-15): " COLOR_RESET);
    else printf(COLOR_WHITE "\n  How many Epic Targets (Subjects) will you define? (1-15): " COLOR_RESET);

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

        profile->study_stats[i] = 0;
    }

    // Temizlik
    for(int i = profile->active_subject_count; i < 15; i++) {
        strcpy(profile->subject_names[i], "BOS");
        profile->subject_exams[i] = 0;
        profile->subject_projects[i] = 0;
        profile->study_stats[i] = 0;
    }
    set_cursor_visibility(false);
    save_game(profile);

    clear_screen();
    if (in_infirmary) {
        if (current_lang == 1) {
            printf(COLOR_GOLD "\n  \"Harika, %s. Kaydın tamamlandı.\"\n" COLOR_RESET, profile->player_name);
            printf(COLOR_WHITE "  Apollon çocuğu parşömeni alıp rulo yapıyor.\n");
            printf(COLOR_GOLD "  \"Kemiklerin kaynadı. Artık taburcusun. %s Kulübesine gidebilirsin.\"\n\n" COLOR_RESET, profile->god_alignment);
            printf(COLOR_DARK "  [Kişisel Kulübene Geçmek İçin HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n  \"Excellent, %s. Your registration is complete.\"\n" COLOR_RESET, profile->player_name);
            printf(COLOR_WHITE "  The child of Apollo rolls up the parchment.\n");
            printf(COLOR_GOLD "  \"Your bones have mended. You are discharged. You may proceed to the Shrine of %s.\"\n\n" COLOR_RESET, profile->god_alignment);
            printf(COLOR_DARK "  [Press ANY KEY to Proceed to Your Personal Shrine]\n" COLOR_RESET);
        }
    } else {
        if (current_lang == 1) {
            printf(COLOR_GOLD "\n  \"Harika, %s. Kaydın tamamlandı.\"\n" COLOR_RESET, profile->player_name);
            printf(COLOR_WHITE "  Kardeşin parşömeni alıp rulo yapıyor.\n");
            printf(COLOR_GOLD "  \"Dinlenmeye bak. Hedeflerin masanda duruyor olacak.\"\n\n" COLOR_RESET);
            printf(COLOR_DARK "  [Masa Başına Geçmek İçin HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n  \"Excellent, %s. Your registration is complete.\"\n" COLOR_RESET, profile->player_name);
            printf(COLOR_WHITE "  Your sibling rolls up the parchment.\n");
            printf(COLOR_GOLD "  \"Get some rest. Your targets will be on your desk.\"\n\n" COLOR_RESET);
            printf(COLOR_DARK "  [Press ANY KEY to Sit at Your Desk]\n" COLOR_RESET);
        }
    }
    _getch();

    // Sadece kendi kulübemize geçiyoruz, gereksiz scene_init_subjects çağrısını sildik
    scene_own_shrine(profile);
}



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
// KOZMİK HAFIZA (KAYIT SEÇME VE SİLME EKRANI)
// ============================================================================
bool scene_continue_journey(CharacterProfile* current_player) {
    CharacterProfile saves[MAX_SAVES];
    load_all_saves(saves);

    bool in_menu = true;
    while(in_menu) {
        clear_screen();
        if (current_lang == 1) {
            printf(COLOR_GOLD "\n  === KOZMİK HAFIZA (KAYITLI KADERLER) ===\n\n" COLOR_RESET);
        } else {
            printf(COLOR_GOLD "\n  === COSMIC MEMORY (SAVED DESTINIES) ===\n\n" COLOR_RESET);
        }

        // 10 Slotu Ekrana Yazdır
        for(int i = 0; i < MAX_SAVES; i++) {
            int display_num = (i == 9) ? 0 : (i + 1); // Klavyedeki 1-9 ve 0 düzeni

            // Eğer ismin uzunluğu 0'dan büyükse karakter var demektir
            if(strlen(saves[i].player_name) > 0) {
                if (current_lang == 1) {
                    printf("  [" COLOR_CYAN "%d" COLOR_RESET "] %-15s | EXP: %04d | " COLOR_GOLD "%s" COLOR_RESET " [%s]\n",
                        display_num, saves[i].player_name, saves[i].total_exp,
                        saves[i].god_alignment, saves[i].archetype_alignment_tr);
                } else {
                    printf("  [" COLOR_CYAN "%d" COLOR_RESET "] %-15s | EXP: %04d | " COLOR_GOLD "%s" COLOR_RESET " [%s]\n",
                        display_num, saves[i].player_name, saves[i].total_exp,
                        saves[i].god_alignment, saves[i].archetype_alignment);
                }
            } else {
                if (current_lang == 1) {
                    printf(COLOR_DARK "  [%d] --- BOŞ KADER YUVASI ---\n" COLOR_RESET, display_num);
                } else {
                    printf(COLOR_DARK "  [%d] --- EMPTY DESTINY SLOT ---\n" COLOR_RESET, display_num);
                }
            }
        }

        printf(COLOR_DARK "\n  =================================================================\n" COLOR_RESET);
        if (current_lang == 1) {
            printf("  [" COLOR_RED "D" COLOR_RESET "] Bir Kaderi Sil (Delete)\n");
            printf("  [" COLOR_CYAN "Q" COLOR_RESET "] Ana Menüye Dön\n\n");
            printf(COLOR_CYAN "  Yüklenecek Slot (1-0) veya Eylem: " COLOR_RESET);
        } else {
            printf("  [" COLOR_RED "D" COLOR_RESET "] Delete a Destiny\n");
            printf("  [" COLOR_CYAN "Q" COLOR_RESET "] Return to Main Menu\n\n");
            printf(COLOR_CYAN "  Load Slot (1-0) or Action: " COLOR_RESET);
        }

        char ch;
        while(1) {
            if(_kbhit()) {
                ch = _getch();
                break;
            }
            Sleep(20);
        }

        // Q: Menüden Çık
        if(ch == 'q' || ch == 'Q') {
            return false;
        }
        // D: Silme Modu
        else if(ch == 'd' || ch == 'D') {
            if (current_lang == 1) printf(COLOR_RED "\n  Silinecek slot numarasını girin (İptal için Q): " COLOR_RESET);
            else printf(COLOR_RED "\n  Enter slot number to delete (Q to cancel): " COLOR_RESET);

            char del_ch;
            while(1) {
                if(_kbhit()) {
                    del_ch = _getch();
                    break;
                }
                Sleep(20);
            }

            if((del_ch >= '0' && del_ch <= '9')) {
                int slot = (del_ch == '0') ? 9 : (del_ch - '1');
                if(strlen(saves[slot].player_name) > 0) {
                    memset(&saves[slot], 0, sizeof(CharacterProfile)); // Slotu tamamen sıfırla
                    save_all_saves(saves); // Değişikliği diske kaydet
                    if (current_lang == 1) printf(COLOR_GRN "  Kader ipliği kesildi. (Kayıt silindi)\n" COLOR_RESET);
                    else printf(COLOR_GRN "  Thread of destiny severed. (Save deleted)\n" COLOR_RESET);
                    Sleep(1200);
                }
            }
        }
        // Sayıya Basıldıysa Yükleme Yap
        else if((ch >= '0' && ch <= '9')) {
            int slot = (ch == '0') ? 9 : (ch - '1');
            if(strlen(saves[slot].player_name) > 0) {
                *current_player = saves[slot]; // Karakteri ana yapıya kopyala
                if (current_lang == 1) printf(COLOR_GRN "\n  Kozmik hafıza geri çağrılıyor... Ebedi döngüye dönülüyor!\n" COLOR_RESET);
                else printf(COLOR_GRN "\n  Recalling cosmic memory... Returning to the eternal cycle!\n" COLOR_RESET);
                Sleep(1500);
                return true; // Başarıyla yüklendi
            }
        }
    }
    return false;
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

        save_game(profile);

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

            save_game(profile);

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
// KÜTÜPHANE İÇ MENÜSÜ (İSTATİSTİKLER VE ÇALIŞMA)
// ============================================================================
void scene_library_menu(CharacterProfile* profile) {
    bool in_library = true;
    while(in_library) {
        clear_screen();
        printf("\n");
        printf(COLOR_DARK "  ==============================================================================================================\n" COLOR_RESET);
        if (current_lang == 1) {
            printf(COLOR_GOLD "                                      === KÜTÜPHANE ===\n" COLOR_RESET);
        } else {
            printf(COLOR_GOLD "                                      === THE LIBRARY ===\n" COLOR_RESET);
        }
        printf(COLOR_DARK "  ==============================================================================================================\n\n" COLOR_RESET);

        if (current_lang == 1) {
            printf("  [" COLOR_CYAN "1" COLOR_RESET "] Odak Kronometresi (Ders Çalış ve EXP Kazan)\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] Savaş Günlüğünü Oku (Geçmiş Çalışma Kayıtları)\n");
            printf("  [" COLOR_CYAN "3" COLOR_RESET "] Kehanet Takvimini İncele (Yaklaşan Sınav ve Projeler)\n");
            printf("  [" COLOR_CYAN "4" COLOR_RESET "] Ruh Aynasına Bak (Karakter Statları ve Matris)\n");
            printf("  [" COLOR_CYAN "0" COLOR_RESET "] Koridora Dön (Çıkış)\n\n");
            printf(COLOR_CYAN "  Eylem Seçimi: " COLOR_RESET);
        } else {
            printf("  [" COLOR_CYAN "1" COLOR_RESET "] Focus Stopwatch (Study and Gain EXP)\n");
            printf("  [" COLOR_CYAN "2" COLOR_RESET "] Read Battle Logs (Past Study Records)\n");
            printf("  [" COLOR_CYAN "3" COLOR_RESET "] Examine Prophecy Calendar (Upcoming Exams & Projects)\n");
            printf("  [" COLOR_CYAN "4" COLOR_RESET "] Mirror of Souls (Character Stats & Matrix)\n");
            printf("  [" COLOR_CYAN "0" COLOR_RESET "] Return to Hallway (Exit)\n\n");
            printf(COLOR_CYAN "  Select Action: " COLOR_RESET);
        }

        bool valid_input = false;
        while (!valid_input) {
            if (_kbhit()) {
                char ch = _getch();
                if (ch == '1') { scene_library_stopwatch(profile); valid_input = true; }
                else if (ch == '2') { scene_read_logs(); valid_input = true; }
                else if (ch == '3') { scene_view_calendar(profile); valid_input = true; }
                else if (ch == '4') { display_character_sheet(profile); valid_input = true; }
                else if (ch == '0') { in_library = false; valid_input = true; }
            }
            Sleep(20);
        }
    }
}

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
                    scene_library_menu(profile); // Artık alt menüye geçiyor
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
               M_YEL "  [APOLLO & REVIR]   "
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
                    // Köy menüsü döngüsünü sonlandırır, oyuncu zaten arkada çalışan
                    // scene_own_shrine (Yatak Odası) döngüsüne pürüzsüzce geri döner.
                    in_village = false;
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



// ============================================================================
// KADİM SAVAŞ GÜNLÜĞÜ (LOG OKUYUCU)
// ============================================================================
void scene_read_logs(void) {
    clear_screen();
    printf(COLOR_DARK "  ==============================================================================================================\n" COLOR_RESET);
    if (current_lang == 1) {
        printf(COLOR_GOLD "                                     [ KADİM SAVAŞ GÜNLÜĞÜ ]\n" COLOR_RESET);
    } else {
        printf(COLOR_GOLD "                                     [ ANCIENT BATTLE LOGS ]\n" COLOR_RESET);
    }
    printf(COLOR_DARK "  ==============================================================================================================\n\n" COLOR_RESET);

    FILE *file = fopen("shrine_chronicles.txt", "r");
    if (file == NULL) {
        if (current_lang == 1) {
            printf(COLOR_DARK "  Masadaki parşömenler henüz bomboş. Tarih yazılmayı bekliyor...\n" COLOR_RESET);
        } else {
            printf(COLOR_DARK "  The scrolls on the desk are empty. History waits to be written...\n" COLOR_RESET);
        }
    } else {
        char line[256];
        printf(COLOR_CYAN);
        while (fgets(line, sizeof(line), file)) {
            printf("  > %s", line);
        }
        printf(COLOR_RESET);
        fclose(file);
    }

    if (current_lang == 1) {
        printf(COLOR_DARK "\n  ==============================================================================================================\n");
        printf("  [Mabede dönmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
    } else {
        printf(COLOR_DARK "\n  ==============================================================================================================\n");
        printf("  [Press ANY KEY to return to the shrine]\n" COLOR_RESET);
    }

    while (_kbhit()) _getch(); // Klavye arabelleğini temizle
    _getch();
}

// ============================================================================
// KEHANET TAKVİMİ (BOSS VE PROJE GÖRÜNTÜLEYİCİ)
// ============================================================================
void scene_view_calendar(CharacterProfile* profile) {
    clear_screen();
    printf(COLOR_DARK "  ==============================================================================================================\n" COLOR_RESET);
    if (current_lang == 1) {
        printf(COLOR_MAG "                                  [ KEHANET TAKVİMİ VE YAKLAŞAN TEHDİTLER ]\n" COLOR_RESET);
    } else {
        printf(COLOR_MAG "                                  [ CALENDAR OF PROPHECY & IMPENDING THREATS ]\n" COLOR_RESET);
    }
    printf(COLOR_DARK "  ==============================================================================================================\n\n" COLOR_RESET);

    if (profile->active_subject_count == 0) {
        if (current_lang == 1) {
            printf(COLOR_DARK "  Ufukta görünen bir tehdit yok. Kader ağları henüz örülmedi...\n" COLOR_RESET);
        } else {
            printf(COLOR_DARK "  No threats on the horizon. The threads of destiny are not yet woven...\n" COLOR_RESET);
        }
    } else {
        for (int i = 0; i < profile->active_subject_count; i++) {
            if (current_lang == 1) {
                printf(COLOR_WHITE "  [%d] KADER YOLU (Ders) : " COLOR_CYAN "%s\n" COLOR_RESET, i + 1, profile->subject_names[i]);
                printf(COLOR_RED   "      - Bekleyen Büyük Boss (Sınav) : %d\n" COLOR_RESET, profile->subject_exams[i]);
                printf(COLOR_GOLD  "      - Bekleyen Kuşatma (Proje)    : %d\n" COLOR_RESET, profile->subject_projects[i]);
                printf(COLOR_GRN   "      - Mevcut Güç Seviyesi         : %d EXP\n\n" COLOR_RESET, profile->study_stats[i]);
            } else {
                printf(COLOR_WHITE "  [%d] PATH OF DESTINY   : " COLOR_CYAN "%s\n" COLOR_RESET, i + 1, profile->subject_names[i]);
                printf(COLOR_RED   "      - Impending Great Boss (Exam) : %d\n" COLOR_RESET, profile->subject_exams[i]);
                printf(COLOR_GOLD  "      - Impending Siege (Project)   : %d\n" COLOR_RESET, profile->subject_projects[i]);
                printf(COLOR_GRN   "      - Current Power Level         : %d EXP\n\n" COLOR_RESET, profile->study_stats[i]);
            }
        }
    }

    if (current_lang == 1) {
        printf(COLOR_DARK "  ==============================================================================================================\n");
        printf("  [Mabede dönmek için HERHANGİ BİR TUŞA BAS]\n" COLOR_RESET);
    } else {
        printf(COLOR_DARK "  ==============================================================================================================\n");
        printf("  [Press ANY KEY to return to the shrine]\n" COLOR_RESET);
    }

    while (_kbhit()) _getch();
    _getch();
}
// ============================================================================
// KULÜBE (SHRINE) İÇ MEKAN TASARIM YÖNETİCİSİ (YATAK ODASI)
// ============================================================================
void draw_shrine_interior(const char* god_name) {
    char* left[8];
    char* l_color = COLOR_WHITE;

    // SOL TARAF: Tanrıya Özel Duvar Dekoru ve Yatak Tasarımı (Sabit 40 Karakter Genişlik)
    if (strcmp(god_name, "Zeus") == 0) {
        left[0] = "  .. Gok Gurultusu & Simsekler ..       ";
        left[1] = "      \\_\\_          _/_/                ";
        left[2] = "        \\_\\_      _/_/                  ";
        left[3] = "   _|\\========================/|_       ";
        left[4] = "   \\____________________________/       ";
        left[5] = "    ||      [ BULUT YATAGI ]      ||    ";
        left[6] = "   _||________________________||_       ";
        left[7] = "  /______________________________\\      ";
        l_color = COLOR_CYAN;
    }
    else if (strcmp(god_name, "Poseidon") == 0) {
        left[0] = "  ~ ~ Okyanus & Mercan Resifleri ~ ~    ";
        left[1] = "      _\\|/_         _\\|/_               ";
        left[2] = "      \\ | /         \\ | /               ";
        left[3] = "   _|\\========================/|_       ";
        left[4] = "   \\____________________________/       ";
        left[5] = "    ||    [ ISTIRIDYE YATAK ]     ||    ";
        left[6] = "   _||________________________||_       ";
        left[7] = "  /______________________________\\      ";
        l_color = COLOR_CYAN;
    }
    else if (strcmp(god_name, "Hades") == 0) {
        left[0] = "  ... Karanlik Taslar & Zincirler ...   ";
        left[1] = "        ( )         ( )                 ";
        left[2] = "       _| |_       _| |_                ";
        left[3] = "   _|\\========================/|_       ";
        left[4] = "   \\____________________________/       ";
        left[5] = "    ||     [ OBSIDYEN YATAK ]     ||    ";
        left[6] = "   _||________________________||_       ";
        left[7] = "  /______________________________\\      ";
        l_color = COLOR_MAG;
    }
    else if (strcmp(god_name, "Athena") == 0) {
        left[0] = "  === Zirhlar & Bilgi Parsomenleri ===  ";
        left[1] = "       {o,o}       {o,o}                ";
        left[2] = "       /)__)       /)__)                ";
        left[3] = "   _|\\========================/|_       ";
        left[4] = "   \\____________________________/       ";
        left[5] = "    ||    [ STRATEJI YATAGI ]     ||    ";
        left[6] = "   _||________________________||_       ";
        left[7] = "  /______________________________\\      ";
        l_color = COLOR_WHITE;
    }
    else if (strcmp(god_name, "Ares") == 0) {
        left[0] = "  \\|/ Kanli Silahlar & Kalkanlar \\|/    ";
        left[1] = "       /   \\       /   \\                ";
        left[2] = "      | / \\ |     | / \\ |               ";
        left[3] = "   _|\\========================/|_       ";
        left[4] = "   \\____________________________/       ";
        left[5] = "    ||     [ SPARTA YATAGI ]      ||    ";
        left[6] = "   _||________________________||_       ";
        left[7] = "  /______________________________\\      ";
        l_color = COLOR_RED;
    }
    else if (strcmp(god_name, "Apollo") == 0) {
        left[0] = "  * * * Altin Isik Huzmeleri * * *      ";
        left[1] = "       \\ | /       \\ | /                ";
        left[2] = "       - O -       - O -                ";
        left[3] = "   _|\\========================/|_       ";
        left[4] = "   \\____________________________/       ";
        left[5] = "    ||      [ GUNES YATAGI ]      ||    ";
        left[6] = "   _||________________________||_       ";
        left[7] = "  /______________________________\\      ";
        l_color = COLOR_GOLD;
    }
    else if (strcmp(god_name, "Demeter") == 0) {
        left[0] = "  #%# Sarmasiklar & Basaklar #%#        ";
        left[1] = "        \\|/         \\|/                 ";
        left[2] = "       -\\|/-       -\\|/-                ";
        left[3] = "   _|\\========================/|_       ";
        left[4] = "   \\____________________________/       ";
        left[5] = "    ||     [ TOPRAK YATAGI ]      ||    ";
        left[6] = "   _||________________________||_       ";
        left[7] = "  /______________________________\\      ";
        l_color = COLOR_GRN;
    }
    else if (strcmp(god_name, "Aphrodite") == 0) {
        left[0] = "  ~ ~ ~ Ipek Perdeler & Guller ~ ~ ~    ";
        left[1] = "       _   _       _   _                ";
        left[2] = "      / \\ / \\     / \\ / \\               ";
        left[3] = "   _|\\========================/|_       ";
        left[4] = "   \\____________________________/       ";
        left[5] = "    ||    [ ISTIRAHAT YATAGI ]    ||    ";
        left[6] = "   _||________________________||_       ";
        left[7] = "  /______________________________\\      ";
        l_color = COLOR_MAG;
    }
    else if (strcmp(god_name, "Hermes") == 0) {
        left[0] = "  >>> Haritalar & Kanatli Cizmeler >>>  ";
        left[1] = "        / \\         / \\                 ";
        left[2] = "       /| |\\       /| |\\                ";
        left[3] = "   _|\\========================/|_       ";
        left[4] = "   \\____________________________/       ";
        left[5] = "    ||     [ SEYYAH YATAGI ]      ||    ";
        left[6] = "   _||________________________||_       ";
        left[7] = "  /______________________________\\      ";
        l_color = COLOR_CYAN;
    }
    else if (strcmp(god_name, "Hephaestus") == 0) {
        left[0] = "  +++ Carklar & Ors & Cekicler +++      ";
        left[1] = "       [___]       [___]                ";
        left[2] = "       /   \\       /   \\                ";
        left[3] = "   _|\\========================/|_       ";
        left[4] = "   \\____________________________/       ";
        left[5] = "    ||     [ DEMIRCI YATAGI ]     ||    ";
        left[6] = "   _||________________________||_       ";
        left[7] = "  /______________________________\\      ";
        l_color = COLOR_RED;
    }
    else if (strcmp(god_name, "Dionysus") == 0) {
        left[0] = "  &&& Uzum Baglari & Kadehler &&&       ";
        left[1] = "        ooo         ooo                 ";
        left[2] = "       ooooo       ooooo                ";
        left[3] = "   _|\\========================/|_       ";
        left[4] = "   \\____________________________/       ";
        left[5] = "    ||      [ SOLEN YATAGI ]      ||    ";
        left[6] = "   _||________________________||_       ";
        left[7] = "  /______________________________\\      ";
        l_color = COLOR_MAG;
    }
    else {
        left[0] = "  ... Sessiz ve Gosterissiz ...         ";
        left[1] = "                                        ";
        left[2] = "                                        ";
        left[3] = "   _|\\========================/|_       ";
        left[4] = "   \\____________________________/       ";
        left[5] = "    ||      [ FANI YATAGI ]       ||    ";
        left[6] = "   _||________________________||_       ";
        left[7] = "  /______________________________\\      ";
        l_color = COLOR_WHITE;
    }

    // SAĞ TARAF: Kadim Çalışma Masası ve Parşömenler (Sabit 40 Karakter Genişlik)
    char* right[8];
    right[0] = "         .---------------------------.  ";
    right[1] = "        /                           / \\ ";
    right[2] = "       /___________________________/   \\";
    right[3] = "       |                           |   |";
    right[4] = "       |  KADIM PARSOMENLER VE     |   |";
    right[5] = "       |  SAVAS PLANLARI           |   |";
    right[6] = "       |                           |   /";
    right[7] = "       \\___________________________\\_/  ";

    // ODAYI ÇİZ (Sol ve Sağ tarafı mükemmel birleştirir)
    for (int i = 0; i < 8; i++) {
        // Sol tarafı yatağın/tanrının kendi renginde, sağdaki masayı beyaz çizer
        printf("%s%s" COLOR_WHITE "%s\n", l_color, left[i], right[i]);
    }
    printf(COLOR_RESET);
}

// ============================================================================
// KİŞİSEL MABET (YATAK ODASI / GÜVENLİ ALAN)
// ============================================================================
void scene_own_shrine(CharacterProfile* profile) {
    bool in_shrine = true;

    while(in_shrine) {
        clear_screen();
        printf("\n");

        printf(COLOR_DARK "  ==============================================================================================================\n" COLOR_RESET);
        if (current_lang == 1) {
            printf(COLOR_GOLD "                                     === %s KULÜBESİ ===\n" COLOR_RESET, profile->god_alignment);
        } else {
            printf(COLOR_GOLD "                                     === SHRINE OF %s ===\n" COLOR_RESET, profile->god_alignment);
        }
        printf(COLOR_DARK "  ==============================================================================================================\n" COLOR_RESET);

        // Yeni iç mekan tasarım fonksiyonunu çağırır (Ekran hatasını çözen kısım budur)
        draw_shrine_interior(profile->god_alignment);

        printf(COLOR_DARK  "  ==============================================================================================================\n" COLOR_RESET);

        print_mythic_date(); // Mitolojik tarihi yazdırır

        if (current_lang == 1) {
            printf(COLOR_WHITE "\n  Yatağından kalktın. Burası senin dinlendiğin güvenli alanın.\n\n" COLOR_RESET);

            printf("  [" COLOR_CYAN "1" COLOR_RESET "] Tapınak Köyüne Çık (Dış Dünyaya Adım At)\n");
            printf("  [" COLOR_CYAN "0" COLOR_RESET "] Uykuya Dal (Sistemi Kapat ve Çık)\n\n");

            printf(COLOR_CYAN "  Eylem Seçimi: " COLOR_RESET);
        } else {
            printf(COLOR_WHITE "\n  You rise from your bed. This is your safe haven to rest.\n\n" COLOR_RESET);

            printf("  [" COLOR_CYAN "1" COLOR_RESET "] Step out to the Village of Shrines (Enter the Outside World)\n");
            printf("  [" COLOR_CYAN "0" COLOR_RESET "] Fall Asleep (Exit System)\n\n");

            printf(COLOR_CYAN "  Select Action: " COLOR_RESET);
        }

        bool valid_input = false;
        while (!valid_input) {
            if (_kbhit()) {
                char ch = _getch();
                if (ch == '1') {
                    scene_inner_shrine(profile); // Dışarı çıkış
                    valid_input = true;
                } else if (ch == '0') {
                    in_shrine = false; // Döngüyü kırar ve ana menüye döner
                    valid_input = true;
                    clear_screen();
                }
            }
            Sleep(20);
        }
    }
}

