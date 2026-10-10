#include "rezervasyon.h"

#include <stdio.h>
#include <string.h>

/* Dosya bicimi: her satir bir bilet -> otobus;koltuk;ad;soyad */
#define AYIRICI ';'

bool otobus_gecerli(char otobus) {
    return otobus >= 'A' && otobus < 'A' + OTOBUS_SAYISI;
}

bool koltuk_gecerli(int koltuk) {
    return koltuk >= 1 && koltuk <= KOLTUK_SAYISI;
}

bool isim_gecerli(const char *isim) {
    size_t uzunluk = strlen(isim);
    return uzunluk > 0 && uzunluk < AD_UZUNLUGU && strchr(isim, AYIRICI) == NULL;
}

/* Kaydin listedeki sirasini, yoksa -1 dondurur. */
static int kayit_sirasi(const RezervasyonListesi *liste, char otobus, int koltuk) {
    for (int i = 0; i < liste->adet; i++) {
        if (liste->kayitlar[i].otobus == otobus && liste->kayitlar[i].koltuk == koltuk) {
            return i;
        }
    }
    return -1;
}

const Rezervasyon *rezervasyon_bul(const RezervasyonListesi *liste, char otobus, int koltuk) {
    int sira = kayit_sirasi(liste, otobus, koltuk);
    return sira < 0 ? NULL : &liste->kayitlar[sira];
}

bool rezervasyon_ekle(RezervasyonListesi *liste, char otobus, int koltuk,
                      const char *ad, const char *soyad) {
    if (!otobus_gecerli(otobus) || !koltuk_gecerli(koltuk)) return false;
    if (!isim_gecerli(ad) || !isim_gecerli(soyad)) return false;
    if (liste->adet >= EN_FAZLA_KAYIT) return false;
    if (kayit_sirasi(liste, otobus, koltuk) >= 0) return false;  /* koltuk dolu */

    Rezervasyon *yeni = &liste->kayitlar[liste->adet++];
    yeni->otobus = otobus;
    yeni->koltuk = koltuk;
    snprintf(yeni->ad, sizeof yeni->ad, "%s", ad);
    snprintf(yeni->soyad, sizeof yeni->soyad, "%s", soyad);
    return true;
}

bool rezervasyon_sil(RezervasyonListesi *liste, char otobus, int koltuk) {
    int sira = kayit_sirasi(liste, otobus, koltuk);
    if (sira < 0) return false;

    /* Son kaydi silinenin yerine tasi; listede sira onemli degil. */
    liste->kayitlar[sira] = liste->kayitlar[liste->adet - 1];
    liste->adet--;
    return true;
}

int dolu_koltuk_sayisi(const RezervasyonListesi *liste, char otobus) {
    int dolu = 0;
    for (int i = 0; i < liste->adet; i++) {
        if (liste->kayitlar[i].otobus == otobus) dolu++;
    }
    return dolu;
}

bool rezervasyonlari_yukle(RezervasyonListesi *liste, const char *dosya_yolu, int *atlanan_satir) {
    liste->adet = 0;
    if (atlanan_satir != NULL) *atlanan_satir = 0;

    FILE *dosya = fopen(dosya_yolu, "r");
    if (dosya == NULL) return true;  /* ilk calistirma: henuz kayit yok */

    char satir[128];
    while (fgets(satir, sizeof satir, dosya) != NULL) {
        char otobus;
        int koltuk;
        char ad[AD_UZUNLUGU], soyad[AD_UZUNLUGU];

        /* %31[^;] : ayiriciya kadar en fazla 31 karakter oku (tasmayi onler) */
        bool okundu = sscanf(satir, " %c;%d;%31[^;];%31[^\r\n]", &otobus, &koltuk, ad, soyad) == 4;
        if (!okundu || !rezervasyon_ekle(liste, otobus, koltuk, ad, soyad)) {
            if (atlanan_satir != NULL && satir[0] != '\n' && satir[0] != '\r') (*atlanan_satir)++;
        }
    }

    bool hata = ferror(dosya) != 0;
    fclose(dosya);
    return !hata;
}

bool rezervasyonlari_kaydet(const RezervasyonListesi *liste, const char *dosya_yolu) {
    FILE *dosya = fopen(dosya_yolu, "w");
    if (dosya == NULL) return false;

    for (int i = 0; i < liste->adet; i++) {
        const Rezervasyon *r = &liste->kayitlar[i];
        fprintf(dosya, "%c%c%d%c%s%c%s\n", r->otobus, AYIRICI, r->koltuk, AYIRICI, r->ad, AYIRICI, r->soyad);
    }

    bool hata = ferror(dosya) != 0;
    return fclose(dosya) == 0 && !hata;
}
