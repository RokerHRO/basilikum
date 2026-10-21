// Decoder robustness test vectors from the real C reference build (gcc 13.1 / MinGW).
// Inputs the encoder can never produce: invalid chars, '-' (value 62),
// short/aligned inputs, extreme decoded bytes.
// Invalid characters are skipped (intended behavior, matches C);
// the expected hex reflects that skipping.
// Format: [secret, decodeInput, decode(decodeInput) as hex]
window.basilikumDecodeVectors = [
  ["12345", "a", ""],
  ["1", "ab", "5d"],
  ["12345", "!!", ""],
  ["12345", "6ywb.", "616263"],
  ["12345", "6y.wb", "616263"],
  ["12345", "--------", "6b212331397f3b"],
  ["42", "----------------", "2058464d287d696e6c207c363a5d"],
  ["89", "aaaaaaaaaa", "207f7f7f7f7f7f7f7f"],
  ["90", "aaaaaaaaaa", "7f7e7e7e7e7e7e7e7e"],
  ["54321", "zzzzzzzzzz", "346673326d3c2b752a"],
  ["54321", "6ywb-6ywb-", "61675f42786460596f"],
  ["101", "aaaaaaaaaabbbbbbbbbbcccccccccc", "7473737373737373735e4a4d7a6c4c7c6a74492228226526266176"],
  ["18446744073709551615", "1234567890", "553c4e71772f617d78"],
];
