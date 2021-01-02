#include "php_eric.h"

ZEND_BEGIN_MODULE_GLOBALS(eric)
    int errCode;
ZEND_END_MODULE_GLOBALS(eric)
ZEND_DECLARE_MODULE_GLOBALS(eric)

static eric_druck_parameter_t eric_print_params;
static eric_verschluesselungs_parameter_t eric_encryption_params;
static int certRequiresPin;


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

PHP_RINIT_FUNCTION(eric)
{
    eric_globals.errCode = 0;   

    certRequiresPin = 0;

    eric_encryption_params.version = 2;
    eric_encryption_params.pin = "";
    eric_encryption_params.zertifikatHandle = NULL;    /* do not hold; open every time we send req */

    eric_print_params.version = 2;
    eric_print_params.vorschau = 0;
    eric_print_params.ersteSeite = 0;
    eric_print_params.duplexDruck = 0;
    eric_print_params.pdfName = "eric_print.pdf";
	eric_print_params.fussText = NULL;

    return SUCCESS;
}

PHP_FUNCTION(eric_init)
{
    int err = pEricInitialisiere(
        getenv("ERICAPI_LIB_PATH"), 
        "/var/log/httpd"
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

PHP_FUNCTION(eric_get_tax_office_country_numbers) /* je bundesland */
{
    EricRueckgabepufferHandle buf = pEricRueckgabepufferErzeugen();
    int ret = pEricHoleFinanzamtLandNummern(buf);
    if(ret == ERIC_OK) {
        int len = pEricRueckgabepufferLaenge(buf) +1;
        
        char *c = pEricRueckgabepufferInhalt(buf);
        char const rbuf[len];
        memcpy(
            &rbuf[0],
            &c[0],
            len
        );

        pEricRueckgabepufferFreigeben(buf);

        RETURN_STRING(rbuf);
    } 

    php_log_err("eric_get_tax_office_country_numbers error");

    RETURN_BOOL(IS_FALSE);
}

PHP_FUNCTION(eric_get_tax_offices_for_country_number)
{
    const char *cn;
    int cnlen;
    ZEND_PARSE_PARAMETERS_START(1,1)
        Z_PARAM_STRING(cn, cnlen)
    ZEND_PARSE_PARAMETERS_END();

    EricRueckgabepufferHandle buf = pEricRueckgabepufferErzeugen();
    
    int ret = pEricHoleFinanzaemter(
        cn,
        buf
    );
    if(ret == ERIC_OK) {
        char *b = pEricRueckgabepufferInhalt(buf);
        int l = pEricRueckgabepufferLaenge(buf) +1;
        const char rbuf[l];
        memcpy(
            &rbuf[0],
            &b[0],
            l
        );

        pEricRueckgabepufferFreigeben(buf);

        RETURN_STRING(rbuf);
    }

    if(ret == ERIC_GLOBAL_UTI_COUNTRY_NOT_SUPPORTED) {
        php_log_err("eric country code not supported eric_get_tax_offices_for_country_number error\n");
    }

    RETURN_FALSE;
}
ZEND_BEGIN_ARG_INFO(arginfo_eric_get_tax_offices_for_country_number, 1)
    ZEND_ARG_INFO(0, country_number)
ZEND_END_ARG_INFO();

PHP_FUNCTION(eric_format_tax_number)
{
    char *orig;
    int origLen;

    ZEND_PARSE_PARAMETERS_START(1,1)
        Z_PARAM_STRING(orig, origLen);
    ZEND_PARSE_PARAMETERS_END();

    EricRueckgabepufferHandle hout = pEricRueckgabepufferErzeugen();
    int format = pEricFormatStNr(
        orig,
        hout
    );
    if(format != ERIC_OK) {
        if(format == ERIC_GLOBAL_STEUERNUMMER_UNGUELTIG) {
            php_log_err("steuernummer ungültig\n");
        }

        pEricRueckgabepufferFreigeben(hout);

        RETURN_BOOL(IS_FALSE);
    }

    int outLen = pEricRueckgabepufferLaenge(hout) +1;
    const char buf[outLen];
    const char* _b = pEricRueckgabepufferInhalt(hout);
    memcpy(
        &buf[0],
        &_b[0],
        outLen
    );
    pEricRueckgabepufferFreigeben(hout);

    RETURN_STRING(buf);
}
ZEND_BEGIN_ARG_INFO(arginfo_eric_format_tax_number, 1)
    ZEND_ARG_INFO(0, tax_number)
ZEND_END_ARG_INFO();

PHP_FUNCTION(eric_format_tax_number_to_elster)
{
    char *orig;
    int origLen;
    char *countryCode;
    int ccLen;
    char *taxOfficeId;
    int toiLen;

    ZEND_PARSE_PARAMETERS_START(3,3)
        Z_PARAM_STRING(orig, origLen);
        Z_PARAM_STRING(countryCode, ccLen);
        Z_PARAM_STRING(taxOfficeId, toiLen);
    ZEND_PARSE_PARAMETERS_END();

    EricRueckgabepufferHandle hout = pEricRueckgabepufferErzeugen();
    int format = pEricMakeElsterStnr(
        orig,
        countryCode,
        taxOfficeId,
        hout
    );
    if(format != ERIC_OK) {
        if(format == ERIC_GLOBAL_STEUERNUMMER_UNGUELTIG) {
            php_log_err("eric_format_tax_number_to_elster: steuernummer ungültig\n");
        }
        if(format == ERIC_GLOBAL_LANDESNUMMER_UNBEKANNT) {
            php_log_err("eric_format_tax_number_to_elster: landesnummer unbekannt\n");
        }

        pEricRueckgabepufferFreigeben(hout);

        RETURN_BOOL(IS_FALSE);
    }

    int outLen = pEricRueckgabepufferLaenge(hout) +1;
    const char buf[outLen];
    const char* _b = pEricRueckgabepufferInhalt(hout);
    memcpy(
        &buf[0],
        &_b[0],
        outLen
    );
    pEricRueckgabepufferFreigeben(hout);

    RETURN_STRING(buf);
}
ZEND_BEGIN_ARG_INFO(arginfo_eric_format_tax_number_to_elster, 1)
    ZEND_ARG_INFO(0, tax_number)
    ZEND_ARG_INFO(0, country_code)
    ZEND_ARG_INFO(0, tax_office_id)
ZEND_END_ARG_INFO();

PHP_FUNCTION(eric_transfer)
{
    if(lericapi != NULL) {
        char *certPath;
        int certLength;
        char *pin;
        int pinLength;
        char *dataType; /* Est_2019 */
        int dataTypeVersionLength;
        char *xml;
        int xmlLength;
        zval *serverResponse;

        ZEND_PARSE_PARAMETERS_START(5,5)
            Z_PARAM_ZVAL(serverResponse)
            Z_PARAM_STRING(dataType, dataTypeVersionLength)
            Z_PARAM_STRING(xml, xmlLength)
            Z_PARAM_STRING(certPath, certLength)
            Z_PARAM_STRING(pin, pinLength)
        ZEND_PARSE_PARAMETERS_END();

        if(pEricGetHandleToCertificate(
                &(eric_encryption_params.zertifikatHandle),
                certRequiresPin,
                certPath
            ) != ERIC_OK
        ) {
            eric_globals.errCode = 303; /* eric no cert found */

            RETURN_BOOL(IS_FALSE);
        }

        if(pinLength == 0) {
            if(certRequiresPin != 0) {
                eric_globals.errCode = 5; /* eric decryption cert err */

                //pEricCloseHandleToCertificate(eric_encryption_params.zertifikatHandle);
                RETURN_BOOL(IS_FALSE);
            }
            pin = "";
        }
        eric_encryption_params.pin = pin;

        EricRueckgabepufferHandle dataHandle = pEricRueckgabepufferErzeugen();
        EricRueckgabepufferHandle serverResponseHandle = pEricRueckgabepufferErzeugen();

        int err = pEricBearbeiteVorgang(
            xml,
            dataType,
            ERIC_SENDE, /* ERIC_SENDE */
            &eric_print_params,
            &eric_encryption_params,
            NULL,
            dataHandle,
            serverResponseHandle
        );
        //pEricCloseHandleToCertificate(eric_encryption_params.zertifikatHandle);

        int bufLength = pEricRueckgabepufferLaenge(dataHandle) +1;
        const char buf[bufLength];

        const char* _buf = pEricRueckgabepufferInhalt(dataHandle);
        memcpy(
            &buf[0],
            &_buf[0],
            bufLength
        );

        pEricRueckgabepufferFreigeben(dataHandle);

        int serverResponseLen = pEricRueckgabepufferLaenge(serverResponseHandle);
        const char res[serverResponseLen];

        const char* _resBuf = pEricRueckgabepufferInhalt(serverResponseHandle);
        memcpy(
            &res[0],
            &_resBuf[0],
            serverResponseLen
        );

        ZVAL_STRING(
            serverResponse,
            res
        );

        pEricRueckgabepufferFreigeben(serverResponseHandle);
        
        eric_globals.errCode = err;

        if(err == ERIC_OK) {    
            RETURN_STRING(buf);   
        }
    } else {
        eric_globals.errCode = -1;
    }

    RETURN_FALSE;
}
ZEND_BEGIN_ARG_INFO(arginfo_eric_transfer, 1)
    ZEND_ARG_INFO(1, server_response)
    ZEND_ARG_INFO(0, dataType)
    ZEND_ARG_INFO(0, xml)
    ZEND_ARG_INFO(0, eric_certificate_file_path)
    ZEND_ARG_INFO(0, eric_certificate_pin)
ZEND_END_ARG_INFO()

PHP_FUNCTION(eric_get_error_code)
{
    RETURN_LONG(eric_globals.errCode);
}

PHP_FUNCTION(eric_get_error)
{
    if(eric_globals.errCode == -1) {
        return "ericapilib not loaded";
    }

    if(eric_globals.errCode != 0)  {
        EricRueckgabepufferHandle buf = pEricRueckgabepufferErzeugen();
        pEricHoleFehlerText(eric_globals.errCode, buf);

        eric_globals.errCode = 0;

        const char* ericbuf = pEricRueckgabepufferInhalt(buf);
        int len = pEricRueckgabepufferLaenge(buf) +1;
        char ret[len];
        memcpy(
            &ret[0],
            &ericbuf[0],
            len
        );

        pEricRueckgabepufferFreigeben(buf);

        RETURN_STRING(ret);
    }

    RETURN_NULL();
}

static zend_function_entry eric_functions[] = {
	PHP_FE(eric_init, NULL)
    PHP_FE(eric_close, NULL)
    PHP_FE(eric_get_tax_office_country_numbers, NULL)
    PHP_FE(eric_get_tax_offices_for_country_number, arginfo_eric_get_tax_offices_for_country_number)
    PHP_FE(eric_format_tax_number, arginfo_eric_format_tax_number)
    PHP_FE(eric_format_tax_number_to_elster, arginfo_eric_format_tax_number_to_elster)
    PHP_FE(eric_transfer, arginfo_eric_transfer)
    PHP_FE(eric_get_error, NULL)
    PHP_FE(eric_get_error_code, NULL)
    PHP_FE_END
};

zend_module_entry eric_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_ERIC_EXTNAME,
    eric_functions,
    PHP_MINIT(eric),
    PHP_MSHUTDOWN(eric),
    PHP_RINIT(eric),
    NULL,
    NULL,
#if ZEND_MODULE_API_NO >= 20010901
    PHP_ERIC_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};

ZEND_GET_MODULE(eric);

