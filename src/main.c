/*
 * Otobus Rezervasyon Sistemi
 *
 * Uc otobus (A, B, C) ve her birinde 20 koltuk icin konsoldan bilet satisi,
 * iptali ve koltuk durumu sorgulama. Biletler rezervasyonlar.txt dosyasinda
 * saklanir; program kapatilip acildiginda kayitlar korunur.
 *
 * Kaynak dosyalar bilerek yalnizca ASCII karakter kullanir: Windows konsolu ve
 * eski derleyicilerde Turkce karakterler bozuk gorunebiliyor.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "girdi.h"
#include "rezervasyon.h"

#define KAYIT_DOSYASI "rezervasyonlar.txt"

/* Ornek giris bilgileri. Gercek bir sistemde sifre kaynak kodda durmaz. */
#define KULLANICI_ADI "nida"
#define SIFRE "1234"
#define GIRIS_HAKKI 3

/* Koltuk plani: her sirada 2 + 2 koltuk */
#define SIRADAKI_KOLTUK 4

static bool giris_yap(void) {
    char kullanici_adi[50], sifre[50];

    for (int kalan = GIRIS_HAKKI; kalan > 0; kalan--) {
        if (!satir_oku("Kullanici adi: ", kullanici_adi, sizeof kullanici_adi)) return false;
        if (!satir_oku("Sifre: ", sifre, sizeof sifre)) return false;

        /* Kullanici adinda buyuk-kucuk harf onemsiz, sifrede onemli. */
        if (harf_duyarsiz_esit(kullanici_adi, KULLANICI_ADI) && strcmp(sifre, SIFRE) == 0) {
            return true;
        }
        printf("Girilen bilgiler yanlis. Kalan hak: %d\n\n", kalan - 1);
    }
    return false;
}

/* Gecerli bir otobus harfi girilene kadar sorar; kucuk harf de kabul edilir. */
static bool otobus_sor(char *otobus) {
    char satir[16];

    while (satir_oku("Otobus (A, B, C): ", satir, sizeof satir)) {
        char secim = satir[0];
        if (secim >= 'a' && secim <= 'z') secim = (char)(secim - 'a' + 'A');

        if (strlen(satir) == 1 && otobus_gecerli(secim)) {
            *otobus = secim;
            return true;
        }
        printf("Gecersiz otobus secimi.\n");
    }
    return false;
}

static bool isim_sor(const char *istek, char *hedef, size_t boyut) {
    while (satir_oku(istek, hedef, boyut)) {
        if (isim_gecerli(hedef)) return true;
        printf("Bu alan bos birakilamaz ve ';' iceremez.\n");
    }
    return false;
}

static void koltuk_plani_yazdir(const RezervasyonListesi *liste, char otobus) {
    printf("\nOtobus %c  (dolu: %d / %d)\n", otobus, dolu_koltuk_sayisi(liste, otobus), KOLTUK_SAYISI);

    for (int koltuk = 1; koltuk <= KOLTUK_SAYISI; koltuk++) {
        if (rezervasyon_bul(liste, otobus, koltuk) != NULL) {
            printf(" [XX]");
        } else {
            printf(" [%2d]", koltuk);
        }

        if (koltuk % SIRADAKI_KOLTUK == 0) {
            printf("\n");
        } else if (koltuk % SIRADAKI_KOLTUK == SIRADAKI_KOLTUK / 2) {
            printf("   ");  /* koridor */
        }
    }
    if (KOLTUK_SAYISI % SIRADAKI_KOLTUK != 0) printf("\n");
    printf(" [XX] = dolu\n");
}

/* Degisikligi dosyaya yazar; yazilamazsa kullaniciyi uyarir. */
static void kaydet(const RezervasyonListesi *liste) {
    if (!rezervasyonlari_kaydet(liste, KAYIT_DOSYASI)) {
        printf("UYARI: %s dosyasina yazilamadi; degisiklik kalici olmayabilir.\n", KAYIT_DOSYASI);
    }
}

static bool bilet_sat(RezervasyonListesi *liste) {
    char ad[AD_UZUNLUGU], soyad[AD_UZUNLUGU], otobus;
    int koltuk;

    if (!isim_sor("Ad: ", ad, sizeof ad)) return false;
    if (!isim_sor("Soyad: ", soyad, sizeof soyad)) return false;
    if (!otobus_sor(&otobus)) return false;

    if (dolu_koltuk_sayisi(liste, otobus) == KOLTUK_SAYISI) {
        printf("Otobus %c tamamen dolu.\n", otobus);
        return true;
    }

    koltuk_plani_yazdir(liste, otobus);
    if (!sayi_oku("Koltuk no: ", 1, KOLTUK_SAYISI, &koltuk)) return false;

    if (rezervasyon_ekle(liste, otobus, koltuk, ad, soyad)) {
        kaydet(liste);
        printf("Bilet kesildi: %s %s, otobus %c, koltuk %d\n", ad, soyad, otobus, koltuk);
    } else {
        printf("Koltuk %d dolu. Lutfen baska bir koltuk seciniz.\n", koltuk);
    }
    return true;
}

static bool bilet_iptal(RezervasyonListesi *liste) {
    char otobus, soyad[AD_UZUNLUGU];
    int koltuk;

    if (!otobus_sor(&otobus)) return false;
    if (!sayi_oku("Koltuk no: ", 1, KOLTUK_SAYISI, &koltuk)) return false;

    const Rezervasyon *kayit = rezervasyon_bul(liste, otobus, koltuk);
    if (kayit == NULL) {
        printf("Otobus %c, koltuk %d icin bilet bulunamadi.\n", otobus, koltuk);
        return true;
    }

    /* Baskasinin biletini iptal etmeyi zorlastirmak icin soyadi dogrulanir. */
    if (!isim_sor("Bilet sahibinin soyadi: ", soyad, sizeof soyad)) return false;
    if (!harf_duyarsiz_esit(soyad, kayit->soyad)) {
        printf("Soyad bilet kaydiyla eslesmiyor; iptal edilmedi.\n");
        return true;
    }

    printf("%s %s adina kesilen bilet iptal edildi.\n", kayit->ad, kayit->soyad);
    rezervasyon_sil(liste, otobus, koltuk);
    kaydet(liste);
    return true;
}

static bool koltuk_durumu(const RezervasyonListesi *liste) {
    char otobus;
    if (!otobus_sor(&otobus)) return false;

    koltuk_plani_yazdir(liste, otobus);
    for (int koltuk = 1; koltuk <= KOLTUK_SAYISI; koltuk++) {
        const Rezervasyon *kayit = rezervasyon_bul(liste, otobus, koltuk);
        if (kayit != NULL) printf(" %2d  %s %s\n", koltuk, kayit->ad, kayit->soyad);
    }
    return true;
}

int main(void) {
    RezervasyonListesi liste;
    int atlanan;

    if (!giris_yap()) {
        printf("Giris basarisiz.\n");
        return EXIT_FAILURE;
    }

    if (!rezervasyonlari_yukle(&liste, KAYIT_DOSYASI, &atlanan)) {
        printf("%s okunamadi.\n", KAYIT_DOSYASI);
        return EXIT_FAILURE;
    }
    if (atlanan > 0) printf("UYARI: %s icinde %d bozuk satir atlandi.\n", KAYIT_DOSYASI, atlanan);

    printf("\n---------- Otobus Rezervasyon Sistemine Hos Geldiniz ----------\n");

    bool devam = true;
    while (devam) {
        int secim;

        printf("\n1 - Bilet satin al\n");
        printf("2 - Bilet iptal et\n");
        printf("3 - Otobus ve koltuk durumunu gor\n");
        printf("4 - Cikis\n");
        if (!sayi_oku("Seciminiz: ", 1, 4, &secim)) break;

        switch (secim) {
            case 1: devam = bilet_sat(&liste); break;
            case 2: devam = bilet_iptal(&liste); break;
            case 3: devam = koltuk_durumu(&liste); break;
            default: devam = false; break;
        }
    }

    printf("\nSistemden cikiliyor...\n");
    return EXIT_SUCCESS;
}
