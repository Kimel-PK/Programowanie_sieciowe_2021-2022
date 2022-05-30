#include <stdio.h>
#include <curl/curl.h>
#include <string>
#include <iostream>
#include <map>
#include <list>
#include <sstream>

#include "RSJparser.tcc"

using namespace std;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp){
	((string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

// https://stackoverflow.com/questions/5878775/how-to-find-and-replace-string
void replace_all(string& s, string const& toReplace, string const& replaceWith) {
    ostringstream oss;
    std::size_t pos = 0;
    std::size_t prevPos = pos;

    while (true) {
        prevPos = pos;
        pos = s.find(toReplace, pos);
        if (pos == std::string::npos)
            break;
        oss << s.substr(prevPos, pos - prevPos);
        oss << replaceWith;
        pos += toReplace.size();
    }

    oss << s.substr(prevPos);
    s = oss.str();
}

string NaprawKodowanie (string tekst) {
	replace_all (tekst, "\\u0104", "Ą");
	replace_all (tekst, "\\u0106", "Ć");
	replace_all (tekst, "\\u0118", "Ę");
	replace_all (tekst, "\\u0141", "Ł");
	replace_all (tekst, "\\u0143", "Ń");
	replace_all (tekst, "\\u00d3", "Ó");
	replace_all (tekst, "\\u015a", "Ś");
	replace_all (tekst, "\\u0179", "Ź");
	replace_all (tekst, "\\u017b", "Ż");
	replace_all (tekst, "\\u0105", "ą");
	replace_all (tekst, "\\u0107", "ć");
	replace_all (tekst, "\\u0119", "ę");
	replace_all (tekst, "\\u0142", "ł");
	replace_all (tekst, "\\u0144", "ń");
	replace_all (tekst, "\\u00f3", "ó");
	replace_all (tekst, "\\u015b", "ś");
	replace_all (tekst, "\\u017a", "ź");
	replace_all (tekst, "\\u017c", "ż");
	return tekst;
}

string ZapytajAPI (string url) {
	CURL* curl;
	CURLcode res;
	string readBuffer;
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		res = curl_easy_perform(curl);
		long http_code = 0;
		string content_type = "null";
		curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
		if (http_code == 200 && res != CURLE_ABORTED_BY_CALLBACK) {
			res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &content_type);
			if (content_type == "appl") {
				return readBuffer;
			} else {
				cout << endl << "Zapytanie nie powiodlo sie! Zwrocony wynik ma niepoprawny typ danych! Pobrany typ: " << content_type << " - oczekiwano: appl" << endl;
				return "{\"error\": true}";
			}
		} else {
			// wystapil blad podczas pobierania
			cout << endl << "Zapytanie nie powiodlo sie! Wystapil blad podczas pobierania! Kod http " << http_code << endl;
			return "{\"error\": true}";
		}
		curl_easy_cleanup(curl);
	}
	cout << endl << "Zapytanie nie powiodlo sie! Wystapil nieznany blad!" << endl;
	return "{\"error\": true}";
}

int main(int argc, char * argv[]) {
	
	if (argc != 2) {
		cout << "Niepoprawna liczba argumentow!" << endl;
		cout << "Użycie programu: " << argv[0] << " [id zespolu bazy Discogs.com]" << endl;
		return 1;
	}
	
	string zespol_id = argv[1];
	bool blad = false;
	
	cout << "Pobieranie danych: [";
	
	// zapytanie do API
	RSJresource wynik (ZapytajAPI ("https://api.discogs.com/artists/" + zespol_id));
	if ((blad = wynik["error"].as<bool>()) == true) {
		return 429;
	}
	
	cout << "=" << flush;
	string zespol_nazwa = NaprawKodowanie (wynik["name"].as<string>());
	
	list<pair<string, string>> czlonkowie;
	map<string, list<string>> wspolneZespoly;
	
	// znajdz wszystkich czlonkow Budki Suflera i zapisz ich jako pare <id, imie i nazwisko>
	string imie;
	for (int i = 0; (imie = wynik["members"][i]["name"].as<string>()).compare ("") != 0; i++) {
		string id_wykonawcy = wynik["members"][i]["id"].as<string>();
		string nazwisko_wykonawcy = NaprawKodowanie (wynik["members"][i]["name"].as<string>());
		czlonkowie.push_back (make_pair (id_wykonawcy, nazwisko_wykonawcy));
	}
	
	// dla kazdego znalezionego wykonawcy znajdz liste zespolow w jakich gral
	string zespol;
	for (pair<string, string> wykonawca : czlonkowie) {
		// zapytanie do API
		RSJresource wynik (ZapytajAPI ("https://api.discogs.com/artists/" + wykonawca.first));
		if ((blad = wynik["error"].as<bool>()) == true) {
			return 429;
		}
		cout << "=" << flush;
		for (int i = 0; (zespol = NaprawKodowanie(wynik["groups"][i]["name"].as<string>())).compare ("") != 0; i++) {
			wspolneZespoly.insert (make_pair (zespol, list<string>()));
			wspolneZespoly.at (zespol).push_back (wykonawca.second);
		}
	}
	
	cout << "]" << endl << endl;
	// koniec pobierania
	
	cout << "Czlonkowie zespolu " << zespol_nazwa << " grali razem rowniez w:" << endl << endl;
	
	for (pair<string, list<string>> rekord : wspolneZespoly) {
		
		// nie wypisuj zespolow, w ktorych gral tylko jeden czlonek wyszukiwanego zespolu
		if (rekord.second.size() <= 1 || rekord.first.compare (zespol_nazwa) == 0)
			continue;
		
		// wypisz nazwe zespolu
		cout << rekord.first << endl;
		
		// wypisz wszystkich czlonkow zespolu
		rekord.second.sort ();
		
		for (string czlonek : rekord.second) {
			cout << " -> " << czlonek << endl;
		}
	}
}