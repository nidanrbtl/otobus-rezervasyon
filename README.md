# Otobüs Rezervasyon Sistemi

C ile yazılmış, konsolda çalışan küçük bir bilet rezervasyon uygulaması. Üç
otobüs (A, B, C) ve her birinde 20 koltuk için bilet satışı, iptali ve koltuk
durumu sorgulama yapar. Biletler bir metin dosyasında saklandığı için program
kapatılıp açıldığında kayıtlar korunur.

## Özellikler

- Kullanıcı adı ve şifreyle giriş (3 deneme hakkı)
- Bilet satışı: koltuk planı gösterilir, dolu koltuk ikinci kez satılamaz
- Bilet iptali: otobüs ve koltuk numarasıyla, soyadı doğrulanarak
- Koltuk durumu: koltuk planı ve yolcu listesi
- Hatalı girdiye dayanıklılık: harf yerine sayı, aralık dışı koltuk, boş ad
  gibi durumlarda program çökmez, yeniden sorar

```
Otobus A  (dolu: 1 / 20)
 [ 1] [ 2]    [ 3] [ 4]
 [XX] [ 6]    [ 7] [ 8]
 [ 9] [10]    [11] [12]
 [13] [14]    [15] [16]
 [17] [18]    [19] [20]
 [XX] = dolu
```

## Derleme ve çalıştırma

Bir C11 derleyicisi yeterlidir; standart kütüphane dışında bağımlılık yoktur.

```bash
gcc -std=c11 -Wall -Wextra -o otobus src/main.c src/rezervasyon.c src/girdi.c
./otobus            # Windows: otobus.exe
```

`make` kuruluysa `make` ile derlenir, `make test` ile testler çalışır.

Örnek giriş bilgileri: kullanıcı adı `nida`, şifre `1234`. Bunlar gösterim
amaçlıdır ve `src/main.c` içinde tanımlıdır; gerçek bir sistemde şifre kaynak
kodda tutulmaz.

## Yapı

```
src/
├── main.c          menü, giriş ve ekran çıktıları
├── rezervasyon.c   bilet listesi, dosyaya kaydetme ve dosyadan okuma
├── rezervasyon.h
├── girdi.c         klavyeden güvenli girdi okuma
└── girdi.h
test/
└── test_rezervasyon.c
```

Biletler `rezervasyonlar.txt` dosyasında, her satırda bir bilet olacak şekilde
tutulur:

```
A;5;Ayse Nur;Yilmaz
B;20;Zeynep;Demir
```

## Tasarım notları

- **Girdi `scanf("%s")` ile okunmaz.** Uzun bir girdi tamponu taşırabileceği
  için bütün girdiler `fgets` ile satır olarak okunur, sayılar `strtol` ile
  çevrilip aralığı kontrol edilir.
- **Koltuk durumu ayrı tutulmaz.** Bir koltuğun dolu olup olmadığı bilet
  listesinden bulunur; böylece "koltuk dolu görünüyor ama bileti yok" gibi bir
  tutarsızlık oluşamaz.
- **Bozuk kayıt dosyası programı durdurmaz.** Okunamayan ya da geçersiz satırlar
  atlanır ve kaç satırın atlandığı kullanıcıya bildirilir.
- **Kaynak kodda Türkçe karakter kullanılmadı.** Windows konsolunda ve eski
  derleyicilerde bu karakterler bozuk görünebildiği için ekran metinleri ve
  yorumlar ASCII ile yazıldı.

## Sınırlamalar

- Tek kullanıcı ve tek sefer için tasarlanmıştır; tarih, güzergâh ve ücret yoktur.
- Kayıt dosyası düz metindir, şifreleme ya da eşzamanlı erişim koruması yoktur.
