# Zestaw 04

## Zadanie 1

Przetestuj ręcznie serwer UDP dodający liczby. Jeśli akceptuje końcowe `\r\n`, to możesz to zrobić uruchamiając

```bash
ncat --udp --crlf 127.0.0.1 2020
```

albo

```bash
socat stdio udp4:127.0.0.1:2020,crlf
```

i wpisując kolejne zapytania z klawiatury.

Zapytanie bez końcowego `\r\n` czy też `\n` można wygenerować np. poleceniem `printf` podłączonym do wejścia socata:

```bash
printf "1 23 7" | socat -t 5.0 stdio udp4:127.0.0.1:2020
```

Przełącznik `-t 5.0` nakazuje socatowi odczekać pięć sekund po zakończeniu wysyłania danych do serwera i zakończyć działanie. Jakiś timeout jest niezbędny, bo socat nie wie, że nasze sumatory zwracają dokładnie jeden datagram. W ogólnym przypadku odpowiedź z serwera UDP może się przecież składać z wielu datagramów, a na poziomie bezpołączeniowego protokołu transportowego, jakim jest UDP, nie ma po czym poznać że już je wszystkie odebrano.

## Zadanie 2

Często spotykanym w poprzednich latach błędem było zwracanie przez serwer dodatkowych bajtów o wartości zero, bo np. ktoś w kodzie zadeklarował sobie `char wynik[20]`, użył `sprintf` aby "wydrukować" do tej tablicy tekstową reprezentację obliczonej sumy (która zajęła tylko kilka początkowych elementów tablicy), a potem przez pomyłkę wysłał klientowi całą 20-bajtową tablicę. Ten błąd łatwo przegapić, bo bajty o wartości zero są niewidoczne gdy się je wyświetla na ekranie.

Aby sprawdzić jakie dokładnie bajty są w strumieniu danych trzeba ten strumień wysłać nie wprost na ekran, lecz np. na wejście programu `od`. Proszę porównać to, co wypisują dwa poniższe polecenia:

```bash
printf "abc ijk\0xyz\n"

printf "abc ijk\0xyz\n" | od -A d -t u1 -t c
```

Użyte przełączniki nakazują wyświetlić kolejne bajty w postaci dziesiętnej oraz jako znaki ASCII (bajty odpowiadające niedrukowalnym znakom kontrolnym są wyświetlane jako sekwencje z backslashem na początku).

Proszę spróbować zapisać zwrócone przez serwer dane do pliku, a potem ten plik wyświetlić za pomocą `od`:

```bash
ncat --udp --crlf 127.0.0.1 2020 > wynik-z-serwera.txt

printf "2 2" | socat -t 5.0 stdio udp4:127.0.0.1:2020 > wynik-z-serwera.txt

od -A d -t u1 -t c < wynik-z-serwera.txt
```

## Zadanie 3

Możliwość przekierowania równocześnie wejścia i wyjścia socata można wykorzystać do stworzenia powtarzalnych testów. Załóżmy, że w pliku `test-dane.txt` jest ciąg bajtów składający się na zapytanie testowe. Uruchamiamy

```bash
socat -t 5.0 stdio udp4:127.0.0.1:2020 < test-dane.txt > wynik-z-serwera.txt
```

Jeśli przygotowaliśmy również plik `test-wynik.txt`, to przy pomocy poleceń `cmp` albo `diff` można łatwo porównać zawartość pliku `wynik-z-serwera.txt` ze wzorcowym wynikiem.

socat nie radzi sobie z datagramami mającymi długość zero bajtów, nie da się więc przy jego pomocy wysłać pustego zapytania. Jeśli chcesz sprawdzić, jak sumator reaguje na pusty datagram, to zamiast socata użyj programu `mini-udpcat.py`, który został napisany specjalnie na potrzeby tych zajęć.

## Zadanie 4

Przygotuj kilka par plików z przykładowymi zapytaniami i oczekiwanymi wynikami. Uwzględnij także błędne zapytania, na które odpowiedzią powinno być `ERROR`

Wymień się tymi plikami z dwiema-trzema innymi osobami z grupy. Sprawdź, czy Twój serwer poprawnie obsługuje zapytania przygotowane przez inne osoby. Jeśli nie, to spróbujcie wspólnie ustalić przyczynę: różnice w rozumieniu specyfikacji protokołu, korzystanie w teście z opcjonalnej funkcjonalności, którą nie wszystkie serwery muszą implementować (u nas: `\r\n` na końcu datagramu), błędy w kodzie serwera, coś innego?

## Zadanie 5 (nieobowiązkowe)

Przygotuj sobie narzędzie automatycznie testujące sumator w oparciu o powyższe pliki z zapytaniami i odpowiedziami. Na przykład prosty skrypt w języku uniksowej powłoki, wywołujący polecenia używane w poprzednich punktach. Możesz też napisać program w C albo innym języku, wczytujący te pliki oraz komunikujący się przez gniazdko z serwerem. Zanim jednak się zabierzesz za jego pisanie, to lepiej sprawdź czy w sieci nie da się znaleźć gotowego narzędzia do testowania usług UDP — wielce możliwe, że ktoś już coś takiego zaimplementował.
