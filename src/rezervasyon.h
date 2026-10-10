/* Rezervasyon verisi: bellekteki liste ve dosyaya kaydetme / dosyadan okuma. */
#ifndef REZERVASYON_H
#define REZERVASYON_H

#include <stdbool.h>

#define OTOBUS_SAYISI 3   /* A, B, C */
#define KOLTUK_SAYISI 20  /* her otobuste */
#define AD_UZUNLUGU 32
#define EN_FAZLA_KAYIT (OTOBUS_SAYISI * KOLTUK_SAYISI)

typedef struct {
    char otobus;  /* 'A', 'B' veya 'C' */
    int koltuk;   /* 1..KOLTUK_SAYISI */
    char ad[AD_UZUNLUGU];
    char soyad[AD_UZUNLUGU];
} Rezervasyon;

typedef struct {
    Rezervasyon kayitlar[EN_FAZLA_KAYIT];
    int adet;
} RezervasyonListesi;

bool otobus_gecerli(char otobus);
bool koltuk_gecerli(int koltuk);

/* Ad ve soyad bos olamaz, dosyadaki ayirici karakteri (;) iceremez. */
bool isim_gecerli(const char *isim);

/* Koltuk doluysa kaydi, bossa NULL dondurur. */
const Rezervasyon *rezervasyon_bul(const RezervasyonListesi *liste, char otobus, int koltuk);

/* Koltuk bossa ve bilgiler gecerliyse kaydi ekler. */
bool rezervasyon_ekle(RezervasyonListesi *liste, char otobus, int koltuk,
                      const char *ad, const char *soyad);

/* Koltuk doluysa kaydi siler. */
bool rezervasyon_sil(RezervasyonListesi *liste, char otobus, int koltuk);

int dolu_koltuk_sayisi(const RezervasyonListesi *liste, char otobus);

/* Dosya yoksa liste bos baslar ve true doner. Bozuk satirlar atlanir;
   atlanan satir sayisi atlanan_satir'a yazilir (NULL verilebilir). */
bool rezervasyonlari_yukle(RezervasyonListesi *liste, const char *dosya_yolu, int *atlanan_satir);

bool rezervasyonlari_kaydet(const RezervasyonListesi *liste, const char *dosya_yolu);

#endif
