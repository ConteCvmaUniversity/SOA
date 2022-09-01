# SOA


## Installazione del driver

Per poter installare il driver bisogna posizionarsi nella cartella `/driver` e utilizzare i seguenti comandi:

```sh
    make all
    sudo insmod project-driver.ko
```
utilizzare poi `dmseg` per ottenere il Major number del driver, esso verrà restituito in un messaggio del tipo:

```
 project-module: new char device registered, it is assigned major number 236
```

## Utilizzo del client

Per poter utilizzare il client posizionarsi nella cartella `/client` e ottenere i permessi di root tramite `sudo su`

### Script interattivi per aggiunta dispositivi

- Per aggiungere un dispositivo utilizzare lo script `add_dev.sh`

```sh
./add_dev.sh 
```

- Per rimuovere un dispositivo utilizzare lo script `rm_dev.sh`
  
```sh
./rm_dev.sh
```
- Per rimuovere tutti i dispositivi usare l'opzione `-a` dello script `rm_dev.sh` 

```sh
./rm_dev.sh -a
```


### Script interattivo per utilizzare un dispositivo

Compilare il programma eseguendo `make all`, verrà creato l'eseguibile ***client*** utilizzabile come in esempio

```sh
./client /dev/[device name]
```

**IMPORTANTE**: bisogna avere i permessi di root.

### Script pulizia del client 

utilizzare il comando `make clean per rimuovere i file`

```sh
make clean
```