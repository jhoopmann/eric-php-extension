#include <dlfcn.h>

#include "php.h"

#include "include/ericapi.h"
#include "include/eric_fehlercodes.h"
#include "include/eric_types.h"

#ifndef PHP_ERIC_H
#define PHP_ERIC_H
#endif

#define PHP_ERIC_VERSION "1.0"
#define PHP_ERIC_EXTNAME "eric"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

extern zend_module_entry eric_module_entry;
#define eric_module_ptr &eric_module_entry
#define phpext_eric_ptr eric_module_ptre

#ifdef PHP_WIN32
#define PHP_ERIC_API __declspec(dllexport)
#else
#define PHP_ERIC_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

void *lericapi;
EricZertifikatHandle eric_cert_handle;
eric_druck_parameter_t eric_print_params = {
	.version = 2,
	.vorschau = 0,
	.ersteSeite = 0,
	.duplexDruck = 0,
	.pdfName = "eric_print.pdf",
	.fussText = NULL
};
eric_verschluesselungs_parameter_t eric_encryption_params = {
	.version = 2,
	.pin = "",
	.zertifikatHandle = NULL
};

int (*pEricInitialisiere)(const char* pluginPath, const char* logPath);
int (*pEricBeende)();
int (*pEricChangePassword)(
	const char* psePath,
	const char* oldBin,
	const char* newPin
);
int(*pEricPruefeBuFaNummer)(const char* steuernummer);
int (*pEricBearbeiteVorgang)(
	const char *datenPuffer,
	const char* datenartVersion,
	uint32_t bearbeitungsFlag,
	const eric_druck_parameter_t* druckParameter,
	const eric_verschluesselungs_parameter_t* cryptoParameter,
	EricTransferHandle* transferHandle,
	EricRueckgabepufferHandle rueckgabeXmlPuffer,
	EricRueckgabepufferHandle serverantwortXmlPuffer
);
int (*pEricCheckXML) (
	const char* xml, 
	const char* datenartVersion, 
	EricRueckgabepufferHandle fehlertextPuffer
);
int (*pEricCloseHandleToCertificate)(EricZertifikatHandle hToken);
int (*pEricCreateKey)(
	const char* pin, 
	const char *pfad, 
	eric_zertifikat_parameter_t* zertifikatInfo
);
int (*pEricCreateTH)(
	const char* xml,
	const char* verfahren,
	const char* datendart,
	const char* vorgang,
	const char* testmerker,
	const char* herstellerId,
	const char* datenLieferant,
	const char* versionClient,
	const char* pulicKey,
	EricRueckgabepufferHandle xmlRueckgabePuffer
);
int (*pEricDekodiereDaten)(
	EricZertifikatHandle zertifikatHandle,
	const char* pin,
	const char* base64Eingabe,
	EricRueckgabepufferHandle rueckgabePuffer
);
int (*pEricEinstellungAlleZuruecksetzen)();
int (*pEricEinstellungLesen)(
	const char* name,
	EricRueckgabepufferHandle rueckgabePuffer
);
int (*pEricEinstellungSetzen)(
	const char* name,
	const char* wert
);
int (*pEricEinstellungZuruecksetzen)(const char* name);
int (*pEricEntladePlugins)();
int (*pEricFormatStNr)(
	const char* eingabeSteuernummer,
	EricRueckgabepufferHandle rueckgabePuffer
);
int (*pEricGetAuswahlListen)(
	const char* datenartVersion,
	const char* feldkennung,
	EricRueckgabepufferHandle rueckgabeXmlPuffer
);
int (*pEricGetErrormessagesFromXMLAnswer)(
	const char* xml,
	EricRueckgabepufferHandle transferticketPuffer,
	EricRueckgabepufferHandle returncodeTHPuffer,
	EricRueckgabepufferHandle fehlertextTHPuffer,
	EricRueckgabepufferHandle returncodesUndFehlertexteNDHXmlPuffer
);
int (*pEricGetHandleToCertificate)(
	EricZertifikatHandle* hToken,
	uint32_t* iInfoPinSupport,
	const byteChar* pathToKeystore
);
int (*pEricGetPinStatus)(
	EricZertifikatHandle hToken,
	uint32_t* pinStatus,
	uint32_t keyType
);
int (*pEricGetPublicKey)(
	const eric_verschluesselungs_parameter_t* cryptoParameter,
	EricRueckgabepufferHandle rueckgabePuffer
);
int (*pEricHoleFehlerText)(
	int fehlerkode,
	EricRueckgabepufferHandle rueckgabePuffer
);
int (*pEricHoleFinanzaemter)(
	const char* finanzamtLandNummer,
	EricRueckgabepufferHandle rueckgabeXmlPuffer
);
int (*pEricHoleFinanzamtLandNummern)(EricRueckgabepufferHandle ruckgabeXmlPuffer);
int (*pEricHoleFinanzamtsdaten)(
	const char bufaNr[5],
	EricRueckgabepufferHandle rueckgabeXmlPuffer
);
int (*pEricHoleTestfinanzaemter)(EricRueckgabepufferHandle rueckgabeXmlPuffer);
int (*pEricHoleZertifikatEigenschaften)(
	EricZertifikatHandle hToken,
	const char* pin,
	EricRueckgabepufferHandle rueckgabeXmlPuffer
);
int (*pEricHoleZertifikatFingerabdruck) (
	const eric_verschluesselungs_parameter_t* cryptoParameter,
	EricRueckgabepufferHandle fingerabdruckPuffer,
	EricRueckgabepufferHandle signaturPuffer	
);
int (*pEricMakeElsterStnr)(
	const char* steuernrBescheid,
    const char landesnr[2+1],
    const char bundesfinanzamtsnr[4+1],
    EricRueckgabepufferHandle steuernrPuffer
);
int (*pEricPruefeBIC)(const char* bic);
int (*pEricPruefeIBAN) (const char* iban);
int (*pEricPruefeIdentifikationsMerkmal)(const char* steuerId);
int (*pEricPruefeSteuernummer) (const char* steuernummer);
int (*pEricPruefeZertifikatPin) (
	const char* pathToKeystore,
	const char* pin,
	uint32_t keyType
);
int (*pEricRegistriereFortschrittCallback)(
	EricFortschrittCallback funktion,
	void* benutzerdaten
);
int (*pEricRegistriereGlobalenFortschrittCallback)(
	EricFortschrittCallback funktion,
	void* benutzerdaten
);
int (*pEricRegistriereLogCallback)(
	EricLogCallback funktion,
	uint32_t schreibeEricLogDatei,
	void* benutzerdaten
);
EricRueckgabepufferHandle (*pEricRueckgabepufferErzeugen)();
int (*pEricRueckgabepufferFreigeben)(EricRueckgabepufferHandle handle);
const char* (*pEricRueckgabepufferInhalt)(EricRueckgabepufferHandle handle);
uint32_t (*pEricRueckgabepufferLaenge)(EricRueckgabepufferHandle handle);
int (*pEricSystemCheck)();
int (*pEricVersion)(EricRueckgabepufferHandle rueckgabeXmlPuffer);