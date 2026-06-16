# Supervision réseau et IoT — Projet Portail Captif

**BTS CIEL Option A : Informatique et Réseaux**  
Lycée polyvalent Henri Brisson — Vierzon — Session 2024–2026  
**Étudiant 3 : MRAD Abdel**  
Encadré par : M. CHATELOT et M. GUEYE

---

## Présentation

Ce dépôt contient les composants logiciels de la partie supervision
du projet Portail Captif : supervision réseau via Zabbix (SNMPv3)
et supervision électronique via un capteur IoT ESP32/DHT22 (MQTT).

---

## Structure du dépôt
Projet-Portail-Captif-Supervision/

├── esp32/

│   └── supervision_esp32.ino

├── pont-mqtt/

│   └── mqtt_to_zabbix.py

├── zabbix-config/

│   └── zabbix_export_hosts.xml

└── README.md

---

## Technologies utilisées

| Composant | Technologie |
|-----------|-------------|
| Serveur supervision | Zabbix 6.0 sur Debian 12 / Hyper-V |
| Collecte réseau | SNMPv3 (authPriv, SHA-1, AES-128) |
| Microcontrôleur IoT | ESP32 NodeMCU-32S |
| Capteur | DHT22 (température + humidité) |
| Afficheur | OLED SSD1306 0,96" |
| Protocole IoT | MQTT (Mosquitto, port 1883, VLAN admin) |
| Pont IoT vers Zabbix | Python 3 + paho-mqtt + zabbix_sender |
| Base de données | MySQL 8.0 |

---

## Composants logiciels

### 1. Programme ESP32
- Lit température et humidité via DHT22
- Affiche les mesures sur écran OLED SSD1306
- Publie via MQTT sur topics `iot/temperature` et `iot/humidite`
- Reconnexion automatique Wi-Fi et MQTT en cas de coupure

### 2. Pont MQTT vers Zabbix
- S'abonne au topic `iot/#` sur Mosquitto local
- Transfère chaque mesure vers Zabbix via zabbix_sender
- Clés : `esp32.temperature` et `esp32.humidite`
- Exécuté en service systemd avec redémarrage automatique

### 3. Configuration Zabbix
- Export XML des 7 hôtes supervisés
- Templates SNMP, agent Windows, agent Linux, Zabbix server health

---

## Architecture

ESP32 + DHT22

│ MQTT (iot/temperature, iot/humidite)

▼

Mosquitto 127.0.0.1:1883

│

▼

mqtt_to_zabbix.py (service systemd)

│ zabbix_sender

▼

Zabbix Server 6.0 (VM Debian 12 / Hyper-V)

│ SNMPv3 (authPriv SHA-1 AES-128)

▼

Switch Cisco 2960 / Routeur Zyxel / Bornes Wi-Fi Zyxel

---

## Historique des versions

| Version | Date | Description |
|---------|------|-------------|
| v1.0 | Mars 2026 | Installation Zabbix, ajout hôtes réseau |
| v1.1 | Avril 2026 | Programme ESP32 + MQTT fonctionnel |
| v1.2 | Mai 2026 | Pont Python + service systemd |
| v1.3 | Juin 2026 | Tableaux de bord + alertes e-mail |
