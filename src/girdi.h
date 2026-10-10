/* Klavyeden guvenli girdi okuma. Fonksiyonlar girdi biterse (EOF) false dondurur. */
#ifndef GIRDI_H
#define GIRDI_H

#include <stdbool.h>
#include <stddef.h>

/* Bir satir okur; bastaki ve sondaki bosluklari atar. Uzun satirin fazlasi yok sayilir. */
bool satir_oku(const char *istek, char *hedef, size_t boyut);

/* [alt, ust] araliginda bir tam sayi girilene kadar tekrar sorar. */
bool sayi_oku(const char *istek, int alt, int ust, int *sonuc);

/* Buyuk-kucuk harf ayrimi yapmadan karsilastirir. */
bool harf_duyarsiz_esit(const char *a, const char *b);

#endif
