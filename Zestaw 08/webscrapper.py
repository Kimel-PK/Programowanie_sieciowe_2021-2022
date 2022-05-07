import sys
import requests
from bs4 import BeautifulSoup

zapytanie = requests.get('https://github-readme-stats.vercel.app/api?username=kimel-pk&count_private=true&include_all_commits=true', 'html.parser')

if zapytanie.status_code != 200:
    print('Błąd, kod statusu: ' + str(zapytanie.status_code))
    sys.exit(1)

if 'Kimel-PK\\\'s GitHub Stats' not in str(zapytanie.content):
    print('Zwrócono niepoprawną treść')
    sys.exit(1)

# parsowana strona jest obrazkiem svg
if 'image/svg+xml' not in str(zapytanie.headers['Content-Type']):
    print('Niepoprawny typ danych')
    sys.exit(1)

soup = BeautifulSoup(zapytanie.content, 'html.parser')

gwiazdki = soup.find ('svg').find_all ('g')[2].find ('svg').find ('g').find ('g').find_all ('text')[1].text

if isinstance(gwiazdki, int) and gwiazdki < 0:
	print ('Niepoprawna liczba')
	sys.exit(1)

print ('Liczba zebranych gwiazdek w profilu na GitHub\'ie: ' + str (gwiazdki))