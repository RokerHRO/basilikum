// basilikum.js - JavaScript port of basilikum.c (base96 -> base62 codec
// for printable ASCII texts, e.g. URLs).
//
// API:
//   basilikum.encode(input, secret) -> string
//   basilikum.decode(input, secret) -> string
//
// secret: unsigned 64-bit integer. Pass it as a decimal string (recommended,
// exact for the full 64-bit range), number (exact up to 2^53) or bigint.
//
// Notes (faithful to the C original):
//   - Encoding uses only 62 alphabet characters (a-z, 0-9, A-Z); '-' and '.'
//     are in the alphabet table but never produced.
//   - Decoding accepts a-z, 0-9, A-Z and '-' (the C code fills unalpha[0..62]
//     only). Any other character is silently skipped (intended behavior; the
//     C original additionally prints "!ERROR!" to stdout for each of them).
//   - Input is processed as UTF-8 bytes, like the C byte strings.
//
// Uses BigInt because a full 9-byte block maps to up to 96^9-1 ~= 6.9e17,
// which exceeds Number.MAX_SAFE_INTEGER (2^53-1).

'use strict';

(function (root, factory) {
  if (typeof module === 'object' && module.exports) module.exports = factory();
  else root.basilikum = factory();
})(typeof self !== 'undefined' ? self : this, function () {
  const ALPHABET =
    'abcdefghijklmnopqrstuvwxyz' +
    '0123456789' +
    'ABCDEFGHIJKLMNOPQRSTUVWXYZ' +
    '-.';

  const UNALPHA = new Int8Array(256);
  UNALPHA.fill(-1);
  // C quirk kept: only indices 0..62 are mapped ('-' is valid, '.' is not).
  for (let u = 0; u <= 62; ++u)
    UNALPHA[ALPHABET.charCodeAt(u)] = u;

  const MASK64 = 0xFFFFFFFFFFFFFFFFn;
  const utf8enc = new TextEncoder();
  const utf8dec = new TextDecoder('utf-8');

  function toSecret64(secret) {
    if (typeof secret === 'bigint') return secret & MASK64;
    if (typeof secret === 'string') {
      const t = secret.trim();
      if (t === '') throw new TypeError('secret is empty');
      return BigInt(t) & MASK64;
    }
    if (typeof secret === 'number' && Number.isFinite(secret))
      return BigInt(Math.trunc(secret)) & MASK64;
    throw new TypeError('secret must be an integer (number, string, or bigint)');
  }

  // C: sidx() - printable ASCII (32..126) -> 0..94, anything else -> 94.
  function sidxByte(c) {
    return (c < 32 || c > 126) ? 94 : c - 32;
  }

  // C: base62() - writes exactly `length` base62 digits (leading 'a' included).
  function base62(coll, length) {
    let out = '';
    for (let n = 0; n < length; ++n) {
      out += ALPHABET[Number(coll % 62n)];
      coll /= 62n;
    }
    return out;
  }

  // C: unbase96() - writes iidx-1 bytes (low digit first), returns new rs.
  function unbase96(coll, iidx, rs, secret64, out) {
    while (--iidx > 0) {
      const v = Number((96n + (coll % 96n) - (rs % 89n)) % 96n);
      out.push(32 + v);
      coll /= 96n;
      rs /= 89n;
      if (rs < 101n) rs = (rs + secret64) & MASK64;
    }
    return rs;
  }

  function basilikumEncode(input, secret) {
    if (typeof input !== 'string') throw new TypeError('input must be a string');
    const secret64 = toSecret64(secret);
    const bytes = utf8enc.encode(input);

    let rs = secret64;
    let iidx = 0;
    let factor = 1n;
    let coll = 0n;
    let out = '';

    for (let i = 0; i < bytes.length; ++i) {
      const si = sidxByte(bytes[i]);
      coll += BigInt((si + Number(rs % 89n)) % 96) * factor;

      rs /= 89n;
      if (rs < 101n) rs = (rs + secret64) & MASK64;

      factor *= 96n;
      if (++iidx === 9) {
        out += base62(coll, iidx + 1);
        iidx = 0;
        factor = 1n;
        coll = 0n;
      }
    }

    if (iidx)
      out += base62(coll, iidx + 1);

    return out;
  }

  function basilikumDecode(input, secret) {
    if (typeof input !== 'string') throw new TypeError('input must be a string');
    const secret64 = toSecret64(secret);

    let rs = secret64;
    let iidx = 0;
    let factor = 1n;
    let coll = 0n;
    const out = [];

    for (let i = 0; i < input.length; ++i) {
      const c = input.charCodeAt(i);
      const val = c < 256 ? UNALPHA[c] : -1;
      if (val < 0) continue; // intended: invalid characters are skipped (matches C)

      coll += BigInt(val) * factor;
      ++iidx;
      factor *= 62n;
      if (iidx === 10) {
        rs = unbase96(coll, iidx, rs, secret64, out);
        iidx = 0;
        factor = 1n;
        coll = 0n;
      }
    }

    if (iidx)
      rs = unbase96(coll, iidx, rs, secret64, out);

    return utf8dec.decode(new Uint8Array(out));
  }

  return {
    encode: basilikumEncode,
    decode: basilikumDecode,
    alphabet: ALPHABET
  };
});
