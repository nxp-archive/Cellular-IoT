#ifndef AWS_CLIENT_CREDENTIAL_KEYS_H
#define AWS_CLIENT_CREDENTIAL_KEYS_H

#include <stdint.h>

/* Alex's Credentials */

/*
 * PEM-encoded client certificate.
 * Client Authentication certificate 'Starfield Cross-signing CA' SFSRootCAG2.pem
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----"
 */
#define keyCERTIFICATE_AUTHORITY_PEM \
"-----BEGIN CERTIFICATE-----\n"\
"MIID7zCCAtegAwIBAgIBADANBgkqhkiG9w0BAQsFADCBmDELMAkGA1UEBhMCVVMx\n"\
"EDAOBgNVBAgTB0FyaXpvbmExEzARBgNVBAcTClNjb3R0c2RhbGUxJTAjBgNVBAoT\n"\
"HFN0YXJmaWVsZCBUZWNobm9sb2dpZXMsIEluYy4xOzA5BgNVBAMTMlN0YXJmaWVs\n"\
"ZCBTZXJ2aWNlcyBSb290IENlcnRpZmljYXRlIEF1dGhvcml0eSAtIEcyMB4XDTA5\n"\
"MDkwMTAwMDAwMFoXDTM3MTIzMTIzNTk1OVowgZgxCzAJBgNVBAYTAlVTMRAwDgYD\n"\
"VQQIEwdBcml6b25hMRMwEQYDVQQHEwpTY290dHNkYWxlMSUwIwYDVQQKExxTdGFy\n"\
"ZmllbGQgVGVjaG5vbG9naWVzLCBJbmMuMTswOQYDVQQDEzJTdGFyZmllbGQgU2Vy\n"\
"dmljZXMgUm9vdCBDZXJ0aWZpY2F0ZSBBdXRob3JpdHkgLSBHMjCCASIwDQYJKoZI\n"\
"hvcNAQEBBQADggEPADCCAQoCggEBANUMOsQq+U7i9b4Zl1+OiFOxHz/Lz58gE20p\n"\
"OsgPfTz3a3Y4Y9k2YKibXlwAgLIvWX/2h/klQ4bnaRtSmpDhcePYLQ1Ob/bISdm2\n"\
"8xpWriu2dBTrz/sm4xq6HZYuajtYlIlHVv8loJNwU4PahHQUw2eeBGg6345AWh1K\n"\
"Ts9DkTvnVtYAcMtS7nt9rjrnvDH5RfbCYM8TWQIrgMw0R9+53pBlbQLPLJGmpufe\n"\
"hRhJfGZOozptqbXuNC66DQO4M99H67FrjSXZm86B0UVGMpZwh94CDklDhbZsc7tk\n"\
"6mFBrMnUVN+HL8cisibMn1lUaJ/8viovxFUcdUBgF4UCVTmLfwUCAwEAAaNCMEAw\n"\
"DwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMCAQYwHQYDVR0OBBYEFJxfAN+q\n"\
"AdcwKziIorhtSpzyEZGDMA0GCSqGSIb3DQEBCwUAA4IBAQBLNqaEd2ndOxmfZyMI\n"\
"bw5hyf2E3F/YNoHN2BtBLZ9g3ccaaNnRbobhiCPPE95Dz+I0swSdHynVv/heyNXB\n"\
"ve6SbzJ08pGCL72CQnqtKrcgfU28elUSwhXqvfdqlS5sdJ/PHLTyxQGjhdByPq1z\n"\
"qwubdQxtRbeOlKyWN7Wg0I8VRw7j6IPdj/3vQQF3zCepYoUz8jcI73HPdwbeyBkd\n"\
"iEDPfUYd/x7H4c7/I9vG+o1VTqkC50cRRj70/b17KSa7qWFiNyi2LSr2EIZkyXCn\n"\
"0q23KXB56jzaYyWf/Wi3MOxw+3WKt21gZ7IeyLnp2KhvAotnDU0mV3HaIPzBSlCN\n"\
"sSi6\n"\
"-----END CERTIFICATE-----"

#define keyCLIENT_CERTIFICATE_PEM \
"-----BEGIN CERTIFICATE-----\n"\
"MIIDWTCCAkGgAwIBAgIUHoYqisAEhFYnMUgXyIQ84eiLkfUwDQYJKoZIhvcNAQEL\n"\
"BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n"\
"SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTE5MDcxODE1NTc0\n"\
"MVoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n"\
"ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAJ47QcLIsaSCx3jgcLjN\n"\
"K99CVIa6UrLl52+ZXocqTZt2kzz4er9ooT0uWs8vuok2766/kKuIKGebfpN6Oy6G\n"\
"YXCTSQO9jYPRG+5uSHHfHY+9QNN6UjBjxDq5hsauOoXExb2aR9yO6GedRkPJZ5Dg\n"\
"f+bSSzmu7ReyGVt2Bn9KSc0ho/in0Pfv5r8JK6783IRmXCxz6Eklhv2+JO3/QWqO\n"\
"eJa8CP9qCD8mA2SMnijwcXGYeOHJ/Iu5EbZTn/CslW5JsGgbeDw/VdWhrkjINC3k\n"\
"VIk/o2Azk1O/EZKTolsPifd3+YS8/xmVWF+IBRcto0srlqlRyoAox6llH5YQZjXn\n"\
"Qd0CAwEAAaNgMF4wHwYDVR0jBBgwFoAUkuNefFwHmwTlpP8c0yOPrcc1vXswHQYD\n"\
"VR0OBBYEFFkocQ25iTtna1kJNKGKGNEtU6a4MAwGA1UdEwEB/wQCMAAwDgYDVR0P\n"\
"AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQAzAh1ZZSgMkyUIRgm3gYhJh9bB\n"\
"LilIv78y526UNwJJRKHI5wGh+7nQKqQvwAgqrkk0y+Va09Wkbr3va2YlgzFwz589\n"\
"1ZppuWgcY5bjd1sTZW/u6gncfcBQcPs51SJGcnjs5+U1AT2eRorcWuUOW8+gZC1/\n"\
"hddBHeyt2N4KGbGBZc+efoF8BICbM7cm2IfJNjg5dAA4XtbUeHq1ZbnUtTkVirSj\n"\
"bA1Rwsvvb//Q5UwFw6gYE/466W/JGbUDwmK11LCCnaECFXRfGbuQRoEpBws1SJA3\n"\
"j2fB93nhFP6U1s8s2JLzYahBRFPEo24ewHdSfAJetnyYMkR/ButjjsRcnyYC\n"\
"-----END CERTIFICATE-----"

/*
 * PEM-encoded client private key.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN RSA PRIVATE KEY-----\n"\
 * "...base64 data...\n"\
 * "-----END RSA PRIVATE KEY-----"
 */
#define keyCLIENT_PRIVATE_KEY_PEM \
"-----BEGIN RSA PRIVATE KEY-----\n"\
"MIIEogIBAAKCAQEAnjtBwsixpILHeOBwuM0r30JUhrpSsuXnb5lehypNm3aTPPh6\n"\
"v2ihPS5azy+6iTbvrr+Qq4goZ5t+k3o7LoZhcJNJA72Ng9Eb7m5Icd8dj71A03pS\n"\
"MGPEOrmGxq46hcTFvZpH3I7oZ51GQ8lnkOB/5tJLOa7tF7IZW3YGf0pJzSGj+KfQ\n"\
"9+/mvwkrrvzchGZcLHPoSSWG/b4k7f9Bao54lrwI/2oIPyYDZIyeKPBxcZh44cn8\n"\
"i7kRtlOf8KyVbkmwaBt4PD9V1aGuSMg0LeRUiT+jYDOTU78RkpOiWw+J93f5hLz/\n"\
"GZVYX4gFFy2jSyuWqVHKgCjHqWUflhBmNedB3QIDAQABAoIBAC7Dop7Xw7C0OhHe\n"\
"e1ISyTmVl/etj41x5yGTeV2HQiKFO+TUJyr02FqFna8IEyBEjTffQ/l+JzrfuNwL\n"\
"TvxkGN41mWmyluVzYsbn7x9AarM1orgduba1daRJeqyzh3ZLfomX1E86KTOeRsr3\n"\
"9lTaFNL6KeNlcIsFRvtB+Y+bPA6UwJoOiBqjZkb9FA9BJbDhK15vB0AUQ6rhE/Hc\n"\
"hEsDBo8k4nGr+kkjqPV44mRTc+E+lew9ZlYrTjqxAgbnYbl/cpiMwOR0C3jeeN44\n"\
"5DOSeu58nGVoGPO5utSWFz4S/p7gSlyPARLCWsdDKJqsLfZ38FdvvsCKRNWw9vix\n"\
"CUBb0L0CgYEA0ZMOZwGgWcd/+j652vxmSVmvBZwlvXWQ7boT4GKLqzq6uWHdMuFx\n"\
"1O5MdGEAEbYJhm5Z8C5FzU2HXjC/SqZE6C09VUrm42mNC09AkVZ1GJOwcbUTjIvv\n"\
"gzuXh7UT3Bc1K9zxvKuB2s7m28k1+OEoJBHyY6G1bfDwJuqFyEXcDGcCgYEAwUiL\n"\
"6s/5jHxqNjtdVb/llVitrjq0eupUU7nT9Ns9ov8mXRpF+krzGzdaX9KRRmLuqY++\n"\
"hfDPsDvROurt+/ux4kfa/ClOnU/+x3iMYBpRFF/vj4kLj9RzTqf3Y0zODuP2f5xv\n"\
"jTcCK0tSCG+lr3satkimrbiz25CY3CcAV2t2lRsCgYBevccmA9M0ov6U52RnvfCT\n"\
"ukpoE9Xv10UkYxl3N7WCO/OLuNUhmk82fl/rwKoiV5PoxNw9uv1tOV4303u2yIGg\n"\
"IEt40nqcVewgL7y1t0sKSEHctYpX4C7vubHf9Jk8xvUPljtmGnaCr0QoGccfctIN\n"\
"tFtKdSEqyWAXh6/hAMvggwKBgHG/qSBKvzlPaw55h2dUpN9sreIQiHCiAXi4ryW1\n"\
"9SM8963pLW+stj+5JsWn3XlH9d7ySpT28LNtsKf05JPcP4bE9PpbsPlhv7ZSmoJg\n"\
"WoEcKJcXdydxR17t+OiGpeobfNEDFp9L1Un3bNKsKh0I3OKUBQvN3IOGKH+/sDLC\n"\
"eqorAoGAff+l7w5+bBWSvCJ/V9I1Gz4dNC0yRUOnliLO0ScA7EaiHTIn25zuQLSo\n"\
"MMZlpm1meMUeCpL9hTLyy8D17GWJ27wMERCYC7MRm0Q8khefW50jTeJZfEZXxJ2B\n"\
"eHusJG4iHnYKj2uPNCdncz7l9yhRN5aFkXgvlSriuWrUWrBuW78=\n"\
"-----END RSA PRIVATE KEY-----"

/*
 * PEM-encoded Just-in-Time Registration (JITR) certificate (optional).
 *
 * If used, must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----"
 */
#define keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM  NULL

/* The constants above are set to const char * pointers defined in aws_dev_mode_key_provisioning.c,
 * and externed here for use in C files.  NOTE!  THIS IS DONE FOR CONVENIENCE
 * DURING AN EVALUATION PHASE AND IS NOT GOOD PRACTICE FOR PRODUCTION SYSTEMS
 * WHICH MUST STORE KEYS SECURELY. */
extern const char clientcredentialCLIENT_CERTIFICATE_PEM[];
extern const char* clientcredentialJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM;
extern const char clientcredentialCLIENT_PRIVATE_KEY_PEM[];
extern const uint32_t clientcredentialCLIENT_CERTIFICATE_LENGTH;
extern const uint32_t clientcredentialCLIENT_PRIVATE_KEY_LENGTH;

#endif /* AWS_CLIENT_CREDENTIAL_KEYS_H */
