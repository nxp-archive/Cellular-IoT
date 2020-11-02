/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef MSFT_AZURE_IOT_CLIENTCREDENTIAL_KEYS_H_
#define MSFT_AZURE_IOT_CLIENTCREDENTIAL_KEYS_H_

#ifdef SAS_KEY

#define keyDEVICE_SAS_PRIMARY_KEY "dfgeFdOyG+Jcnm4L77SuFfz9OZfFDu5MvPmikZfJONw="
#define keyCLIENT_CERTIFICATE_PEM NULL
#define keyCLIENT_PRIVATE_KEY_PEM NULL

#else

/*
 * @brief PEM-encoded client certificate.
 *
 * @todo If you are running one of the FreeRTOS demo projects, set this
 * to the certificate that will be used for TLS client authentication.
 *
 * @note Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----\n"
 */
#define keyCLIENT_CERTIFICATE_PEM \
"-----BEGIN CERTIFICATE-----\n"\
"MIIDZDCCAkygAwIBAgIQbOiYXxJJX71C5nPvTYejZzANBgkqhkiG9w0BAQsFADAo\n"\
"MSYwJAYDVQQDDB1BenVyZSBJb1QgQ0EgVGVzdE9ubHkgUm9vdCBDQTAeFw0yMDEw\n"\
"MjgxMDQzNTZaFw0yMDExMjcxMDUzNTZaMBYxFDASBgNVBAMMCzI5dzY2aXJpcXQ1\n"\
"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAwUeSvbo2TiiFxN8OsAlt\n"\
"ZPvvaE7pA3UMjOsnDNUBPYjkhdc7RjFNLhP/HEimyPVidLqztcJNVbuPQANkAAAe\n"\
"Gj+but43PMbeWQz6ocfnNoyRvBEqs7qGujLb97EiLmBTJIKsKIV6M5N0+kqnYeY0\n"\
"8iOE6TlU0aUJRJ41zBy4cWOqlXLzmhejYZ6QqxyFwvYEwOFutiqeVx1qN4sBvCbD\n"\
"ADPF7w29UOUgfdgv28Wr77DhlRWIft1K3HMgQRNweX3pMj4OdXmoNmrpA11SEGOe\n"\
"wh9tOvvDXhEf8Rew5DDBSY58VSX885ypSt5iJbnmC+HzZhCyc/CQiNTMHU/lzqn0\n"\
"TQIDAQABo4GbMIGYMA4GA1UdDwEB/wQEAwIFoDAWBgNVHREEDzANggsyOXc2Nmly\n"\
"aXF0NTAdBgNVHSUEFjAUBggrBgEFBQcDAgYIKwYBBQUHAwEwDwYDVR0TAQH/BAUw\n"\
"AwIBADAfBgNVHSMEGDAWgBSMqM5FhZcaFvPwBrZtZ33YPYUTCzAdBgNVHQ4EFgQU\n"\
"FTuL81/+PI+S91UnoyXQXNd/Ml0wDQYJKoZIhvcNAQELBQADggEBAH+W84aZpQZR\n"\
"6JZGwuMSe+g0Uc9qttnnQ1vKdW0iTyZi81ohdikM+t+c4trPsph/QLdXnvsindPw\n"\
"Op0yVm8/VIIxa/gtP2DNKnr2jzJvOoxLO7EKr0pfpEkEgDlcuOVhfYj+eEblPUAB\n"\
"D1V9/3ERA+0gPxt5/nZdT89qEM5JOKSRCrTqHx3v+cXJ1/SrzaO8isA+TKegDOwd\n"\
"5LRaa/mFv1FUyEie00kdsmwrGzdEgpaUIh4mcsIu00byIwCul2rnLddiG2LOqL9D\n"\
"91yE6LrBjkMHaRqrAK1lM0ibL/EoG/5hK0WFNm4rqi8RtOMKafcKdvrOWx56VAtT\n"\
"9Psldx6YOj4=\n"\
"-----END CERTIFICATE-----"

/*
 * @brief PEM-encoded client private key.
 *
 * @todo If you are running one of the FreeRTOS demo projects, set this
 * to the private key that will be used for TLS client authentication.
 *
 * @note Must include the PEM header and footer:
 * "-----BEGIN RSA PRIVATE KEY-----\n"\
 * "...base64 data...\n"\
 * "-----END RSA PRIVATE KEY-----\n"
 */
#define keyCLIENT_PRIVATE_KEY_PEM \
"-----BEGIN RSA PRIVATE KEY-----\n"\
"MIIEowIBAAKCAQEAwUeSvbo2TiiFxN8OsAltZPvvaE7pA3UMjOsnDNUBPYjkhdc7\n"\
"RjFNLhP/HEimyPVidLqztcJNVbuPQANkAAAeGj+but43PMbeWQz6ocfnNoyRvBEq\n"\
"s7qGujLb97EiLmBTJIKsKIV6M5N0+kqnYeY08iOE6TlU0aUJRJ41zBy4cWOqlXLz\n"\
"mhejYZ6QqxyFwvYEwOFutiqeVx1qN4sBvCbDADPF7w29UOUgfdgv28Wr77DhlRWI\n"\
"ft1K3HMgQRNweX3pMj4OdXmoNmrpA11SEGOewh9tOvvDXhEf8Rew5DDBSY58VSX8\n"\
"85ypSt5iJbnmC+HzZhCyc/CQiNTMHU/lzqn0TQIDAQABAoIBAAnfsitnG+caQtsv\n"\
"2fw8Pw5toGlfvp+ilyWLzc7mAs419BayTCDl29+CvgGf5IVXo9P6xbt/VZwPnN3U\n"\
"ToOBfggxP69oMsI0Fp/N/e/N2o362GcXKWtNGBzjBEG/FtPvr8+prUnUp82CtvMi\n"\
"ZTKnJAVKszuG5kJff/21Z2IqBXIlYpKJWNBi9r9MKHX56vO3H3DH75ybJKhHflAc\n"\
"uLiwSh3LXr/7ewGG8E6qOVkC3hZetpjVAY7Ik6MiMbBGWKdLJ84w5Rxa7bRgtxXF\n"\
"7sHHcFLxEsjZSbu72Mo+SkveUGdPTtS69ErVEM3cqX29hM7a0yMjDdDiz5/t4mBH\n"\
"lO2xUDECgYEA4icX4iusDhlraEC/DO/afS3QoyleWGkWrAbdWp2brvq8LGGW5ml4\n"\
"Z4J+D+PMaiOWdoc7W4ZYYYGvpS+dhgzOtFE4pLGRDcLaQ49yLg/bsacu00tBibAc\n"\
"naCglZPacpVxah3avjZNI4imVl++mwQXpoF1j/ihxlQkiPjBAdUReTMCgYEA2snP\n"\
"9D4cvS0QAMwOfZfrROfVvDdBA0w3yYlzK8CRl69okkL4mVmqXWingUntF36pLZy3\n"\
"bnCOg4b48T65cENon4TnzHXpq1OSN3X7T3SWckaR9doBmQcky0Dv4LsImYOBOkes\n"\
"bbRWR3JHY16tt5SC4k5DlmevZpf5zAADBdPN3H8CgYBY2o06snfuLjSSCGZH2fEZ\n"\
"H6gKgxVrXRaoTiYoFSZ20FKgiVM1ghjpak7HRXQp+4OBXE6I/ja5UK/HEfctFNkd\n"\
"5Y+AvbdFFdqVksWErXxex1XRdI7yXvxPPi35ttasEGU4ETTBaSpTnLvl0cZct3Xn\n"\
"WJPtFf3Ro3ubSUjjxGgSGwKBgQDL4C9mO9Lv/ePhukTuh/h2l0q2Pw7YEdgSd4DZ\n"\
"DnMdVBkv4jRiQgndTT553fePYzonBNhxk3VZ3xZvR7HpiQcfTuvpvR7nxdKE1+49\n"\
"Vjkzd8tfu6ytetiwhmyBkAgtdTugoVVuTRPxM+6ODJX4ce2dEK0upKPwCEJ/9Krc\n"\
"bUhx5QKBgArknCpm3bIzcqxRdw4u3diO30UqBwKevCTDM8qis+nqBFcEpfsnxLpj\n"\
"7SYBiur+QOWV7Am9PWcqMPYuqQCRPl9P06Yv8e05KO6Dicr0thBgumzhtjuJWcPo\n"\
"ZoUq+i0IU9h9uBglLYmK+DliRWwNJos8xT3+2t4yNXFRZJPkJXEj\n"\
"-----END RSA PRIVATE KEY-----"

#endif

#endif /* MSFT_AZURE_IOT_CLIENTCREDENTIAL_KEYS_H_ */
