#include "php_eric.h"

PHP_MINIT_FUNCTION(eric)
{
    lericapi = dlopen("/home/jhoopmann/projects/eric-php-extension/lib/libericapi.so", RTLD_NOW);
    if(!lericapi) {
        printf("cant load lericapi\n");
        exit(1);
    }

    pEricInitialisiere = dlsym(lericapi, "EricInitialisiere");
    pEricBeende = dlsym(lericapi, "EricBeende");
    pEricChangePassword = dlsym(lericapi, "EricChangePassword");
    pEricPruefeBuFaNummer = dlsym(lericapi, "EricPruefeBuFaNummer");
    pEricBearbeiteVorgang = dlsym(lericapi, "EricBearbeiteVorgang");
    pEricCheckXML = dlsym(lericapi, "EricCheckXML");
    pEricCloseHandleToCertificate = dlsym(lericapi, "EricClosehandleToCertificate");
    pEricCreateKey = dlsym(lericapi, "EricCreateKey");
    pEricCreateTH = dlsym(lericapi, "EricCreateTH");
    pEricDekodiereDaten = dlsym(lericapi, "EricDekodiereDaten");
    pEricEinstellungAlleZuruecksetzen = dlsym(lericapi, "EricEinstellungAlleZuruecksetzen");
    pEricEinstellungLesen = dlsym(lericapi, "EricEinstellungLesen");
    pEricEinstellungSetzen = dlsym(lericapi, "EricEinstellungSetzen");
    pEricEinstellungZuruecksetzen = dlsym(lericapi, "EricEinstellungZuruecksetzen");
    pEricEntladePlugins = dlsym(lericapi, "EricEntladePlugins");
    pEricFormatStNr = dlsym(lericapi, "EricFormatStNr");
    pEricGetAuswahlListen = dlsym(lericapi, "EricGetAuswahlListen");
    pEricGetErrormessagesFromXMLAnswer = dlsym(lericapi, "EricGetErrormessagesFromXMLAnswer");
    pEricGetHandleToCertificate = dlsym(lericapi, "EricGetHandleToCertificate");
    pEricGetPinStatus = dlsym(lericapi, "EricGetPinStatus");
    pEricGetPublicKey = dlsym(lericapi, "EricGetPublicKey");
    pEricHoleFehlerText = dlsym(lericapi, "EricHoleFehlerText");
    pEricHoleFinanzaemter = dlsym(lericapi, "EricHoleFinanzaemter");
    pEricHoleFinanzamtLandNummern = dlsym(lericapi, "EricHoleFinanzamtLandNummern");
    pEricHoleFinanzamtsdaten = dlsym(lericapi, "EricHoleFinanzamtsdaten");
    pEricHoleTestfinanzaemter = dlsym(lericapi, "EricHoleTestfinanzaemter");
    pEricHoleZertifikatEigenschaften = dlsym(lericapi, "EricHoleZertifikatEigenschaften");
    pEricHoleZertifikatFingerabdruck = dlsym(lericapi, "EricHoleZertifikatFingerabdruck");
    pEricMakeElsterStnr = dlsym(lericapi, "EricMakeElsterStnr");
    pEricPruefeBIC = dlsym(lericapi, "EricPruefeBIC");
    pEricPruefeIBAN = dlsym(lericapi, "EricPruefeIBAN");
    pEricPruefeSteuernummer = dlsym(lericapi, "EricPruefeSteuernummer");
    pEricPruefeIdentifikationsMerkmal = dlsym(lericapi, "EricPruefeIdentifikationsMerkmal");
    pEricPruefeZertifikatPin = dlsym(lericapi, "EricPruefeZertifikatPin");
    pEricRegistriereFortschrittCallback = dlsym(lericapi, "EricRegistriereFortschrittCallback");
    pEricRegistriereGlobalenFortschrittCallback = dlsym(lericapi, "EricRegistriereGlobalenFortschrittCallback");
    pEricRegistriereLogCallback = dlsym(lericapi, "EricRegistriereLogCallback");
    pEricRueckgabepufferErzeugen = dlsym(lericapi, "EricRueckgabepufferErzeugen");
    pEricRueckgabepufferFreigeben = dlsym(lericapi, "EricRueckgabepufferFreigeben");
    pEricRueckgabepufferInhalt = dlsym(lericapi, "EricRueckgabepufferInhalt");
    pEricRueckgabepufferLaenge = dlsym(lericapi, "EricRueckgabepufferLaenge");
    pEricSystemCheck = dlsym(lericapi, "EricSystemCheck");
    pEricVersion = dlsym(lericapi, "EricVersion");
}

PHP_MSHUTDOWN_FUNCTION(eric)
{
    if(lericapi) {
        dlclose(lericapi);
    }
}

PHP_FUNCTION(eric_init)
{
    if(pEricInitialisiere(
            getenv("ERICAPI_LIB_PATH"), 
            getenv("ERICAPI_LOG_PATH")
        ) == ERIC_OK
    )  {
        RETURN_BOOL(IS_TRUE);
    }
    RETURN_BOOL(IS_FALSE);
}

PHP_FUNCTION(eric_close)
{
    if(lericapi) {
        if(pEricBeende() == ERIC_OK) {
            RETURN_BOOL(IS_TRUE);
        }
    }
    RETURN_BOOL(IS_FALSE);
}

static zend_function_entry eric_functions[] = {
	PHP_FE(eric_init, NULL)
    PHP_FE(eric_close, NULL)
    PHP_FE_END
};

zend_module_entry eric_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_ERIC_EXTNAME,
    eric_functions,
    PHP_MINIT(eric),
    PHP_MSHUTDOWN(eric),
    NULL,
    NULL,
    NULL,
#if ZEND_MODULE_API_NO >= 20010901
    PHP_ERIC_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};

ZEND_GET_MODULE(eric);

