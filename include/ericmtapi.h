/*
 * Copyright © LfSt Bayern, Muenchen, IuK 111, 2006
 */

#ifndef ERICMTAPI_H_
#define ERICMTAPI_H_

#include "platform.h"

#include "ericapiExport.h"
#include "eric_types.h"
#include "ericdef.h"

/**
 * @file
* @brief Deklaration der ERiC API-Funktionen für die Multithreading-API
 *
 */

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Diese API-Funktion ist die zentrale Schnittstellenfunktion zur Kommunikation
     *        mit dem ELSTER-Annahmeserver.
     *
     * Als Austauschformat wird XML verwendet, siehe Kapitel "Datenverarbeitung mit ERiC"
     * im Entwicklerhandbuch. Dort sind die Arbeitsabläufe von Einzel- und Sammellieferung
     * beschrieben.
     *
     * Die Funktion kann Steuerdaten plausibilisieren, an den ELSTER-Annahmeserver übertragen und
     * ausdrucken, sowie Protokolle der Übertragung erzeugen. Die ProcessingFlags im Parameter
     * @c bearbeitungsFlags definieren, welche der Schritte wie ausgeführt werden.
     *
     * Je nach Anwendungsfall können die Daten authentifiziert übertragen werden und es kann
     * ein PDF-Druck der Daten erfolgen. In diesen Fällen sind die Parameter @c cryptoParameter
     * und @c druckParameter entsprechend zu befüllen. Die möglichen Parameterkombinationen und
     * Druckkennzeichnungen können im Entwicklerhandbuch nachgelesen werden.
     *
     * Sind für einen Anwendungsfall mehrere voneinander abhängige Aufrufe von
     * ::EricMtBearbeiteVorgang() nötig, so ist der Parameter @c transferHandle zu übergeben. Dies
     * ist derzeit nur für die Datenabholung der Fall.
     *
     * Es werden an bestimmten Punkten der Verarbeitung benutzerdefinierte Callback Funktionen
     * aufgerufen. Siehe hierzu \ref MTSection_Fortschrittcallbacks.
     *
     * Zur Erzeugung, Verwendung und Freigabe von Rückgabepuffern siehe Dokumentation
     * zu ::EricRueckgabepufferHandle.
     *
     * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
     * @param[in] datenpuffer
     *              Enthält die zu verarbeitenden XML-Daten.
     * @param[in] datenartVersion
     *              Die @c datenartVersion ist der Datenartversionmatrix zu entnehmen,
     *              siehe common\Datenartversionmatrix.xml und ERiC-Entwicklerhandbuch.pdf.
     *              Dieser Parameter darf nicht NULL sein und muss zu den XML-Eingangsdaten passen.
     * @param[in] bearbeitungsFlags
     *              Oder-Verknüpfung von Bearbeitungsvorgaben.
     *              Anhand dieser Vorgaben werden die übergebenen Daten verarbeitet.
     *              Der Parameter darf nicht 0 sein, zu gültigen Werten siehe ::eric_bearbeitung_flag_t.
     *              Bei welchen Anwendungsfällen welche Flags möglich oder notwendig sind,
     *              ist im Entwicklerhandbuch nachzulesen.
     * @param[in] druckParameter
     *              Parameter, die für den PDF-Druck benötigt werden, siehe
     *              ::eric_druck_parameter_t.
     *              Bei welchen Anwendungsfällen welche Druckparameter möglich oder notwendig sind
     *              ist im Entwicklerhandbuch nachzulesen. Soll kein PDF-Druck erfolgen,
     *              so ist NULL zu übergeben.
     * @param[in] cryptoParameter
     *              Enthält die für den authentifizierten Versand benötigten
     *              Informationen und darf nur dann übergeben werden,
     *              siehe ::eric_verschluesselungs_parameter_t.
     *              Erfolgt kein authentifizierter Versand, so ist NULL zu übergeben.
     * @param[in,out] transferHandle
     *              Bei der Datenabholung ist ein Zeiger auf ein vom Aufrufer verwaltetes
     *              und anfangs mit 0 befülltes ::EricTransferHandle zu übergeben, über das die
     *              zusammenhängenden Versandvorgänge einer Datenabholung gebündelt werden
     *              (Bündelung der Versandvorgänge "Anforderung", "Abholung" und optional
     *              "Quittierung").
     *              Wenn bei der Datenabholung kein Versandflag gesetzt ist (nur Validierung),
     *              darf dem transferHandle auch ein Nullzeiger (NULL) übergeben werden.
     *              Bei allen anderen Anwendungsfällen ist immer NULL zu übergeben.
     * @param[out] rueckgabeXmlPuffer
     *              Handle auf einen Rückgabepuffer, in den beim Versand
     *              Telenummer und Ordnungsbegriff, Hinweise oder Fehler bei der
     *              Regelprüfung geschrieben werden, siehe
     *              \ref MTSection_Rueckgabepuffer und ::EricRueckgabepufferHandle.
     * @param[out] serverantwortXmlPuffer
     *              Handle auf einen Rückgabepuffer, in den beim Versand die
     *              Antwort des Empfangsservers geschrieben wird, siehe
     *              \ref MTSection_Rueckgabepuffer und ::EricRueckgabepufferHandle.
     *
     * @return
     *        - ::ERIC_OK
     *        - ::ERIC_GLOBAL_UNGUELTIGER_PARAMETER
     *        - ::ERIC_GLOBAL_NULL_PARAMETER
     *        - ::ERIC_GLOBAL_DATENARTVERSION_UNBEKANNT
     *        - ::ERIC_GLOBAL_VERSCHLUESSELUNGS_PARAMETER_NICHT_ANGEGEBEN
     *        - ::ERIC_GLOBAL_PRUEF_FEHLER
     *          Plausibilitätsfehler in den Eingabedaten, die Fehlermeldungen
     *          werden im Rückgabepuffer @c rueckgabeXmlPuffer zurückgegeben.
     *          Siehe Abschnitt \ref MTSection_Plausibilitaetsfehler.
     *        - ::ERIC_GLOBAL_HINWEISE
     *          Kann nur zurückgegeben werden, falls das Bearbeitungsflag
     *          ::ERIC_PRUEFE_HINWEISE angegeben wurde.
     *          Es wurden ausschließlich Hinweise zu den Eingabedaten gemeldet,
     *          die Hinweise werden im Rückgabepuffer @c rueckgabeXmlPuffer zurückgegeben.
     *          Siehe Abschnitt \ref MTSection_Hinweise.
     *        - ::ERIC_GLOBAL_DATENSATZ_ZU_GROSS
     *          Die maximal zulässige Größe des XML-Eingangsdatensatzes oder des zu übermittelnden,
     *          komprimierten, verschlüsselten und base64-kodierten Datenteils,
     *          siehe ERiC-Entwicklerhandbuch.pdf Kap. "Größenbegrenzung der
     *          Eingangsdaten", ist überschritten.
     *        - ::ERIC_TRANSFER_ERR_XML_THEADER, ::ERIC_TRANSFER_ERR_XML_NHEADER
     *          Die Serverantwort enthält Fehlermeldungen. Zur Auswertung kann
     *          entweder die Serverantwort selbst ausgewertet werden oder es wird
     *          ::EricMtGetErrormessagesFromXMLAnswer() aufgerufen.
     *        - ::ERIC_IO_READER_SCHEMA_VALIDIERUNGSFEHLER
     *        - ::ERIC_IO_PARSE_FEHLER
     *        - ::ERIC_GLOBAL_COMMONDATA_NICHT_VERFUEGBAR
     *        - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
     *        - weitere, siehe eric_fehlercodes.h
     *
     * \section MTSection_Rueckgabepuffer Inhalt des Rückgabepuffers und des Serverantwortpuffers
     *
     * Der Inhalt der Pufferspeicher kann mit ::EricMtRueckgabepufferInhalt() abgefragt
     * und ausgewertet werden. @c rueckgabeXmlPuffer gibt im \ref MTSection_Erfolgsfall oder bei
     * \ref MTSection_Plausibilitaetsfehler XML-Daten nach Schema
     * common/API-Rueckgabe-Schemata/EricBearbeiteVorgang.xsd zurück.
     * @c serverantwortXmlPuffer gibt bei Sendevorgängen die Antwort des ELSTER-
     * Annahmeservers zurück.
     *
     * Nach dem Aufruf der Funktion müssen programmatisch folgende Fälle aufgrund des Rückgabewerts
     * unterschieden werden.
     *
     * \subsection MTSection_Erfolgsfall Erfolgsfall
     *
     * Sind alle Bearbeitungsschritte fehlerfrei durchlaufen worden, dann ist
     * der Rückgabewert ::ERIC_OK und der Text im Pufferspeicher
     * @c rueckgabeXmlPuffer enthält beim Versand XML-Daten mit generierter
     * Telenummer und bei Neuaufnahmen den Ordnungsbegriff.
     *
     * Beispiel:
     * \code
     * <?xml version="1.0" encoding="UTF-8"?>
     * <EricBearbeiteVorgang xmlns="http://www.elster.de/EricXML/1.0/EricBearbeiteVorgang"
     *    <Erfolg>
     *        <Telenummer>N55</Telenummer>
     *    </Erfolg>
     * </EricBearbeiteVorgang>
     * \endcode
     *
     * Beim Versand befindet sich zusätzlich im Pufferspeicher @c serverantwortXmlPuffer die Antwort
     * des ELSTER-Annahmeservers. Bei einer Datenabholung kann diese ausgewertet werden. Details
     * hierzu befinden sich im Entwicklerhandbuch.
     *
     * \subsection MTSection_Hinweise Hinweise
     *
     * Falls das Bearbeitungsflag ::ERIC_PRUEFE_HINWEISE angegeben worden ist, kann der
     * Rückgabewert ::ERIC_GLOBAL_HINWEISE zurückgegeben werden.
     * Der Rückgabepuffer enthält dann die gemeldeten Hinweise.
     *
     * Beispiel:
     * \code
     * <?xml version="1.0" encoding="UTF-8"?>
     * <EricBearbeiteVorgang xmlns="http://www.elster.de/EricXML/1.0/EricBearbeiteVorgang">
     *    <Hinweis>
     *        <Nutzdatenticket>1075</Nutzdatenticket>
     *        <Feldidentifikator>100001</Feldidentifikator>
     *        <Mehrfachzeilenindex>1</Mehrfachzeilenindex>
     *        <LfdNrVordruck>1</LfdNrVordruck>
     *        <Untersachbereich>5</Untersachbereich>
     *        <RegelName>testRegelName</RegelName>
     *        <FachlicheHinweisId>9995</FachlicheHinweisId>
     *        <Text>Weitere Angaben können erforderlich sein</Text>
     *    </Hinweis>
     * </EricBearbeiteVorgang>
     * \endcode
     *
     * Die einzelnen Elemente sind in der Schemadefinition
     * common/API-Rueckgabe-Schemata/EricBearbeiteVorgang.xsd dokumentiert.
     * Wenn die Bearbeitungsflags ::ERIC_PRUEFE_HINWEISE und ::ERIC_VALIDIERE übergeben
     * worden sind, wurden bei der Plausibilisierung keine Fehler gefunden. Es sind
     * keine Fehler im Rückgabepuffer enthalten.
     *
     * \subsection MTSection_Plausibilitaetsfehler Plausibilitätsfehler
     *
     * Bei fehlgeschlagener Plausibilitätsprüfung ist der Rückgabewert
     * ::ERIC_GLOBAL_PRUEF_FEHLER, und die Fehler werden im Rückgabepuffer
     * als XML-Daten zurückgeliefert.
     *
     * Beispiel:
     * \code
     * <?xml version="1.0" encoding="UTF-8"?>
     * <EricBearbeiteVorgang xmlns="http://www.elster.de/EricXML/1.0/EricBearbeiteVorgang">
     *    <FehlerRegelpruefung>
     *        <Nutzdatenticket>1075</Nutzdatenticket>
     *        <Feldidentifikator>100001</Feldidentifikator>
     *        <Mehrfachzeilenindex>1</Mehrfachzeilenindex>
     *        <LfdNrVordruck>1</LfdNrVordruck>
     *        <Untersachbereich>5</Untersachbereich>
     *        <RegelName>testRegelName</RegelName>
     *        <FachlicheFehlerId>9995</FachlicheFehlerId>
     *        <Text>Beim Ankreuzfeld muss der Wert 'X' angegeben werden.</Text>
     *    </FehlerRegelpruefung>
     * </EricBearbeiteVorgang>
     * \endcode
     *
     * Die einzelnen Elemente sind in der Schemadefinition
     * common/API-Rueckgabe-Schemata/EricBearbeiteVorgang.xsd dokumentiert.
     * Wenn die Bearbeitungsflags ::ERIC_PRUEFE_HINWEISE und ::ERIC_VALIDIERE übergeben
     * worden sind, kann der Rückgabepuffer auch Hinweise enthalten.
     *
     * \subsection MTSection_Serverfehlerfall Fehler in der Serverantwort
     *
     * Ist der Rückgabewert ::ERIC_TRANSFER_ERR_XML_THEADER oder
     * ::ERIC_TRANSFER_ERR_XML_NHEADER so enthält der Serverantwortpuffer
     * Fehlermeldungen. Zur Auswertung kann entweder die Serverantwort selbst
    * ausgewertet werden oder es wird ::EricMtGetErrormessagesFromXMLAnswer() aufgerufen.
     *
     * \subsection MTSection_Fehler Sonstige Fehler
     *
     * Bei sonstigen Fehlern ist der Inhalt der Rückgabepuffer undefiniert. Um nähere
     * Informationen über die Fehlerursache herauszufinden, kann ::EricMtHoleFehlerText() mit
     * dem Rückgabewert aufgerufen werden.
     *
     * \section MTSection_Fortschrittcallbacks Fortschrittcallbacks
     *
     * Während der Verarbeitung eines Anwendungsfalls werden die durch die Funktionen
    * ::EricMtRegistriereFortschrittCallback() und ::EricMtRegistriereGlobalenFortschrittCallback()
     * registrierten Callbacks aufgerufen.
     *
     * @see
     *      - ERiC-Entwicklerhandbuch.pdf, Kapitel "Anwendungsfälle von EricBearbeiteVorgang()"
     *      - ERiC-Entwicklerhandbuch.pdf, Kapitel der jeweiligen Datenart
     *      - ERiC-Entwicklerhandbuch.pdf, Kapitel "Datenabholung"
     *      - ERiC-Entwicklerhandbuch.pdf, Kapitel "Größenbegrenzung der Eingangsdaten"
     *      - ERiC-Entwicklerhandbuch.pdf, Kapitel "Funktionen für Fortschrittcallbacks"
     *      - EricMtHoleFehlerText()
     *      - EricMtGetErrormessagesFromXMLAnswer()
     *      - EricMtRegistriereFortschrittCallback()
     *      - EricMtRegistriereGlobalenFortschrittCallback()
     *
     */
    ERICAPI_IMPORT int STDCALL EricMtBearbeiteVorgang(
        EricInstanzHandle instanz,
        const char* datenpuffer,
        const char* datenartVersion,
        uint32_t bearbeitungsFlags,
        const eric_druck_parameter_t* druckParameter,
        const eric_verschluesselungs_parameter_t* cryptoParameter,
        EricTransferHandle* transferHandle,
        EricRueckgabepufferHandle rueckgabeXmlPuffer,
        EricRueckgabepufferHandle serverantwortXmlPuffer);


    /**
     * @brief Die PIN für ein clientseitig erzeugtes Zertifikat (CEZ) wird geändert.
     *
     * Die Funktion ändert die bei der Funktion EricMtCreateKey() angegebene PIN
     * und entsprechend hierfür die Prüfsumme in der Datei eric.sfv.
     * Falls die Datei eric.sfv nicht vorhanden ist, wird sie, wie bei EricMtCreateKey(), erstellt.
     * Eine Passwortänderung von einem Portalzertifikat (POZ) ist nicht möglich.
     *
     * Pfade müssen auf Windows in der für Datei-Funktionen benutzten ANSI-Codepage,
     * auf Linux, AIX und Linux Power in der für das Dateisystem benutzten Locale und auf macOS in der
     * "decomposed form" von UTF-8 übergeben werden.
     * Bitte weitere Betriebssystemspezifika bzgl. nicht erlaubter Zeichen in
     * Pfaden und Pfadtrennzeichen beachten.
     * Für Details zu Pfaden im ERiC siehe Entwicklerhandbuch Kapitel
     * "Übergabe von Pfaden an ERiC API-Funktionen"
     *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
     * @param[in] psePath
     *            In dem angegebenen Pfad liegt das Schlüsselpaar
     *            (eric_private.p12 und eric_public.cer).
     * @param[in] oldPin
     *            Bisherige PIN.
     * @param[in] newPin
     *            Neue PIN. Die Mindestlänge beträgt 4 Stellen. Zulässige
     *            Zeichen sind alle ASCII-Zeichen ohne die Kontrollzeichen.
     *
     * @return
     *         - ::ERIC_OK
     *         - ::ERIC_GLOBAL_UNGUELTIGER_PARAMETER
     *         - ::ERIC_GLOBAL_UNGUELTIGER_PARAMETER
     *         - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
     *         - ::ERIC_GLOBAL_UNKNOWN
     *         - ::ERIC_CRYPT_PIN_STAERKE_NICHT_AUSREICHEND
     *         - ::ERIC_CRYPT_PIN_ENTHAELT_UNGUELTIGE_ZEICHEN
     *         - ::ERIC_CRYPT_E_PSE_PATH
     *         - ::ERIC_CRYPT_NICHT_UNTERSTUETZTES_PSE_FORMAT
     *         - ::ERIC_CRYPT_ERROR_CREATE_KEY

     *
     * @see
    *         - EricMtCreateKey()
    *         - ERiC-Entwicklerhandbuch.pdf, Kap. "Zuordnung der API-Funktionen zur Verwendung von POZ, CEZ und AHZ"
     */
    ERICAPI_IMPORT int STDCALL EricMtChangePassword(
    EricInstanzHandle instanz,
        const byteChar* psePath,
        const byteChar* oldPin,
        const byteChar* newPin);

     /**
     * @brief Die Bundesfinanzamtsnummer wird überprüft.
     *
     * Wird eine 13-stellige Steuernummer im
     * ELSTER-Steuernummernformat angegeben, so wird nur die
     * Bundesfinanzamtsnummer (= die ersten 4 Stellen der
     * 13-stelligen Steuernummer) geprüft.
     *
     * Eine Prüfung der Steuernummer selbst findet nicht statt
     * (hierfür EricMtPruefeSteuernummer() verwenden).
     *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
     * @param[in] steuernummer
     *            13-stellige Steuernummer im ELSTER Steuernummernformat bzw.
     *            4-stellige Bundesfinanzamtsnummer.
     * @return
     *         - ::ERIC_OK
     *         - ::ERIC_GLOBAL_BUFANR_UNBEKANNT:
     *           Die Bundesfinanzamtsnummer ist unbekannt oder ungültig.
     *         - ::ERIC_GLOBAL_NULL_PARAMETER:
     *           Es wurde keine Bundesfinanzamtsnummer übergeben (Parameter ist NULL).
     *         - ::ERIC_GLOBAL_COMMONDATA_NICHT_VERFUEGBAR
     *         - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
     *         - ::ERIC_GLOBAL_UNKNOWN
     *
     * @see
     *      - EricMtPruefeSteuernummer()
     *      - Pruefung_der_Steuer-_und_Steueridentifikatsnummer.pdf
     */
    ERICAPI_IMPORT int STDCALL EricMtPruefeBuFaNummer(
    EricInstanzHandle instanz,
        const byteChar* steuernummer);

    /**
    * @brief Das @c xml wird gegen das Schema der @c datenartVersion validiert.
    *
    *        Das verwendete Schema kann unter common\\Schnittstellenbeschreibungen\\ nachgeschlagen werden.
    *        \n Nicht unterstützte Datenartversionen:
    *        - ElsterKMV
    *        - alle Bilanz Datenartversionen
    *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
    * @param[in]  xml
    *             XML-Zeichenfolge
    * @param[in]  datenartVersion
    *             Die @c datenartVersion ist der Datenartversionmatrix zu entnehmen,
    *             siehe common\Datenartversionmatrix.xml und ERiC-Entwicklerhandbuch.pdf.
    *             Dieser Parameter darf nicht NULL sein und muss zu den XML-Eingangsdaten passen.
    * @param[out] fehlertextPuffer Handle auf einen Rückgabepuffer, in den
    *             Fehlertexte geschrieben werden.
    *             Zur Erzeugung, Verwendung und Freigabe von Rückgabepuffern
    *             siehe Dokumentation zu ::EricRueckgabepufferHandle.
    *
    * @return
    *         - ::ERIC_OK
    *         - ::ERIC_GLOBAL_NULL_PARAMETER
    *         - ::ERIC_GLOBAL_FUNKTION_NICHT_UNTERSTUETZT: Schemavalidierung wird für die übergebene @c datenartVersion nicht unterstützt.
    *         - ::ERIC_GLOBAL_COMMONDATA_NICHT_VERFUEGBAR
    *         - ::ERIC_GLOBAL_DATENARTVERSION_UNBEKANNT
    *         - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
    *         - ::ERIC_IO_READER_SCHEMA_VALIDIERUNGSFEHLER: Die Fehlerbeschreibung steht im fehlertextPuffer.
    *         - ::ERIC_IO_PARSE_FEHLER: Die Fehlerbeschreibung steht im fehlertextPuffer.
    *         - weitere, siehe eric_fehlercodes.h
    */
    ERICAPI_IMPORT int STDCALL EricMtCheckXML(
    EricInstanzHandle instanz,
        const char* xml,
        const char* datenartVersion,
        EricRueckgabepufferHandle fehlertextPuffer);

    /**
     * @brief Das Zertifikat-Handle @c hToken wird freigegeben.
     *
     * Diese Funktion gibt das übergebene Zertifikat-Handle frei.
     * Zertifikat-Handles sollten möglichst frühzeitig, d.h. wenn sie nicht
     * mehr benötigt werden, mit EricMtCloseHandleToCertificate() freigegeben
     * werden, spätestens jedoch zum Programmende bzw. vor dem Entladen der
     * ericapi Bibliothek.
     * Das Ad Hoc-Zertifikat eines neuen Personalausweises sollte immer genau
     * dann freigegeben werden, wenn es nicht mehr benötigt wird, jedoch spätestens
     * vor Ablauf der 24 Stunden, die das Ad Hoc-Zertifikat gültig ist.
     * Tritt ein Fehler auf, kann die Fehlermeldung mit EricMtHoleFehlerText()
     * ausgelesen werden.
     *
    * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
     * @param[in] hToken
     *            Zertifikat-Handle wie von der Funktion
     *            EricMtGetHandleToCertificate() zurückgeliefert.
     * @return
     *         - ::ERIC_OK
     *         - ::ERIC_CRYPT_E_INVALID_HANDLE
     *         - ::ERIC_GLOBAL_UNGUELTIGER_PARAMETER
     *         - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
     *         - ::ERIC_GLOBAL_UNKNOWN
     * \n\n <b>Nur bei Verwendung des neuen Personalausweises:</b>
     *         - ::ERIC_TRANSFER_EID_CLIENTFEHLER
     *         - ::ERIC_TRANSFER_EID_FEHLENDEFELDER
     *         - ::ERIC_TRANSFER_EID_IDENTIFIKATIONABGEBROCHEN
     *         - ::ERIC_TRANSFER_EID_NPABLOCKIERT
     *         - ::ERIC_TRANSFER_EID_IDNRNICHTEINDEUTIG
     *         - ::ERIC_TRANSFER_EID_KEINCLIENT
     *         - ::ERIC_TRANSFER_EID_KEINKONTO
     *         - ::ERIC_TRANSFER_EID_SERVERFEHLER
     *         - ::ERIC_TRANSFER_ERR_CONNECTSERVER
     *         - ::ERIC_TRANSFER_ERR_NORESPONSE
     *         - ::ERIC_TRANSFER_ERR_PROXYAUTH
     *         - ::ERIC_TRANSFER_ERR_PROXYCONNECT
     *         - ::ERIC_TRANSFER_ERR_SEND
     *         - ::ERIC_TRANSFER_ERR_SEND_INIT
     *         - ::ERIC_TRANSFER_ERR_TIMEOUT
     *
     * @see
    *         - EricMtGetHandleToCertificate()
    *         - EricMtGetPinStatus()
    *         - ERiC-Entwicklerhandbuch.pdf, Kap. "Authentifizierung mit dem neuen Personalausweis (nPA)"
    */
    ERICAPI_IMPORT int STDCALL EricMtCloseHandleToCertificate(
    EricInstanzHandle instanz,
        EricZertifikatHandle hToken);

    /**
     * @brief Es werden die Kryptomittel für ein clientseitig erzeugtes
     *        Zertifikat (CEZ) in einem Verzeichnis des Dateisystems
     *        erstellt.
     *
     * Im angegebenen Verzeichnis @c pfad sind nach Ausführung
     * der Funktion EricMtCreateKey() drei Dateien erstellt worden:
     * - eric_public.cer: Enthält das Zertifikat mit den Daten aus
     *   @c zertifikatInfo und darin den öffentlichen Schlüssel.
     * - eric_private.p12: Enthält den privaten Schlüssel. Der
     *   Zugriff ist über die @c pin geschützt.
     * - eric.sfv: Enthält die Prüfsumme der Dateien
     *   eric_public.cer und eric_private.p12. Die Integrität
     *   dieser beiden Dateien kann damit jederzeit überprüft werden.
     *
     * Ein CEZ kann unter anderem für die
     * Bescheiddaten-Rückübermittlung verwendet werden.
     * Weitere Informationen zur Datenabholung lesen Sie bitte im
     * ERiC-Entwicklerhandbuch.pdf nach.
     *
     * Über eine Meldung sollte der Benutzer darauf hingewiesen
     * werden, dass die Generierung der Kryptomittel je nach
     * Leistungsfähigkeit der verwendeten Hardware bis zu einigen Minuten
     * dauern kann.
     *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
     * @param[in] pin
     *            PIN (Passwort), mit der auf den privaten Schlüssel
     *            zugegriffen werden kann.
     *            \n Die Mindestlänge beträgt 4 Stellen. Zulässige
     *            Zeichen sind alle ASCII-Zeichen ohne die
     *            Kontrollzeichen.
     * @param[in] pfad
     *            Pfad (1) in dem die Kryptomittel erzeugt werden sollen.
     *            Das durch den angegebenen
     *            Pfad bezeichnete Verzeichnis muss im Dateisystem
     *            bereits existieren und beschreibbar sein.
     *            Es gibt folgende Möglichkeiten:
     *            - Absoluter Pfad: Empfehlung
     *            - Relativer Pfad: Wird an das Arbeitsverzeichnis
     *              angehängt
     *            - Leere Zeichenkette: In diesem Fall wird das
     *              Arbeitsverzeichnis verwendet.<br>
     * @param[in] zertifikatInfo
     *            Daten, die zur Identifikation des Schlüsselinhabers
     *            im Zertifikat abgelegt werden.
     *
     * (1) Pfade müssen auf Windows in der für Datei-Funktionen benutzten ANSI-Codepage,
     * auf Linux, AIX und Linux Power in der für das Dateisystem benutzten Locale und auf macOS in der
     * "decomposed form" von UTF-8 übergeben werden.
     * Bitte weitere Betriebssystemspezifika bzgl. nicht erlaubter Zeichen in
     * Pfaden und Pfadtrennzeichen beachten.
     * Für Details zu Pfaden im ERiC siehe Entwicklerhandbuch Kapitel
     * "Übergabe von Pfaden an ERiC API-Funktionen".
     *
     * @return
     * - ::ERIC_OK
     * - ::ERIC_GLOBAL_NULL_PARAMETER
     * - ::ERIC_GLOBAL_UNGUELTIGE_PARAMETER_VERSION
     * - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
     * - ::ERIC_GLOBAL_UNKNOWN
     * - ::ERIC_CRYPT_ZERTIFIKATSPFAD_KEIN_VERZEICHNIS
     * - ::ERIC_CRYPT_ZERTIFIKATSDATEI_EXISTIERT_BEREITS
     * - ::ERIC_CRYPT_PIN_STAERKE_NICHT_AUSREICHEND
     * - ::ERIC_CRYPT_PIN_ENTHAELT_UNGUELTIGE_ZEICHEN
     * - ::ERIC_CRYPT_ERROR_CREATE_KEY
     *
     * @see
     *     - EricMtChangePassword()
     *     - ERiC-Entwicklerhandbuch.pdf, Kap. "Zertifikate und Authentifizierungsverfahren"
     *     - ERiC-Entwicklerhandbuch.pdf, Kap. "Übergabe von Pfaden an ERiC API-Funktionen"
     */
    ERICAPI_IMPORT int STDCALL EricMtCreateKey(
    EricInstanzHandle instanz,
        const byteChar* pin,
        const byteChar* pfad,
        const eric_zertifikat_parameter_t* zertifikatInfo);

    /**
     * @brief Diese Funktion erzeugt einen TransferHeader.
     *
     * Dieser ist der oberste Header in der Datenstruktur.
     * Er enthält Felder für die Kommunikation zwischen Server
     * und Client. Es wird nur die Kombination
     * NutzdatenHeader-Version "11" und TransferHeader-Version "11" unterstützt.
     *
     * Zur Erzeugung, Verwendung und Freigabe von Rückgabepuffern siehe Dokumentation
     * zu ::EricRueckgabepufferHandle.
     *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
     * @param[in] xml
     *            XML-Datensatz, für den der TransferHeader erzeugt werden soll.
     *            \n Es kann entweder ein komplettes Elster-XML oder nur der Datenteil übergeben werden.
     *            \n ERiC nimmt bei diesem Parameter keine Konvertierung von Sonderzeichen in Entitätenreferenzen vor.
     *            \n Attribute und Namespace-Definitionen, die in den Start-Tags der Elemente "Elster"
     *            bzw. "DatenTeil" im übergebenen XML-Datensatz definiert werden, werden nicht in
     *            das Rückgabe-XML übernommen. Im Rückgabe-XML werden im Start-Tag des
     *            Elements "Elster" die URI "http://www.elster.de/elsterxml/schema/v11" als
     *            Default-Namensraum definiert. Die dem Element "DatenTeil"
     *            untergeordneten Elemente aus dem übergebenen XML-Datensatz werden
     *            unverändert übernommen.
     *            \n Der allgemeine Aufbau des Elster-XMLs wird im
     *            ERiC-Entwicklerhandbuch.pdf im Kapitel "Datenverarbeitung mit
     *            ERiC" beschrieben.
     * @param[in] verfahren
     *            Name des Verfahrens, z.B:
     *            'ElsterAnmeldung', siehe ERiC-Entwicklerhandbuch.pdf,
     *            Tabelle "Eigenschaften der Datenart" im
     *            jeweiligen Kapitel zur Datenart.
     * @param[in] datenart
     *            Name der Datenart, z.B.:'LStB' oder 'UStVA',
     *            siehe ERiC-Entwicklerhandbuch.pdf,
     *            Tabelle "Eigenschaften der Datenart" im
     *            jeweiligen Kapitel zur Datenart.
     * @param[in] vorgang
     *            Name der Übertragungsart, z.B.
     *            'send-NoSig', siehe ERiC-Entwicklerhandbuch.pdf,
     *            Tabelle "Eigenschaften der Datenart" im
     *            jeweiligen Kapitel zur Datenart.
     * @param[in] testmerker
     *            Für eine Testübertragung muss der entsprechende Testmerker angegeben werden,
     *            siehe ERiC-Entwicklerhandbuch.pdf, Kap. "Test Unterstützung bei der
     *            ERiC-Anbindung". Falls ein Echtfall übertragen werden soll, muss der Wert NULL angegeben werden.
     * @param[in] herstellerId
     *            Hersteller-ID des Softwareproduktes.
     * @param[in] datenLieferant
     *            Der Wert entspricht dem XML-Element "DatenLieferant", wie es im
     *            Schema des Transferheaders der ElsterBasis-XML-Schnittstelle
     *            definiert ist.
     *            \n ERiC konvertiert bei diesem Parameter Sonderzeichen in Entitätenreferenzen.
     * @param[in] versionClient
     *            Angabe von Versionsinformation, die in der Serverantwort
     *            auch zurückgegeben wird und ausgewertet werden kann.
     *            Der Wert NULL entspricht "keine Angabe von Versionsinformation",
     *            d.h. es wird kein Element VersionClient im Transferheader erzeugt.
     *            \n ERiC konvertiert bei diesem Parameter Sonderzeichen in Entitätenreferenzen.
     * @param[in] publicKey
     *            Öffentlicher Schlüssel für die Transportverschlüsselung beim Verfahren
     *            ElsterLohn. Bei anderen Verfahren sollte NULL übergeben werden.
     *            Dieser Wert kann mit dem Rückgabewert von ::EricMtGetPublicKey() befüllt
     *            werden.
     *            Der Inhalt dieses Parameters wird in das \<TransportSchluessel>-
     *            Element der Rückgabe-XML geschrieben.
     * @param[out] xmlRueckgabePuffer Handle auf einen Rückgabepuffer, in den
     *            das Elster-XML mit dem erzeugten TransportHeader geschrieben wird,
     *            siehe ::EricRueckgabepufferHandle. Es wird immer ein vollständiger
     *            Elster-XML-Datensatz mit dem "Elster"-Element als Wurzel-Element
     *            zurückgeliefert. Bzgl. der darin enthaltenen XML-Namespace-Definitionen sind
     *            die bei der Beschreibung des Parameters "xml" genannten Einschränkungen
     *            zu berücksichtigen.
     *
     * @return
     *         - ::ERIC_OK
     *         - ::ERIC_GLOBAL_NULL_PARAMETER
     *         - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
     *         - ::ERIC_TRANSFER_ERR_XML_ENCODING: Die übergebenen XML-Daten sind
     *                  nicht UTF-8 kodiert.
     *         - ::ERIC_IO_PARSE_FEHLER
     *         - ::ERIC_IO_DATENTEILNOTFOUND
     *         - ::ERIC_IO_DATENTEILENDNOTFOUND
     *         - weitere, siehe eric_fehlercodes.h
     *
     *
     * @see
     *     - ERiC-Entwicklerhandbuch.pdf, Kap. "Datenverarbeitung mit ERiC"
     *     - ERiC-Entwicklerhandbuch.pdf, Kap. "Anwendungsfälle von EricBearbeiteVorgang()"
     *     - ERiC-Returncodes und Fehlertexte sind in eric_fehlercodes.h zu finden.
     */
    ERICAPI_IMPORT int STDCALL EricMtCreateTH(
    EricInstanzHandle instanz,
        const char* xml, const char* verfahren,
        const char* datenart, const char* vorgang,
        const char* testmerker, const char* herstellerId,
        const char* datenLieferant, const char* versionClient,
        const byteChar* publicKey,
        EricRueckgabepufferHandle xmlRueckgabePuffer);

    /**
    *  @brief Es werden die mit der Datenabholung abgeholten und verschlüsselten Daten entschlüsselt.
    *
    *  Falls während der Bearbeitung ein Fehler auftritt,
    *  liefert die Funktion EricMtHoleFehlerText() den
    *  dazugehörigen Fehlertext.
    *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
    * @param[in] zertifikatHandle Handle auf das zum Entschlüsseln zu verwendende Zertifikat.
    * @param[in] pin PIN zum Zugriff auf das Zertifikat.
    * @param[in] base64Eingabe Base64-kodierte verschlüsselte Daten, welche mit dem Verfahren
    *                          ElsterDatenabholung abgeholt wurden. Sie befinden sich in den Abholdaten im Element
    *                          /Elster[1]/DatenTeil[1]/Nutzdatenblock/Nutzdaten[1]/Datenabholung[1]/Abholung[1]/Datenpaket.
    * @param[out] rueckgabePuffer Handle auf einen Rückgabepuffer, in den
    *            die entschlüsselten Daten geschrieben werden. Im Fehlerfall ist der Inhalt des Rückgabepuffers undefiniert.
    *            Zur Erzeugung, Verwendung und Freigabe von Rückgabepuffern
    *            siehe ::EricRueckgabepufferHandle.
    *
    * @return
    *  - ::ERIC_OK
    *  - ::ERIC_GLOBAL_NULL_PARAMETER
    *  - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
    *  - ::ERIC_GLOBAL_ERR_DEKODIEREN
    *  - ::ERIC_GLOBAL_UNKNOWN
    *  - Ein Zertifikatsfehler aus dem Statuscodebereich von ::ERIC_CRYPT_E_INVALID_HANDLE = 610201101 bis 610201212
    *
    * @see
    *      - EricMtHoleFehlerText()
    *      - ERiC-Entwicklerhandbuch.pdf, Kap. "Datenabholung"
    */
    ERICAPI_IMPORT int STDCALL EricMtDekodiereDaten(
    EricInstanzHandle instanz,
        EricZertifikatHandle zertifikatHandle,
        const byteChar* pin,
        const byteChar* base64Eingabe,
        EricRueckgabepufferHandle rueckgabePuffer);

    /**
   * @brief Alle Einstellungen, der übergebenen ERiC-Instanz werden auf den jeweiligen Standardwert zurück gesetzt.
     *
     * Die Standardwerte sind im Dokument ERiC-Entwicklerhandbuch.pdf, Kap. "Vorbelegung der ERiC-Einstellungen"
     * zu finden.
     *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
     *
     * @return
     *    -    ::ERIC_OK
     *    -    ::ERIC_GLOBAL_UNKNOWN
     *    -    ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
     *    -    ::ERIC_GLOBAL_UNGUELTIGER_PARAMETER
    *
    * @see
    *      - EricMtEinstellungSetzen()
    *      - EricMtEinstellungLesen()
    *      - EricMtEinstellungZuruecksetzen()
    *      - ERiC-Entwicklerhandbuch.pdf, Kap. "Bedeutung der ERiC-Einstellungen"
    */
    ERICAPI_IMPORT int STDCALL EricMtEinstellungAlleZuruecksetzen(
    EricInstanzHandle instanz);

    /**
    * @brief Der Wert der API-Einstellung @c name wird im @c rueckgabePuffer zurück geliefert.
    *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
    * @param[in] name Name der API-Einstellung, NULL-terminierte Zeichenfolge.
    * @param[out] rueckgabePuffer Handle auf einen Rückgabepuffer, in den
    *            der Wert der API-Einstellung geschrieben wird.
    *            Zur Erzeugung, Verwendung und Freigabe von Rückgabepuffern
    *            siehe ::EricRueckgabepufferHandle.
    *
    * @return
    * - ::ERIC_OK
    * - ::ERIC_GLOBAL_EINSTELLUNG_NAME_UNGUELTIG
    * - ::ERIC_GLOBAL_NULL_PARAMETER
    * - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
    * - ::ERIC_GLOBAL_UNKNOWN
    *
    * @see
    *      - EricMtEinstellungSetzen()
    *      - EricMtEinstellungZuruecksetzen()
    *      - EricMtEinstellungAlleZuruecksetzen()
    *      - ERiC-Entwicklerhandbuch.pdf, Kap. "Bedeutung der ERiC-Einstellungen"
    */
    ERICAPI_IMPORT int STDCALL EricMtEinstellungLesen(
    EricInstanzHandle instanz,
        const char* name,
        EricRueckgabepufferHandle rueckgabePuffer);

    /**
    * @brief Die API-Einstellung @c name wird auf den @c wert gesetzt.
    *
    * Nach dem Laden der ERiC-Bibliotheken hat jede API-Einstellung
    * ihren Standardwert. Mit dieser Funktion kann der Wert
    * verändert werden. Der Wertebereich der jeweiligen API-Einstellung
    * ist zu beachten.
    *
    * Bei Pfad-Einstellungen muss auf Windows der Wert in der für Datei-Funktionen
    * benutzten ANSI-Codepage, auf Linux, AIX und Linux Power in der für das Dateisystem benutzten Locale
    * und auf macOS in der "decomposed form" von UTF-8 übergeben werden.
    * Bitte weitere Betriebssystemspezifika bzgl. nicht erlaubter Zeichen in
    * Pfaden und Pfadtrennzeichen beachten.
    * Für Details zu Pfaden im ERiC siehe Entwicklerhandbuch Kapitel
    * "Übergabe von Pfaden an ERiC API-Funktionen"
    *
    * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
    * @param[in] name Name der API-Einstellung, NULL-terminierte Zeichenfolge.
    * @param[in] wert Wert der API-Einstellung, NULL-terminierte Zeichenfolge.
    *
    * @return
    * - ::ERIC_OK
    * - ::ERIC_GLOBAL_EINSTELLUNG_NAME_UNGUELTIG
    * - ::ERIC_GLOBAL_EINSTELLUNG_WERT_UNGUELTIG
    * - ::ERIC_GLOBAL_NULL_PARAMETER
    * - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
    * - ::ERIC_GLOBAL_UNKNOWN
    *
    * @see
    *      - EricMtEinstellungLesen()
    *      - EricMtEinstellungZuruecksetzen()
    *      - EricMtEinstellungAlleZuruecksetzen()
    *      - ERiC-Entwicklerhandbuch.pdf, Kap. "Bedeutung der ERiC-Einstellungen"
    */
    ERICAPI_IMPORT int STDCALL EricMtEinstellungSetzen(
    EricInstanzHandle instanz,
        const char* name,
        const char* wert);

    /**
    * @brief Der Wert der API-Einstellung @c name wird auf den Standardwert zurück gesetzt.
    *
    * Die Standardwerte sind im Dokument ERiC-Entwicklerhandbuch.pdf,
    * Kap. "Vorbelegung der ERiC-Einstellungen" zu finden.
    *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
    * @param[in] name Name der API-Einstellung, NULL-terminierte Zeichenfolge.
    *
    * @return
    * - ::ERIC_OK
    * - ::ERIC_GLOBAL_EINSTELLUNG_NAME_UNGUELTIG
    * - ::ERIC_GLOBAL_NULL_PARAMETER
    * - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
    * - ::ERIC_GLOBAL_UNKNOWN
    *
    * @see
    *      - EricMtEinstellungSetzen()
    *      - EricMtEinstellungLesen()
    *      - EricMtEinstellungAlleZuruecksetzen()
    *      - ERiC-Entwicklerhandbuch.pdf, Kap. "Bedeutung der ERiC-Einstellungen"
    */
    ERICAPI_IMPORT int STDCALL EricMtEinstellungZuruecksetzen(
    EricInstanzHandle instanz,
        const char* name);

    /**
    * @brief Für die übergebene ERiC-Instanz werden alle verwendeten Plugin-Bibliotheken entladen und deren Speicher wird freigegeben.
     *
     * Der ERiC lädt die für die Bearbeitung notwendigen Plugin-Bibliotheken permanent
     * in den Speicher und gibt diese erst mit dem Aufruf dieser Funktion wieder frei.
     * \n Falls eine Plugin-Bibliothek nicht entladen werden kann,
     * wird dies in eric.log protokolliert. Der Returncode ist immer ::ERIC_OK.
     *
     * @note
     *       Wenn die Steuersoftware darauf angewiesen ist, den ERiC erfolgreich
     *       und komplett zu entladen, muss zuvor EricMtEntladePlugins() aufgerufen werden.
     *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
     *
     * @return
     *         - ::ERIC_OK
     *         - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
     *         - ::ERIC_GLOBAL_UNGUELTIGER_PARAMETER
     *         - ::ERIC_GLOBAL_UNKNOWN
     *
     * @see
     *         - ERiC-Entwicklerhandbuch.pdf, Kap. "Verwendung von EricEntladePlugins()"
     */
    ERICAPI_IMPORT int STDCALL EricMtEntladePlugins(
    EricInstanzHandle instanz);

    /**
    * @brief Die Steuernummer @c eingabeSteuernummer wird in das Bescheid-Format des jeweiligen Bundeslandes umgewandelt.
    *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
    * @param[in] eingabeSteuernummer
    *            Gültige, zu formatierende Steuernummer im
    *            ELSTER-Steuernummernformat.
    * @param[out] rueckgabePuffer Handle auf einen Rückgabepuffer, in den
    *            die formatierte Steuernummer im Bescheid-Format des
    *            jeweiligen Bundeslandes geschrieben wird.
    *            Zur Erzeugung, Verwendung und Freigabe von Rückgabepuffern
    *            siehe Dokumentation zu ::EricRueckgabepufferHandle.
    *
    * @return
    *     - ::ERIC_OK
    *     - ::ERIC_GLOBAL_NULL_PARAMETER
    *     - ::ERIC_GLOBAL_STEUERNUMMER_UNGUELTIG
    *     - ::ERIC_GLOBAL_COMMONDATA_NICHT_VERFUEGBAR
    *     - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
    *     - ::ERIC_GLOBAL_UNKNOWN
    *
    * @see
    *     - Pruefung_der_Steuer_und_Steueridentifikatsnummer.pdf
    */
    ERICAPI_IMPORT int STDCALL EricMtFormatStNr(
    EricInstanzHandle instanz,
        const byteChar* eingabeSteuernummer,
        EricRueckgabepufferHandle rueckgabePuffer);

    /**
    * @brief Die Auswahlliste(n) für @c datenartVersion oder @c feldkennung wird zurück geliefert.
    *
    * Anwendungsfälle:
    * 1. Parameter @c feldkennung ist nicht NULL: Die Funktion liefert die zur @c feldkennung und @c datenartVersion gehörige Auswahlliste.
    * 2. Parameter @c feldkennung ist NULL: Die Funktion liefert alle zur @c datenartVersion gehörigen Feldkennungen mit hinterlegten Auswahllisten.
    *
    * Für die Ermittlung der Auswahllisten vieler Feldkennungen wird aus Performanzgründen Anwendungsfall 2 empfohlen.
    * Die Funktion liefert Auswahllisten zu Feldkennungen vom Format "NichtAbgeschlosseneEnumeration" zurück.
    * Diese Auswahllisten werden auch in der Jahres-/Deltadokumentation dokumentiert.
    *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
    * @param[in] datenartVersion
    *            Dieser Parameter darf nicht NULL sein.
    *            Die gültigen Datenartversionen sind in common\Datenartversionmatrix.xml enthalten.
    * @param[in] feldkennung
    *            Feldkennung, für welche die Auswahlliste zu ermitteln ist.
    * @param[out] rueckgabeXmlPuffer Handle auf einen Rückgabepuffer, in den
    *            die angeforderten Auswahlliste(n) als XML-Daten geschrieben werden.
    *            Die XML-Daten folgen der XML Schema Definition in common/API-Rueckgabe-Schemata/EricGetAuswahlListen.xsd.
    *            \n Zur Erzeugung, Verwendung und Freigabe von Rückgabepuffern
    *            siehe ::EricRueckgabepufferHandle.
    *
    * Beispiel:
    * \code
    * <?xml version="1.0" encoding="UTF-8"?>
    * <EricGetAuswahlListen xmlns="http://www.elster.de/EricXML/1.0/EricGetAuswahlListen">
    *     <AuswahlListe>
    *         <Feldkennung>0104110</Feldkennung>
    *         <ListenElement>Arbeitslosengeld</ListenElement>
    *         <ListenElement>Elterngeld</ListenElement>
    *         <ListenElement>Insolvenzgeld</ListenElement>
    *         <ListenElement>Krankengeld</ListenElement>
    *         <ListenElement>Mutterschaftsgeld</ListenElement>
    *     </AuswahlListe>
    * </EricGetAuswahlListen>
    * \endcode
    *
    * @return
    *         - ::ERIC_OK
    *         - ::ERIC_GLOBAL_NULL_PARAMETER
    *         - ::ERIC_GLOBAL_KEINE_DATEN_VORHANDEN
    *         - ::ERIC_GLOBAL_DATENARTVERSION_UNBEKANNT
    *         - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
    *         - ::ERIC_GLOBAL_UNKNOWN
    */
    ERICAPI_IMPORT int STDCALL EricMtGetAuswahlListen(
    EricInstanzHandle instanz,
        const char* datenartVersion,
        const char* feldkennung,
        EricRueckgabepufferHandle rueckgabeXmlPuffer);

    /**
     * @brief Aus dem Antwort-XML des Finanzamtservers wird das Transferticket und Returncodes/Fehlermeldungen zurückgegeben.
     *
     * Die Funktion liefert bei erfolgreicher Ausführung:
     * - Das Transferticket aus dem Antwort-XML in dem Parameter @c transferticketPuffer.
     * - Den Returncode und die Fehlermeldung aus dem Transferheader
     * in den Parametern @c returncodeTHPuffer und @c fehlertextTHPuffer.
     * - Für jeden Nutzdatenheader dessen Returncode und Fehlermeldung als XML-Daten im
     * Parameter @c returncodesUndFehlertexteNDHXmlPuffer nach XML Schema Definition
     * common/API-Rueckgabe-Schemata/EricGetErrormessagesFromXMLAnswer.xsd.
     * Enthält das Antwort-XML keine Nutzdaten, wird kein \<Fehler\> Element zurückgegeben.
     *
     * Zur Erzeugung, Verwendung und Freigabe von Rückgabepuffern siehe Dokumentation
     * zu ::EricRueckgabepufferHandle.
     *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
     * @param[in] xml
     *            Antwort-XML des ELSTER-Servers, das ausgewertet
     *            werden soll. Der originale XML-Server-Datenstrom
     *            sollte unverändert übergeben werden und darf
     *            insbesondere keine Zeilenumbruchzeichen enthalten.
     * @param[out] transferticketPuffer Handle auf einen Rückgabepuffer, in den
     *            das Transferticket geschrieben wird, siehe ::EricRueckgabepufferHandle.
     * @param[out] returncodeTHPuffer Handle auf einen Rückgabepuffer, in den
     *            der Returncode aus dem Transferheader geschrieben wird.
     *            Siehe ::EricRueckgabepufferHandle.
     * @param[out] fehlertextTHPuffer Handle auf einen Rückgabepuffer, in den
     *            die Fehlermeldung aus dem Transferheader geschrieben wird, siehe
     *            ::EricRueckgabepufferHandle.
     * @param[out] returncodesUndFehlertexteNDHXmlPuffer Handle auf einen
     *            Rückgabepuffer, in den die Liste der Returncodes nach XML-Schema
     *            common/API-Rueckgabe-Schemata/EricGetErrormessagesFromXMLAnswer.xsd
     *            geschrieben werden, siehe ::EricRueckgabepufferHandle.
     *
     * Beispiel:
     * \code
     * <?xml version="1.0" encoding="UTF-8"?>
     * <EricGetErrormessagesFromXMLAnswer xmlns="http://www.elster.de/EricXML/1.0/EricGetErrormessagesFromXMLAnswer">
     *     <Fehler>
     *       <Code>1</Code>
     *       <Meldung>Fehlermeldung 1</Meldung>
     *     </Fehler>
     *     <Fehler>
     *       <Code>2</Code>
     *       <Meldung>Fehlermeldung 2</Meldung>
     *     </Fehler>
     *     (...)
     * </EricGetErrormessagesFromXMLAnswer>
     * \endcode
     *
     * @return
     *         - ::ERIC_OK
     *         - ::ERIC_IO_PARSE_FEHLER
     *         - ::ERIC_GLOBAL_NULL_PARAMETER
     *         - ::ERIC_GLOBAL_PUFFER_ZUGRIFFSKONFLIKT
     *         - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
     *         - ::ERIC_GLOBAL_UNKNOWN
     *
     * @note
     * - Diese Funktion kann nicht dafür verwendet werden, die Antwort im Datenteil
     *   aus einer dekodierten Serverantwort für Lohnsteuerbescheinigungen
     *   auszuwerten.
     *
     * @see
     *      - XML-Schema des Transferheaders: common\Schnittstellenbeschreibungen\ElsterBasisSchema\th000011_extern.xsd
     *      - XML-Schema des Nutzdatenheaders: common\Schnittstellenbeschreibungen\ElsterBasisSchema\ndh000011.xsd
     *      - ERiC-Entwicklerhandbuch.pdf, Kap. "Schnittstellenbeschreibungen",
     *        Tabelle "Ergänzende Softwarepakete und Dateien – Schnittstellenbeschreibungen"
     */
    ERICAPI_IMPORT int STDCALL EricMtGetErrormessagesFromXMLAnswer(
    EricInstanzHandle instanz,
        const char* xml,
        EricRueckgabepufferHandle transferticketPuffer,
        EricRueckgabepufferHandle returncodeTHPuffer,
        EricRueckgabepufferHandle fehlertextTHPuffer,
        EricRueckgabepufferHandle returncodesUndFehlertexteNDHXmlPuffer);

    /**
     * @brief Für das übergebene Zertifikat in @c pathToKeystore wird das Handle @c hToken
     *        und die unterstützten PIN-Werte @c iInfoPinSupport zurückgeliefert.
     *
     * Das ERiC API benötigt Zertifikat-Handles typischerweise
     * bei kryptografischen Operationen.
     * \n Zertifikat-Handles sollten möglichst frühzeitig, d.h. wenn sie nicht
     * mehr benötigt werden, mit EricMtCloseHandleToCertificate() freigegeben
     * werden, spätestens jedoch zum Programmende bzw. vor dem Entladen der
     * ericapi Bibliothek.
     *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
     * @param[out] hToken Handle zu einem der folgenden Zertifikate:
     *                      - Portalzertifikat
     *                      - clientseitig erzeugtes Zertifikat
     *                      - Ad Hoc-Zertifikat für den neuen Personalausweis
     * @param[out] iInfoPinSupport
     *            Wird in @c iInfoPinSupport ein Zeiger ungleich NULL übergeben und die Funktion mit ::ERIC_OK beendet,
     *            dann enthält @c iInfoPinSupport einen vorzeichenlosen Integer-Wert.
     *            In diesem Wert ist kodiert abgelegt, ob eine PIN-Eingabe
     *            erforderlich ist und welche PIN-Statusinformationen
     *            unterstützt werden.
     *            Die kodierten Werte (nachfolgend in hexadezimaler Form angegeben)
     *            können durch ein binäres ODER kombiniert werden und bedeuten im
     *            Einzelnen:
     *            - 0x00: Keine PIN-Angabe erforderlich, kein PIN-Status unterstützt.
     *            - 0x01: PIN-Angabe für Signatur erforderlich.
     *            - 0x02: PIN-Angabe für Entschlüsselung erforderlich.
     *            - 0x04: PIN-Angabe für Verschlüsselung des Zertifikats erforderlich.
     *            - 0x08: reserviert (wird derzeit nicht verwendet)
     *            - 0x10: PIN-Status "Pin Ok" wird unterstützt.
     *            - 0x20: PIN-Status "Der letzte Versuch der Pin-Eingabe schlug fehl" wird unterstützt.
     *            - 0x40: PIN-Status "Beim nächsten fehlerhaften Versuch wird die Pin gesperrt" wird unterstützt.
     *            - 0x80: PIN-Status "Pin ist gesperrt" wird unterstützt.
     *            \n Falls vom Aufrufer NULL übergeben wird, gibt die Funktion nichts zurück.
     * @param[in] pathToKeystore
     *            1. Clientseitig erzeugtes Zertifikat: <br>
     *              Pfad zum Verzeichnis, in dem sich die Zertifikats-Datei (.cer)
     *              und die Datei mit dem privaten Schlüssel (.p12) befinden.
     *              Diese Kryptomittel wurden mit EricMtCreateKey() erzeugt.
     *              Der Pfad zum Verzeichnis ist bei clientseitig erzeugten
     *              Zertifikaten relativ zum aktuellen Arbeitsverzeichnis oder absolut
     *              anzugeben.
     *            2. Software-Portalzertifikat: <br>
     *              Pfad zur Software-Zertifikatsdatei (i.d.R. mit der Endung .pfx).
     *              Der Pfad zur Datei ist bei Software-Zertifikaten relativ zum
     *              aktuellen Arbeitsverzeichnis oder absolut anzugeben.
     *            3. Sicherheitsstick: <br>
     *              Pfad zur Treiberdatei, siehe (1). Bitte beachten, dass der Treiber
     *              betriebssystemabhängig sein kann. Weitere Informationen in der
     *              Anleitung zum Sicherheitsstick oder unter
     *              https://www.sicherheitsstick.de .
     *            4. Signaturkarte: <br>
     *              Pfad zur Treiberdatei, welcher einen Zugriff auf die
     *              Signaturkarte ermöglicht, siehe (1). Weitere Informationen in
     *              der Anleitung zur Signaturkarte.
     *            5. Neuer Personalausweis (nPA): <br>
     *              URL des eID-Clients wie zum Beispiel der AusweisApp 2
     *              In den meisten Fällen lautet diese URL: http://127.0.0.1:24727/eID-Client
     *              Optional kann auf die folgende Weise noch ein Testmerker angehängt werden:
     *              http://127.0.0.1:24727/eID-Client?testmerker=520000000
     *              Zu den verfügbaren Testmerkern siehe ERiC-Entwicklerhandbuch.pdf,
     *              Kap. "Test Unterstützung bei der ERiC-Anbindung".
     *              \n\b Wichtig: Das Ad Hoc-Zertifikat, das in diesem Fall für den neuen
     *                       Personalausweis erzeugt wird, ist nur 24 Stunden gültig.
     *
     *            (1) Bei Sicherheitssticks und Signaturkarten ist bei der Angabe
     *            des Treibers der Suchmechanismus nach dynamischen Modulen des
     *            jeweiligen Betriebssystems zu berücksichtigen. Weitere
     *            Informationen sind z.B. unter Windows der Dokumentation der
     *            LoadLibrary() oder unter Linux und macOS der Dokumentation der
     *            dlopen() zu entnehmen.
     *
     *            Pfade müssen auf Windows in der für Datei-Funktionen benutzten ANSI-Codepage,
     *            auf Linux, AIX und Linux Power in der für das Dateisystem benutzten Locale 
     *            und auf macOS in der "decomposed form" von UTF-8 übergeben werden.
     *            Bitte weitere Betriebssystemspezifika bzgl. nicht erlaubter Zeichen in
     *            Pfaden und Pfadtrennzeichen beachten.
     *            Für Details zu Pfaden im ERiC siehe Entwicklerhandbuch Kapitel
     *            "Übergabe von Pfaden an ERiC API-Funktionen"
     *
     * @return
     *         - ::ERIC_OK
     *         - ::ERIC_GLOBAL_NULL_PARAMETER
     *         - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
     *         - ::ERIC_GLOBAL_UNKNOWN
     *         - ::ERIC_CRYPT_NICHT_UNTERSTUETZTES_PSE_FORMAT
     *         - ::ERIC_CRYPT_E_MAX_SESSION
     *         - ::ERIC_CRYPT_E_PSE_PATH
     *         - ::ERIC_CRYPT_E_BUSY
     *         - ::ERIC_CRYPT_E_P11_SLOT_EMPTY
     *         - ::ERIC_CRYPT_E_NO_SIG_ENC_KEY
     *         - ::ERIC_CRYPT_E_LOAD_DLL
     *         - ::ERIC_CRYPT_E_NO_SERVICE
     *         - ::ERIC_CRYPT_E_ESICL_EXCEPTION
     * \n\n <b>Nur bei Verwendung des neuen Personalausweises:</b>
     *         - ::ERIC_TRANSFER_EID_CLIENTFEHLER
     *         - ::ERIC_TRANSFER_EID_FEHLENDEFELDER
     *         - ::ERIC_TRANSFER_EID_IDENTIFIKATIONABGEBROCHEN
     *         - ::ERIC_TRANSFER_EID_NPABLOCKIERT
     *         - ::ERIC_TRANSFER_EID_IDNRNICHTEINDEUTIG
     *         - ::ERIC_TRANSFER_EID_KEINCLIENT
     *         - ::ERIC_TRANSFER_EID_KEINKONTO
     *         - ::ERIC_TRANSFER_EID_SERVERFEHLER
     *         - ::ERIC_TRANSFER_ERR_CONNECTSERVER
     *         - ::ERIC_TRANSFER_ERR_NORESPONSE
     *         - ::ERIC_TRANSFER_ERR_PROXYAUTH
     *         - ::ERIC_TRANSFER_ERR_PROXYCONNECT
     *         - ::ERIC_TRANSFER_ERR_SEND
     *         - ::ERIC_TRANSFER_ERR_SEND_INIT
     *         - ::ERIC_TRANSFER_ERR_TIMEOUT
     *
     * @see
     *         - EricMtCloseHandleToCertificate()
     *         - EricMtGetPinStatus()
     */
    ERICAPI_IMPORT int STDCALL EricMtGetHandleToCertificate(
    EricInstanzHandle instanz,
        EricZertifikatHandle* hToken,
        uint32_t* iInfoPinSupport,
        const byteChar* pathToKeystore);

    /**
     * @brief Der PIN-Status wird für ein passwortgeschütztes Kryptomittel abgefragt und in @c pinStatus zurückgegeben.
     *
     * Der PIN-Status wird für einen passwortgeschützten Bereich
     * ermittelt, der durch das übergebene Zertifikat-Handle im
     * Parameter @c hToken referenziert wird. Da bei Sicherheitssticks
     * und Signaturkarten durch ein einziges Zertifikat-Handle zwei
     * Schlüsselpaare referenziert werden können (eines für die
     * Signatur und eines für die Verschlüsselung von Daten),
     * muss grundsätzlich der Parameter @c keyType gesetzt werden.
     * \n Mit dem Rückgabewert der Funktion kann der Endanwender
     * rechtzeitig informiert werden, falls bei einer weiteren
     * falschen PIN-Eingabe das Kryptomittel gesperrt wird. Im
     * Fehlerfall ist @c pinStatus nicht definiert.
     * \n Der Karten- bzw. Stickhersteller ist verantwortlich,
     * dass seine Implementierung den korrekten PIN-Status
     * zurückgibt, siehe auch
     * Tabelle "PIN-Statusabfrage für POZ" im Unterkap.
     * "Das Portalzertifikat (POZ)" im Dokument
     * ERiC-Entwicklerhandbuch.pdf.
     *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
     * @param[in] hToken
     *            Zertifikat-Handle für dessen passwortgeschützten
     *            Bereich der PIN-Status ermittelt werden soll.
     *            Wird von der Funktion
     *            EricMtGetHandleToCertificate() zurückgeliefert.
     * @param[out] pinStatus
     *            Mögliche Rückgabewerte:
     *            - 0: StatusPinOk: Kein Fehlversuch oder keine
     *                 Informationen verfügbar
     *            - 1: StatusPinLocked: PIN gesperrt
     *            - 2: StatusPreviousPinError: Die letzte
     *                 PIN-Eingabe war fehlerhaft
     *            - 3: StatusLockedIfPinError: Beim nächsten
     *                 fehlerhaften Versuch wird die PIN gesperrt
     * @param[in] keyType
     *            Mögliche Eingabewerte:
     *            - 0: eSignatureKey: Schlüssel für die Signatur
     *                 von Daten
     *            - 1: eEncryptionKey: Schlüssel für die
     *                 Verschlüsselung von Daten
     *
     * @return
     *         - ::ERIC_OK
     *         - ::ERIC_GLOBAL_NULL_PARAMETER
     *         - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
     *         - weitere, siehe eric_fehlercodes.h
     *
     * @see
     *     - EricMtGetHandleToCertificate()
     *     - ERiC-Entwicklerhandbuch.pdf, Kap. "Zertifikate und Authentifizierungsverfahren"
     *
     */
    ERICAPI_IMPORT int STDCALL EricMtGetPinStatus(
    EricInstanzHandle instanz,
        EricZertifikatHandle hToken,
        uint32_t* pinStatus,
        uint32_t keyType);

    /**
     * @brief Es wird der öffentliche Schlüssel als base64-kodierte Zeichenkette
     *        für das übergebene Zertifikat in @c cryptoParameter zurückgeliefert.
     *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
     * @param[in] cryptoParameter
     *            Die Struktur enthält das Zertifikat-Handle und die PIN. Der Abrufcode wird ignoriert.
     *            Falls der Zugriff auf den öffentlichen Schlüssel keine PIN erfordert,
     *            ist PIN=NULL anzugeben.
     * @param[out] rueckgabePuffer
     *            Handle auf den Rückgabepuffer. Bei Erfolg enthält
     *            der Rückgabepuffer den öffentlichen Schlüssel als base64-kodierte Zeichenkette.
     *            \n Zur Erzeugung, Verwendung und Freigabe von Rückgabepuffern
     *            siehe Dokumentation zu ::EricRueckgabepufferHandle.
     *
     * @return
     *         - ::ERIC_OK
     *         - ::ERIC_GLOBAL_NULL_PARAMETER
     *         - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
     *         - ::ERIC_CRYPT_E_INVALID_HANDLE
     *         - ::ERIC_CRYPT_E_P12_ENC_KEY
     *         - ::ERIC_CRYPT_E_PIN_WRONG
     *         - ::ERIC_CRYPT_E_PIN_LOCKED
     *         - weitere, siehe eric_fehlercodes.h
     *
     */
    ERICAPI_IMPORT int STDCALL EricMtGetPublicKey(
    EricInstanzHandle instanz,
        const eric_verschluesselungs_parameter_t* cryptoParameter,
        EricRueckgabepufferHandle rueckgabePuffer);

    /**
     * @brief  Es wird die Klartextfehlermeldung zu dem @c fehlerkode ermittelt.
     *
     * Die Funktion liefert die Klartextfehlermeldung zu einem
     * ERiC Fehlercode - definiert in eric_fehlercodes.h
     *
     *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
     * @param[in] fehlerkode
     *            Eingabe-Fehlercode, definiert in eric_fehlercodes.h.
     * @param[out] rueckgabePuffer Handle auf einen Rückgabepuffer, in den
     *            die Klartextfehlermeldung geschrieben wird.
     *            Zur Erzeugung, Verwendung und Freigabe von Rückgabepuffern
     *            siehe Dokumentation zu ::EricRueckgabepufferHandle. \n
     *            Die Klartextfehlermeldung ist gemäß UTF-8 kodiert. \n
     *
     * @return
     *        - ::ERIC_OK
     *        - ::ERIC_GLOBAL_NULL_PARAMETER
     *        - ::ERIC_GLOBAL_FEHLERMELDUNG_NICHT_VORHANDEN
     *        - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
     *        - ::ERIC_GLOBAL_UNKNOWN
     */
    ERICAPI_IMPORT int STDCALL EricMtHoleFehlerText(
    EricInstanzHandle instanz,
        int fehlerkode,
        EricRueckgabepufferHandle rueckgabePuffer);

    /**
     * @brief Es wird die Finanzamtliste für eine bestimmte @c finanzamtLandNummer zurückgegeben.
     *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
     * @param[in] finanzamtLandNummer
     *              Die Finanzamtlandnummer besteht aus den ersten zwei Stellen der
     *              Bundesfinanzamtsnummer. Eine Liste aller Finanzamtlandnummern wird
     *              von ::EricMtHoleFinanzamtLandNummern() zurückgegeben.
     * @param[out] rueckgabeXmlPuffer Handle auf einen Rückgabepuffer, in den
     *            die Ergebnis XML-Daten geschrieben werden.
     *            Die XML-Daten folgen der XML Schema Definition
     *            common/API-Rueckgabe-Schemata/EricHoleFinanzaemter.xsd.
     *            Zur Erzeugung, Verwendung und Freigabe von Rückgabepuffern
     *            siehe Dokumentation zu ::EricRueckgabepufferHandle.
     *
     * Beispiel:
     * \code
     * <?xml version="1.0" encoding="UTF-8"?>
     * <EricHoleFinanzaemter xmlns="http://www.elster.de/EricXML/1.0/EricHoleFinanzaemter">
     *     <Finanzamt>
     *       <BuFaNummer>2801</BuFaNummer>
     *       <Name>Finanzamt Offenburg Außenstelle Achern</Name>
     *     </Finanzamt>
     *     <Finanzamt>
     *       <BuFaNummer>2804</BuFaNummer>
     *       <Name>Finanzamt Villingen-Schwenningen Außenstelle Donaueschingen</Name>
     *     </Finanzamt>
     *     (...)
     * </EricHoleFinanzaemter>
     * \endcode
     *
     * @return
     *        - ::ERIC_OK
     *        - ::ERIC_GLOBAL_NULL_PARAMETER
     *        - ::ERIC_GLOBAL_UTI_COUNTRY_NOT_SUPPORTED
     *        - ::ERIC_GLOBAL_COMMONDATA_NICHT_VERFUEGBAR
     *        - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
     *        - ::ERIC_GLOBAL_UNKNOWN
     */
    ERICAPI_IMPORT int STDCALL EricMtHoleFinanzaemter(
    EricInstanzHandle instanz,
        const byteChar* finanzamtLandNummer,
        EricRueckgabepufferHandle rueckgabeXmlPuffer);

    /**
     * @brief Die Liste aller Finanzamtlandnummern wird zurückgegeben.
     *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
     * @param[out] rueckgabeXmlPuffer Handle auf einen Rückgabepuffer, in den
     *            die Ergebnis XML-Daten geschrieben werden.
     *            Die XML-Daten folgen der XML Schema Definition
     *            common/API-Rueckgabe-Schemata/EricHoleFinanzamtLandNummern.xsd.
     *            Zur Erzeugung, Verwendung und Freigabe von Rückgabepuffern
     *            siehe Dokumentation zu ::EricRueckgabepufferHandle.
     *
     * Beispiel:
     * \code
     * <?xml version="1.0" encoding="UTF-8"?>
     * <EricHoleFinanzamtLandNummern xmlns="http://www.elster.de/EricXML/1.0/EricHoleFinanzamtLandNummern">
     *     <FinanzamtLand>
     *       <FinanzamtLandNummer>28</FinanzamtLandNummer>
     *       <Name>Baden-Württemberg</Name>
     *     </FinanzamtLand>
     *     <FinanzamtLand>
     *       <FinanzamtLandNummer>91</FinanzamtLandNummer>
     *       <Name>Bayern (Zuständigkeit LfSt - München)</Name>
     *     </FinanzamtLand>
     *     (...)
     * </EricHoleFinanzamtLandNummern>
     * \endcode
     *
     * @return
     *        - ::ERIC_OK
     *        - ::ERIC_GLOBAL_NULL_PARAMETER
     *        - ::ERIC_GLOBAL_COMMONDATA_NICHT_VERFUEGBAR
     *        - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
     *        - ::ERIC_GLOBAL_UNKNOWN
     */
    ERICAPI_IMPORT int STDCALL EricMtHoleFinanzamtLandNummern(
    EricInstanzHandle instanz,
        EricRueckgabepufferHandle rueckgabeXmlPuffer);

    /**
     * @brief Die @c finanzamtsdaten werden für eine Bundesfinanzamtsnummer zurückgegeben.
     *
     * Die Bundesfinanzamtsnummer kann über die Kombination der Funktionen ::EricMtHoleFinanzamtLandNummern()
     * und ::EricMtHoleFinanzaemter() ermittelt werden.
     *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
     * @param[in] bufaNr
     *            Übergabe der 4-stelligen Bundesfinanzamtsnummer.
     *
     * @param[out] rueckgabeXmlPuffer Handle auf einen Rückgabepuffer, in den
     *            die Ergebnis XML-Daten geschrieben werden.
     *            Die XML-Daten folgen der XML Schema Definition
     *            common/API-Rueckgabe-Schemata/EricHoleFinanzamtsdaten.xsd.
     *            Zur Erzeugung, Verwendung und Freigabe von Rückgabepuffern
     *            siehe Dokumentation zu ::EricRueckgabepufferHandle.
     *
     * @return
     *        - ::ERIC_OK
     *        - ::ERIC_GLOBAL_NULL_PARAMETER: Parameter @c bufaNr ist NULL.
     *        - ::ERIC_GLOBAL_PRUEF_FEHLER: Die übergebene Bundesfinanzamtsnummer ist keine Ganzzahl.
     *        - ::ERIC_GLOBAL_KEINE_DATEN_VORHANDEN: Immer bei Testfinanzämtern.
     *        - ::ERIC_GLOBAL_COMMONDATA_NICHT_VERFUEGBAR
     *        - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
     *        - ::ERIC_GLOBAL_UNKNOWN
     *
     * @see
     *    - ::EricMtHoleFinanzamtLandNummern()
     *    - ::EricMtHoleFinanzaemter()
     */
    ERICAPI_IMPORT int STDCALL EricMtHoleFinanzamtsdaten(
    EricInstanzHandle instanz,
        const byteChar bufaNr[5],
        EricRueckgabepufferHandle rueckgabeXmlPuffer);

    /**
     * @brief Die Testfinanzamtliste wird in @c rueckgabeXmlPuffer zurückgegeben.
     *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
     * @param[out] rueckgabeXmlPuffer Handle auf einen Rückgabepuffer, in den
     *            die Ergebnis XML-Daten geschrieben werden.
     *            Die XML-Daten folgen der XML Schema Definition
     *            common/API-Rueckgabe-Schemata/EricHoleTestFinanzaemter.xsd.
     *            Zur Erzeugung, Verwendung und Freigabe von Rückgabepuffern
     *            siehe Dokumentation zu ::EricRueckgabepufferHandle.
     *
     * Beispiel:
     * \code
     * <?xml version="1.0" encoding="UTF-8"?>
     * <EricHoleTestFinanzaemter xmlns="http://www.elster.de/EricXML/1.0/EricHoleTestFinanzaemter">
     *   <Finanzamt>
     *       <BuFaNummer>1096</BuFaNummer>
     *       <Name>Testfinanzamt Saarland</Name>
     *     </Finanzamt>
     *     <Finanzamt>
     *       <BuFaNummer>1097</BuFaNummer>
     *       <Name>Finanzschule (Edenkoben)</Name>
     *     </Finanzamt>
     *     (...)
     * </EricHoleTestFinanzaemter>
     * \endcode
     *
     * @return
     *        - ::ERIC_OK
     *        - ::ERIC_GLOBAL_NULL_PARAMETER
     *        - ::ERIC_GLOBAL_COMMONDATA_NICHT_VERFUEGBAR
     *        - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
     *        - ::ERIC_GLOBAL_UNKNOWN
     */
    ERICAPI_IMPORT int STDCALL EricMtHoleTestfinanzaemter(
    EricInstanzHandle instanz,
        EricRueckgabepufferHandle rueckgabeXmlPuffer);

   /**
    * @brief Die Eigenschaften des übergebenen Zertifikats werden im @c rueckgabeXmlPuffer zurückgegeben.
    *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
    * @param[in] hToken Handle des Zertifikats, dessen Eigenschaften geholt werden sollen.
    *            Wird von der Funktion EricMtGetHandleToCertificate() zurückgeliefert.
    * @param[in] pin PIN zum Öffnen des Zertifikats. Wird bei Software-Portalzertifikaten
    *            benötigt.
    * @param[out] rueckgabeXmlPuffer Handle auf einen Rückgabepuffer, in den die Zertifikateigenschaften
    *             im XML-Format geschrieben werden. Das Format ist im XML Schema
    *             common/API-Rueckgabe-Schemata/EricHoleZertifikatEigenschaften.xsd
    *             definiert.
    *             Zur Erzeugung, Verwendung und Freigabe von Rückgabepuffern
    *             siehe Dokumentation zu ::EricRueckgabepufferHandle.
    *
    * Beispiel:
    * \code
    * <EricHoleZertifikatEigenschaften xmlns="http://www.elster.de/EricXML/1.0/EricHoleZertifikatEigenschaften">
    *   <Signaturzertifikateigenschaften>
    *       <AusgestelltAm>20090317163811Z</AusgestelltAm>
    *       <GueltigBis>20170317163811Z</GueltigBis>
    *       <Signaturalgorithmus>sha256WithRSAEncryption(1.2.840.113549.1.1.11)</Signaturalgorithmus>
    *       <PublicKeyMD5>9e107d9d372bb6826bd81d3542a419d6</PublicKeyMD5>
    *       <PublicKeySHA1>2fd4e1c67a2d28fced849ee1bb76e7391b93eb12</PublicKeySHA1>
    *       <PublicKeyBitLength>2048</PublicKeyBitLength>
    *       <Issuer>
    *           <Info><Name>CN</Name><Wert>ElsterSoftCA</Wert></Info>
    *           <Info><Name>OU</Name><Wert>CA</Wert></Info>
    *           (...)
    *       </Issuer>
    *       <Identifikationsmerkmaltyp>Steuernummer</Identifikationsmerkmaltyp>
    *       <Registrierertyp>Person</Registrierertyp>
    *       <Verifikationsart>Postweg</Verifikationsart>
    *       <TokenTyp>Software</TokenTyp>
    *       <Testzertifikat>true</Testzertifikat>
    *   </Signaturzertifikateigenschaften>
    *   <Verschluesselungszertifikateigenschaften>
    *       (...)
    *   </Verschluesselungszertifikateigenschaften>
    * </EricHoleZertifikatEigenschaften>
    * \endcode
    *
    * @return
    *      - ::ERIC_OK
    *      - ::ERIC_GLOBAL_NULL_PARAMETER
    *      - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
    *      - ::ERIC_GLOBAL_UNKNOWN
    *      - ERIC_CRYPT_E_*: Ein Zertifikatsfehler aus dem Statuscodebereich
    *                        von ::ERIC_CRYPT_E_INVALID_HANDLE = 610201101 bis 610201212
    *
    * @see
    *      - ERiC-Entwicklerhandbuch.pdf, Kap. "Verwendung von EricHoleZertifikatEigenschaften()"
    *      - common/API-Rueckgabe-Schemata/EricHoleZertifikatEigenschaften.xsd
    *
    **/
    ERICAPI_IMPORT int STDCALL EricMtHoleZertifikatEigenschaften(
    EricInstanzHandle instanz,
        EricZertifikatHandle hToken,
        const byteChar * pin,
        EricRueckgabepufferHandle rueckgabeXmlPuffer);

    /**
     * @brief  Der Fingerabdruck und dessen Signatur wird für das übergebene
     *         Zertifikat zurückgegeben.
     *
     * Zur Erzeugung, Verwendung und Freigabe von Rückgabepuffern siehe Dokumentation
     * zu ::EricRueckgabepufferHandle.
     *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
     * @param[in] cryptoParameter Zertifikatsdaten,
     *            siehe eric_verschluesselungs_parameter_t.
     *            Das in der übergebenen Struktur referenzierte Zertifikat muss
     *            ein clientseitig erzeugtes Zertifikat (CEZ) sein.
     * @param[out] fingerabdruckPuffer Handle auf einen Rückgabepuffer, in
     *            den der Fingerabdruck geschrieben wird, siehe
     *            ::EricRueckgabepufferHandle.
     * @param[out] signaturPuffer Handle auf einen Rückgabepuffer, in den
     *            die Signatur des Fingerabdrucks geschrieben wird, siehe
     *            ::EricRueckgabepufferHandle.
     *
     * @note Die Erzeugung eines Fingerabdrucks mit dieser Funktion ist nur
     *       in Zusammenhang mit clientseitig erzeugten Zertifikaten definiert.
     *
     * @return
     * - ::ERIC_OK
     * - ::ERIC_GLOBAL_NULL_PARAMETER
     * - ::ERIC_GLOBAL_PUFFER_ZUGRIFFSKONFLIKT
     * - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
     * - ::ERIC_GLOBAL_UNKNOWN
     * - ::ERIC_CRYPT_E_P12_READ
     * - ::ERIC_CRYPT_E_P12_DECODE
     * - ::ERIC_CRYPT_E_PIN_WRONG
     * - ::ERIC_CRYPT_E_P12_SIG_KEY
     * - ::ERIC_CRYPT_E_P12_ENC_KEY
     * - ::ERIC_CRYPT_ZERTIFIKAT
     * - ::ERIC_CRYPT_EIDKARTE_NICHT_UNTERSTUETZT
     * - ::ERIC_CRYPT_SIGNATUR
     * - ::ERIC_CRYPT_CORRUPTED
     */
    ERICAPI_IMPORT int STDCALL EricMtHoleZertifikatFingerabdruck(
    EricInstanzHandle instanz,
        const eric_verschluesselungs_parameter_t* cryptoParameter,
        EricRueckgabepufferHandle fingerabdruckPuffer,
        EricRueckgabepufferHandle signaturPuffer);


/**
 * @brief Erstellt und initialisiert eine neue ERiC-Instanz.
 *
 * Der erzeugte EricInstanzHandle ist im Parameter \c instanz der
 * Multithreading-API zu übergeben. Das Erzeugen einer ERiC-Instanz
 * ist ressourcen- und zeitintensiv. Zum Beenden einer ERiC-Instanz ist
 * EricMtInstanzFreigeben() aufzurufen.
 *
 * @param[in] pluginPfad Pfad, in dem die Plugins rekursiv gesucht werden.
 *                       Ist der Zeiger gleich \c NULL, wird der Pfad zur
 *                       Bibliothek ericapi verwendet.
 *
 * @param[in] logPfad Optionaler Pfad zur Log-Datei eric.log. Ist der
 *                    Wert gleich \c NULL, wird das betriebssystemspezifische
 *                    Verzeichnis für temporäre Dateien verwendet.
 *
 * @return
 *          - ::EricInstanzHandle != NULL: Zeiger auf die erzeugte ERiC-Instanz.
 *          - ::EricInstanzHandle == NULL: Fehler, Fehlerursache siehe Protokolldatei eric.log
 *
 * @see
 *      - EricMtInstanzFreigeben()
 */
ERICAPI_IMPORT EricInstanzHandle STDCALL EricMtInstanzErzeugen(
    const char *pluginPfad,
    const char *logPfad);

/**
 * @brief Die übergebene ERiC-Instanz wird beendet und deren Speicher freigegeben.
 *
 * Die freigegebene ERiC-Instanz kann nicht mehr verwendet werden. Andere
 * ERiC-Instanzen bleiben von der Freigabe unberührt und können weiter
 * verwendet werden.
 *
 * @param[in] instanz ERiC-Instanz, die freigegeben werden soll.
 *
 * @return
 *         - ::ERIC_OK
 *         - ::ERIC_GLOBAL_UNGUELTIGE_INSTANZ
 *         - ::ERIC_GLOBAL_UNGUELTIGER_PARAMETER
 *         - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
 *         - ::ERIC_GLOBAL_UNKNOWN
 *
 * @see
 *      - EricMtEntladePlugins()
 *      - EricMtInstanzErzeugen()
 */
ERICAPI_IMPORT int STDCALL EricMtInstanzFreigeben(
    EricInstanzHandle instanz);



    /**
     * @brief Es wird eine Steuernummer im ELSTER-Steuernummerformat erzeugt.
     *
     * Die Funktion erzeugt aus einer angegebenen Steuernummer
     * im Format des Steuerbescheides eine 13-stellige
     * Steuernummer im ELSTER-Steuernummerformat.<br>
     *
     * Die sich ergebende 13-stellige Steuernummer im
     * ELSTER-Steuernummerformat wird von der Funktion
     * ::EricMtMakeElsterStnr() auch auf Gültigkeit geprüft.<br>
     *
     * Einer der beiden Parameter @c landesnr oder @c bundesfinanzamtsnr muss
     * korrekt angegeben werden. Der jeweils andere Parameter darf NULL oder leer
     * sein. Bei bayerischen und berliner Steuernummern im Format BBB/UUUUP ist die
     * Angabe der Bundesfinanzamtsnummer zwingend erforderlich.
     *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
     * @param[in]  steuernrBescheid
     *             Format der Steuernummer wie auch auf amtlichen
     *             Schreiben angegeben.
     * @param[in]  landesnr
     *             2-stellige Landesnummer (entspricht den ersten
     *             zwei Stellen der Bundesfinanzamtsnummer).
     * @param[in]  bundesfinanzamtsnr
     *             4-stellige Bundesfinanzamtsnummer.
     * @param[out] steuernrPuffer Handle auf einen Rückgabepuffer, in den
     *             die Steuernummer im ELSTER-Steuernummerformat geschrieben wird.
     *             Zur Erzeugung, Verwendung und Freigabe von Rückgabepuffern
     *             siehe Dokumentation zu ::EricRueckgabepufferHandle.
     *
     * @return
     *  - ::ERIC_OK
     *  - ::ERIC_GLOBAL_STEUERNUMMER_UNGUELTIG
     *  - ::ERIC_GLOBAL_LANDESNUMMER_UNBEKANNT
     *  - ::ERIC_GLOBAL_NULL_PARAMETER
     *  - ::ERIC_GLOBAL_UNGUELTIGER_PARAMETER
     *  - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
     *  - ::ERIC_GLOBAL_UNKNOWN
     */
    ERICAPI_IMPORT int STDCALL EricMtMakeElsterStnr(
    EricInstanzHandle instanz,
    const byteChar* steuernrBescheid,
    const byteChar landesnr[2+1],
    const byteChar bundesfinanzamtsnr[4+1],
    EricRueckgabepufferHandle steuernrPuffer);

    /**
     * @brief Die @c bic wird auf Gültigkeit überprüft.
     *
     * Die Prüfung erfolgt in zwei Schritten:
     *
     * -# Formale Prüfung auf gültige Zeichen und richtige Länge.
     * -# Prüfung, ob das Länderkennzeichen für BIC gültig ist.
     *
     * Falls die BIC ungültig ist liefert die Funktion EricMtHoleFehlerText()
     * den zugehörigen Fehlertext.
     *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
     * @param[in] bic Zeiger auf eine NULL-terminierte Zeichenkette.
     *
     * @return
     *  - ::ERIC_OK
     *  - ::ERIC_GLOBAL_BIC_FORMALER_FEHLER: Ungültige Zeichen, falsche Länge.
     *  - ::ERIC_GLOBAL_BIC_LAENDERCODE_FEHLER
     *  - ::ERIC_GLOBAL_NULL_PARAMETER: Parameter @c bic ist NULL.
     *  - ::ERIC_GLOBAL_COMMONDATA_NICHT_VERFUEGBAR
     *  - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
     *  - ::ERIC_GLOBAL_UNKNOWN
     *
     * @see
     *      - ERiC-Entwicklerhandbuch.pdf, Kap. "BIC ISO-Ländercodes"
     *      - ERiC-Entwicklerhandbuch.pdf, Kap. "BIC-Prüfung"
     */
    ERICAPI_IMPORT int STDCALL EricMtPruefeBIC(
    EricInstanzHandle instanz,
        const byteChar* bic);

    /**
     * @brief Die @c iban wird auf Gültigkeit überprüft.
     *
     * Die Prüfung erfolgt in vier Schritten:
     *
     * -# Formale Prüfung auf gültige Zeichen und richtige Länge.
     * -# Prüfung, ob das Länderkennzeichen für IBAN gültig ist.
     * -# Prüfung, ob das länderspezifische Format gültig ist.
     * -# Prüfung, ob die Prüfziffer der IBAN gültig ist.
     *
     * Falls die IBAN ungültig ist liefert die Funktion EricMtHoleFehlerText()
     * den zugehörigen Fehlertext.
     *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
     * @param[in] iban Zeiger auf eine NULL-terminierte Zeichenkette.
     *
     * @return
     *  - ::ERIC_OK
     *  - ::ERIC_GLOBAL_IBAN_FORMALER_FEHLER: Ungültige Zeichen, falsche Länge.
     *  - ::ERIC_GLOBAL_IBAN_LAENDERCODE_FEHLER
     *  - ::ERIC_GLOBAL_IBAN_LANDESFORMAT_FEHLER
     *  - ::ERIC_GLOBAL_IBAN_PRUEFZIFFER_FEHLER
     *  - ::ERIC_GLOBAL_NULL_PARAMETER: Parameter @c iban ist NULL.
     *  - ::ERIC_GLOBAL_COMMONDATA_NICHT_VERFUEGBAR
     *  - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
     *  - ::ERIC_GLOBAL_UNKNOWN
     *
     * @see
     *      - ERiC-Entwicklerhandbuch.pdf, Kap. "IBAN - länderspezifische Formate"
     *      - ERiC-Entwicklerhandbuch.pdf, Kap. "IBAN-Prüfung"
     *
     */
    ERICAPI_IMPORT int STDCALL EricMtPruefeIBAN(
    EricInstanzHandle instanz,
        const byteChar* iban);

    /**
     * @brief Die @c steuerId wird auf Gültigkeit überprüft.
     *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
     * @param[in] steuerId
     *            Steuer-Identifikationsnummer (IdNr)
     *
     * @return
     *  - ::ERIC_OK
     *  - ::ERIC_GLOBAL_NULL_PARAMETER
     *  - ::ERIC_GLOBAL_IDNUMMER_UNGUELTIG
     *  - ::ERIC_GLOBAL_COMMONDATA_NICHT_VERFUEGBAR
     *  - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
     *  - ::ERIC_GLOBAL_UNKNOWN
     *
     * @see
     *         - EricMtPruefeSteuernummer()
     *         - ERiC-Entwicklerhandbuch.pdf, Kap. "Prüfung der Steueridentifikationsnummer (IdNr)"
     *         - ERiC-Entwicklerhandbuch.pdf, Kap. "Test-Steueridentifikationsnummer"
     */
    ERICAPI_IMPORT int STDCALL EricMtPruefeIdentifikationsMerkmal(
    EricInstanzHandle instanz,
        const byteChar* steuerId);

    /**
     * @brief Die @c steuernummer wird einschließlich Bundesfinanzamtsnummer
     *        auf formale Richtigkeit geprüft.
     *
     * Zur Prüfung der Bundesfinanzamtsnummer wird
     * ::EricMtPruefeBuFaNummer() verwendet.
     *
     * Zur Erzeugung, Verwendung und Freigabe von Rückgabepuffern siehe Dokumentation
     * zu ::EricRueckgabepufferHandle.
     *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
     * @param[in] steuernummer NULL-terminierte 13-stellige Steuernummer im
     *            ELSTER-Steuernummernformat.
     *
     * @return
     *  - ::ERIC_OK
     *  - ::ERIC_GLOBAL_NULL_PARAMETER
     *  - ::ERIC_GLOBAL_STEUERNUMMER_UNGUELTIG
     *  - ::ERIC_GLOBAL_COMMONDATA_NICHT_VERFUEGBAR
     *  - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
     *  - ::ERIC_GLOBAL_UNKNOWN
     *
     * @see
     *         - EricMtPruefeBuFaNummer()
     *         - Pruefung_der_Steuer-_und_Steueridentifikatsnummer.pdf
     */
    ERICAPI_IMPORT int STDCALL EricMtPruefeSteuernummer(
    EricInstanzHandle instanz,
        const byteChar* steuernummer);

    /**
     * @brief Prüft, ob die @c pin zum Zertifikat @c pathToKeystore passt. 
     *        Nicht anwendbar auf Ad Hoc-Zertifikate (AHZ), die für einen
     *        neuen Personalausweis (nPA) ausgestellt sind.
     *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
     * @param[in] pathToKeystore
     *            Folgende Zertifikatstypen werden unterstützt:
     *            1. Clientseitig erzeugtes Zertifikat: <br>
     *              Pfad zum Verzeichnis, in dem sich die Zertifikats-Datei (.cer)
     *              und die Datei mit dem privaten Schlüssel (.p12) befinden.
     *              Diese Kryptomittel wurden mit EricMtCreateKey() erzeugt.
     *              Der Pfad zum Verzeichnis ist bei clientseitig erzeugten
     *              Zertifikaten relativ zum aktuellen Arbeitsverzeichnis oder absolut
     *              anzugeben.
     *            2. Software-Portalzertifikat: <br>
     *              Pfad zur Software-Zertifikatsdatei (i.d.R. mit der Endung .pfx).
     *              Der Pfad zur Datei ist bei Software-Zertifikaten relativ zum
     *              aktuellen Arbeitsverzeichnis oder absolut anzugeben.
     *            3. Sicherheitsstick: <br>
     *              Pfad zur Treiberdatei, siehe (2). Bitte beachten, dass der Treiber
     *              betriebssystemabhängig sein kann. Weitere Informationen in der
     *              Anleitung zum Sicherheitsstick oder unter
     *              https://www.sicherheitsstick.de .
     *            4. Signaturkarte: <br>
     *              Pfad zur Treiberdatei, welcher einen Zugriff auf die
     *              Signaturkarte ermöglicht, siehe (2). Weitere Informationen in
     *              der Anleitung zur Signaturkarte.
     * @param[in] pin
     *            PIN für den Zugriff auf den privaten Schlüssel des Zertifikats.
     * @param[in] keyType
     *            Mögliche Eingabewerte:
     *            - 0: eSignatureKey: Schlüssel für die Signatur von Daten, siehe (1).
     *            - 1: eEncryptionKey: Schlüssel für die Verschlüsselung von Daten, siehe (1).
     *
    *  (1) Bei einem Zertifikat wie dem mit EricMtCreateKey() clientseitig
     *  erzeugten Zertifikat (CEZ), das nur einen einzigen, gemeinsamen Schlüssel
     *  für Signatur und Verschlüsselung besitzt, sind beide Eingabewerte
     *  erlaubt. Die Werte beziehen sich dann beide auf denselben Schlüssel.
     *
     *  (2) Bei Sicherheitssticks und Signaturkarten ist bei der Angabe
     *  des Treibers der Suchmechanismus nach dynamischen Modulen des
     *  jeweiligen Betriebssystems zu berücksichtigen. Weitere
     *  Informationen sind z.B. unter Windows der Dokumentation der
     *  LoadLibrary() oder unter Linux und macOS der Dokumentation der
     *  dlopen() zu entnehmen.
     *
     *  Pfade müssen auf Windows in der für Datei-Funktionen benutzten ANSI-Codepage,
     *  auf Linux, AIX und Linux Power in der für das Dateisystem benutzten Locale 
     *  und auf macOS in der "decomposed form" von UTF-8 übergeben werden.
     *  Bitte weitere Betriebssystemspezifika bzgl. nicht erlaubter Zeichen in
     *  Pfaden und Pfadtrennzeichen beachten.
     *  Für Details zu Pfaden im ERiC siehe Entwicklerhandbuch Kapitel
     *  "Übergabe von Pfaden an ERiC API-Funktionen".
     *
     *  Es wird empfohlen, geöffnete Zertifikatshandle zu schließen, bevor mit
     *  der API-Funktion EricMtPruefeZertifikatPin() das gewünschte Zertifikat geprüft wird.
     *
     *  @note Eine falsche PIN-Eingabe erhöht bei Sicherheitsstick und Signaturkarte den
     *        Zähler für Fehlversuche. Welche Zertifikatstypen aufgrund von 3 Fehlversuchen
     *        gesperrt werden, ist im ERiC-Entwicklerhandbuch.pdf Kap.
     *        "Das Portalzertifikat (POZ)" beschrieben.
     *
     * @return
     *  - ::ERIC_OK
     *  - ::ERIC_CRYPT_E_PIN_WRONG
     *  - ::ERIC_CRYPT_NICHT_UNTERSTUETZTES_PSE_FORMAT
     *  - ::ERIC_CRYPT_EIDKARTE_NICHT_UNTERSTUETZT
     *  - ::ERIC_CRYPT_E_PSE_PATH
     *  - ::ERIC_GLOBAL_NULL_PARAMETER
     *  - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
     *  - ::ERIC_GLOBAL_UNKNOWN
     */
     ERICAPI_IMPORT int STDCALL EricMtPruefeZertifikatPin(
    EricInstanzHandle instanz,
        const byteChar *pathToKeystore,
        const byteChar *pin,
        uint32_t keyType);

    /**
     * @brief Die @c funktion wird als Callback-Funktion für
     *        ::EricMtBearbeiteVorgang() registriert.
     *
     * Die registrierte Callback-Funktion wird von der Funktion
     * ::EricMtBearbeiteVorgang() aufgerufen, um bei der Verarbeitung
     * den Fortschritt der einzelnen Arbeitsbereiche anzuzeigen.
     *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
     * @param funktion      Zeiger auf die zu registrierende Funktion oder \c NULL.
     * @param benutzerdaten Zeiger, der der registrierten Funktion immer
     *                      mitgegeben wird. Die Anwendung kann diesen Parameter
     *                      dazu verwenden, einen Zeiger auf eigene Daten oder
     *                      Funktionen an die zu registrierende Funktion
     *                      übergeben zu lassen.
     *
     * @return
     *  - ::ERIC_OK
     *  - ::ERIC_GLOBAL_UNGUELTIGER_PARAMETER
     *  - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
     *  - ::ERIC_GLOBAL_UNKNOWN
     *
     * @remark
     *         - Wenn eine zuvor registrierte Funktion nicht mehr aufgerufen
     *           werden soll, ist ::EricMtRegistriereFortschrittCallback() mit dem
     *           Wert \c NULL im Parameter \c funktion aufzurufen.
     *         - Es ist nicht erlaubt eine ERiC API-Funktion aus einer Callback-Funktion aufzurufen.
     *         - Die Verarbeitung im Callback findet synchron statt.
     *           Deshalb sollte der Callback sehr schnell ausgeführt werden.
     *
     * @see
     *         - EricFortschrittCallback
     *         - EricMtBearbeiteVorgang()
     *         - ERiC-Entwicklerhandbuch.pdf, Kap. "Funktionen für Fortschrittcallbacks"
     *
     */
    ERICAPI_IMPORT int STDCALL EricMtRegistriereFortschrittCallback(
    EricInstanzHandle instanz,
        EricFortschrittCallback funktion,
        void* benutzerdaten);

    /**
     * @brief Die registrierte @c funktion wird als Callback-Funktion
     *        von ::EricMtBearbeiteVorgang() aufgerufen und zeigt den
     *        Gesamtfortschritt der Verarbeitung an.
     *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
     * @param funktion      Zeiger auf die zu registrierende Funktion oder \c NULL.
     * @param benutzerdaten Zeiger, der der registrierten Funktion immer
     *                      mitgegeben wird. Die Anwendung kann diesen Parameter
     *                      dazu verwenden, einen Zeiger auf eigene Daten oder
     *                      Funktionen an die zu registrierende Funktion
     *                      übergeben zu lassen.
     *
     * @return
     *  - ::ERIC_OK
     *  - ::ERIC_GLOBAL_UNGUELTIGER_PARAMETER
     *  - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
     *  - ::ERIC_GLOBAL_UNKNOWN
     *
     * @remark
     *         - Wenn eine zuvor registrierte Funktion nicht mehr aufgerufen
     *           werden soll, ist ::EricMtRegistriereGlobalenFortschrittCallback()
     *           mit dem Wert \c NULL im Parameter \c funktion aufzurufen.
     *         - Es ist nicht erlaubt eine ERiC API-Funktion aus einer
     *           Callback-Funktion aufzurufen.
     *         - Die Verarbeitung im Callback findet synchron statt.
     *           Deshalb sollte der Callback sehr schnell ausgeführt werden.
     *
     * @see
     *         - EricMtBearbeiteVorgang()
     *         - ERiC-Entwicklerhandbuch.pdf, Kap. "Funktionen für Fortschrittcallbacks"
     */
    ERICAPI_IMPORT int STDCALL EricMtRegistriereGlobalenFortschrittCallback(
    EricInstanzHandle instanz,
        EricFortschrittCallback funktion,
        void* benutzerdaten);

    /**
     * @brief Die registrierte @c funktion wird als Callback-Funktion für
     *        jede Lognachricht aufgerufen.
     *        Die Ausgabe entspricht einer Zeile im eric.log.
     *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
     * @param funktion Zeiger auf die zu registrierende Funktion oder NULL.
     * @param schreibeEricLogDatei
     *          - \c 1  Jede Log-Nachricht wird nach eric.log geschrieben.
     *                  Der Parameter \c funktion kann auf eine
     *                  Funktion zeigen oder \c NULL sein.
     *          - \c 0  Falls \c funktion \c != \c NULL werden keine Log-Nachrichten
     *                  nach eric.log geschrieben, andernfalls werden die
     *                  Log-Nachrichten nach eric.log geschrieben.
     * @param benutzerdaten Zeiger, welcher der registrierten Funktion immer
     *                      mitgegeben wird. Die Anwendung kann diesen Parameter
     *                      dazu verwenden, einen Zeiger auf eigene Daten oder
     *                      Funktionen an die zu registrierende Funktion
     *                      übergeben zu lassen.
     *
     * @return
     *  - ::ERIC_OK
     *  - ::ERIC_GLOBAL_UNGUELTIGER_PARAMETER
     *  - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
     *  - ::ERIC_GLOBAL_UNKNOWN
     *
     * @remark
     *         - Wenn eine zuvor registrierte Funktion nicht mehr aufgerufen
     *           werden soll, ist ::EricMtRegistriereLogCallback() mit dem Wert
     *           \c NULL im Parameter \c funktion aufzurufen (=Deregistrierung).
     *         - Vor dem Beenden der Steueranwendung ist eine registrierte
     *           Funktion zu deregistrieren, da es sonst zu einem Absturz
     *           kommen kann.
     *         - Es ist nicht erlaubt eine ERiC API-Funktion aus einer
     *           Callback-Funktion aufzurufen.
     *         - Die Verarbeitung im Callback findet synchron statt.
     *           Deshalb sollte der Callback sehr schnell ausgeführt werden.
     *
     */
    ERICAPI_IMPORT int STDCALL EricMtRegistriereLogCallback(
    EricInstanzHandle instanz,
        EricLogCallback funktion,
        uint32_t schreibeEricLogDatei,
        void* benutzerdaten);

    /**
     * @brief Diese API-Funktion erzeugt einen Rückgabepuffer und gibt
     *        ein Handle darauf zurück.
     *
     * Die von dieser Funktion erzeugten Rückgabepuffer werden verwendet, um
     * die Ausgaben von ERiC-Funktionen (z.B. EricMtBearbeiteVorgang())
     * aufzunehmen. Dazu wird das Rückgabepuffer-Handle für den Schreibvorgang
     * an die ausgebende Funktion übergeben.<br>
     * Zum Auslesen des von den API-Funktionen beschriebenen Puffers wird das
     * Rückgabepuffer-Handle an EricMtRueckgabepufferInhalt() übergeben.
     * Ein einmal erzeugtes Rückgabepuffer-Handle kann für weitere nachfolgende
     * Aufrufe von ERiC API-Funktionen wiederverwendet werden. Bei einer
     * Wiederverwendung eines Handles werden frühere Inhalte überschrieben.
     * Nach Verwendung muss jeder Rückgabepuffer mit
     * EricMtRueckgabepufferFreigeben() freigegeben werden.
     * Rückgabepuffer sind der Singlethreading-API bzw. einer ERiC-Instanz der
     * Multithreading-API fest zugeordnet. Die Funktionen der ERiC API, die
     * einen Rückgabepuffer entgegen nehmen, geben den Fehlercode
     * ::ERIC_GLOBAL_PUFFER_UNGLEICHER_INSTANZ zurück, wenn der übergebene
     * Rückgabepuffer
     *      - mit der Singlethreading-API erzeugt worden ist und dann mit der
     *        Multithreading-API verwendet wird
     *      - mit der Multithreading-API erzeugt worden ist und dann mit der
     *        Singlethreading-API verwendet wird
     *      - mit einer ERiC-Instanz erzeugt worden ist und dann mit einer
     *        anderen Instanz verwendet wird.
     *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
     *
     * @return
     *         - ::EricRueckgabepufferHandle im Erfolgsfall.
     *         - NULL im Fehlerfall.
     *
     * @see
     *         - EricMtRueckgabepufferLaenge()
     *         - EricMtRueckgabepufferInhalt()
     *         - EricMtRueckgabepufferFreigeben()
     */
    ERICAPI_IMPORT EricRueckgabepufferHandle STDCALL EricMtRueckgabepufferErzeugen(
    EricInstanzHandle instanz);

    /**
     * @brief Der durch das @c handle bezeichnete Rückgabepuffer wird
     *        freigegeben.
     *
     * Das Handle darf danach nicht weiter verwendet werden. Es wird daher
     * empfohlen, Handle-Variablen nach der Freigabe explizit auf
     * NULL zu setzen.
     *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
     * @param[in] handle Handle auf einen mit EricMtRueckgabepufferErzeugen()
     *            angelegten Rückgabepuffer. Dieser Rückgabepuffer darf nicht
     *            bereits freigegeben worden sein.
     *
     * @return
     *  - ::ERIC_OK
     *  - ::ERIC_GLOBAL_UNGUELTIGER_PARAMETER
     *  - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
     *  - ::ERIC_GLOBAL_UNKNOWN
     *
     * @see
     *         - EricMtRueckgabepufferErzeugen()
     *         - EricMtRueckgabepufferLaenge()
     *         - EricMtRueckgabepufferInhalt()
     */
    ERICAPI_IMPORT int STDCALL EricMtRueckgabepufferFreigeben(
    EricInstanzHandle instanz,
        EricRueckgabepufferHandle handle);

    /**
     * @brief Der durch das @c handle bezeichnete Inhalt des Rückgabepuffers wird zurückgegeben.
     *
     * Der zurückgegebene Zeiger verweist auf ein Character-Array, das alle in
     * den Rückgabepuffer geschriebenen Bytes sowie eine abschließende
     * NULL-Terminierung enthält. Dieses Array existiert so lange im Speicher,
     * bis der Rückgabepuffer entweder (bei einer Wiederverwendung des Handles)
     * erneut beschrieben oder der Puffer explizit freigegeben wird.
     *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
     * @param[in] handle Handle auf einen mit EricMtRueckgabepufferErzeugen()
     *            angelegten Rückgabepuffer. Dieser Rückgabepuffer darf nicht
     *            bereits freigegeben worden sein.
     *
     * @return
     *         - Zeiger auf den NULL-terminierten Rückgabepufferinhalt, wenn ein gültiges Handle übergeben wird.
     *         - NULL: Bei Übergabe des ungültigen Handles NULL.
     *
     * @see
     *         - EricMtRueckgabepufferErzeugen()
     *         - EricMtRueckgabepufferLaenge()
     *         - EricMtRueckgabepufferFreigeben()
     */
    ERICAPI_IMPORT const char* STDCALL EricMtRueckgabepufferInhalt(
    EricInstanzHandle instanz,
        EricRueckgabepufferHandle handle);

    /**
     * @brief Die Länge des Rückgabepufferinhalts wird zurückgegeben.
     *
     * Die zurückgegebene Zahl entspricht der Anzahl von Bytes, die von einer
     * zuvor aufgerufenen ERiC API-Funktion in den Rückgabepuffer geschrieben
     * wurden.
     * Die NULL-Terminierung, die bei Aufruf von EricMtRueckgabepufferInhalt() an
     * das zurückgegebene Character-Array angefügt wird, wird bei dieser
     * Längenangabe nicht berücksichtigt.
     *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
     * @param[in] handle Handle auf einen mit EricMtRueckgabepufferErzeugen()
     *            angelegten Rückgabepuffer. Dieser Rückgabepuffer darf nicht
     *            bereits freigegeben worden sein.
     *
     * @return
     *         - Anzahl der in den Rückgabepuffer geschriebenen Bytes, wenn ein gültiges Handle übergeben wird.
     *         - 0: Bei Übergabe des ungültigen Handles NULL.
     *
     * @see
     *         - EricMtRueckgabepufferErzeugen()
     *         - EricMtRueckgabepufferInhalt()
     *         - EricMtRueckgabepufferFreigeben()
     */
    ERICAPI_IMPORT uint32_t STDCALL EricMtRueckgabepufferLaenge(
    EricInstanzHandle instanz,
        EricRueckgabepufferHandle handle);

    /**
     * @brief Es werden Plattform-, Betriebssystem- und
     *        ERiC-Informationen ausgegeben.
     *
     * Diese Funktion liefert Informationen über die
     * verwendeten ERiC-Bibliotheken, ERiC-Druckvorlagen,
     * die eingesetzte Plattform, den Arbeitsspeicher und
     * das verwendete Betriebssystem.
     *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
     *
     * @return
     *  - ::ERIC_OK
     *  - ::ERIC_GLOBAL_UNGUELTIGER_PARAMETER
     *  - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
     *  - weitere, siehe eric_fehlercodes.h
     *
     * @see
     *      - EricMtVersion()
     */
    ERICAPI_IMPORT int STDCALL EricMtSystemCheck(
    EricInstanzHandle instanz);

    /**
     * @brief Es wird eine Liste sämtlicher Produkt- und Dateiversionen
     *        der verwendeten ERiC-Bibliotheken als XML-Daten zurückgegeben.
     *
     * Diese Funktion kann bei auftretenden Fehlern die
     * Fehlersuche beschleunigen und Supportfälle unterstützen.
     *
   * @param[in] instanz Die ERiC-Instanz, auf der diese Funktion ausgeführt werden soll.
     * @param[out] rueckgabeXmlPuffer Handle auf einen Rückgabepuffer, in den
     *            zu allen ERiC-Bibliotheken die Produkt- und Dateiversionen
     *            als XML-Daten nach XML Schema Definition common/API-Rueckgabe-Schemata/EricVersion.xsd
     *            geschrieben werden. Zur Erzeugung, Verwendung und Freigabe von Rückgabepuffern
     *            siehe Dokumentation zu ::EricRueckgabepufferHandle.
     *
     * Beispiel:
     * \code
     *  <?xml version="1.0" encoding="UTF-8"?>
     * <EricVersion xmlns="http://www.elster.de/EricXML/1.0/EricVersion">
     *     <Bibliothek>
     *       <Name>ericapi.dll</Name>
     *       <Produktversion>99, 1, 2, 32767</Produktversion>
     *       <Dateiversion>2008, 3, 5, 0</Dateiversion>
     *     </Bibliothek>
     *     <Bibliothek>
     *       <Name>ericctrl.dll</Name>
     *       <Produktversion>99, 1, 2, 32767</Produktversion>
     *       <Dateiversion>2008, 3, 5, 0</Dateiversion>
     *     </Bibliothek>
     *     (...)
     * </EricVersion>
     * \endcode
     *
     * @return
     *  - ::ERIC_OK
     *  - ::ERIC_GLOBAL_NULL_PARAMETER
     *  - ::ERIC_GLOBAL_NICHT_GENUEGEND_ARBEITSSPEICHER
     *  - weitere, siehe eric_fehlercodes.h
     *
     * @see
     *      - EricMtSystemCheck()
     */
    ERICAPI_IMPORT int STDCALL EricMtVersion(
    EricInstanzHandle instanz,
        EricRueckgabepufferHandle rueckgabeXmlPuffer);

#ifdef __cplusplus
}
#endif

#endif /*ERICMTAPI_H_*/
