#ifndef MY_BYTESWAP_H_
#define MY_BYTESWAP_H_

unsigned long long hton64(const unsigned long long ho);

unsigned long long ntoh64(const unsigned long long no);

int64_t btoll(const u_char bs[], int len);

int btoi(const u_char bs[], int len);

short btos(const u_char bs[], int len);

#endif /*MY_BYTESWAP_H_*/


