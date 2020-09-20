#include "php_eric.h"

ZEND_BEGIN_MODULE_GLOBALS(eric)
	eric_druck_parameter_t eric_print_params;
	eric_verschluesselungs_parameter_t eric_encryption_params;
    int certRequiresPin;
    int errCode;
ZEND_END_MODULE_GLOBALS(eric)
ZEND_DECLARE_MODULE_GLOBALS(eric)

PHP_MINIT_FUNCTION(eric)
{
    lericapi = dlopen("/home/jhoopmann/projects/eric-php-extension/lib/libericapi.so", RTLD_NOW);
    if(!lericapi) {
        php_log_err("cant dlopen lericapi\n");
        
        return FAILURE;
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

    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(eric)
{
    if(lericapi) {
        pEricBeende();

        dlclose(lericapi); /* no need 4 nullset */ 
    }
    
    return SUCCESS;
}

PHP_GINIT_FUNCTION(eric)
{
    return SUCCESS;
}

PHP_RINIT_FUNCTION(eric)
{
    eric_globals.errCode = 0;   

    eric_globals.certRequiresPin = 0;
    eric_globals.eric_encryption_params.pin = NULL;
    eric_globals.eric_encryption_params.zertifikatHandle = NULL;    /* do not hold; open every time we send req */

    eric_globals.eric_print_params.version = 2;
    eric_globals.eric_print_params.vorschau = 0;
    eric_globals.eric_print_params.ersteSeite = 0;
    eric_globals.eric_print_params.duplexDruck = 0;
    eric_globals.eric_print_params.pdfName = "eric_print.pdf";
	eric_globals.eric_print_params.fussText = NULL;

    return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(eric)
{
    if(eric_globals.eric_encryption_params.zertifikatHandle != NULL) {
        pEricCloseHandleToCertificate(eric_globals.eric_encryption_params.zertifikatHandle);

        eric_globals.eric_encryption_params.zertifikatHandle = NULL;
    }

    return SUCCESS;
}

PHP_FUNCTION(eric_init)
{
    int err = pEricInitialisiere(
        getenv("ERICAPI_LIB_PATH"), 
        getenv("ERICAPI_LOG_PATH")
    );
    if(err == ERIC_OK)  {
        RETURN_BOOL(IS_TRUE);
    }
    eric_globals.errCode = err;
    RETURN_BOOL(IS_FALSE);
}

PHP_FUNCTION(eric_close)
{
    int err = pEricBeende();
    if(err == ERIC_OK) {
        RETURN_BOOL(IS_TRUE);
    }
    eric_globals.errCode = err;
    RETURN_BOOL(IS_FALSE);
}

PHP_FUNCTION(eric_transfer)
{
    if(lericapi != NULL) {
        char* certPath;
        int certLength;
        char* pin;
        int pinLength;
        char* dataType; /* Est_2019 */
        int dataTypeVersionLength;
        char* xml;
        int xmlLength;
        int mode;

        ZEND_PARSE_PARAMETERS_START(5,5)
            Z_PARAM_STRING(dataType, dataTypeVersionLength)
            Z_PARAM_STRING(xml, xmlLength)
            Z_PARAM_STRING(certPath, certLength)
            Z_PARAM_STRING(pin, pinLength)
            Z_PARAM_LONG(mode)
        ZEND_PARSE_PARAMETERS_END();

        if(eric_globals.eric_encryption_params.zertifikatHandle == NULL) {
            if(pEricGetHandleToCertificate(
                    &(eric_globals.eric_encryption_params.zertifikatHandle),
                    eric_globals.certRequiresPin,
                    certPath
                ) != ERIC_OK
            ) {
                eric_globals.errCode = 303; /* eric no cert found */

                RETURN_BOOL(IS_FALSE);
            }
        }

        if(pinLength == 0) {
            if(eric_globals.certRequiresPin != 0) {
                eric_globals.errCode = 5; /* eric decryption cert err */

                RETURN_BOOL(IS_FALSE);
            }
            pin = NULL;
        }
        eric_globals.eric_encryption_params.pin = pin;

        EricRueckgabepufferHandle dataHandle = pEricRueckgabepufferErzeugen();
        EricRueckgabepufferHandle serverResponseHandle = pEricRueckgabepufferErzeugen();
        
        int err = pEricBearbeiteVorgang(
            xml,
            dataType,
            mode,
            &eric_globals.eric_print_params,
            &eric_globals.eric_encryption_params,
            NULL,
            dataHandle,
            serverResponseHandle
        );

        int bufLength = pEricRueckgabepufferLaenge(dataHandle);
        const char buf[bufLength];

        const char* _buf = pEricRueckgabepufferInhalt(dataHandle);
        memcpy(
            &buf[0],
            &_buf[0],
            bufLength
        );

        pEricRueckgabepufferFreigeben(serverResponseHandle);
        pEricRueckgabepufferFreigeben(dataHandle);

        eric_globals.errCode = err;

        if(err == ERIC_OK) {    
            RETURN_STRING(buf);   
        }
    } else {
        eric_globals.errCode = -1;
    }

    RETURN_FALSE;
}
ZEND_BEGIN_ARG_INFO_EX(eric_transfer_arginfo, 0, 0, 2)
    ZEND_ARG_INFO(0, dataType)
    ZEND_ARG_INFO(0, xml)
    ZEND_ARG_INFO(0, eric_certificate_file_path)
    ZEND_ARG_INFO(0, eric_certificate_pin)
    ZEND_ARG_INFO(0, mode)
    ZEND_ARG_INFO(1, server_response)
ZEND_END_ARG_INFO()

PHP_FUNCTION(eric_get_error)
{
    if(eric_globals.errCode == -1) {
        return "ericapilib not loaded";
    }

    if(eric_globals.errCode != 0)  {
        EricRueckgabepufferHandle buf = EricRueckgabepufferErzeugen();
        pEricHoleFehlerText(eric_globals.errCode, buf);

        eric_globals.errCode = 0;

        RETURN_STRING(EricRueckgabepufferInhalt(buf));
    }

    RETURN_NULL();
}

static zend_function_entry eric_functions[] = {
	PHP_FE(eric_init, NULL)
    PHP_FE(eric_close, NULL)
    PHP_FE(eric_transfer, NULL)
    PHP_FE_END
};

zend_module_entry eric_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_ERIC_EXTNAME,
    eric_functions,
    PHP_MINIT(eric),
    PHP_MSHUTDOWN(eric),
    PHP_RINIT(eric),
    PHP_RSHUTDOWN(eric),
    NULL,
#if ZEND_MODULE_API_NO >= 20010901
    PHP_ERIC_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};

ZEND_GET_MODULE(eric);

