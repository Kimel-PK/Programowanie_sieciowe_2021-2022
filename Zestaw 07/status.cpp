#include <stdio.h>
#include <curl/curl.h>
#include <string>

using namespace std;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp){
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int main(int argc, char * argv[]) {
    CURL* curl;
    CURLcode res;
    string readBuffer;
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "http://th.if.uj.edu.pl/");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        long http_code = 0;
        string content_type = "null";
        curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (http_code == 200 && res != CURLE_ABORTED_BY_CALLBACK) {
            res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &content_type);
            if (content_type == "text" && readBuffer.find("Institute of Theoretical Physics")) {
                return 0;
            } else {
                return 1;
            }
        } else {
            // wystąpił błąd podczas pobierania
            return 1;
        }
        curl_easy_cleanup(curl);
    }
    return 1;
}