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

bool hash(const char *key_, uint32_t key_length, const char *data, uint32_t data_size, char **hash_data, size_t* hash_data_length)
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

    hmac_hash[hash_size] = 0;
    *hash_data = (char *) hmac_hash;
    *hash_data_length = (size_t) hash_size;

    return true;
}

void generateSasToken(char** ptr_buffer, char* scope_id, size_t scope_id_length, char* device_id, size_t device_id_length, char* key, size_t key_length)
{
	size_t expires = getNow() + EXPIRES;

	/* Convert the Device ID for URL use */
	char * url_device_id = NULL;
	urlEncode(device_id, device_id_length, &url_device_id);

	/* Create the URL-encoded ressource URI */
	char * sr = (char*) AZURE_IOTC_MALLOC(BUFFER_SIZE_256);
	assert(sr != NULL);
	size_t size =
	  snprintf(sr, BUFFER_SIZE_256,
			  "%s%%2Fregistrations%%2F%s",
			  scope_id, url_device_id);
	assert(size < BUFFER_SIZE_256);
	AZURE_IOTC_FREE(url_device_id);

	/* Create data to be signed with the key containing the expiry time */
	char * string_to_sign = (char*) AZURE_IOTC_MALLOC(BUFFER_SIZE_256);
	assert(string_to_sign != NULL);
	size = snprintf(string_to_sign, BUFFER_SIZE_256, "%s\n%lu000", sr, (long unsigned int)expires);
	assert(size < BUFFER_SIZE_256);

	/* Decode the Key */
	size = 0;
	char * key_decoded = (char*) AZURE_IOTC_MALLOC(key_length + 1);
	assert(key_decoded != NULL);
	mbedtls_base64_decode((unsigned char*)key_decoded, key_length, &size,
	        (const unsigned char*)key, key_length);
	assert(size > 0);
	key_decoded[size] = 0;

	/* Hash the data to be signed with the decoded key */
	char * signed_string = NULL;
	size_t signed_string_length = 0;
	hash(key_decoded, size, string_to_sign, strlen(string_to_sign), &signed_string, &signed_string_length);
	AZURE_IOTC_FREE(string_to_sign);

	/* Encode the signed data */
	size = 0;
	char * data_encoded = (char*) AZURE_IOTC_MALLOC(BUFFER_SIZE_256);
	assert(data_encoded != NULL);
	mbedtls_base64_encode((unsigned char*)data_encoded, strlen(signed_string) * 3, &size,
	        (const unsigned char*)signed_string, signed_string_length);
	assert(size > 0);
	char * url_data_encoded = NULL;
	urlEncode(data_encoded, strlen(data_encoded), &url_data_encoded);

	/* Generate the DPS Authentication String */
	char * buffer = (char*) AZURE_IOTC_MALLOC(BUFFER_SIZE_256);
	assert(buffer != NULL);
	size = snprintf(buffer, BUFFER_SIZE_256,
					   "SharedAccessSignature "
					   "sr=%s&sig=%s&se=%lu000&skn=registration",
					   sr, url_data_encoded, (long unsigned int)expires);
	assert(size > 0 && size < (unsigned int) BUFFER_SIZE_256);
	buffer[size] = 0;

	*ptr_buffer = buffer;

	/* Free all remaining allocated ressources */
	AZURE_IOTC_FREE(sr);
	AZURE_IOTC_FREE(key_decoded);
	AZURE_IOTC_FREE(signed_string);
	AZURE_IOTC_FREE(data_encoded);
	AZURE_IOTC_FREE(url_data_encoded);

}

void getUsernameAndPassword(char** ptr_username, char** ptr_password, char* device_id, size_t device_id_length, char* host_name, size_t host_name_length, char* key, size_t key_length)
{
	size_t expires = getNow() + EXPIRES;

	/* Convert the Assigned Hub for URL use */
	char * url_host_name = NULL;
	urlEncode(host_name, host_name_length, &url_host_name);

	/* Convert the Device ID for URL use */
	char * url_device_id = NULL;
	urlEncode(device_id, device_id_length, &url_device_id);

	/* Create data to be signed with the key containing the expiry time */
	char * string_to_sign = (char*) AZURE_IOTC_MALLOC(BUFFER_SIZE_256);
	assert(string_to_sign != NULL);
	size_t size = snprintf(string_to_sign, BUFFER_SIZE_256, "%s%s%s\n%lu000", url_host_name, "%2Fdevices%2F", url_device_id, (long unsigned int)expires);
	assert(size < BUFFER_SIZE_256);

	/* Decode the Key */
	size = 0;
	char * key_decoded = (char*) AZURE_IOTC_MALLOC(key_length + 1);
	assert(key_decoded != NULL);
	mbedtls_base64_decode((unsigned char*)key_decoded, key_length, &size,
	        (const unsigned char*)key, key_length);
	assert(size > 0);
	key_decoded[size] = 0;

	/* Hash the data to be signed with the decoded key */
	char * signed_string = NULL;
	size_t signed_string_length = 0;
	hash(key_decoded, size, string_to_sign, strlen(string_to_sign), &signed_string, &signed_string_length);
	AZURE_IOTC_FREE(string_to_sign);

	/* Encode the signed data */
	size = 0;
	char * data_encoded = (char*) AZURE_IOTC_MALLOC(BUFFER_SIZE_256);
	assert(data_encoded != NULL);
	mbedtls_base64_encode((unsigned char*)data_encoded, strlen(signed_string) * 3, &size,
	        (const unsigned char*)signed_string, signed_string_length);
	assert(size > 0);
	char * url_data_encoded = NULL;
	urlEncode(data_encoded, strlen(data_encoded), &url_data_encoded);

	/* Generate Username String */
	char* username = (char*) AZURE_IOTC_MALLOC(BUFFER_SIZE_256);
	assert(username != NULL);
	size = snprintf(username, BUFFER_SIZE_256,
					   "%s/%s/api-version=2016-11-14",
					   url_host_name, url_device_id);
	assert(size > 0 && size < (unsigned int) BUFFER_SIZE_256);
	username[size] = 0;

	/* Generate Password String */
	char* password = (char*) AZURE_IOTC_MALLOC(BUFFER_SIZE_256);
	assert(password != NULL);
	size = snprintf(password, BUFFER_SIZE_256,
					   "SharedAccessSignature "
					   "sr=%s%s%s&sig=%s&se=%ld000",
					   url_host_name, "%2Fdevices%2F", url_device_id, url_data_encoded, (long unsigned int)expires);
	assert(size > 0 && size < (unsigned int) BUFFER_SIZE_256);
	password[size] = 0;

	AZURE_PRINTF(
			("\r\n"
            "hostname: %s\r\n"
            "device_id: %s\r\n"
            "username: %s\r\n"
            "password: %s\r\n",
			host_name, device_id, username, password));

	*ptr_username = username;
	*ptr_password = password;

	/* Free all remaining allocated ressources */
	AZURE_IOTC_FREE(url_host_name);
	AZURE_IOTC_FREE(url_device_id);
	AZURE_IOTC_FREE(signed_string);
	AZURE_IOTC_FREE(key_decoded);
	AZURE_IOTC_FREE(data_encoded);
	AZURE_IOTC_FREE(url_data_encoded);
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

void get_method_name(const char* topic, const char* topic_root, char * sub_topic, size_t sub_topic_size)
{
	size_t i;

	if(topic == NULL || topic_root == NULL)	return;

	for (i = 0; i < strlen(topic_root); i++)
	{
		if (topic[i] != topic_root[i])		return;
	}

	for(size_t j = 0; j < sub_topic_size; j++)
	{
		/* 0x2F = "/" */
		if (topic[i] != 0x2F)	sub_topic[j] = topic[i++];
		else	break;
	}
}

int8_t get_rid_from_topic(const char* topic)
{
	char ref[] = "?$rid=";
	size_t j = 0;
	uint8_t rid = 0;

	for (size_t i = 0; i < strlen(topic); i++)
	{
		if (j < 6)
		{
			/* Search the following characters: ?$rid= */
			if(topic[i] == ref[j])
			{
				j++;
			}
			else
			{
				j = 0;
			}
		}
		else
		{
			/* We need to take into account that some data can reside after ?$rid=<num>
			 * e.g. $iothub/methods/POST/device_reboot/?$rid=2nulla0\230,\001 h
			 * In this example, only the value 2 needs to be taken and the following should be dropped
			 */
			if(topic[i] >= 0x30 && topic[i] <= 0x39)
			{
				rid *= 10;
				rid += (topic[i] - 0x30);
			}
			else
			{
				break;
			}
		}
	}

	if (rid >= 0)	return rid;
	else			return -1;	/* Error */
}

