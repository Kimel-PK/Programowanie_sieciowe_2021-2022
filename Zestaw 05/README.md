# Zestaw 05

Od protokołów przesyłających dane w datagramach przechodzimy do protokołów korzystających z transportu strumieniowego. Dalej będziemy rozważać sumowanie liczb, ale teraz zapytania i odpowiedzi będą przesyłane za pomocą TCP.

## Zadanie 1

Napisz specyfikację strumieniowego protokołu sumowania liczb. Dopuść możliwość przesyłania przez jedno połączenie wielu ciągów liczb do zsumowania i wielu odpowiedzi (obliczonych sum albo komunikatów o wystąpieniu błędu). Zastanów się, czego użyć jako terminatora mówiącego "w tym miejscu kończy się ciąg liczb" — dwuznaku `\r\n`, tak jak w wielu innych protokołach sieciowych? A może czegoś innego (ale wtedy miej jakieś uzasadnienie odejścia od powszechnie przyjętej konwencji)? Czy odpowiedzi serwera będą używać takiego samego terminatora?

Rozważ, czy trzeba do specyfikacji dodawać warunek ograniczający długość przesyłanych przez klienta zapytań, np. 1024 bajty łącznie z terminatorem. To ułatwiłoby implementowanie serwera, bo dzięki temu programista piszący serwer mógłby zadeklarować roboczy bufor o rozmiarze 1024 bajtów i to na pewno wystarczyłoby, aby wczytać do niego całe zapytanie. Ale czy to jest niezbędne? Czy problem dodawania liczb wymaga, aby serwer odebrał całe zapytanie, zanim zacznie je przetwarzać?

### Opowiedź Z1

Komunikacja między klientem a serwerem odbywa się z użyciem ramek TCP. Klient wysyła do serwera liczby, a serwer sumuje je i zwraca wynik.

Dane wysyłane od klienta:

- dozwolone znaki ASCII w ramce to `0123456789`, `' '` (spacja) oraz terminator `\0`
- maksymalna długość poprawnej liczby zapisanej w ramce to 20 znaków (maksymalna liczba znaków możliwa do przechowania w zmiennej typu `unsigned long int`)
- ciągi cyfr oddzielone jednym lub więcej znakiem spacji są traktowane jako pojedyncza liczba
  - liczby, zaczynające się od jednego lub więcej `0` są traktowane jako poprawne i zostaną przycięte zgodnie z prawami matematyki
  - ciąg jednego lub większej ilości `0` jest uznawany za `0`
- zapytanie zadane serwerowi musi kończyć się terminatorem - znakiem `\0`
- puste zapytanie traktowane jest jako poprawne i zwróci wynik równy `0`
- w jednej ramce może znajdować się kilka zapytań zakończonych terminatorami lub fragmenty zapytań
- w sytuacji gdy klient nie zakończy jawnie połączenia, serwer zakończy je w ciągu 60 sekund

> Nie musimy tworzyć bufora, który będzie w stanie pomieścić całe zapytanie lub ograniczać klienta do wysyłania pełnego zapytania za kazdym razem. Serwer będzie przetwarzał zapytanie znak po znaku i w razie gdy trafi na niepełne zapytanie czekał na kolejną porcję danych

Dane zwracane przez serwer:

- dozwolone znaki ASCII w ramce to `0123456789` lub ciąg znaków `ERROR` oznaczający błąd serwera
- serwer odpowiada jedną ramką na wszystkie zapytania, które zostały mu zadane (jeśli klient wyśle w jednej ramce 2 zapytania - nie wykluczając fragmentu następnego - serwer odpowie na oba w tej samej ramce z wykorzystaniem terminatora `\0`)
- serwer przetwarza zapytanie znak po znaku, jeśli trafi na liczbę dłuższą niż 20 cyfr lub odczyta niedozwolony znak zwróci ciąg symbolizujący błąd
- przepełnienie podczas dodawania jest traktowane jako błąd i odeśle do klienta informację o nim

> Terminator `\0` wybieram, ponieważ serwer przetwarza zapytania znak po znaku. Trafienie na `\r\n` jest trudniej przetworzyć niż pojedynczy znak końca. Jak reagować kiedy trafimy na `\r` ale następnym znakiem będzie jakiś inny? Co jeśli trafimy na `\r` na końcu ramki? Będziemy musieli czekać na następną żeby sprawdzić czy wystąpił błąd czy mamy odesłać klientowi odpowiedź. Ponadto w C/C++ znak `\0` jest traktowany jako koniec zmiennej typu `string` co sprawdza się logicznie w tej sytuacji.

## Zadanie 2

Zastanów się nad algorytmem serwera. Będzie on musiał być bardziej złożony niż w przypadku serwera UDP. Tam pojedyncza operacja odczytu zawsze zwracała jeden kompletny datagram, czyli jeden kompletny ciąg liczb do zsumowania. W przypadku połączeń TCP niestety tak łatwo nie jest.

Po pierwsze, jeśli klient od razu po nawiązaniu połączenia wysłał kilka zapytań jedno za drugim, to serwer może je odebrać sklejone ze sobą. Pojedyncza operacja odczytu ze strumienia może np. zwrócić 15 bajtów odpowiadających znakom `2 2\r\n10 22 34\r\n` - jak widać są to dwa ciągi liczb. Serwer w odpowiedzi powinien zwrócić `4\r\n66\r\n`.

Po drugie, operacja odczytu może zwrócić tylko początkową część zapytania. Kod serwera musi wtedy ponownie wywołać `read()`. Takie ponawianie odczytów i odbieranie kolejnych fragmentów ciągu liczb musi trwać aż do chwili odebrania `\r\n` — dopiero wtedy wiemy, że dotarliśmy do końca zapytania.

Po trzecie, mogą się zdarzyć oba powyższe przypadki równocześnie. Serwer może np. odczytać ze strumienia 9 bajtów odpowiadających znakom `2 2\r\n10 2`.

### Odpowiedź Z2

Serwer powinien przetwarzać zapytanie znak po znaku. Wykluczy to problemy z niepełnymi lub wieloma zapytaniami. Wywołujemy metodę `read()` ładujemy odczytane bajty do bufora. Przetwarzamy bufor a kiedy skończymy wywołujemy metodę `read()` ponownie.

## Zadanie 3

Spróbuj rozpisać w formie pseudokodu algorytm serwera obsługujący powyższe komplikacje i starannie przeanalizuj, czy na pewno poradzi on sobie nawet przy założeniu maksymalnie złej woli ze strony klienta.

> Polecam Państwa uwadze przedstawioną na wykładzie koncepcję automatu przetwarzającego kolejne bajty z wejścia. Automat nie potrzebuje bufora z kompletnym ciągiem liczb, po prostu konsumuje bajty w miarę tego jak nadchodzą, więc będzie w stanie przetwarzać dowolnie długie zapytania. Użycie automatu powinno wielce ułatwić zaprojektowanie poprawnie działającego algorytmu.

### Odpowiedź Z3

```text
Uruchom serwer
Czekaj na połączenie z klientem
Nawiąż połączenie z klientem

Dopóki istnieje połączenie z klientem

    Kopiuj dane przesłane przez klienta do bufora

    Dla każdego znaku znajdującego się w buforze:
        Jeśli znak to spacja
            Przetłumacz bufor liczby z tekstu na wartość
            Dodaj bufor liczby do sumy
            Jeśli wystąpiło przepełnienie
                Zgłoś błąd i ignoruj pozostałe znaki w tym zapytaniu
            Opróżnij bufor liczby
        Jeśli znak to cyfra
            Dopisz znak do bufora liczby
        Jeśli znak to niedozwolony znak
            Zgłoś błąd i ignoruj pozostałe znaki w tym zapytaniu
        Jeśli znak to terminator
            Przetłumacz bufor liczby z tekstu na wartość
            Dodaj bufor liczby do sumy
            Jeśli wystąpiło przepełnienie
                Zgłoś błąd i ignoruj pozostałe znaki w tym zapytaniu
            Opróżnij bufor liczby
            Zapisz odpowiedź w buforze odpowiedzi

    Jeśli bufor odpowiedzi zawiera jakieś znaki
        Wyślij bufor odpowiedzi użytkownikowi
```

## Zadanie 4 (nieobowiązkowe)

Jeśli chcesz, możesz już teraz zaimplementować taki algorytm w C++, Pythonie lub Javie. Zdobyte doświadczenie i napisany kod przydadzą się na następnych zajęciach.
