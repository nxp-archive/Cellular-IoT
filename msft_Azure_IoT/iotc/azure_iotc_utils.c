/*
 * Copyright 2019-2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <ctype.h>
#include "azure_iotc_utils.h"
#include "msft_Azure_IoT.h"
#include "msft_Azure_IoT_clientcredential.h"
#include "msft_Azure_IoT_clientcredential_keys.h"
#include <mbedtls/md.h>
#include <mbedtls/sha256.h>
#include <mbedtls/base64.h>


#define BUFFER_SIZE_256   256
static uint32_t EXPIRES = 21600;

unsigned long getNow() {
  return (unsigned long)2200000;  // use ntp utc time to enable this
}

static char convertToHex(char ch) {
    static const char * lst = "0123456789ABCDEF";
    return *(lst + (ch & 15));
}

bool urlEncode(const char * buffer, size_t length, char ** url_enc_data)
{
    size_t buffer_length = (length * 3) + 1;
    *url_enc_data = (char*) AZURE_IOTC_MALLOC(buffer_length);
    if (*url_enc_data == NULL) {
        return false;
    }

	char * tmp = (char *)*url_enc_data;

    for (size_t i = 0; i < length; i++) {
        char ch = buffer[i];
        if (isalnum(ch) ||
            ch == '_' || ch == '-' || ch == '~' || ch == '.') {
          *tmp = ch;
        } else if (ch == ' ') {
          *tmp = '+';
        } else {
            *tmp++ = '%';
            *tmp++ = convertToHex(ch >> 4);
            *tmp   = convertToHex(ch & 15);
        }
        tmp++;
    }
    *tmp = 0;

    return true;
}

bool hash(const char *key_, uint32_t key_length, const char *data, uint32_t data_size, char **hash_data)
{
#define LOG_FAIL__(X) if ((res=X)!=0) { AZURE_PRINTF(("Error: hash %s has failed with %d return value.", #X, res)); return false;}
    mbedtls_md_type_t md = MBEDTLS_MD_SHA256;
    const mbedtls_md_info_t *md_info;
    mbedtls_md_context_t ctx;
    int res = 0;

    md_info = mbedtls_md_info_from_type(md);
    uint32_t hash_size = (uint32_t) mbedtls_md_get_size(md_info);
    unsigned char *hmac_hash = (unsigned char*) AZURE_IOTC_MALLOC(hash_size + 1);
    memset(hmac_hash, 0, hash_size + 1);

    mbedtls_md_init(&ctx);
    LOG_FAIL__(mbedtls_md_setup(&ctx, md_info, 1));
    LOG_FAIL__(mbedtls_md_hmac_starts(&ctx, (const unsigned char*) key_, key_length));
    LOG_FAIL__(mbedtls_md_hmac_update(&ctx, (const unsigned char*) data, data_size));
    LOG_FAIL__(mbedtls_md_hmac_finish(&ctx, (unsigned char*) hmac_hash));
#undef LOG_FAIL__

    mbedtls_md_free(&ctx);

    *hash_data = (char *) hmac_hash;

    return true;
}

void generate_sas_token(char * buffer, size_t bufferSize)
{
	size_t expires = getNow() + EXPIRES * 2;

	/* Convert the Device ID for URL use */
	size_t device_id_length = strlen(clientcredentialAZURE_IOT_THING_NAME);
	char * device_id = NULL;
	urlEncode(clientcredentialAZURE_IOT_THING_NAME, device_id_length, &device_id);

	/* Create the URL-encoded ressource URI */
	char * sr = (char*) AZURE_IOTC_MALLOC(BUFFER_SIZE_256);
	assert(sr != NULL);
	size_t size =
	  snprintf(sr, BUFFER_SIZE_256, "%s%%2Fregistrations%%2F%s",
			  clientcredentialAZURE_IOT_SCOPE_ID, clientcredentialAZURE_IOT_THING_NAME);
	assert(size < BUFFER_SIZE_256);
	AZURE_IOTC_FREE(device_id);

	/* Create data to be signed with the key containing the expiry time */
	char * stringToSign = (char*) AZURE_IOTC_MALLOC(BUFFER_SIZE_256);
	assert(stringToSign != NULL);
	size = snprintf(stringToSign, BUFFER_SIZE_256, "%s\n%lu000", sr, (long unsigned int)expires);
	assert(size < BUFFER_SIZE_256);

	/* Decode the Key */
	size = 0;
	size_t key_size = strlen(keyDEVICE_SAS_PRIMARY_KEY);
	char * key_decoded = (char*) AZURE_IOTC_MALLOC(key_size + 1);
	assert(key_decoded != NULL);
	mbedtls_base64_decode((unsigned char*)key_decoded, key_size, &size,
	        (const unsigned char*)keyDEVICE_SAS_PRIMARY_KEY, key_size);
	assert(size > 0);
	key_decoded[key_size] = 0;

	/* Hash the data to be signed with the decoded key */
	char * signedString = NULL;
	hash(key_decoded, key_size, stringToSign, strlen(stringToSign), &signedString);
	AZURE_IOTC_FREE(stringToSign);

	/* Encode the signed data */
	size = 0;
	char * data_encoded = (char*) AZURE_IOTC_MALLOC(BUFFER_SIZE_256);
	assert(data_encoded != NULL);
	mbedtls_base64_encode((unsigned char*)data_encoded, strlen(signedString) * 3, &size,
	        (const unsigned char*)signedString, strlen(signedString));
	assert(size > 0);
	char * data_encoded_url = NULL;
	urlEncode(data_encoded, strlen(data_encoded), &data_encoded_url);

	/* Generate the DPS Authentication String */
	uint32_t outLength = snprintf(buffer, bufferSize,
					   "SharedAccessSignature "
					   "sr=%s&sig=%s&se=%lu000&skn=registration",
					   sr, data_encoded_url, (long unsigned int)expires);
	assert(outLength > 0 && outLength < (unsigned int) bufferSize);
	buffer[outLength] = 0;

	/* Free all remaining allocated ressources */
	AZURE_IOTC_FREE(sr);
	AZURE_IOTC_FREE(key_decoded);
	AZURE_IOTC_FREE(signedString);
	AZURE_IOTC_FREE(data_encoded);
	AZURE_IOTC_FREE(data_encoded_url);

}

bool topic_check(const char* topic, size_t len, char* str, size_t str_len)
{
	if (len > str_len) return false;

	for (size_t i = 0; i < len; i++)
	{
		if (topic[i] != str[i]) return false;
	}
	return true;
}


