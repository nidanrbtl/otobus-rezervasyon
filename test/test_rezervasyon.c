/* Rezervasyon mantiginin testleri. Calistirmak icin: make test */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../src/girdi.h"
#include "../src/rezervasyon.h"

#define TEST_DOSYASI "test_rezervasyonlar.tmp"

static void test_gecerlilik(void) {
    assert(otobus_gecerli('A') && otobus_gecerli('C'));
    assert(!otobus_gecerli('D') && !otobus_gecerli('a'));
    assert(koltuk_gecerli(1) && koltuk_gecerli(KOLTUK_SAYISI));
    assert(!koltuk_gecerli(0) && !koltuk_gecerli(KOLTUK_SAYISI + 1));
    assert(isim_gecerli("Ayse Nur"));
    assert(!isim_gecerli("") && !isim_gecerli("a;b"));
}

static void test_ekle_bul_sil(void) {
    RezervasyonListesi liste = {.adet = 0};

    assert(rezervasyon_ekle(&liste, 'A', 5, "Ayse", "Yilmaz"));
    assert(!rezervasyon_ekle(&liste, 'A', 5, "Mehmet", "Kaya"));  /* ayni koltuk iki kez satilamaz */
    assert(rezervasyon_ekle(&liste, 'B', 5, "Mehmet", "Kaya"));   /* baska otobuste ayni numara olur */
    assert(!rezervasyon_ekle(&liste, 'D', 1, "x", "y"));
    assert(!rezervasyon_ekle(&liste, 'A', 21, "x", "y"));

    const Rezervasyon *kayit = rezervasyon_bul(&liste, 'A', 5);
    assert(kayit != NULL && strcmp(kayit->soyad, "Yilmaz") == 0);
    assert(rezervasyon_bul(&liste, 'C', 5) == NULL);
    assert(dolu_koltuk_sayisi(&liste, 'A') == 1 && dolu_koltuk_sayisi(&liste, 'C') == 0);

    assert(rezervasyon_sil(&liste, 'A', 5));
    assert(!rezervasyon_sil(&liste, 'A', 5));  /* zaten silindi */
    assert(rezervasyon_bul(&liste, 'A', 5) == NULL);
    assert(rezervasyon_bul(&liste, 'B', 5) != NULL);  /* digerine dokunulmadi */
    assert(rezervasyon_ekle(&liste, 'A', 5, "Zeynep", "Demir"));  /* bosalan koltuk yeniden satilir */
}

static void test_otobus_dolar(void) {
    RezervasyonListesi liste = {.adet = 0};

    for (int otobus = 0; otobus < OTOBUS_SAYISI; otobus++) {
        for (int koltuk = 1; koltuk <= KOLTUK_SAYISI; koltuk++) {
            assert(rezervasyon_ekle(&liste, (char)('A' + otobus), koltuk, "Ad", "Soyad"));
        }
    }
    assert(liste.adet == EN_FAZLA_KAYIT);
    assert(dolu_koltuk_sayisi(&liste, 'B') == KOLTUK_SAYISI);
    assert(!rezervasyon_ekle(&liste, 'A', 1, "Ad", "Soyad"));
}

static void test_kaydet_yukle(void) {
    RezervasyonListesi yazilan = {.adet = 0}, okunan;
    int atlanan;

    assert(rezervasyon_ekle(&yazilan, 'A', 1, "Ayse Nur", "Yilmaz"));
    assert(rezervasyon_ekle(&yazilan, 'C', 20, "Mehmet", "Kaya"));
    assert(rezervasyonlari_kaydet(&yazilan, TEST_DOSYASI));

    assert(rezervasyonlari_yukle(&okunan, TEST_DOSYASI, &atlanan));
    assert(okunan.adet == 2 && atlanan == 0);
    const Rezervasyon *kayit = rezervasyon_bul(&okunan, 'A', 1);
    assert(kayit != NULL && strcmp(kayit->ad, "Ayse Nur") == 0 && strcmp(kayit->soyad, "Yilmaz") == 0);
    assert(rezervasyon_bul(&okunan, 'C', 20) != NULL);
    remove(TEST_DOSYASI);

    /* Dosya yoksa liste bos baslar. */
    assert(rezervasyonlari_yukle(&okunan, TEST_DOSYASI, &atlanan));
    assert(okunan.adet == 0 && atlanan == 0);
}

static void test_bozuk_dosya(void) {
    RezervasyonListesi okunan;
    int atlanan;

    FILE *dosya = fopen(TEST_DOSYASI, "w");
    assert(dosya != NULL);
    fputs("A;3;Ayse;Yilmaz\n"
          "bu satir bozuk\n"
          "Z;3;Gecersiz;Otobus\n"
          "B;99;Gecersiz;Koltuk\n"
          "A;3;Ayni;Koltuk\n"
          "\n"
          "B;7;Mehmet;Kaya\r\n",
          dosya);
    fclose(dosya);

    assert(rezervasyonlari_yukle(&okunan, TEST_DOSYASI, &atlanan));
    assert(okunan.adet == 2 && atlanan == 4);
    const Rezervasyon *kayit = rezervasyon_bul(&okunan, 'B', 7);
    assert(kayit != NULL && strcmp(kayit->soyad, "Kaya") == 0);  /* satir sonu (\r) temizlendi */
    remove(TEST_DOSYASI);
}

static void test_harf_duyarsiz(void) {
    assert(harf_duyarsiz_esit("Nida", "nida") && harf_duyarsiz_esit("", ""));
    assert(!harf_duyarsiz_esit("nida", "nidan") && !harf_duyarsiz_esit("nida", "nita"));
}

int main(void) {
    test_gecerlilik();
    test_ekle_bul_sil();
    test_otobus_dolar();
    test_kaydet_yukle();
    test_bozuk_dosya();
    test_harf_duyarsiz();
    printf("Butun testler gecti.\n");
    return 0;
}
