/*
 * Copyright © LfSt Bayern, Muenchen, IuK 111, 2007
 */

#ifndef _ERIC_TYPES_H_
#define _ERIC_TYPES_H_
#include "platform.h"

/**
 * @file
 * @brief Definition von Datenstrukturen und Datentypen
 */
 
/**
 * @brief Handle auf eine ERiC-Instanz.
 *
 * ERiC-Instanzen werden von der Multithreading-API, siehe ericmtapi.h angelegt,
 * verwendet und wieder freigegeben.
 * \n Alle API-Funktionen der Multithreading-API nehmen einen Zeiger auf eine
 * ERiC-Instanz entgegen und verrichten ihre Aufgaben auf dieser ERiC-Instanz.
 * Die EricInstanz enthält sämtliche veränderlichen Zustände des ERiC.
 * Dies sind ERiC-Einstellungen, Plugin- und Log-Verzeichnis, Proxyeinstellungen,
 * Zertifikatshandle, Rückgabepuffer, etc.
 * \n Es können mehrere ERiC-Instanzen parallel angelegt werden. Jede dieser
 * ERiC-Instanzen ist unabhängig von allen anderen ERiC-Instanzen. Verfügen
 * mehrere Threads jeweils über ihre eigene ERiC-Instanz, können sie diese
 * parallel verwenden. Dazu müssen die Threads den API-Funktionen der
 * Multithreading-API ihre jeweils eigene ERiC-Instanz übergeben.
 * \n ERiC-Instanzen sollen nicht für jede Aufgabe neu erstellt und konfiguriert
 * werden. Das Erstellen und Zerstören einer ERiC-Instanz ist ressourcen- und zeitintensiv.
 * Die Lebenszeit einer ERiC-Instanz sollte beispielsweise eher der Lebenszeit
 * eines Arbeiter-Threads in einem Pool entsprechen, als der Verarbeitungsdauer
 * einer einzelnen Aufgabe an einen Arbeiter-Thread.
 * \n ERiC-Instanzen können zwischen Threads ausgetauscht werden. Eine
 * ERiC-Instanz darf aber nicht in zwei Threads gleichzeitig verwendet werden.
 *
 * @see
 *       - ::EricMtInstanzErzeugen()
 *       - ::EricMtInstanzFreigeben()
 */
typedef struct EricInstanz *EricInstanzHandle;

/**
 * @brief Der Datentyp byteChar wird immer dann verwendet, wenn an diesem Parameter 
 *        keine UTF-8 codierte Daten erwartet werden.
 *        Diese Daten werden ungeprüft verwendet. 
 *        Zum Beispiel Pfade.
 */
typedef char byteChar;


/**
 * @brief Integer-Typ für Zertifikat-Handles.
 */
typedef uint32_t EricZertifikatHandle;

/**
* @brief Das ::EricTransferHandle wird beim Anwendungsfall "Datenabholung"
*        der API-Funktion ::EricBearbeiteVorgang() übergeben.
*
* Es ist vom Aufrufer zu initialisieren
* und wird ::EricBearbeiteVorgang() als Pointer übergeben. Es wird
* verwendet, um bei der Datenabholung mehrere Versandvorgänge zu bündeln.
* Dabei ist das Handle für den ersten Vorgang "Anfrage" mit dem Wert 0
* zu intialisieren bevor ::EricBearbeiteVorgang() aufgerufen wird.
* Das von ::EricBearbeiteVorgang() zurückgegebene Handle ist dann
* bei allen Folgevorgängen derselben Datenabholung unverändert
* wieder zu übergeben.
* \n Wird bei einer Datenabholung NULL oder ein ungültiger Zeiger
* als Handle übergeben, gibt ::EricBearbeiteVorgang()
* den Fehlercode ::ERIC_GLOBAL_TRANSFERHANDLE zurück.
* \n Bei allen Verfahren außer der Datenabholung sollte das Transferhandle
* beim Aufruf der ::EricBearbeiteVorgang() NULL sein. Wird bei solchen
* Verfahren ein Handle übergeben, so wird dieses ignoriert.
*
*
*/
typedef uint32_t EricTransferHandle;


/**
 * @brief Bearbeitungsflags für die Anwendungsfälle von EricBearbeiteVorgang().
 *
 * Welche Anwendungsfälle von der jeweiligen Datenart unterstützt werden,
 * ist dem ERiC-Entwicklerhandbuch.pdf zu entnehmen.
 */
typedef enum
{
    /**
     * @brief Der Datensatz soll validiert werden.
     */
    ERIC_VALIDIERE = 1L << 1,

    /**
     * @brief Der Datensatz soll an den ELSTER Annahmeserver versendet werden.
     */
    ERIC_SENDE = 1L << 2,

    /**
     * @brief Der Datensatz soll gedruckt werden.
     */
    ERIC_DRUCKE = 1L << 5,

    /**
     * @brief Der Datensatz soll auf Hinweise hin geprüft werden.
     */
    ERIC_PRUEFE_HINWEISE = 1L << 7,
} eric_bearbeitung_flag_t;


/* Vorwärts-Deklaration. Anwender der ERiC API verwenden ausschließlich
   Zeiger auf Instanzen dieses Datentyps. */
struct EricReturnBufferApi;


/**
 * @brief Handle zur Verwaltung und Verwendung von Rückgabepuffern.
 *
 * Viele ERiC API-Funktionen geben Informationen an ihren Aufrufer zurück,
 * indem sie Daten in sogenannte Rückgabepuffer schreiben.
 * Solche Rückgabepuffer müssen mit ::EricRueckgabepufferErzeugen() angelegt
 * werden. Das bei dieser Erzeugung generierte Puffer-Handle wird vom Aufrufer
 * an die API-Funktion übergeben, die den Puffer leert bevor sie dann in den
 * Puffer schreibt. Ein einmal generiertes Puffer-Handle kann damit auch für
 * mehrere aufeinanderfolgende Aufrufe von ERiC API-Funktionen wiederverwendet
 * werden.
 * Mittels ::EricRueckgabepufferLaenge() kann danach die Anzahl der in den
 * Puffer geschriebenen Bytes ermittelt werden.
 * Mit ::EricRueckgabepufferInhalt() kann der Pufferinhalt als
 * null-terminiertes Character-Array abgefragt werden.
 * Jeder Rückgabepuffer muss nach seiner Verwendung mit
 * ::EricRueckgabepufferFreigeben() wieder freigegeben werden.
 *
 * Die Struktur EricReturnBufferApi kapselt die Rückgabepuffer-Implementierung.
 * Anwender der ERiC API verwenden ausschließlich Zeiger auf Instanzen dieser
 * Struktur und müssen daher deren Felder nicht kennen.
 *
 * Rückgabepuffer sind der Singlethreading-API bzw. einer ERiC-Instanz der
 * Multithreading-API fest zugeordnet. Die Funktionen der ERiC API, die einen
 * Rückgabepuffer entgegen nehmen, geben den Fehlercode
 * ::ERIC_GLOBAL_PUFFER_UNGLEICHER_INSTANZ zurück, wenn der übergebene
 * Rückgabepuffer
 *      - mit der Singlethreading-API erzeugt worden ist und dann mit der
 *        Multithreading-API verwendet wird
 *      - mit der Multithreading-API erzeugt worden ist und dann mit der
 *        Singlethreading-API verwendet wird
 *      - mit einer ERiC-Instanz erzeugt worden ist und dann mit einer
 *        anderen Instanz verwendet wird.
 *
 * @see
 *      - ERiC-Entwicklerhandbuch.pdf Kap. "Rückgabepuffer der ERiC Programmierschnittstelle"
 *      - ::EricRueckgabepufferErzeugen()
 *      - ::EricRueckgabepufferLaenge()
 *      - ::EricRueckgabepufferInhalt()
 *      - ::EricRueckgabepufferFreigeben()
 */
typedef struct EricReturnBufferApi* EricRueckgabepufferHandle;


/**
 * @brief Diese Struktur enthält alle für den Druck notwendigen Informationen.
 *
 * Der Anwendungsentwickler muss diese Struktur allokieren und nach Verwendung
 * wieder freigeben.
 *
 */
typedef struct
{
    /**
    * @brief Version dieser Struktur. Muss derzeit immer 2 sein.
    *        Bei Änderungen dieser Struktur wird dieser Wert inkrementiert.
    *
    * @note Bei einem Wert ungleich 2 wird #ERIC_GLOBAL_UNGUELTIGE_PARAMETER_VERSION zurückgegeben und eine Fehlermeldung in die Logdatei geschrieben.
    *
    */
    uint32_t version;

    /**
     * @brief Soll ein Vorschau-PDF erstellt werden?
     *
     * - vorschau = 1: Ein Vorschau-PDF wird erzeugt und als solches gekennzeichnet.
     * - vorschau = 0: Es wird kein Vorschau-PDF erzeugt.
     *
     * @note Bei Werten ungleich 0 oder 1 wird #ERIC_GLOBAL_UNGUELTIGER_PARAMETER zurückgegeben und eine Fehlermeldung in die Logdatei geschrieben.
     *
     */
    uint32_t vorschau;
    /**
     * @brief Soll das PDF nur die erste Seite oder alles enthalten?
     *
     * - ersteSeite = 1: Es wird nur die erste Seite einer komprimierten Erklärung gedruckt.
     * - ersteSeite = 0: Es wird alles gedruckt.
     *
     * @note
     *       - Fachliche Informationen sind im ERiC-Entwicklerhandbuch.pdf nachzulesen.
     *       - Bei Werten ungleich 0 oder 1 wird #ERIC_GLOBAL_UNGUELTIGER_PARAMETER zurückgegeben und eine Fehlermeldung in die Logdatei geschrieben.
     */
    uint32_t ersteSeite;
    /**
     * @brief Soll die PDF-Datei für einen doppelseitigen Ausdruck mit Heftrand zum Lochen vorbereitet werden?
     *
     * - duplexDruck = 1: Die geraden Seiten werden für einen Heftrand zum Lochen nach links eingerückt, Details siehe ERiC-Entwicklerhandbuch.pdf
     * - duplexDruck = 0: Es erfolgt keine Einrückung der geraden Seiten. Das erstellte PDF ist nur zum blattweisen Ausdruck der Seiten vorgesehen.
     *
     * @note Bei Werten ungleich 0 oder 1 wird #ERIC_GLOBAL_UNGUELTIGER_PARAMETER zurückgegeben und eine Fehlermeldung in die Logdatei geschrieben.
     */
    uint32_t duplexDruck;
    
	/**
     * @brief Pfad der erzeugten PDF-Datei.
     *
     * Pfade müssen auf Windows in der für Dateifunktionen benutzen ANSI-Codepage,
     * auf Linux in der für das Dateisystem benutzten Locale und auf macOS in der
     * "decomposed form" von UTF-8 übergeben werden. 
     * Weiterführende Informationen hierzu, sowie zu nicht erlaubten Zeichen in
     * Pfaden und Pfadtrennzeichen, relative Pfadangabe, etc. siehe
     * Entwicklerhandbuch Kapitel "Übergabe von Pfaden an ERiC API-Funktionen".
     *
     * Windows-Beispiel: "c:\\test\\ericprint.pdf"
     *
     * Soll eine PDF-Datei angelegt werden, ist der pdfName zwingend erforderlich
     *
     * <B> Besonderheiten bei Sammeldaten </B>
     * Für Sammeldaten wird dem PDF-Dateinamen vor der Dateiendung das Nutzdatenticket
     * angefügt:
     * \n <PDF-Dateiname>_<Nutzdatenticket>.pdf
     * Optional kann der PDF-Dateiname den Platzhalter "%t" enthalten, der dann
     * durch das Nutzdatenticket ersetzt wird:
     * \n "%t_ericprint.pdf" --> "<Nutzdatenticket>_ericprint.pdf"
     *
     * @note Es ist sicherzustellen, dass alle PDF-Dateien im Dateisystem
     * erstellt bzw. geschrieben werden können.  Falls es beim Erstellen der
     * PDF-Dokumente einen Fehler gibt oder falls diese nicht geschrieben
     * werden können, wird die Bearbeitung abgebrochen, eine Log-Ausgabe
     * erstellt, aus der hervorgeht, welcher Steuerfall nicht gedruckt werden
     * konnte, und eine Fehlermeldung an den Aufrufer zurückgeliefert.
     */
    const byteChar* pdfName;
    
	/**
     * @brief Fußtext der auf dem Ausdruck verwendet werden soll (optional).
     * @details Wenn der übergebene Text länger als #ERIC_MAX_LAENGE_FUSSTEXT
     * Zeichen ist, dann bricht der Druck mit Fehlerkode
     * #ERIC_PRINT_FUSSTEXT_ZU_LANG ab!
     *
     * @note Fachliche Informationen sind im ERiC-Entwicklerhandbuch.pdf nachzulesen.
     */
    const char* fussText;
} eric_druck_parameter_t;


/**
 * @brief Für die Signatur oder Authentifizierung benötigte
 * Informationen.
 *
 * Diese Struktur ist vom Anwender zu allokieren und samt
 * Inhalt auch wieder freizugeben.
 */
typedef struct
{
    /**
    * @brief Version dieser Struktur. Muss derzeit immer 2 sein.
    *        Bei Änderungen dieser Struktur wird dieser Wert inkrementiert.
    *
    * @note Bei einem Wert ungleich 2 wird #ERIC_GLOBAL_UNGUELTIGE_PARAMETER_VERSION zurückgegeben und eine Fehlermeldung in die Logdatei geschrieben.
    *
    */
    uint32_t version;

    /**
     * @brief Verweis auf den KeyStore, siehe EricGetHandleToCertificate().
     */
    EricZertifikatHandle zertifikatHandle;
    /**
     * @brief PIN für den KeyStore.
     */
    const char *pin;
    /**
     * @brief Dieser muss für Datenlieferungen zum Verfahren
              ElsterDatenabholung und Datenart ElsterVaStDaten
              angegeben werden, falls für die Signatur ein
              SoftPSE-Zertifikat verwendet wird. In allen anderen Fällen
              muss hier NULL übergeben werden. Der Parameter abrufCode
              besteht aus 2 x 5 Zeichen, die mit "-" verbunden sind.
              Beispiel: "K6FG5-RS32P"
     */
    const char *abrufCode;
} eric_verschluesselungs_parameter_t;


/**
 * @brief Struktur mit Informationen zur Erzeugung von Zertifikaten mit #EricCreateKey.
 *
 * Die Elemente der Struktur beschreiben den Anwender, für den ein Schlüssel
 * erstellt werden soll.
 * Unbenutzte Parameter müssen mit NULL oder Leerstring initialisiert werden.
 *
 * Diese Struktur und ihre Elemente sind vom Anwender zu allokieren und samt
 * Inhalt auch wieder freizugeben.
 * Alle Elemente sind vom Anwender zu initialisieren.
 *
 */
typedef struct
{
    /**
     * @brief Version dieser Struktur. Muss derzeit immer 1 sein.
     *        Bei Änderungen dieser Struktur wird dieser Wert inkrementiert.
     *
     * @note Bei einem Wert ungleich 1 wird #ERIC_GLOBAL_UNGUELTIGE_PARAMETER_VERSION zurückgegeben und eine Fehlermeldung in die Logdatei geschrieben.
     *
     */
    uint32_t version;

    /**
     * @brief Name des Anwenders.
     *
     * Die Angabe des Namens ist obligatorisch.
     * Der Parameter darf nicht mit NULL oder einem Leerstring (String der Länge 0) belegt werden.
     */
    const char* name;

    /**
     * @brief Land des Anwenders. Beispiel: "DE".
     *
     * Die Angabe dieses Wertes ist optional.
     */
    const char* land;

    /**
     * @brief Wohnort des Anwenders, inklusive PLZ. Beispiel: "D-10179 Berlin".
     *
     * Die Angabe dieses Wertes ist optional.
     */
    const char* ort;

    /**
     * @brief Straßenangabe mit Hausnummer des Anwenders mit Zusätzen, Beispiel: "Musterstraße 123 Zugang im Rückgebäude".
     *
     * Die Angabe dieses Wertes ist optional.
     */
    const char* adresse;

    /**
     * @brief E-Mail-Adresse des Anwenders.
     *
     * Die Angabe dieses Wertes ist optional.
     */
    const char* email;

    /**
     * @brief Name der Organisation.
     *
     * Die Angabe dieses Wertes ist optional.
     * Wenn Organisation und Abteilung nicht angegeben werden, wird "ELSTER" verwendet.
     */
    const char* organisation;

    /**
     * @brief Name der Abteilung (organizational unit) der Organisation.
     *
     * Die Angabe dieses Wertes ist optional.
     * Wenn Organisation und Abteilung nicht angegeben werden, wird "ERiC" verwendet.
     */
    const char* abteilung;

    /**
     * @brief Beschreibung, welche für den Anwender im Zertifikat abgelegt wird.
     *
     * Die Angabe dieses Wertes ist optional.
     */
    const char* beschreibung;
} eric_zertifikat_parameter_t;

    /**
     * ::eric_log_level_t ist ein Parameter für Funktionen vom Typ ::EricLogCallback.
     * Der Loglevel kann zum Filtern für das ERiC Protokoll verwendet werden, siehe
     * ERiC-Entwicklerhandbuch.pdf Kap. "Das ERiC Protokoll eric.log".
     */
typedef enum
{
    /// Fehler, der zum Programmabbruch führt.
    ERIC_LOG_ERROR = 4,
    /// Hinweise auf Zustände, die zu Fehlern führen können.
    ERIC_LOG_WARN = 3,
    /// Grobe Informationen über den Programmablauf und Werte.
    ERIC_LOG_INFO = 2,
    /// Feingranulare Informationen über den Programmablauf und Werte.
    ERIC_LOG_DEBUG = 1,
    /// Sehr feingranulare Informationen über den Programmablauf und Werte.
    ERIC_LOG_TRACE = 0
} eric_log_level_t;

/**
 * @brief Typ der Callback-Funktion zum Logging.
 *
 * Wenn registriert, wird diese Callback-Funktion
 * für jeden Log-Eintrag mit folgenden Parametern aufgerufen.
 *
 * @param kategorie Kategorie des Logeintrags. Beinhaltet das Modul, welches
 *                  den Log-Eintrag ausgibt. Zum Beispiel "eric.ctrl2". Kann
 *                  zum Filtern verwendet werden. Alle Log-Nachrichten besitzen
 *                  eine Kategorie. Der Zeiger ist nur innerhalb dieser
 *                  Funktion gültig.
 *
 * @param loglevel Log-Level des Logeintrags. Kann zum Filtern verwendet werden.
 *
 * @param nachricht Enthält die Log-Nachricht als Zeichenkette. Der Zeiger ist
 *                  nur innerhalb dieser Funktion gültig.
 *
 * @param benutzerdaten Der Zeiger, der bei der Registrierung mit
 *                      ::EricRegistriereLogCallback() übergeben worden ist,
 *                      wird in diesem Parameter vom ERiC unverändert
 *                      übergeben.
 *
 * @see
 *      - ::EricRegistriereLogCallback()
 *
 */
typedef void(STDCALL *EricLogCallback)(
    const char* kategorie,
    eric_log_level_t loglevel,
    const char* nachricht,
    void* benutzerdaten);

enum
{
    /**
     * @brief \c id, die beim Einlesen des XMLs von Fortschrittcallbacks
     *        ausgegeben wird.
     */
    ERIC_FORTSCHRITTCALLBACK_ID_EINLESEN    = 10,

    /**
     * @brief \c id, die gemeldet wird, wenn die Daten zum Versand noch
     *        vorbereitet werden müssen.
     */
    ERIC_FORTSCHRITTCALLBACK_ID_VORBEREITEN = 20,
 
    /**
     * @brief \c id, die beim Validieren der Eingangsdaten von
     *        Fortschrittcallbacks ausgegeben wird.
     */
    ERIC_FORTSCHRITTCALLBACK_ID_VALIDIEREN  = 30,

    /**
     * @brief \c id, die beim Versand der Ausgangsdaten von
     *       Fortschrittcallbacks ausgegeben wird.
     */
    ERIC_FORTSCHRITTCALLBACK_ID_SENDEN      = 40,

    /**
     * @brief \c id, die beim Druck der Eingangsdaten von
     *        Fortschrittcallbacks ausgegeben wird.
     */
    ERIC_FORTSCHRITTCALLBACK_ID_DRUCKEN     = 50
};

/**
 * @brief Typ der Callback-Funktionen, die am ERiC für Fortschrittanzeigen
 *        registriert werden können.
 *
 * @param id Aktueller Verarbeitungsschritt
 *
 * @param pos Aktueller Fortschritt bezogen auf \c max
 *
 * @param max Maximalwert des aktuellen Fortschritts \c pos
 *
 * @param benutzerdaten Der Zeiger, der bei der Registrierung mit
 *                      ::EricRegistriereGlobalenFortschrittCallback() oder
 *                      ::EricRegistriereFortschrittCallback() übergeben
 *                      worden ist, wird in diesem Parameter vom ERiC
 *                      unverändert übergeben.
 *
 * @details Es gilt stets, dass:
 *           - \c pos größer oder gleich 0 und kleiner oder gleich \c max ist
 *           - \c max ist immer größer als 0
 */
typedef void (STDCALL *EricFortschrittCallback)(
    uint32_t id,
    uint32_t pos,
    uint32_t max,
    void *benutzerdaten);

#endif /*ERIC_TYPES_H_*/
