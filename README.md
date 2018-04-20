# Anforderungen

## Pflicht: CoAP URI handling


### Implementieren: 

- Nachrichten senden und empfangen (mit NON, CON, REQ, RST, etc.)
- CON bekommt ACK oder RST
- CON sollte mit CON beantwortet werden
- NON sollte mit NON beantwortet werden
- NON darf kein ACK bekommen
- Parsen von Nachrichten (Options, payload etc. auseinander ziehen)
- Options decodieren und optional mitverschicken (die, die wichtig sind)
- Responsecodes schicken und decodieren (Success 2.xx, Client Error 4.xx etc.)
- Übertragung abbrechen unter bedingungen (Timeout)
- ignorieren/rejecten von fehlerhaften nachrichten
- REQ und RES schicken; Request Response matching über Token ID
- Message deduplication
- piggybacked responses (senden und empfangen)
- seperate responses (empfangen)
- Transmission Parameters wie im RFC standart (z.B. MAX_TRANSMISSION TIME etc.)
- MID (16 bit) und TID (0-8 bytes) generieren ('XOR shift plus' random gen)
- Critical und elective options; Support für repeatable options.
- Server - Client modell, Server muss mit mehreren Endpoints rechnen. (Anzahl kann als Variable übergeben werden.)
- diagnostic Payload schicken wenn Fehler auftritt.
- Format des Payloads muss angegeben werden.
- Alle nachrichten ASCII compatible machen
- Method definition: GET, POST, PUT, DELETE



### Optional:
- ETags
- Resource discovery
- content-format, accept
- viele Options wie Size1 etc.
- seperate responses (senden)


# Konventionen

### Build Automation Tool:
GNU Make mit gcc

### Code Conventions:
Linux Kernel style mit 4 indent characters (nicht 8)

### Commit Rules: Die sechs Gebote
Commit messages aus Sicht des Programmieres (added a bug)
Master is always stable (or not at all)
Seperate Feature Branches
Commit often, perfect later, publish once
2+ branch (develop, master + feature)
Ein Commit ändert eine Sache