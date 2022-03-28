#!/bin/bash

for (( i=1; i<=11; i++ )) do
	
    python3 mini-udpcat.py 127.0.0.1 2020 < testy/test$i.txt > testy/wyjscie$i.txt;
	
    wynik=$(cat testy/wyjscie$i.txt)
    oczekiwane=$(cat testy/wynik$i.txt)
    if [ "$wynik" = "$oczekiwane" ]; then
        echo "Test $i zaliczony!"
    else
        echo "Test $i nie zaliczony!"
    fi
done
rm testy/wyjscie*