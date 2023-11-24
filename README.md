# basilikum – a base62 codec for printable ASCII texts (e.g. URLs)
## Motivation
Even today where the Internet speaks UTF-8, there are still a lot of data out there that consists of printable ASCII characters only. One famous example are URLs (URIs, URNs).

These data are often human-readable, which is often a good idea but sometimes you might want to hide e.g. URL paths and query parameters a bit for too curious eyes. Encodings like Base64 add 33% overhead which can be done better when the input data are not arbitrary binary data but printable ASCII caracters only.

## Encoding idea
There are 95 printable ASCII characters. If we map all other byte values to one different value, we have 96 different characters. A string of seceral consecutive characters can be encoded in a base-96 number, so 9 printable ASCII characters can build a number up to (96^9)-1 = 69'253'399'582'480'255. This number can be expressed in ASCII letters and digits, hence a max. 10 digit base-62 number. So this encoding has only 11% overhad.

To make things a bit more complicated I also add a little quirk to the codec: A 64 bit "secret" which scrambles the encoding a bit and without knowing this secret it is not so easy (albeit not impossible) to decode the data.

## Example data

| Input | Secret | Output |
|:-----:|:------:|:------:|
| abc   | 12345  | 6ywb |
| abc   | 54321  | yuua |
| foo=123&bar=X:Y&yuk=Yabba+Yabba | 12345 | P7FsANV90scozdg8iu9uRjCeB4oLEneHpZb |
| foo=123&bar=X:Y&yuk=Yabba+Yabba | 54321 | ViNp9oDHleLtrYCKEmO8gDCmOrvH9OR0dya |
| fo=123&bar=X:Y&yuk=Yabba+Yabba | 54321 | Re6RmNgo2dUz8cEEFx23K233eLdjzdCeKb |
