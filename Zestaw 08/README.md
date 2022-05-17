# Zestaw 08

Kontynuacja poprzednich ćwiczeń, zajmujemy się techniką znaną jako `web scraping`, czyli zautomatyzowanym pobieraniem danych z witryn internetowych.

## Zadanie 1

Proszę znaleźć stronę WWW zawierającą jakąś potencjalnie potrzebną informację (aktualna temperatura w Krakowie, kurs dolara, itp.), a następnie napisać program ściągający tę stronę i wyłuskujący z niej te dane. Dane te można zapisywać do jakiegoś pliku lub drukować na `stdout`, nie jest to ważne — ważne za to jest to, aby format zapisywanych danych pozwalał na ich wygodne dalsze przetwarzanie. Jeśli programowi z jakiejkolwiek przyczyny nie uda się ściągnąć poszukiwanej informacji, to musi zakończyć swe działanie zwracając z `main()` kod porażki.

Radzę wykorzystać kod programu z poprzednich zajęć — ściąganie już w nim jest, trzeba tylko dodać ekstrahowanie interesujących nas danych.

Wyłuskiwanie danych ze strony HTML jest dość kruchą techniką, bo witryna może nieoczekiwanie (dla nas) zmienić wygląd bądź zawartość. Biorąc to pod uwagę łatwo zauważyć, że podejście typu "zwróć bajty od 5078 do 5081" jest skazane na rychłą porażkę; "zwróć zawartość czwartego elementu `<p>` znajdującego się wewnątrz elementu `<div>` o identyfikatorze `temp`" jest lepsze. Warto postarać się o to, aby program zauważał nieoczekiwane bądź podejrzane sytuacje i je zgłaszał (np. jeśli w tym czwartym `<p>` jest ciąg znaków nie będący liczbą, to raczej nie jest to temperatura; jeśli znaleziona liczba wykracza poza przedział `[-30, 40]` to raczej nie jest to temperatura w stopniach Celsjusza).

Nawigację po treści strony ułatwia zbudowanie drzewa obiektów reprezentujących elementy strony HTML; ponownie polecam Waszej uwadze bibliotekę `Beautiful Soup` (`Python`) i jej odpowiednik `JSoup` (`Java`). Do sprawdzania czy łańcuch znaków pasuje do zadanego wzorca dobrze nadają się wyrażenia regularne.

Może się zdarzyć, że traficie na stronę, która wewnątrz przeglądarki wyświetla potrzebne nam informacje, ale gdy się ją ściągnie przez `HTTP` to nigdzie w jej treści nie można ich znaleźć. Prawie na pewno przyczyną jest jej dynamiczna, `AJAX`-owa natura. Takie strony mają puste miejsca, wypełniane zawartością ściąganą przez `javascript`owy kod z innych `URL`-i.

Współczesne przeglądarki mają wbudowane narzędzia deweloperskie, jednym z nich jest analizator połączeń sieciowych. Można przy jego pomocy spróbować odnaleźć rzeczywiste źródło potrzebnych nam danych. Poszukiwania proponuję zacząć od połączeń zainicjowanych przez `XHR` (czyli `javascript`ową klasę `XMLHttpRequest`) i / lub tych, które ściągały dokumenty typu `JSON`.

Udane znalezienie `JSON`-owego (lub `XML`-owego) źródła surowych danych jest dobrą wiadomością, bo ryzyko zmiany formatu tych danych jest znacznie mniejsze niż ryzyko zmiany struktury strony `HTML`.