# TP Actionneur

Ce TP est réalisé par Jean-Philippe THOMAR et Paul Boulot, sous la supervision du professeur Alexis MARTIN. 

## Objectifs
A partir d'un hacheur complet et d'une carte Nucleo-STM32G474RE, nous allons  :
- Réaliser un shell pour commander le hacheur,
- Réaliser la commande des 4 transistors du hacheur en commande complémentaire décalée,
- Faire l'acquisition des différents capteurs,
- Réaliser l'asservissement en temps réel.

## Séance 1 : Commande MCC basique
### 1. Génération de 4 PWM

#### Configurartion du dead-time
On a tDTS = 1/f_sysclk et x la valeur du dead time dans la configuration du timer 1 (comprise entre 0 et 255).  
Si x <= 127 alors DT = x * tDTS.  
Si 128 <= x <= 191 alors DT = (64+x[5:0]) * 2 * tDTS.  
Si 192 <= x <= 233 alors DT = (32+x[4:0]) * 8 * tDTS.  
Si 233 <= x <= 255 alors DT = (32+x[4:0]) * 16 * tDTS.  

Ici tDTS = 1/170 MHz = 5.88 ns  
On veut un dead-time d'environ 100 ns donc on choisit x = 16


### 2. Commande de vitesse
### 3. Premiers tests

## Séance 2 : Commande en boucle ouverte, mesure de vitesse et de courant 

### 1. Commande de la vitesse 

### 2. Mesure de courant 

### 3. Mesure de la vitesse

 
