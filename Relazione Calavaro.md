<!-- omit in toc -->
# Relazione Progetto Sistemi Operativi Avanzati <br>Anno 2021/22<br> 
<sub> Marco Calavaro 0295233 <sub>

<!-- omit in toc -->
## Tabella dei contenuti 
- [1 Traccia del progetto](#1-traccia-del-progetto)
- [2 Organizzazione del codice](#2-organizzazione-del-codice)
- [3 Descrizione del divice file](#3-descrizione-del-divice-file)
- [4 Descrizione del comportamento del driver](#4-descrizione-del-comportamento-del-driver)
  - [4.1 Operazioni di lettura e scrittura](#41-operazioni-di-lettura-e-scrittura)
    - [Gestione delle operazioni bloccanti](#gestione-delle-operazioni-bloccanti)
  - [4.2 Operazioni deferred](#42-operazioni-deferred)
  - [4.3 IO Control](#43-io-control)
- [5 Operazioni di init e clenup del modulo](#5-operazioni-di-init-e-clenup-del-modulo)
- [6 Test](#6-test)

## 1 Traccia del progetto
***Multi-flow device file***

This specification is related to a Linux device driver implementing low and high priority flows of data. Through an open session to the device file a thread can read/write data segments. The data delivery follows a First-in-First-out policy along each of the two different data flows (low and high priority). After read operations, the read data disappear from the flow. Also, the high priority data flow must offer synchronous write operations while the low priority data flow must offer an asynchronous execution (based on delayed work) of write operations, while still keeping the interface able to synchronously notify the outcome. Read operations are all executed synchronously. The device driver should support 128 devices corresponding to the same amount of minor numbers.

The device driver should implement the support for the ioctl(..) service in order to manage the I/O session as follows:

>- setup of the priority level (high or low) for the operations
>- blocking vs non-blocking read and write operations
>- setup of a timeout regulating the awake of blocking operations 

A a few Linux module parameters and functions should be implemented in order to enable or disable the device file, in terms of a specific minor number. If it is disabled, any attempt to open a session should fail (but already open sessions will be still managed). Further additional parameters exposed via VFS should provide a picture of the current state of the device according to the following information:

>- enabled or disabled
>- number of bytes currently present in the two flows (high vs low priority)
>- number of threads currently waiting for data along the two flows (high vs low priority)

## 2 Organizzazione del codice
Il codice è stato organizzato in 3 blocchi logici, per poterne semplificare la lettura, ognuno di essi è formato da un file `.c` e da un header file `.h` e sono rispettivamente:
- Driver-core: blocco che sviluppa le funzionalità principali del driver tra cui le operazioni che esso deve eseguire e le istruzioni da eseguire per l'istallazione del modulo.
- Deferred-work: blocco che sviluppa il lavoro da eseguire in modo differito necessario per la scrittura sul dispositivo con priorità bassa.
- klist: blocco che sviluppa il codice del device file e tutte le funzioni necessarie al suo funzionamento.

## 3 Descrizione del divice file

## 4 Descrizione del comportamento del driver

### 4.1 Operazioni di lettura e scrittura

#### Gestione delle operazioni bloccanti

### 4.2 Operazioni deferred

### 4.3 IO Control

## 5 Operazioni di init e clenup del modulo

## 6 Test