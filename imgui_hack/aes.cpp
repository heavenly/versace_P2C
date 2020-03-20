#include "aes.h"
#include "helpers/utils.hpp"

std::string c_aes::decrypt_str(const std::string& str_to_dec) const
{
	const std::string b64_decoded_str = Utils::base64_decode(str_to_dec);
	CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption dec;
	dec.SetKeyWithIV(key, sizeof(key), iv);

	std::string decrypted_text;
	CryptoPP::StreamTransformationFilter decryption_filter(dec, new CryptoPP::StringSink(decrypted_text));
	decryption_filter.Put(reinterpret_cast<const CryptoPP::byte*>(b64_decoded_str.c_str()), b64_decoded_str.size());
	decryption_filter.MessageEnd();

	return decrypted_text;
}

std::string c_aes::encrypt_str(const std::string& str_to_enc) const
{
	CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption enc;
	enc.SetKeyWithIV(key, sizeof(key), iv);

	std::string encrypted_text;
	CryptoPP::StreamTransformationFilter encryption_filter(enc, new CryptoPP::StringSink(encrypted_text)); //using new here bad?

	// encryption
	encryption_filter.Put(reinterpret_cast<const CryptoPP::byte*>(str_to_enc.c_str()), str_to_enc.size());
	encryption_filter.MessageEnd();

	return Utils::base64_encode(reinterpret_cast<const unsigned char*>(encrypted_text.c_str()), encrypted_text.size()); //casting is dumb as hell
}
