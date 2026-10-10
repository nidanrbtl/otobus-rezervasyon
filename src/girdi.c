#include "girdi.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool satir_oku(const char *istek, char *hedef, size_t boyut) {
    char tampon[256];

    printf("%s", istek);
    fflush(stdout);
    if (fgets(tampon, sizeof tampon, stdin) == NULL) return false;

    /* Satir tampona sigmadiysa kalanini oku ve at; sonraki soruya tasmasin. */
    if (strchr(tampon, '\n') == NULL) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {
        }
    }

    char *bas = tampon;
    while (isspace((unsigned char)*bas)) bas++;

    char *son = bas + strlen(bas);
    while (son > bas && isspace((unsigned char)son[-1])) son--;
    *son = '\0';

    snprintf(hedef, boyut, "%s", bas);
    return true;
}

bool sayi_oku(const char *istek, int alt, int ust, int *sonuc) {
    char satir[32];

    while (satir_oku(istek, satir, sizeof satir)) {
        char *bitis;
        errno = 0;
        long deger = strtol(satir, &bitis, 10);

        bool sayi_mi = bitis != satir && *bitis == '\0' && errno == 0;
        if (sayi_mi && deger >= alt && deger <= ust) {
            *sonuc = (int)deger;
            return true;
        }
        printf("Lutfen %d ile %d arasinda bir sayi giriniz.\n", alt, ust);
    }
    return false;
}

bool harf_duyarsiz_esit(const char *a, const char *b) {
    while (*a != '\0' && *b != '\0') {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return false;
        a++;
        b++;
    }
    return *a == *b;
}
